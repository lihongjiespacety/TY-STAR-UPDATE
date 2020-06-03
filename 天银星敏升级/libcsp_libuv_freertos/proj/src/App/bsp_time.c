#include "bsp_time.h"
#include <stdint.h>
#include "stm32f10x.h"
#include "misc.h"
#include <stdint.h>
static bsp_time_callback s_time_callback_pfun = 0;
/*
frq 定时器频率 hz
period中断周期 us

*/
void bsp_time6_init(uint32_t frq,uint32_t period)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
  TIM6->CR1 = 0x04;  /*ARR寄存器不缓存 写立即生效 持续模式  只有溢出才中断  使能更新事件*/
  //TIM6->CR2
  TIM6->DIER = 0x01;  /*使能更新事件中断*/
  //TIM6->SR
  TIM6->CNT = 0;      /*向上计数器  更新事件时重载ARR寄存器的值  所以计数周期是ARR-0xFFFF*/
  TIM6->PSC = RCC_Clocks.PCLK1_Frequency/frq - 1;      /*定时器频率 = fCK_PSC/(PSC[15:0] + 1)  该寄存器值在事件更新时 写入到实际工作的PSR寄存器*/
  TIM6->ARR = (uint16_t)((uint32_t)65536 - (period*1000000)/frq);      /**/
  TIM6->EGR |= 0x01;  /*手动产生更新事件 更新寄存器 由于CR1只使能了溢出中断 所以这里不会中断*/
  
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=14 ;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);  
  
  TIM6->CR1 |= 0x01;  /*启动定时器*/
  
}

void bsp_time6_setcallback(bsp_time_callback callbackfun)
{
  s_time_callback_pfun = callbackfun;

}

void TIM6_IRQHandler(void)
{
  if(TIM6->SR & 0x01)
  {
    TIM6->SR = 0x00;
    if(s_time_callback_pfun != 0)
    {
      s_time_callback_pfun();
    }
  }
}