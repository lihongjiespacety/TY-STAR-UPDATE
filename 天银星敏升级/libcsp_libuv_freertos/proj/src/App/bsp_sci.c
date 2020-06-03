#include "hal_uart.h"
#include "bsp_sci.h"
#include <stdint.h>
#include "stm32f10x.h"
#include "misc.h"
#define SCI_TIMEOUT 30000


#define CR1_OVER8_Set             ((u16)0x8000)  /* USART OVER8 mode Enable Mask */

/*
    SCI配置表
*/
HAL_UART_CFG_t Sci_Cfg[]={
    {HAL_UART_0,HAL_UART_DATA_8,HAL_UART_PARITY_NONE,HAL_UART_STOPB_1,HAL_UART_BAUD_9600},
    {HAL_UART_1,HAL_UART_DATA_8,HAL_UART_PARITY_NONE,HAL_UART_STOPB_1,HAL_UART_BAUD_9600},
    {HAL_UART_2,HAL_UART_DATA_8,HAL_UART_PARITY_NONE,HAL_UART_STOPB_1,HAL_UART_BAUD_9600},
    {HAL_UART_3,HAL_UART_DATA_8,HAL_UART_PARITY_NONE,HAL_UART_STOPB_1,HAL_UART_BAUD_9600},
    {HAL_UART_4,HAL_UART_DATA_8,HAL_UART_PARITY_NONE,HAL_UART_STOPB_1,HAL_UART_BAUD_9600},
    {HAL_UART_5,HAL_UART_DATA_8,HAL_UART_PARITY_NONE,HAL_UART_STOPB_1,HAL_UART_BAUD_9600},
};

