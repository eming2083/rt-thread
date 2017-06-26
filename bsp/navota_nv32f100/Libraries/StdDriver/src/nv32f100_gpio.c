/**************************************************************************!     
 *     �������ۣ�QQȺ  123763203
 *     ����    ��www.navota.com
 *
 * @file       gpio.c
 * @brief      ͨ���������ģ��(GPIO)������
 * @author     Navota
 * @date       2017-1-1
 **************************************************************************/
#include "nv32f100.h"



/*****************************************************************************//*!
* @brief    ��λGPIOģ��.
*        
* @param[in] pGPIO    ָ��GPIOģ�� GPIOA/GPIOB.
*
* @return none
*
*****************************************************************************/
void GPIO_DeInit(GPIO_Type *pGPIO)
{
    /* Sanity check */
#if defined(CPU_NV32)
     ASSERT((pGPIO == GPIOA) || (pGPIO == GPIOB));
#endif
#if defined(CPU_NV32M3)
    ASSERT(pGPIO == GPIOA);
#endif
#if defined(CPU_NV32M4)
      ASSERT((pGPIO == GPIOA) || (pGPIO == GPIOB) || (pGPIO == GPIOC));
#endif

    pGPIO->PCOR = 0x00000000;   /* �˿���������Ĵ��� */
    pGPIO->PDDR = 0x00000000;   /* �˿����ݷ���Ĵ��� */
    //pGPIO->PDIR = 0x00000000;   /* �˿���������Ĵ��� */
    pGPIO->PDOR = 0x00000000;   /* �˿���������Ĵ��� */
    pGPIO->PIDR = 0xFFFFFFFF;   /* �˿�������üĴ��� */
    pGPIO->PSOR = 0x00000000;   /* �˿���λ����Ĵ��� */
    pGPIO->PTOR = 0x00000000;   /* �˿��л�����Ĵ��� */
}

