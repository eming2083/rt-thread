#include <pthread.h>
#include "pthread_internal.h"

int pthread_system_init(void)
{
	/* initialize key area */
	pthread_key_system_init();

	return 0;
}

static void _pthread_cleanup(rt_thread_t tid)
{
	_pthread_data_t *ptd;
	ptd = _pthread_get_data(tid);

	/* clear cleanup function */
	tid->cleanup = RT_NULL;
	if (ptd->attr.detachstate == PTHREAD_CREATE_JOINABLE)
	{
		rt_sem_release(ptd->joinable_sem);
	}
	else
	{
		/* release pthread resource */
		pthread_detach(tid);
	}
}

static void pthread_entry_stub(void* parameter)
{
	_pthread_data_t *ptd;
	void* value;

	ptd = (_pthread_data_t*)parameter;

	/* execute pthread entry */
	value = ptd->thread_entry(ptd->thread_parameter);
	/* set value */
	ptd->return_value = value;
}

int pthread_create (pthread_t *tid, const pthread_attr_t *attr, 
	void *(*start) (void *), void *parameter)
{
	int result;
	void* stack;
	char name[RT_NAME_MAX];
	static rt_uint16_t pthread_number = 0;
	_pthread_data_t *ptd;

	/* tid shall be provided */
	RT_ASSERT(tid != RT_NULL);

	/* allocate posix thread data */
	ptd = (_pthread_data_t*)rt_malloc(sizeof(_pthread_data_t));
	if (ptd == RT_NULL) return ENOMEM;
	/* clean posix thread data memory */
	rt_memset(ptd, 0, sizeof(_pthread_data_t));
	ptd->canceled = 0;
	ptd->cancelstate = PTHREAD_CANCEL_DISABLE;
	ptd->canceltype = PTHREAD_CANCEL_DEFERRED;

	if (attr != RT_NULL) ptd->attr = *attr;
	else 
	{
		/* use default attribute */
		pthread_attr_init(&ptd->attr);
	}

	rt_snprintf(name, sizeof(name), "pth%02d", pthread_number ++);
	if (ptd->attr.stack_base == 0)
	{
		stack = (void*)rt_malloc(ptd->attr.stack_size);
	}
	else stack = (void*)(ptd->attr.stack_base);

	if (stack == RT_NULL) 
	{
		rt_free(ptd);
		return ENOMEM;
	}

	/* pthread is a static thread object */
	ptd->tid = (rt_thread_t) rt_malloc(sizeof(struct rt_thread));
	if (ptd->tid == RT_NULL)
	{
		if (ptd->attr.stack_base ==0) rt_free(stack);
		rt_free(ptd);
		return ENOMEM;
	}

	if (ptd->attr.detachstate == PTHREAD_CREATE_JOINABLE)
	{
		ptd->joinable_sem = rt_sem_create(name, 0, RT_IPC_FLAG_FIFO);
		if (ptd->joinable_sem == RT_NULL)
		{
			if (ptd->attr.stack_base !=0) rt_free(stack);
			rt_free(ptd);
			return ENOMEM;
		}
	}
	else ptd->joinable_sem = RT_NULL;

	/* set parameter */
	ptd->thread_entry = start;
	ptd->thread_parameter = parameter;

	/* initial this pthread to system */
	if (rt_thread_init(ptd->tid, name, pthread_entry_stub, ptd, 
		stack, ptd->attr.stack_size, 
		ptd->attr.priority, 5) != RT_EOK)
	{
		if (ptd->attr.stack_base ==0) rt_free(stack);
		if (ptd->joinable_sem != RT_NULL) rt_sem_delete(ptd->joinable_sem);
		rt_free(ptd);
		return EINVAL;
	}

	/* set pthread id */
	*tid = ptd->tid;

	/* set pthread cleanup function and ptd data */
	(*tid)->cleanup = _pthread_cleanup;
	(*tid)->user_data = (rt_uint32_t)ptd;

	/* start thread */
	result = rt_thread_startup(*tid);
	if (result == RT_EOK) return 0;

	/* start thread failed */
	rt_thread_detach(ptd->tid);
	if (ptd->attr.stack_base ==0) rt_free(stack);
	if (ptd->joinable_sem != RT_NULL) rt_sem_delete(ptd->joinable_sem);

	rt_free(ptd);
	return EINVAL;
}