int BSP_SCI_SetCfg(HAL_UART_ID_e id,HAL_UART_CFG_t* cfg)
{
    uint32_t tmpreg=0x00,apbclock = 0x00;
    uint32_t integerdivider = 0x00;
    uint32_t fractionaldivider = 0x00;
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);
    switch(id)
    {
    case HAL_UART_1:
        //BSP_SCI_Init(HAL_UART_1);
        /*1:配置参数前先关闭发送与接收*/
        /*TE=0 RE=0才允许配置一下寄存器*/
        /*2:配置参数*/
        /*2.1)设置模式为异步模式*/
        /*2.2)设置帧数据长度*/
        USART1->CR1 |= (uint16_t)1<<13;  /*使能模块*/
        switch(cfg->datalen)
        {
            case HAL_UART_DATA_9:
              USART1->CR1 |= (uint16_t)1<<12;
            break;
            default:
              USART1->CR1 &= ~(uint16_t)((uint16_t)1<<12);
            break;
        }
        /*2.3)设置校验位*/
        switch(cfg->parity)
        {
            case HAL_UART_PARITY_EVEN:
               USART1->CR1 &= ~(uint16_t)(1<<10);
               USART1->CR1 &= ~(uint16_t)(1<<9);
            break;
            case HAL_UART_PARITY_ODD:
               USART1->CR1 &= ~(uint16_t)(1<<10);
               USART1->CR1 |= 1<<9;
            break;
            default:
               USART1->CR1 &= ~(uint16_t)(1<<10);
            break;
        }
        /*2.4)设置停止位*/
        switch(cfg->stopb)
        {
            case HAL_UART_STOPB_1:
            USART1->CR2 =  USART1->CR2 & ~((uint16_t)3<<12) | ((uint16_t)0<<12);
            break;
            case HAL_UART_STOPB_2:
            USART1->CR2 =  USART1->CR2 & ~((uint16_t)3<<12) | ((uint16_t)2<<12);
            break;
            default:
            USART1->CR2 =  USART1->CR2 & ~((uint16_t)3<<12) | ((uint16_t)0<<12);
            break;
        }
         /*2.5)设置其他参数:多机处理,时钟分频等*/
        /*3:配置波特率*/
        //USART1 -> BRR = ((RCC_Clocks.PCLK2_Frequency/(cfg->baud*16))<<4)  | ((RCC_Clocks.PCLK2_Frequency /cfg->baud) & (uint16_t)0x0F);
        
        apbclock = RCC_Clocks.PCLK2_Frequency;
        if ((USART1->CR1 & CR1_OVER8_Set) != 0)
        {
          integerdivider = ((25 * apbclock) / (2 * (cfg->baud)));    
        }
        else 
        {
          integerdivider = ((25 * apbclock) / (4 * (cfg->baud)));    
        }
        tmpreg = (integerdivider / 100) << 4;
        
        fractionaldivider = integerdivider - (100 * (tmpreg >> 4));
    
        if ((USART1->CR1 & CR1_OVER8_Set) != 0)
        {
          tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
        }
        else 
        {
          tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
        }
        USART1->BRR = (uint16_t)tmpreg;
  
        /*4:配置中断*/
                        
        break;
    case HAL_UART_2:
        //BSP_SCI_Init(HAL_UART_2);
        /*1:配置参数前先关闭发送与接收*/
        /*TE=0 RE=0才允许配置一下寄存器*/
        /*2:配置参数*/
        /*2.1)设置模式为异步模式*/
        /*2.2)设置帧数据长度*/
        USART2->CR1 |= (uint16_t)1<<13;  /*使能模块*/
        switch(cfg->datalen)
        {
            case HAL_UART_DATA_9:
              USART2->CR1 |= (uint16_t)1<<12;
            break;
            default:
              USART2->CR1 &= ~(uint16_t)((uint16_t)1<<12);
            break;
        }
        /*2.3)设置校验位*/
        switch(cfg->parity)
        {
            case HAL_UART_PARITY_EVEN:
               USART2->CR1 &= ~(uint16_t)(1<<10);
               USART2->CR1 &= ~(uint16_t)(1<<9);
            break;
            case HAL_UART_PARITY_ODD:
               USART2->CR1 &= ~(uint16_t)(1<<10);
               USART2->CR1 |= 1<<9;
            break;
            default:
               USART2->CR1 &= ~(uint16_t)(1<<10);
            break;
        }
        /*2.4)设置停止位*/
        switch(cfg->stopb)
        {
            case HAL_UART_STOPB_1:
            USART2->CR2 =  USART2->CR2 & ~((uint16_t)3<<12) | ((uint16_t)0<<12);
            break;
            case HAL_UART_STOPB_2:
            USART2->CR2 =  USART2->CR2 & ~((uint16_t)3<<12) | ((uint16_t)2<<12);
            break;
            default:
            USART2->CR2 =  USART2->CR2 & ~((uint16_t)3<<12) | ((uint16_t)0<<12);
            break;
        }
         /*2.5)设置其他参数:多机处理,时钟分频等*/
        /*3:配置波特率*/
        //USART2 -> BRR = ((RCC_Clocks.PCLK1_Frequency/(cfg->baud*16))<<4)  | ((RCC_Clocks.PCLK1_Frequency /cfg->baud) & (uint16_t)0x0F);
        
        apbclock = RCC_Clocks.PCLK1_Frequency;
        if ((USART2->CR1 & CR1_OVER8_Set) != 0)
        {
          integerdivider = ((25 * apbclock) / (2 * (cfg->baud)));    
        }
        else 
        {
          integerdivider = ((25 * apbclock) / (4 * (cfg->baud)));    
        }
        tmpreg = (integerdivider / 100) << 4;
        
        fractionaldivider = integerdivider - (100 * (tmpreg >> 4));
    
        if ((USART2->CR1 & CR1_OVER8_Set) != 0)
        {
          tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
        }
        else 
        {
          tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
        }
        USART2->BRR = (uint16_t)tmpreg;
        
        /*4:配置中断*/
        break;
    case HAL_UART_3:
        //BSP_SCI_Init(HAL_UART_3);
        /*1:配置参数前先关闭发送与接收*/
        /*TE=0 RE=0才允许配置一下寄存器*/
        /*2:配置参数*/
        /*2.1)设置模式为异步模式*/
        /*2.2)设置帧数据长度*/
        USART3->CR1 |= (uint16_t)1<<13;  /*使能模块*/
        switch(cfg->datalen)
        {
            case HAL_UART_DATA_9:
              USART3->CR1 |= (uint16_t)1<<12;
            break;
            default:
              USART3->CR1 &= ~(uint16_t)((uint16_t)1<<12);
            break;
        }
        /*2.3)设置校验位*/
        switch(cfg->parity)
        {
            case HAL_UART_PARITY_EVEN:
               USART3->CR1 &= ~(uint16_t)(1<<10);
               USART3->CR1 &= ~(uint16_t)(1<<9);
            break;
            case HAL_UART_PARITY_ODD:
               USART3->CR1 &= ~(uint16_t)(1<<10);
               USART3->CR1 |= 1<<9;
            break;
            default:
               USART3->CR1 &= ~(uint16_t)(1<<10);
            break;
        }
        /*2.4)设置停止位*/
        switch(cfg->stopb)
        {
            case HAL_UART_STOPB_1:
            USART3->CR2 =  USART3->CR2 & ~((uint16_t)3<<12) | ((uint16_t)0<<12);
            break;
            case HAL_UART_STOPB_2:
            USART3->CR2 =  USART3->CR2 & ~((uint16_t)3<<12) | ((uint16_t)2<<12);
            break;
            default:
            USART3->CR2 =  USART3->CR2 & ~((uint16_t)3<<12) | ((uint16_t)0<<12);
            break;
        }
         /*2.5)设置其他参数:多机处理,时钟分频等*/
        /*3:配置波特率*/
        //USART3 -> BRR = ((RCC_Clocks.PCLK1_Frequency/(cfg->baud*16))<<4)  | ((RCC_Clocks.PCLK1_Frequency /cfg->baud) & (uint16_t)0x0F);
        
        apbclock = RCC_Clocks.PCLK1_Frequency;
        if ((USART3->CR1 & CR1_OVER8_Set) != 0)
        {
          integerdivider = ((25 * apbclock) / (2 * (cfg->baud)));    
        }
        else 
        {
          integerdivider = ((25 * apbclock) / (4 * (cfg->baud)));    
        }
        tmpreg = (integerdivider / 100) << 4;
        
        fractionaldivider = integerdivider - (100 * (tmpreg >> 4));
    
        if ((USART3->CR1 & CR1_OVER8_Set) != 0)
        {
          tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
        }
        else 
        {
          tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
        }
        USART3->BRR = (uint16_t)tmpreg;
        /*4:配置中断*/
        break;
    case HAL_UART_4:
        //BSP_SCI_Init(HAL_UART_4);
        /*1:配置参数前先关闭发送与接收*/
        /*TE=0 RE=0才允许配置一下寄存器*/
        /*2:配置参数*/
        /*2.1)设置模式为异步模式*/
        /*2.2)设置帧数据长度*/
        UART4->CR1 |= (uint16_t)1<<13;  /*使能模块*/
        switch(cfg->datalen)
        {
            case HAL_UART_DATA_9:
              UART4->CR1 |= (uint16_t)1<<12;
            break;
            default:
              UART4->CR1 &= ~(uint16_t)((uint16_t)1<<12);
            break;
        }
        /*2.3)设置校验位*/
        switch(cfg->parity)
        {
            case HAL_UART_PARITY_EVEN:
               UART4->CR1 &= ~(uint16_t)(1<<10);
               UART4->CR1 &= ~(uint16_t)(1<<9);
            break;
            case HAL_UART_PARITY_ODD:
               UART4->CR1 &= ~(uint16_t)(1<<10);
               UART4->CR1 |= 1<<9;
            break;
            default:
               UART4->CR1 &= ~(uint16_t)(1<<10);
            break;
        }
        /*2.4)设置停止位*/
        switch(cfg->stopb)
        {
            case HAL_UART_STOPB_1:
            UART4->CR2 =  UART4->CR2 & ~((uint16_t)3<<12) | ((uint16_t)0<<12);
            break;
            case HAL_UART_STOPB_2:
            UART4->CR2 =  UART4->CR2 & ~((uint16_t)3<<12) | ((uint16_t)2<<12);
            break;
            default:
            UART4->CR2 =  UART4->CR2 & ~((uint16_t)3<<12) | ((uint16_t)0<<12);
            break;
        }
         /*2.5)设置其他参数:多机处理,时钟分频等*/
        /*3:配置波特率*/
        //UART4 -> BRR = ((RCC_Clocks.PCLK1_Frequency/(cfg->baud*16))<<4)  | ((RCC_Clocks.PCLK1_Frequency /cfg->baud) & (uint16_t)0x0F);
        
        apbclock = RCC_Clocks.PCLK1_Frequency;
        if ((UART4->CR1 & CR1_OVER8_Set) != 0)
        {
          integerdivider = ((25 * apbclock) / (2 * (cfg->baud)));    
        }
        else 
        {
          integerdivider = ((25 * apbclock) / (4 * (cfg->baud)));    
        }
        tmpreg = (integerdivider / 100) << 4;
        
        fractionaldivider = integerdivider - (100 * (tmpreg >> 4));
    
        if ((UART4->CR1 & CR1_OVER8_Set) != 0)
        {
          tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
        }
        else 
        {
          tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
        }
        UART4->BRR = (uint16_t)tmpreg;
        /*4:配置中断*/
        break;
    case HAL_UART_5:
        //BSP_SCI_Init(HAL_UART_5);
        /*1:配置参数前先关闭发送与接收*/
        /*TE=0 RE=0才允许配置一下寄存器*/
        /*2:配置参数*/
        /*2.1)设置模式为异步模式*/
        /*2.2)设置帧数据长度*/
        UART5->CR1 |= (uint16_t)1<<13;  /*使能模块*/
        switch(cfg->datalen)
        {
            case HAL_UART_DATA_9:
              UART5->CR1 |= (uint16_t)1<<12;
            break;
            default:
              UART5->CR1 &= ~(uint16_t)((uint16_t)1<<12);
            break;
        }
        /*2.3)设置校验位*/
        switch(cfg->parity)
        {
            case HAL_UART_PARITY_EVEN:
               UART5->CR1 &= ~(uint16_t)(1<<10);
               UART5->CR1 &= ~(uint16_t)(1<<9);
            break;
            case HAL_UART_PARITY_ODD:
               UART5->CR1 &= ~(uint16_t)(1<<10);
               UART5->CR1 |= 1<<9;
            break;
            default:
               UART5->CR1 &= ~(uint16_t)(1<<10);
            break;
        }
        /*2.4)设置停止位*/
        switch(cfg->stopb)
        {
            case HAL_UART_STOPB_1:
            UART5->CR2 =  UART5->CR2 & ~((uint16_t)3<<12) | ((uint16_t)0<<12);
            break;
            case HAL_UART_STOPB_2:
            UART5->CR2 =  UART5->CR2 & ~((uint16_t)3<<12) | ((uint16_t)2<<12);
            break;
            default:
            UART5->CR2 =  UART5->CR2 & ~((uint16_t)3<<12) | ((uint16_t)0<<12);
            break;
        }
         /*2.5)设置其他参数:多机处理,时钟分频等*/
        /*3:配置波特率*/
        //UART5 -> BRR = ((RCC_Clocks.PCLK1_Frequency/(cfg->baud*16))<<4)  | ((RCC_Clocks.PCLK1_Frequency /cfg->baud) & (uint16_t)0x0F);
        apbclock = RCC_Clocks.PCLK1_Frequency;
        if ((UART5->CR1 & CR1_OVER8_Set) != 0)
        {
          integerdivider = ((25 * apbclock) / (2 * (cfg->baud)));    
        }
        else 
        {
          integerdivider = ((25 * apbclock) / (4 * (cfg->baud)));    
        }
        tmpreg = (integerdivider / 100) << 4;
        
        fractionaldivider = integerdivider - (100 * (tmpreg >> 4));
    
        if ((UART5->CR1 & CR1_OVER8_Set) != 0)
        {
          tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
        }
        else 
        {
          tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
        }
        UART5->BRR = (uint16_t)tmpreg;
        /*4:配置中断*/
        break;
        default:
        return -1;
        break;
    }
    return 0;
}