/*****************************************************************************//*!
* @brief    ��ʼ��GPIO����--ͨ��32λ��������ȷ��Ҫ��ʼ��������
*        
* @param[in] pGPIO       ָ��GPIOģ�� GPIOA/GPIOB.
* @param[in] u32PinMask  32λ��������  ( GPIO_PTA0_MASK, GPIO_PTA1_MASK ............)
* @param[in] sGpioType   �������� �����롢���롢�ߵ����������ڲ�������
*
* @return   none
*
* @ ע��
*   . �����������Ϊ���룬���øߵ�������
*   . �����������Ϊ����������ڲ�����
*     ��PTH1/0, PTE1/0, PTD1/0, PTB5/4 ֧�ָߵ�������.
*****************************************************************************/
void GPIO_Init(GPIO_Type *pGPIO, uint32_t u32PinMask, GPIO_PinConfigType sGpioType)
{
    /* Sanity check */
#if defined(CPU_NV32)
     ASSERT((pGPIO == GPIOA) || (pGPIO == GPIOB));
#endif
#if defined(CPU_NV32M3)
    ASSERT(pGPIO == GPIOA);
#endif
#if defined(CPU_NV32M4)
      ASSERT((pGPIO == GPIOA) || (pGPIO == GPIOB) || (pGPIO == GPIOC));
#endif
    
    /* ����GPIOΪ���������� */
    if ((sGpioType == GPIO_PinOutput) || (sGpioType == GPIO_PinOutput_HighCurrent))
    {
        pGPIO->PDDR |= u32PinMask;      /* ��������Ϊͨ�����*/
        pGPIO->PIDR |= u32PinMask;      /* ��λ�˿�������üĴ���*/   
    }
    else if ((sGpioType == GPIO_PinInput) || (sGpioType == GPIO_PinInput_InternalPullup))
    {
        pGPIO->PDDR &= ~u32PinMask;   /* ��������Ϊͨ������ */
        pGPIO->PIDR &= ~u32PinMask;   /* ����˿�������üĴ��� */
    }
    /* ����GPIO�˿��������� */
#if defined(CPU_NV32)
    switch((uint32_t)pGPIO)
    {
        case GPIOA_BASE:
            (sGpioType == GPIO_PinInput_InternalPullup)?(PORT->PUEL |= u32PinMask):(PORT->PUEL &= ~u32PinMask);
            break;
        case GPIOB_BASE:
            (sGpioType == GPIO_PinInput_InternalPullup)?(PORT->PUEH |= u32PinMask):(PORT->PUEH &= ~u32PinMask);
            break;
        default:
            break;
     }
#endif
    
#if defined(CPU_NV32M3)
    switch((uint32_t)pGPIO)
    {
        case GPIOA_BASE:
            (sGpioType == GPIO_PinInput_InternalPullup)?(PORT->PUEL |= u32PinMask):(PORT->PUEL &= ~u32PinMask);
            break;
        default:
            break;
     }
#endif
    
#if defined(CPU_NV32M4)
    switch((uint32_t)pGPIO)
    {
        case GPIOA_BASE:
            (sGpioType == GPIO_PinInput_InternalPullup)?(PORT->PUE0 |= u32PinMask):(PORT->PUE0 &= ~u32PinMask);
            break;
        case GPIOB_BASE:
            (sGpioType == GPIO_PinInput_InternalPullup)?(PORT->PUE1 |= u32PinMask):(PORT->PUE1 &= ~u32PinMask);
            break;
        case GPIOC_BASE:
            (sGpioType == GPIO_PinInput_InternalPullup)?(PORT->PUE2 |= u32PinMask):(PORT->PUE2 &= ~u32PinMask);
            break;
        default:
            break;
     }
#endif   
    
    /* ����GPIO�˿�����ߵ������� */
#if defined(CPU_NV32M3)
    if (u32PinMask & GPIO_PTC5_MASK)    
    {   
        PORT->HDRVE |= PORT_HDRVE_PTC5_MASK;
    }   
    if (u32PinMask & GPIO_PTC1_MASK)
    {   
        PORT->HDRVE |= PORT_HDRVE_PTC1_MASK;
    }   
    if (u32PinMask & GPIO_PTB5_MASK)
    {   
        PORT->HDRVE |= PORT_HDRVE_PTB5_MASK;
    }   
#endif
    
#if defined(CPU_NV32) | defined(CPU_NV32M4)
    if (pGPIO == GPIOA)
    {
        if (u32PinMask & GPIO_PTB4_MASK)    //PB4 �ߵ����������
        {   
            PORT->HDRVE |= PORT_HDRVE_PTB4_MASK;
        }   
        if (u32PinMask & GPIO_PTB5_MASK)
        {   
            PORT->HDRVE |= PORT_HDRVE_PTB5_MASK;
        }   
        if (u32PinMask & GPIO_PTD0_MASK)
        {   
            PORT->HDRVE |= PORT_HDRVE_PTD0_MASK;
        }   
        if (u32PinMask & GPIO_PTD1_MASK)
        {   
            PORT->HDRVE |= PORT_HDRVE_PTD1_MASK;
        }   
    }
    if (pGPIO == GPIOB)
    {
        if (u32PinMask & GPIO_PTE0_MASK)
        {   
            PORT->HDRVE |= PORT_HDRVE_PTE0_MASK;
        }   
        if (u32PinMask & GPIO_PTE1_MASK)
        {   
            PORT->HDRVE |= PORT_HDRVE_PTE1_MASK;
        }   
        if (u32PinMask & GPIO_PTH0_MASK)
        {   
            PORT->HDRVE |= PORT_HDRVE_PTH0_MASK;
        }   
        if (u32PinMask & GPIO_PTH1_MASK)
        {   
            PORT->HDRVE |= PORT_HDRVE_PTH1_MASK;
        }   
    }

#endif
    
}

/*****************************************************************************//*!
* @brief    �л�GPIO�˿����������������ͨ��32λ��������ȷ��Ҫ�л����������
*        
* @param[in] pGPIO       ָ��GPIOģ��  GPIOA/GPIOB.
* @param[in] u32PinMask  32λ��������  ( GPIO_PTA0_MASK, GPIO_PTA1_MASK ............)
*
* @return none
*
*****************************************************************************/
void GPIO_Toggle(GPIO_Type *pGPIO, uint32_t u32PinMask)
{
    /* Sanity check */
#if defined(CPU_NV32)
     ASSERT((pGPIO == GPIOA) || (pGPIO == GPIOB));
#endif
#if defined(CPU_NV32M3)
    ASSERT(pGPIO == GPIOA);
#endif
#if defined(CPU_NV32M4)
      ASSERT((pGPIO == GPIOA) || (pGPIO == GPIOB) || (pGPIO == GPIOC));
#endif

    pGPIO->PTOR = u32PinMask;   /* 32λ��������ȷ��Ҫ�л���������� */
}