int pthread_detach(pthread_t thread)
{
	_pthread_data_t* ptd;

	ptd = _pthread_get_data(thread);

	if (thread->stat == RT_THREAD_CLOSE)
	{
		/* delete joinable semaphore */
		if (ptd->joinable_sem != RT_NULL)
			rt_sem_delete(ptd->joinable_sem);
		/* detach thread object */
		rt_thread_detach(ptd->tid);

		/* release thread resource */
		if (ptd->attr.stack_base == RT_NULL)
		{
			/* release thread allocated stack */
			rt_free(ptd->tid->stack_addr);
		}

		/*
		 * if this thread create the local thread data,
		 * delete it
		 */
		if (ptd->tls != RT_NULL) rt_free(ptd->tls);
		rt_free(ptd->tid);
		rt_free(ptd);
	}
	else
	{
		rt_enter_critical();
		/* change to detach state */
		ptd->attr.detachstate = PTHREAD_CREATE_DETACHED;
		rt_exit_critical();

		/* detach joinable semaphore */
		rt_sem_delete(ptd->joinable_sem);
	}

	return 0;
}

int pthread_join (pthread_t thread, void **value_ptr)
{
	_pthread_data_t* ptd;
	rt_err_t result;

	if (thread == rt_thread_self())
	{
		/* join self */
		return EDEADLK;
	}

	ptd = _pthread_get_data(thread);
	if (ptd->attr.detachstate == PTHREAD_CREATE_DETACHED)
		return EINVAL; /* join on a detached pthread */

	result = rt_sem_take(ptd->joinable_sem, RT_WAITING_FOREVER);
	if (result == RT_EOK)
	{
		/* get return value */
		if (value_ptr != RT_NULL) *value_ptr = ptd->return_value;

		/* release resource */
		pthread_detach(thread);
	}
	else return ESRCH;
}

void pthread_exit (void* value)
{
	_pthread_data_t* ptd;
	_pthread_cleanup_t* cleanup;
	_pthread_key_data_t* key;
	extern _pthread_key_data_t _thread_keys[PTHREAD_KEY_MAX];

	ptd = _pthread_get_data(rt_thread_self());

	rt_enter_critical();
	/* disable cancel */
	ptd->cancelstate = PTHREAD_CANCEL_DISABLE;
	/* set return value */
	ptd->return_value = value;
	rt_exit_critical();

	/* invoke pushed cleanup */
	while (ptd->cleanup != RT_NULL)
	{
		cleanup = ptd->cleanup;
		ptd->cleanup = cleanup->next;

		cleanup->cleanup_func(cleanup->parameter);
		/* release this cleanup function */
		rt_free(cleanup);
	}

	/* destruct thread local key */
	if (ptd->tls != RT_NULL)
	{
		void* data;
		rt_uint32_t index;
		
		for (index = 0; index < PTHREAD_KEY_MAX; index ++)
		{
			if (_thread_keys[index].is_used)
			{
				data = ptd->tls[index];
				if (data)
					_thread_keys[index].destructor(data);
			}
		}

		/* release tls area */
		rt_free(ptd->tls);
		ptd->tls = RT_NULL;
	}

	if (ptd->attr.detachstate == PTHREAD_CREATE_JOINABLE)
	{
		/* release the joinable pthread */
		rt_sem_release(ptd->joinable_sem);
	}

	/* detach thread */
	rt_thread_detach(ptd->tid);
	/* reschedule thread */
	rt_schedule();
}