/*

 */
int BSP_SCI_Init(HAL_UART_ID_e id)
{
    int ret=0;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    switch(id)
    {
    case HAL_UART_1:
      /*使能时钟*/
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
      
      /*引脚 中断*/
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA9
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
      GPIO_Init(GPIOA, &GPIO_InitStructure);
      
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
      GPIO_Init(GPIOA, &GPIO_InitStructure);
  
      NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=14 ;//抢占优先级3
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //子优先级3
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
      NVIC_Init(&NVIC_InitStructure);  
 
        break;
    case HAL_UART_2:
       GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
       
        /*使能时钟*/
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
        
      /*引脚 中断*/
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //PD5
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
      GPIO_Init(GPIOD, &GPIO_InitStructure);
      
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PD6
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
      GPIO_Init(GPIOD, &GPIO_InitStructure);
      
       
//        /*使能时钟*/
//        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
//        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//        
//      /*引脚 中断*/
//      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
//      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
//      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
//      GPIO_Init(GPIOA, &GPIO_InitStructure);
//      
//      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
//      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
//      GPIO_Init(GPIOA, &GPIO_InitStructure);
  
      NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=14 ;//抢占优先级3
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //子优先级3
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
      NVIC_Init(&NVIC_InitStructure);  
      
        break;
    case HAL_UART_3:
        /*使能时钟*/
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        
      /*引脚 中断*/
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
      GPIO_Init(GPIOB, &GPIO_InitStructure);
      
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
      GPIO_Init(GPIOB, &GPIO_InitStructure);
  
      NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=14 ;//抢占优先级3
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //子优先级3
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
      NVIC_Init(&NVIC_InitStructure);  
      
        break;
    case HAL_UART_4:
        /*使能时钟*/
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        
        /*引脚 中断*/
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC10
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
      GPIO_Init(GPIOC, &GPIO_InitStructure);
      
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
      GPIO_Init(GPIOC, &GPIO_InitStructure);
  
      NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=14 ;//抢占优先级3
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //子优先级3
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
      NVIC_Init(&NVIC_InitStructure);  
      
        break;
    case HAL_UART_5:
        /*使能时钟*/
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
        
       /*引脚 中断*/
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC12
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
      GPIO_Init(GPIOC, &GPIO_InitStructure);
      
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PD2
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
      GPIO_Init(GPIOD, &GPIO_InitStructure);
  
      NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=14 ;//抢占优先级3
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //子优先级3
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
      NVIC_Init(&NVIC_InitStructure);  
        break;

    default:
        ret = -1;
        break;
    }
    return ret;
}