/*****************************************************************************//*!
* @brief  ��ȡ�˿���������Ĵ��� 
*        
* @param[in] pGPIO      ָ��GPIOģ��  GPIOA/GPIOB.
*
* @return   �˿�����Ĵ���32λ��ֵ
*
*****************************************************************************/
uint32_t GPIO_Read(GPIO_Type *pGPIO)
{
    /* Sanity check */
#if defined(CPU_NV32)
     ASSERT((pGPIO == GPIOA) || (pGPIO == GPIOB));
#endif
#if defined(CPU_NV32M3)
    ASSERT(pGPIO == GPIOA);
#endif
#if defined(CPU_NV32M4)
      ASSERT((pGPIO == GPIOA) || (pGPIO == GPIOB) || (pGPIO == GPIOC));
#endif

    return (pGPIO->PDIR);   /* ���˿���������Ĵ��� */
  
}

/*****************************************************************************//*!
* @brief ��ȡ�˿����ݼĴ�����ĳһλ��������ͨ�������GPIO������ȷ��Ҫ��ȡ��λ��
*        
* @param[in] pGPIO      ָ��GPIOģ��  GPIOA/GPIOB.
* @param[in] GPIO_Pin   GPIO������ (GPIO_PTA0�GPIO_PTA1..............)
*
* @return  �˿����ݼĴ���ĳһλ��ֵ
*
*****************************************************************************/
uint8_t GPIO_BitRead(GPIO_PinType GPIO_Pin)
{
  uint8_t data;  
  /* Sanity check */
    ASSERT(GPIO_Pin <= GPIO_PTI7);
    

      if (GPIO_Pin < GPIO_PTE0)
      {
            if(((1<<GPIO_Pin) & GPIOA->PDIR) > 0)       /*�ж�Ҫ��ȡ��λ����Ӧ����ֵ��1����0*/
                data = 0x1;                             /* �����1����1����0�򷵻�0 */
            else 
                data = 0x0;
          
      }
      
      else if (GPIO_Pin < GPIO_PTI0)
      {
        GPIO_Pin = (GPIO_PinType)(GPIO_Pin - 32);
        
            if(((1<<GPIO_Pin) & GPIOB->PDIR) > 0)       /*�ж�Ҫ��ȡ��λ����Ӧ����ֵ��1����0*/
                data = 0x1;                             /* �����1����1����0�򷵻�0 */
            else 
	       data = 0x0;
          
      }

     return data;
		    
}


/*****************************************************************************//*!
* @brief  д���ݵ��˿���������Ĵ��� 
*        
* @param[in] pGPIO      ָ��GPIOģ��  GPIOA/GPIOB.
* @param[in] u32Value   д�뵽�˿���������Ĵ�����ֵ
*
* @return   none
*
*****************************************************************************/
void GPIO_Write(GPIO_Type *pGPIO, uint32_t u32Value)
{
    /* Sanity check */
#if defined(CPU_NV32)
     ASSERT((pGPIO == GPIOA) || (pGPIO == GPIOB));
#endif
#if defined(CPU_NV32M3)
    ASSERT(pGPIO == GPIOA);
#endif
#if defined(CPU_NV32M4)
      ASSERT((pGPIO == GPIOA) || (pGPIO == GPIOB) || (pGPIO == GPIOC));
#endif
    
    pGPIO->PDOR = u32Value;    /* д���ݵ��˿���������Ĵ��� */
  
}