int pthread_once(pthread_once_t * once_control, void (*init_routine) (void))
{
	RT_ASSERT(once_control != RT_NULL);
	RT_ASSERT(init_routine != RT_NULL);

	rt_enter_critical();
	if (!(*once_control))
	{
		/* call routine once */
		*once_control = 1;
		rt_exit_critical();

		init_routine();
	}
	rt_exit_critical();

	return 0;
}

int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void))
{
	return ENOTSUP;
}

int pthread_kill(pthread_t thread, int sig)
{
	return ENOTSUP;
}

void pthread_cleanup_pop(int execute)
{
	_pthread_data_t* ptd;
	_pthread_cleanup_t* cleanup;

	/* get posix thread data */
	ptd = _pthread_get_data(rt_thread_self());
	RT_ASSERT(ptd != RT_NULL);

	if (execute)
	{
		rt_enter_critical();
		cleanup = ptd->cleanup;
		if (cleanup)
			ptd->cleanup = cleanup->next;
		rt_exit_critical();

		if (cleanup)
		{
			cleanup->cleanup_func(cleanup->parameter);

			rt_free(cleanup);
		}
	}
}

void pthread_cleanup_push(void (*routine)(void*), void *arg)
{
	_pthread_data_t* ptd;
	_pthread_cleanup_t* cleanup;

	/* get posix thread data */
	ptd = _pthread_get_data(rt_thread_self());
	RT_ASSERT(ptd != RT_NULL);

	cleanup = (_pthread_cleanup_t*)rt_malloc(sizeof(_pthread_cleanup_t));
	if (cleanup != RT_NULL)
	{
		cleanup->cleanup_func = routine;
		cleanup->parameter = arg;

		rt_enter_critical();
		cleanup->next = ptd->cleanup;
		ptd->cleanup = cleanup;
		rt_exit_critical();
	}
}

int pthread_setcancelstate(int state, int *oldstate)
{
	_pthread_data_t* ptd;

	/* get posix thread data */
	ptd = _pthread_get_data(rt_thread_self());
	RT_ASSERT(ptd != RT_NULL);

	if ((state == PTHREAD_CANCEL_ENABLE) || (state == PTHREAD_CANCEL_DISABLE))
	{
    	if (oldstate) *oldstate = ptd->cancelstate;
		ptd->cancelstate = state;

		return 0;
	}

	return EINVAL;
}

int pthread_setcanceltype(int type, int *oldtype)
{
	_pthread_data_t* ptd;

	/* get posix thread data */
	ptd = _pthread_get_data(rt_thread_self());
	RT_ASSERT(ptd != RT_NULL);

	if ((type != PTHREAD_CANCEL_DEFERRED) && (type != PTHREAD_CANCEL_ASYNCHRONOUS)) 
		return EINVAL;

	if (oldtype) *oldtype = ptd->canceltype;
	ptd->canceltype = type;

	return 0;
}

void pthread_testcancel(void)
{
	int cancel=0;
	_pthread_data_t* ptd;

	/* get posix thread data */
	ptd = _pthread_get_data(rt_thread_self());
	RT_ASSERT(ptd != RT_NULL);

	if (ptd->cancelstate == PTHREAD_CANCEL_ENABLE) cancel = ptd->canceled;
	if (cancel) pthread_exit((void*)PTHREAD_CANCELED);
}

int pthread_cancel(pthread_t thread)
{
	_pthread_data_t* ptd;

	/* get posix thread data */
	ptd = _pthread_get_data(thread);
	RT_ASSERT(ptd != RT_NULL);

	/* set canceled */
	if (ptd->cancelstate == PTHREAD_CANCEL_ENABLE)
	{
		ptd->canceled = 1;
		if (ptd->canceltype == PTHREAD_CANCEL_ASYNCHRONOUS)
		{
			/* TODO: need cancel thread */
		}
	}

	return 0;
}