int BSP_SCI_Send(HAL_UART_ID_e id,unsigned char byte)
{
    switch(Sci_Cfg[id].id)
    {
    case HAL_UART_1:
        USART1->DR = byte;
        break;
    case HAL_UART_2:
        USART2->DR = byte;
        break;
    case HAL_UART_3:
        USART3->DR = byte;
        break;
    case HAL_UART_4:
         UART4->DR = byte;
        break;
    case HAL_UART_5:
        UART5->DR = byte;
        break;
    default:
        return -1;
        break;
    }
    return 0;
}

int BSP_SCI_CheckSend(HAL_UART_ID_e id,unsigned char byte)
{
    int timeout = SCI_TIMEOUT;
    switch(Sci_Cfg[id].id)
    {
    case HAL_UART_1:
        timeout = SCI_TIMEOUT;
        while(((USART1->SR & (1u<<6))==0) && (timeout-->0));
        if(timeout<=0)
        {
          return -1;
        }
        USART1->DR = byte;
        break;
    case HAL_UART_2:
        timeout = SCI_TIMEOUT;
        while(((USART2->SR & (1u<<6))==0) && (timeout-->0));
        if(timeout<=0)
        {
          return -1;
        }
        USART2->DR = byte;
        break;
    case HAL_UART_3:
        timeout = SCI_TIMEOUT;
        while(((USART3->SR & (1u<<6))==0) && (timeout-->0));
        if(timeout<=0)
        {
          return -1;
        }
        USART3->DR = byte;
        break;
    case HAL_UART_4:
        timeout = SCI_TIMEOUT;
        while(((UART4->SR & (1u<<6))==0) && (timeout-->0));
        if(timeout<=0)
        {
          return -1;
        }
        UART4->DR = byte;
        break;
    case HAL_UART_5:
        timeout = SCI_TIMEOUT;
        while(((UART5->SR & (1u<<6))==0) && (timeout-->0));
        if(timeout<=0)
        {
          return -1;
        }
        UART5->DR = byte;
        break;
    default:
        return -1;
        break;
    }
    return 0;
}