/*****************************************************************************//*!
* @brief   ��ʼ��GPIO����--ͨ�������GPIO������ȷ��Ҫ��ʼ��������
*        
* @param[in] GPIO_Pin        GPIO������ (GPIO_PTA0�GPIO_PTA1..............)
* @param[in] GPIO_PinConfig  �������롢���
*
* @return   none
*
*****************************************************************************/
void GPIO_PinInit(GPIO_PinType GPIO_Pin, GPIO_PinConfigType GPIO_PinConfig)
{
    /* Sanity check */
    ASSERT(GPIO_Pin <= GPIO_PTI7);
      

#if defined(CPU_NV32)
      if (GPIO_Pin < GPIO_PTE0)
      {
        switch (GPIO_PinConfig)
        {
        case GPIO_PinOutput:
            GPIOA->PDDR |= (1<<GPIO_Pin);      /* ��������Ϊͨ����� */
            GPIOA->PIDR |= (1<<GPIO_Pin);      /* �˿�������üĴ�����һ*/
            PORT->PUEL &= ~(1<<GPIO_Pin);      /* �����ڲ����� */
          break;
        case GPIO_PinInput:
            GPIOA->PDDR &= ~(1<<GPIO_Pin);     /* ��������Ϊͨ������ */
            GPIOA->PIDR &= ~(1<<GPIO_Pin);     /* �˿�������üĴ������� */
            PORT->PUEL &= ~(1<<GPIO_Pin);      /*  ��������*/
          break;
        case GPIO_PinInput_InternalPullup:
            GPIOA->PDDR &= ~(1<<GPIO_Pin);     /* ��������Ϊͨ������  */
            GPIOA->PIDR &= ~(1<<GPIO_Pin);     /* �˿�������üĴ�������  */
            PORT->PUEL |= (1<<GPIO_Pin);       /* ʹ���ڲ����� */
          break;
        case GPIO_PinOutput_HighCurrent:
            GPIOA->PDDR |= (1<<GPIO_Pin);      /* ��������Ϊͨ�����  */
            GPIOA->PIDR |= (1<<GPIO_Pin);      /* �˿�������üĴ�����һ */
            PORT->PUEL &= ~(1<<GPIO_Pin);       /* �����ڲ�����*/
          break;
        }
      }
      else if (GPIO_Pin < GPIO_PTI0)
      {
        GPIO_Pin = (GPIO_PinType)(GPIO_Pin - 32);
        switch (GPIO_PinConfig)
        {
        case GPIO_PinOutput:
            GPIOB->PDDR |= (1<<GPIO_Pin);      /* ��������Ϊͨ����� */
            GPIOB->PIDR |= (1<<GPIO_Pin);      /* �˿�������üĴ�����һ */
            PORT->PUEH &= ~(1<<GPIO_Pin);      /* �����ڲ�����*/
          break;
        case GPIO_PinInput:
            GPIOB->PDDR &= ~(1<<GPIO_Pin);      /* ��������Ϊͨ������ */
            GPIOB->PIDR &= ~(1<<GPIO_Pin);      /* �˿�������üĴ�������  */
            PORT->PUEH &= ~(1<<GPIO_Pin);      /* �����ڲ�����*/
          break;
        case GPIO_PinInput_InternalPullup:
            GPIOB->PDDR &= ~(1<<GPIO_Pin);    /* ��������Ϊͨ������ */
            GPIOB->PIDR &= ~(1<<GPIO_Pin);    /* �˿�������üĴ�������  */
            PORT->PUEH |= (1<<GPIO_Pin);      /* ʹ���ڲ�����*/
          break;
        case GPIO_PinOutput_HighCurrent:
            GPIOA->PDDR |= (1<<GPIO_Pin);      /* ��������Ϊͨ�����  */
            GPIOA->PIDR |= (1<<GPIO_Pin);      /* �˿�������üĴ�����һ */
            PORT->PUEL &= ~(1<<GPIO_Pin);       /* �����ڲ�����*/
          break;
        }
      }
#endif
      
#if defined(CPU_NV32M3)
      if (GPIO_Pin < GPIO_PTE0)
      {
        switch (GPIO_PinConfig)
        {
        case GPIO_PinOutput:
            GPIOA->PDDR |= (1<<GPIO_Pin);      /* ��������Ϊͨ����� */
            GPIOA->PIDR |= (1<<GPIO_Pin);      /* �˿�������üĴ�����һ*/
            PORT->PUEL &= ~(1<<GPIO_Pin);      /* �����ڲ����� */
          break;
        case GPIO_PinInput:
            GPIOA->PDDR &= ~(1<<GPIO_Pin);     /* ��������Ϊͨ������ */
            GPIOA->PIDR &= ~(1<<GPIO_Pin);     /* �˿�������üĴ������� */
            PORT->PUEL &= ~(1<<GPIO_Pin);      /*  ��������*/
          break;
        case GPIO_PinInput_InternalPullup:
            GPIOA->PDDR &= ~(1<<GPIO_Pin);     /* ��������Ϊͨ������  */
            GPIOA->PIDR &= ~(1<<GPIO_Pin);     /* �˿�������üĴ�������  */
            PORT->PUEL |= (1<<GPIO_Pin);       /* ʹ���ڲ����� */
          break;
        case GPIO_PinOutput_HighCurrent:
            GPIOA->PDDR |= (1<<GPIO_Pin);      /* ��������Ϊͨ�����  */
            GPIOA->PIDR |= (1<<GPIO_Pin);      /* �˿�������üĴ�����һ */
            PORT->PUEL &= ~(1<<GPIO_Pin);       /* �����ڲ�����*/
          break;
        }
      }
#endif
    
    
#if defined(CPU_NV32M4)
      if (GPIO_Pin < GPIO_PTE0)
      {
         switch (GPIO_PinConfig)
        {
        case GPIO_PinOutput:
            GPIOA->PDDR |= (1<<GPIO_Pin);      /* ��������Ϊͨ����� */
            GPIOA->PIDR |= (1<<GPIO_Pin);      /* �˿�������üĴ�����һ*/
            PORT->PUEL &= ~(1<<GPIO_Pin);      /* �����ڲ����� */
          break;
        case GPIO_PinInput:
            GPIOA->PDDR &= ~(1<<GPIO_Pin);     /* ��������Ϊͨ������ */
            GPIOA->PIDR &= ~(1<<GPIO_Pin);     /* �˿�������üĴ������� */
            PORT->PUEL &= ~(1<<GPIO_Pin);      /*  ��������*/
          break;
        case GPIO_PinInput_InternalPullup:
            GPIOA->PDDR &= ~(1<<GPIO_Pin);     /* ��������Ϊͨ������  */
            GPIOA->PIDR &= ~(1<<GPIO_Pin);     /* �˿�������üĴ�������  */
            PORT->PUEL |= (1<<GPIO_Pin);       /* ʹ���ڲ����� */
          break;
        case GPIO_PinOutput_HighCurrent:
            GPIOA->PDDR |= (1<<GPIO_Pin);      /* ��������Ϊͨ�����  */
            GPIOA->PIDR |= (1<<GPIO_Pin);      /* �˿�������üĴ�����һ */
            PORT->PUEL &= ~(1<<GPIO_Pin);       /* �����ڲ�����*/
          break;
        }
      }
      else if (GPIO_Pin < GPIO_PTI0)
      {
        GPIO_Pin = (GPIO_PinType)(GPIO_Pin - 32);
        switch (GPIO_PinConfig)
        {
        case GPIO_PinOutput:
            GPIOA->PDDR |= (1<<GPIO_Pin);      /* ��������Ϊͨ����� */
            GPIOA->PIDR |= (1<<GPIO_Pin);      /* �˿�������üĴ�����һ*/
            PORT->PUEL &= ~(1<<GPIO_Pin);      /* �����ڲ����� */
          break;
        case GPIO_PinInput:
            GPIOA->PDDR &= ~(1<<GPIO_Pin);     /* ��������Ϊͨ������ */
            GPIOA->PIDR &= ~(1<<GPIO_Pin);     /* �˿�������üĴ������� */
            PORT->PUEL &= ~(1<<GPIO_Pin);      /*  ��������*/
          break;
        case GPIO_PinInput_InternalPullup:
            GPIOA->PDDR &= ~(1<<GPIO_Pin);     /* ��������Ϊͨ������  */
            GPIOA->PIDR &= ~(1<<GPIO_Pin);     /* �˿�������üĴ�������  */
            PORT->PUEL |= (1<<GPIO_Pin);       /* ʹ���ڲ����� */
          break;
        case GPIO_PinOutput_HighCurrent:
            GPIOA->PDDR |= (1<<GPIO_Pin);      /* ��������Ϊͨ�����  */
            GPIOA->PIDR |= (1<<GPIO_Pin);      /* �˿�������üĴ�����һ */
            PORT->PUEL &= ~(1<<GPIO_Pin);       /* �����ڲ�����*/
          break;
        }
      }
      else
      {
        GPIO_Pin = (GPIO_PinType)(GPIO_Pin - 64);
        switch (GPIO_PinConfig)
        {
        case GPIO_PinOutput:
            GPIOA->PDDR |= (1<<GPIO_Pin);      /* ��������Ϊͨ����� */
            GPIOA->PIDR |= (1<<GPIO_Pin);      /* �˿�������üĴ�����һ*/
            PORT->PUEL &= ~(1<<GPIO_Pin);      /* �����ڲ����� */
          break;
        case GPIO_PinInput:
            GPIOA->PDDR &= ~(1<<GPIO_Pin);     /* ��������Ϊͨ������ */
            GPIOA->PIDR &= ~(1<<GPIO_Pin);     /* �˿�������üĴ������� */
            PORT->PUEL &= ~(1<<GPIO_Pin);      /*  ��������*/
          break;
        case GPIO_PinInput_InternalPullup:
            GPIOA->PDDR &= ~(1<<GPIO_Pin);     /* ��������Ϊͨ������  */
            GPIOA->PIDR &= ~(1<<GPIO_Pin);     /* �˿�������üĴ�������  */
            PORT->PUEL |= (1<<GPIO_Pin);       /* ʹ���ڲ����� */
          break;
        case GPIO_PinOutput_HighCurrent:
            GPIOA->PDDR |= (1<<GPIO_Pin);      /* ��������Ϊͨ�����  */
            GPIOA->PIDR |= (1<<GPIO_Pin);      /* �˿�������üĴ�����һ */
            PORT->PUEL &= ~(1<<GPIO_Pin);       /* �����ڲ�����*/
          break;
      }
#endif
      
    /* ����GPIO����ߵ������� */
    if(GPIO_PinConfig == GPIO_PinOutput_HighCurrent)
    {
#if defined(CPU_NV32M3)
        switch (GPIO_Pin)
        {
            case GPIO_PTB5:
                PORT->HDRVE |= PORT_HDRVE_PTB5_MASK;
                break;
            case GPIO_PTC1:
                PORT->HDRVE |= PORT_HDRVE_PTC1_MASK;
                break;
            case GPIO_PTC5:
                PORT->HDRVE |= PORT_HDRVE_PTC5_MASK;
                break;
            default:
                break;
        }
#endif
    
#if defined(CPU_NV32M4) | defined(CPU_NV32)
        switch (GPIO_Pin)
        {
            case GPIO_PTB4:
                PORT->HDRVE |= PORT_HDRVE_PTB4_MASK;
                break;
            case GPIO_PTB5:
                PORT->HDRVE |= PORT_HDRVE_PTB5_MASK;
                break;
            case GPIO_PTD0:
                PORT->HDRVE |= PORT_HDRVE_PTD0_MASK;
                break;
            case GPIO_PTD1:
                PORT->HDRVE |= PORT_HDRVE_PTD1_MASK;
                break;
            case GPIO_PTE0:
                PORT->HDRVE |= PORT_HDRVE_PTE0_MASK;
                break;
            case GPIO_PTE1:
                PORT->HDRVE |= PORT_HDRVE_PTE1_MASK;
                break;
            case GPIO_PTH0:
                PORT->HDRVE |= PORT_HDRVE_PTH0_MASK;
                break;
            case GPIO_PTH1:
                PORT->HDRVE |= PORT_HDRVE_PTH1_MASK;
                break;
            default:
                break; 
        }
#endif
    }
}

/*****************************************************************************//*!
* @brief �л�GPIO�˿��������----ͨ�������GPIO������ȷ��Ҫ�л����������
*        
* @param[in] GPIO_Pin        GPIO������ (GPIO_PTA0�GPIO_PTA1..............)
*
* @return   none
*
*****************************************************************************/
void GPIO_PinToggle(GPIO_PinType GPIO_Pin)
{
    /* Sanity check */
    ASSERT(GPIO_Pin <= GPIO_PTI7);

    if (GPIO_Pin < GPIO_PTE0)
    {
        /* PTA0-7, PTB0-7, PTC0-7, PTD0-7 */
        GPIOA->PTOR = (1<<GPIO_Pin);
    }

#if (defined(CPU_NV32) | defined(CPU_NV32M4))

    else if (GPIO_Pin < GPIO_PTI0)
    {
        /* PTE0-7, PTF0-7, PTH0-7, PTI0-7 */
        GPIO_Pin = (GPIO_PinType)(GPIO_Pin - GPIO_PTE0);
        GPIOB->PTOR = (1<<GPIO_Pin);
    }
#endif

#if defined(CPU_NV32M4)  
    else if(GPIO_Pin < GPIO_PIN_MAX)
    {
        /* PTI0-7 */
        GPIO_Pin = (GPIO_PinType)(GPIO_Pin - GPIO_PTI0);
        GPIOC->PTOR = (1<<GPIO_Pin);
    }
#endif
}

/*****************************************************************************//*!
* @brief  GPIO�˿����������1һͨ�������GPIO������ȷ�����Ҫ��1������
*        
* @param[in] GPIO_Pin       GPIO������ (GPIO_PTA0�GPIO_PTA1..............)
*
* @return   none
*
*****************************************************************************/
void GPIO_PinSet(GPIO_PinType GPIO_Pin)
{
    /* Sanity check */
    ASSERT(GPIO_Pin <= GPIO_PTI7);
    
    if (GPIO_Pin < GPIO_PTE0)
    {
        /* PTA0-7, PTB0-7, PTC0-7, PTD0-7 */
        GPIOA->PSOR = (1<<GPIO_Pin);
    }

#if (defined(CPU_NV32) | defined(CPU_NV32M4))

    else if (GPIO_Pin < GPIO_PTI0)
    {
        /* PTE0-7, PTF0-7, PTH0-7, PTI0-7 */
        GPIO_Pin = (GPIO_PinType)(GPIO_Pin - GPIO_PTE0);
        GPIOB->PSOR = (1<<GPIO_Pin);
    }
#endif

#if defined(CPU_NV32M4) 
    else if(GPIO_Pin < GPIO_PIN_MAX)
    {
        /* PTI0-7 */
        GPIO_Pin = (GPIO_PinType)(GPIO_Pin - GPIO_PTI0);
        GPIOC->PSOR = (1<<GPIO_Pin);
    }
#endif
}

/*****************************************************************************//*!
* @brief    GPIO�˿�����������㡪��ͨ�������GPIO������ȷ��Ҫ������������
*        
* @param[in] GPIO_Pin        GPIO������ (GPIO_PTA0�GPIO_PTA1..............)
*
* @return   none
*
* @ Pass/ Fail criteria: none
*****************************************************************************/
void GPIO_PinClear(GPIO_PinType GPIO_Pin)
{
    /* Sanity check */
    ASSERT(GPIO_Pin <= GPIO_PTI7);
    
    if (GPIO_Pin < GPIO_PTE0)
    {
        /* PTA0-7, PTB0-7, PTC0-7, PTD0-7 */
        GPIOA->PCOR = (1<<GPIO_Pin);
    }

#if (defined(CPU_NV32) | defined(CPU_NV32M4))

    else if (GPIO_Pin < GPIO_PTI0)
    {
        /* PTE0-7, PTF0-7, PTH0-7, PTI0-7 */
        GPIO_Pin = (GPIO_PinType)(GPIO_Pin - GPIO_PTE0);
        GPIOB->PCOR = (1<<GPIO_Pin);
    }
#endif

#if defined(CPU_NV32M4)
    else if(GPIO_Pin < GPIO_PIN_MAX)
    {
        /* PTI0-7 */
        GPIO_Pin = (GPIO_PinType)(GPIO_Pin - GPIO_PTI0);
        GPIOC->PCOR = (1<<GPIO_Pin);
    }
#endif
}