int BSP_SCI_CheckRead(HAL_UART_ID_e id,unsigned char* byte)
{
    int timeout = SCI_TIMEOUT;
    switch(Sci_Cfg[id].id)
    {
    case HAL_UART_1:
        timeout = SCI_TIMEOUT;
        while((!(USART1->SR & (1u<<5))) && (timeout-->0));
        *byte = USART1->DR;
        if(timeout<=0)
        {
          return -1;
        }
        break;
    case HAL_UART_2:
        timeout = SCI_TIMEOUT;
        while((!(USART2->SR & (1u<<5))) && (timeout-->0));
        *byte = USART2->DR;
        if(timeout<=0)
        {
          return -1;
        }
        break;
    case HAL_UART_3:
        timeout = SCI_TIMEOUT;
        while((!(USART3->SR & (1u<<5))) && (timeout-->0));
        *byte = USART3->DR;
        if(timeout<=0)
        {
          return -1;
        }
        break;
    case HAL_UART_4:
        timeout = SCI_TIMEOUT;
        while((!(UART4->SR & (1u<<5))) && (timeout-->0));
        *byte = UART4->DR;
        if(timeout<=0)
        {
          return -1;
        }
        break;
    case HAL_UART_5:
        timeout = SCI_TIMEOUT;
        while((!(UART5->SR & (1u<<5))) && (timeout-->0));
        *byte = UART5->DR;
        if(timeout<=0)
        {
          return -1;
        }
        break;
    default:
        return -1;
        break;
    }
    return 0;
}

int BSP_SCI_Read(HAL_UART_ID_e id,unsigned char* byte)
{
    switch(Sci_Cfg[id].id)
    {
    case HAL_UART_1:
        *byte = USART1->DR;
        break;
    case HAL_UART_2:
        *byte = USART2->DR;
        break;
    case HAL_UART_3:
        *byte = USART3->DR;
        return 0;
        break;
    case HAL_UART_4:
        *byte = UART4->DR;
        break;
    case HAL_UART_5:
        *byte = UART5->DR;
        break;
    default:
        return -1;
        break;
    }
    return 0;
}


HAL_UART_ISRCALLBACK_t SCI_CallBackLists[]=
{
    {HAL_UART_0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0},
    {HAL_UART_1,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0},
    {HAL_UART_2,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0},
    {HAL_UART_3,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0},
    {HAL_UART_4,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0},
    {HAL_UART_5,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0},
    {HAL_UART_6,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0},
    {HAL_UART_7,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0,(HAL_UART_ISRCallBack)0},
};




//////////////////////////////中断函数/////////////////////////////////////////////

void USART1_IRQHandler(void)
{
  uint8_t status = (uint8_t)0;
  uint8_t param = (uint8_t)0;
  status = USART1->SR;
  if(status & (uint8_t)0x0F)
  {
    /*1.获取错误状态并清除标志*/
    if(status & (uint8_t)0x01)
    {
      param |= HAL_UART_ERR_PER;
    }
    if(status & (uint8_t)0x02)
    {
      param |= HAL_UART_ERR_FER;
    }
    if(status & (uint8_t)0x08)
    {
      param |= HAL_UART_ERR_ORER;
    }
    /*2.调用相应的回调函数*/ 
    /*为了减少处理,固定端口索引*/
    if(SCI_CallBackLists[1].rxerrfun != 0)
    {
      SCI_CallBackLists[1].rxerrfun(&param);
    }
    /*3.清除中断标志*/   
    status = USART1->DR;  /*读SR后读DR清除标志*/
  }
  
  if(status & (uint8_t)0x20)
  {
    param  = USART1->DR;
    if(SCI_CallBackLists[1].rxfun != 0)
    {
      SCI_CallBackLists[1].rxfun(&param);
    }
  }
  
//  if(status & (uint8_t)0x80)
//  {
//    if(SCI_CallBackLists[3].txfun != 0)
//    {
//      if(SCI_CallBackLists[3].txfun(&param) == 0)
//      {
//        USART1->DR = param;
//      }
//    }
//  }
  
  if(status & (uint8_t)0x40)
  {
    if(SCI_CallBackLists[1].txcmpfun != 0)
    {
      if(SCI_CallBackLists[1].txcmpfun(&param) == 0)
      {
        USART1->DR = param;
      }
      else
      {
        USART1->CR1 &= ~((uint8_t)(1<<3) | (uint8_t)(1<<6));
      }
    }
  }
}


void USART2_IRQHandler(void)
{
  uint8_t status = (uint8_t)0;
  uint8_t param = (uint8_t)0;
  status = USART2->SR;
  if(status & (uint8_t)0x0F)
  {
    /*1.获取错误状态并清除标志*/
    if(status & (uint8_t)0x01)
    {
      param |= HAL_UART_ERR_PER;
    }
    if(status & (uint8_t)0x02)
    {
      param |= HAL_UART_ERR_FER;
    }
    if(status & (uint8_t)0x08)
    {
      param |= HAL_UART_ERR_ORER;
    }
    /*2.调用相应的回调函数*/ 
    /*为了减少处理,固定端口索引*/
    if(SCI_CallBackLists[2].rxerrfun != 0)
    {
      SCI_CallBackLists[2].rxerrfun(&param);
    }
    /*3.清除中断标志*/   
    status = USART2->DR;  /*读SR后读DR清除标志*/
  }
  
  if(status & (uint8_t)0x20)
  {
    param  = USART2->DR;
    if(SCI_CallBackLists[2].rxfun != 0)
    {
      SCI_CallBackLists[2].rxfun(&param);
    }
  }
  
//  if(status & (uint8_t)0x80)
//  {
//    if(SCI_CallBackLists[2].txfun != 0)
//    {
//      if(SCI_CallBackLists[2].txfun(&param) == 0)
//      {
//        USART2->DR = param;
//      }
//    }
//  }
  
  if(status & (uint8_t)0x40)
  {
    if(SCI_CallBackLists[2].txcmpfun != 0)
    {
      if(SCI_CallBackLists[2].txcmpfun(&param) == 0)
      {
        USART2->DR = param;
      }
      else
      {
        USART2->CR1 &= ~((uint8_t)(1<<3) | (uint8_t)(1<<6));
      }
    }
  }
}

void USART3_IRQHandler(void)
{
  uint8_t status = (uint8_t)0;
  uint8_t param = (uint8_t)0;
  status = USART3->SR;
  if(status & (uint8_t)0x0F)
  {
    /*1.获取错误状态并清除标志*/
    if(status & (uint8_t)0x01)
    {
      param |= HAL_UART_ERR_PER;
    }
    if(status & (uint8_t)0x02)
    {
      param |= HAL_UART_ERR_FER;
    }
    if(status & (uint8_t)0x08)
    {
      param |= HAL_UART_ERR_ORER;
    }
    /*2.调用相应的回调函数*/ 
    /*为了减少处理,固定端口索引*/
    if(SCI_CallBackLists[3].rxerrfun != 0)
    {
      SCI_CallBackLists[3].rxerrfun(&param);
    }
    /*3.清除中断标志*/   
    status = USART3->DR;  /*读SR后读DR清除标志*/
  }
  
  if(status & (uint8_t)0x20)
  {
    param  = USART3->DR;
    if(SCI_CallBackLists[3].rxfun != 0)
    {
      SCI_CallBackLists[3].rxfun(&param);
    }
  }
  
//  if(status & (uint8_t)0x80)
//  {
//    if(SCI_CallBackLists[3].txfun != 0)
//    {
//      if(SCI_CallBackLists[3].txfun(&param) == 0)
//      {
//        USART3->DR = param;
//      }
//    }
//  }
  
  if(status & (uint8_t)0x40)
  {
    if(SCI_CallBackLists[3].txcmpfun != 0)
    {
      if(SCI_CallBackLists[3].txcmpfun(&param) == 0)
      {
        USART3->DR = param;
      }
      else
      {
        USART3->CR1 &= ~((uint8_t)(1<<3) | (uint8_t)(1<<6));
      }
    }
  }
  
}

void UART4_IRQHandler(void)
{
  uint8_t status = (uint8_t)0;
  uint8_t param = (uint8_t)0;
  status = UART4->SR;
  if(status & (uint8_t)0x0F)
  {
    /*1.获取错误状态并清除标志*/
    if(status & (uint8_t)0x01)
    {
      param |= HAL_UART_ERR_PER;
    }
    if(status & (uint8_t)0x02)
    {
      param |= HAL_UART_ERR_FER;
    }
    if(status & (uint8_t)0x08)
    {
      param |= HAL_UART_ERR_ORER;
    }
    /*2.调用相应的回调函数*/ 
    /*为了减少处理,固定端口索引*/
    if(SCI_CallBackLists[4].rxerrfun != 0)
    {
      SCI_CallBackLists[4].rxerrfun(&param);
    }
    /*3.清除中断标志*/   
    status = UART4->DR;  /*读SR后读DR清除标志*/
  }
  
  if(status & (uint8_t)0x20)
  {
    param  = UART4->DR;
    if(SCI_CallBackLists[4].rxfun != 0)
    {
      SCI_CallBackLists[4].rxfun(&param);
    }
  }
  
//  if(status & (uint8_t)0x80)
//  {
//    if(SCI_CallBackLists[4].txfun != 0)
//    {
//      if(SCI_CallBackLists[4].txfun(&param) == 0)
//      {
//        UART4->DR = param;
//      }
//    }
//  }
  
  if(status & (uint8_t)0x40)
  {
    if(SCI_CallBackLists[4].txcmpfun != 0)
    {
      if(SCI_CallBackLists[4].txcmpfun(&param) == 0)
      {
        UART4->DR = param;
      }
      else
      {
        UART4->CR1 &= ~((uint8_t)(1<<3) | (uint8_t)(1<<6));
      }
    }
  }
  
}


void UART5_IRQHandler(void)
{
  uint8_t status = (uint8_t)0;
  uint8_t param = (uint8_t)0;
  status = UART5->SR;
  if(status & (uint8_t)0x0F)
  {
    /*1.获取错误状态并清除标志*/
    if(status & (uint8_t)0x01)
    {
      param |= HAL_UART_ERR_PER;
    }
    if(status & (uint8_t)0x02)
    {
      param |= HAL_UART_ERR_FER;
    }
    if(status & (uint8_t)0x08)
    {
      param |= HAL_UART_ERR_ORER;
    }
    /*2.调用相应的回调函数*/ 
    /*为了减少处理,固定端口索引*/
    if(SCI_CallBackLists[5].rxerrfun != 0)
    {
      SCI_CallBackLists[5].rxerrfun(&param);
    }
    /*3.清除中断标志*/   
    status = UART5->DR;  /*读SR后读DR清除标志*/
  }
  
  if(status & (uint8_t)0x20)
  {
    param  = UART5->DR;
    if(SCI_CallBackLists[5].rxfun != 0)
    {
      SCI_CallBackLists[5].rxfun(&param);
    }
  }
  
//  if(status & (uint8_t)0x80)
//  {
//    if(SCI_CallBackLists[5].txfun != 0)
//    {
//      if(SCI_CallBackLists[5].txfun(&param) == 0)
//      {
//        UART5->DR = param;
//      }
//    }
//  }
  
  if(status & (uint8_t)0x40)
  {
    if(SCI_CallBackLists[5].txcmpfun != 0)
    {
      if(SCI_CallBackLists[5].txcmpfun(&param) == 0)
      {
        UART5->DR = param;
      }
      else
      {
        UART5->CR1 &= ~((uint8_t)(1<<3) | (uint8_t)(1<<6));
      }
    }
  }
  
}


//设置发送中断回调函数
int BSP_SCI_SetTxCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun)
{
    if(id>7)
    {
        return -1;
    }
    SCI_CallBackLists[id].txfun = callbackfun;
    return 0;
}

//设置发送完中断回调函数
int BSP_SCI_SetTxCompleteCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun)
{
    if(id>7)
    {
        return -1;
    }
    SCI_CallBackLists[id].txcmpfun = callbackfun;
    return 0;
}

//设置接收中断回调函数
int BSP_SCI_SetRxCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun)
{
    if(id>7)
    {
        return -1;
    }
    SCI_CallBackLists[id].rxfun = callbackfun;
    return 0;
}

//设置接收错误中断回调函数
int BSP_SCI_SetRxErrCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun)
{
    if(id>7)
    {
        return -1;
    }
    SCI_CallBackLists[id].rxerrfun = callbackfun;
    return 0;
}

int BSP_SCI_EnableTx(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable)
{
    switch(id)
    {
        case HAL_UART_1:
            if(HAL_UART_ENABLE == enable)
            {
                switch(type)
                {
                case HAL_UART_INTERRUPT:
                  USART1->CR1 |= (uint8_t)(1<<3) | (uint8_t)(1<<6);
                    break;
                case HAL_UART_CHECK:
                  USART1->CR1 |= (1<<3);
                    break;
                default:
                    return -1;
                    break;  
                }
            }
            else
            {
                 USART1->CR1 &= ~((uint8_t)(1<<3) | (uint8_t)(1<<6));
            }
        break;
        case HAL_UART_2:
            if(HAL_UART_ENABLE == enable)
            {
                switch(type)
                {
                case HAL_UART_INTERRUPT:
                  USART2->CR1 |= (uint8_t)(1<<3) | (uint8_t)(1<<6);
                    break;
                case HAL_UART_CHECK:
                  USART2->CR1 |= (1<<3);
                    break;
                default:
                    return -1;
                    break;  
                }
            }
            else
            {
                 USART2->CR1 &= ~((uint8_t)(1<<3) | (uint8_t)(1<<6));
            }
        break;
        case HAL_UART_3:
            if(HAL_UART_ENABLE == enable)
            {
                switch(type)
                {
                case HAL_UART_INTERRUPT:
                  USART3->CR1 |= (uint8_t)(1<<3) | (uint8_t)(1<<6);
                    break;
                case HAL_UART_CHECK:
                  USART3->CR1 |= (1<<3);
                    break;
                default:
                    return -1;
                    break;  
                }
            }
            else
            {
                 USART3->CR1 &= ~((uint8_t)(1<<3) | (uint8_t)(1<<6));
            }
        break;
        case HAL_UART_4:
            if(HAL_UART_ENABLE == enable)
            {
                switch(type)
                {
                case HAL_UART_INTERRUPT:
                  UART4->CR1 |= (uint8_t)(1<<3) | (uint8_t)(1<<6);
                    break;
                case HAL_UART_CHECK:
                  UART4->CR1 |= (1<<3);
                    break;
                default:
                    return -1;
                    break;  
                }
            }
            else
            {
                 UART4->CR1 &= ~((uint8_t)(1<<3) | (uint8_t)(1<<6));
            }
        break;
        case HAL_UART_5:
            if(HAL_UART_ENABLE == enable)
            {
                switch(type)
                {
                case HAL_UART_INTERRUPT:
                  UART5->CR1 |= (uint8_t)(1<<3) | (uint8_t)(1<<6);
                    break;
                case HAL_UART_CHECK:
                  UART5->CR1 |= (1<<3);
                    break;
                default:
                    return -1;
                    break;  
                }
            }
            else
            {
                 UART5->CR1 &= ~((uint8_t)(1<<3) | (uint8_t)(1<<6));
            }
        break;
        default:
        return -1;
        break;
    }
    return 0;
}

int BSP_SCI_EnableRx(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable)
{
    switch(id)
    {
        case HAL_UART_1:
            if(HAL_UART_ENABLE == enable)
            {
                switch(type)
                {
                case HAL_UART_INTERRUPT:
                    USART1->CR1 |= (uint8_t)(1<<2) | (uint8_t)(1<<5);
                    break;
                case HAL_UART_CHECK:
                    USART1->CR1 |= (1<<2);
                    break;
                default:
                    return -1;
                    break;  
                }
            }
            else
            {
                 USART1->CR1 &= ~((uint8_t)(1<<2) | (uint8_t)(1<<5));
            }
        break;
        case HAL_UART_2:
            if(HAL_UART_ENABLE == enable)
            {
                switch(type)
                {
                case HAL_UART_INTERRUPT:
                    USART2->CR1 |= (uint8_t)(1<<2) | (uint8_t)(1<<5);
                    break;
                case HAL_UART_CHECK:
                    USART2->CR1 |= (1<<2);
                    break;
                default:
                    return -1;
                    break;  
                }
            }
            else
            {
                 USART2->CR1 &= ~((uint8_t)(1<<2) | (uint8_t)(1<<5));
            }
        break;
        case HAL_UART_3:
            if(HAL_UART_ENABLE == enable)
            {
                switch(type)
                {
                case HAL_UART_INTERRUPT:
                    USART3->CR1 |= (uint8_t)(1<<2) | (uint8_t)(1<<5);
                    break;
                case HAL_UART_CHECK:
                    USART3->CR1 |= (1<<2);
                    break;
                default:
                    return -1;
                    break;  
                }
            }
            else
            {
                 USART3->CR1 &= ~((uint8_t)(1<<2) | (uint8_t)(1<<5));
            }
        break;
        case HAL_UART_4:
            if(HAL_UART_ENABLE == enable)
            {
                switch(type)
                {
                case HAL_UART_INTERRUPT:
                    UART4->CR1 |= (uint8_t)(1<<2) | (uint8_t)(1<<5);
                    break;
                case HAL_UART_CHECK:
                    UART4->CR1 |= (1<<2);
                    break;
                default:
                    return -1;
                    break;  
                }
            }
            else
            {
                 UART4->CR1 &= ~((uint8_t)(1<<2) | (uint8_t)(1<<5));
            }
        break;
        case HAL_UART_5:
            if(HAL_UART_ENABLE == enable)
            {
                switch(type)
                {
                case HAL_UART_INTERRUPT:
                    UART5->CR1 |= (uint8_t)(1<<2) | (uint8_t)(1<<5);
                    break;
                case HAL_UART_CHECK:
                    UART5->CR1 |= (1<<2);
                    break;
                default:
                    return -1;
                    break;  
                }
            }
            else
            {
                 UART5->CR1 &= ~((uint8_t)(1<<2) | (uint8_t)(1<<5));
            }
        break;
        default:
        return -1;
        break;
    }
    return 0;
}

int BSP_SCI_EnableRxErr(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable)
{
    switch(id)
    {
        case HAL_UART_0:
            if(HAL_UART_ENABLE == enable)
            {
                 USART1->CR1 |= (uint8_t)(1<<8);
            }
            else
            {
                 USART1->CR1 &= ~((uint8_t)(1<<8));
            }
        break;
        default:
        return -1;
        break;
    }
    return 0;
}

int BSP_SCI_DeInit(HAL_UART_ID_e id)
{
    return 0;
}