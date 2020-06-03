#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stm32f10x.h"
#include "driver.h"
#include "hal.h"
#include "shell.h"
#include "retarget.h"
#include "osapi_freertos.h"
#include "misc.h"
#include "csp_if_can.h"
#include "can.h"

#define UART_TASK_STACK_SIZE (256 )
#define UART_TASK_PRIORITY	  ( tskIDLE_PRIORITY + 4 )

#define SHELL_TASK_STACK_SIZE     ( 256 )
#define SHELL_TASK_PRIORITY	  ( tskIDLE_PRIORITY + 3 )

#define CAN_RXTASK_STACK_SIZE (256 )
#define CAN_RXTASK_PRIORITY	  ( tskIDLE_PRIORITY + 6 )

#define CAN_TXTASK_STACK_SIZE (256 )
#define CAN_TXTASK_PRIORITY	  ( tskIDLE_PRIORITY + 6 )



extern unsigned  prvRxTask(void* params);
extern int kiss_set(int argc); 

uint8_t buff_sum(void* buff,uint8_t len)
{
  uint8_t sum=0;
  uint8_t i;
  uint8_t* p;
  p = buff;
  for(i=0;i<len;i++)
  {
      sum += p[i];
  }
  return sum;
}

uint8_t ver_buff[8]={0x11,0x22,0x33,0x44,0x00,0x00,0x00,0x00};
uint8_t tm_buff[40]=
{0x00,0x25,0x33,0x44,0x00,0x00,0x00,0x00};

static void uarttask( void *pvParameters )
{
  
  int len;
  int erro;
  int res;
  can_id_t id;
  cfp_ext_t type;
  static unsigned char buff[4096+32];
  HAL_UART_CFG_t cfg;
  cfg.id = HAL_UART_4;
  cfg.baud = 250000;
  cfg.datalen = HAL_UART_DATA_8;
  cfg.parity = HAL_UART_PARITY_NONE;
  cfg.stopb = HAL_UART_STOPB_1;
  driver_uart_set(&cfg);
  driver_uart_flush(HAL_UART_4);

  cfg.id = HAL_UART_5;
  cfg.baud = HAL_UART_BAUD_115200;
  cfg.datalen = HAL_UART_DATA_8;
  cfg.parity = HAL_UART_PARITY_NONE;
  cfg.stopb = HAL_UART_STOPB_1;
  driver_uart_set(&cfg);
  driver_uart_flush(HAL_UART_5);
  

//  cfg.id = HAL_UART_2;
//  cfg.baud = HAL_UART_BAUD_38400;
//  cfg.datalen = HAL_UART_DATA_8;
//  cfg.parity = HAL_UART_PARITY_NONE;
//  cfg.stopb = HAL_UART_STOPB_1;
//  driver_uart_set(&cfg);
//  driver_uart_flush(HAL_UART_2);
  memset(buff,0xA5,sizeof(buff));
  ///driver_uart_recv(HAL_UART_4, buff, sizeof(buff), 100, &erro);
  ///driver_uart_recv(HAL_UART_5, buff, sizeof(buff), 100, &erro);
  
  
  
  while(1)
  {
    /*UART4收到数据转发到CAN自定义数据包*/
    do
    {
      len = driver_uart_recv(HAL_UART_4, buff, sizeof(buff), 300, &erro);
      if(len)
      {
        OsPrintf(OSAPI_DEBUG_INFO,"UART4:get frame len=%d\r\n", len);
        if((len == 8) && (buff[0]==0xAA)  && (buff[1]==0x55) && (buff_sum(buff,7) == buff[7]))
        {
          /*aa55a55a 本机地址 目的地址 端口校验和*/
          kiss_set_clientaddr(buff[2]);
          kiss_set_serveraddr(buff[3]);
          kiss_set_serverport(buff[4]);
          
          struct csp_can_config canconf;
          canconf.bitrate = (uint32_t)500000;
          canconf.clock_speed = 0;
          canconf.ifc=0;
          csp_can_init(CSP_CAN_PROMISC, &canconf);
          driver_uart_send(HAL_UART_4, buff, 8, 200, &erro);
        }
        else if((len == 8) && (buff[0]==0xAA)  && (buff[1]==0x66) && (buff_sum(buff,7) == buff[7]))
        {
          buff[2] = (g_can_send_packet>>24)&0xFF;
          buff[3] = (g_can_send_packet>>16)&0xFF;
          buff[4] = (g_can_send_packet>>8)&0xFF;
          buff[5] =  g_can_send_packet & 0xFF;
          
          buff[6] = (g_can_rcv_packet>>24)&0xFF;
          buff[7] = (g_can_rcv_packet>>16)&0xFF;
          buff[8] = (g_can_rcv_packet>>8)&0xFF;
          buff[9] =  g_can_rcv_packet & 0xFF;
          
          buff[10] = (g_can_rcverr_packet>>24)&0xFF;
          buff[11] = (g_can_rcverr_packet>>16)&0xFF;
          buff[12] = (g_can_rcverr_packet>>8)&0xFF;
          buff[13] =  g_can_rcverr_packet & 0xFF;
          driver_uart_send(HAL_UART_4, buff, 14, 200, &erro);
        }
        else
        {
          if(len<=8)
          {
              type = CFP_SINGLE;
              
#if 0  //模拟返回值
              if(buff[0] == 0xD2)
              {
                driver_uart_send(HAL_UART_4, ver_buff, sizeof(ver_buff), 200, &erro);
              }
              if(buff[0] == 0x20)
              {
                tm_buff[39] = sum_buff(tm_buff,39);
                driver_uart_send(HAL_UART_4, tm_buff, sizeof(tm_buff), 200, &erro);
              }
              
#endif
          }
          else
          {
              type = CFP_BEGIN;
          }
          res = can_tx_raw_data(kiss_get_serveraddr(), buff, len,type,1,500);
          ///driver_uart_send(HAL_UART_4, (unsigned char*)&len, 4,1000, &erro);

#if 0
          ver_buff[0] = 0xF5;
          ver_buff[1] = 0x0A;
          ver_buff[2] = 0x11;
          ver_buff[3] = 0x55;
          ver_buff[4] = 0x00;
          ver_buff[5] = 0x00;
          ver_buff[6] = 0x00;
          ver_buff[7] = 0x00;
          driver_uart_send(HAL_UART_4, ver_buff, 8, 200, &erro);    
#endif
          if(res != 0)
          {
            OsPrintf(OSAPI_DEBUG_INFO,"can-tianyi:put frame len=%d err\r\n", len);
          }
          else
          {
            OsPrintf(OSAPI_DEBUG_INFO,"can-tianyi:put frame len=%d ok\r\n", len);
          }
          
        }
      }
    }
    while(len);
    
    //prvRxTask(0);  /*串口接收处理*/
//    do
//    {
//      len = driver_uart_recv(HAL_UART_5, buff, sizeof(buff), 30, &erro);
//      if(len)
//      {
//        driver_uart_send(HAL_UART_5, buff, len, 30, &erro);
//      }
//    }
//    while(len);
    
//    do
//    {
//      len = driver_uart_recv(HAL_UART_2, buff, sizeof(buff), 30, &erro);
//      if(len)
//      {
//        driver_uart_send(HAL_UART_2, buff, len, 30, &erro);
//      }
//    }
//    while(len);
//    OsPrintf(OSAPI_DEBUG_INFO,"%s\r\n","info");
//    OsPrintf(OSAPI_DEBUG_DEBUG,"%s\r\n","debug");
//    OsPrintf(OSAPI_DEBUG_WARM,"%s\r\n","warm");
//    OsPrintf(OSAPI_DEBUG_ERR,"%s\r\n","err");
    
    ///OsTimeDelay(10);
  }
}

static void shelltask( void *pvParameters )
{
  HAL_UART_CFG_t cfg;
  cfg.id = HAL_UART_2;
  cfg.baud = HAL_UART_BAUD_115200;
  cfg.datalen = HAL_UART_DATA_8;
  cfg.parity = HAL_UART_PARITY_NONE;
  cfg.stopb = HAL_UART_STOPB_1;
  driver_uart_set(&cfg);
  driver_uart_flush(HAL_UART_2);
  HAL_UART_EnableTx(HAL_UART_2,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
  HAL_UART_EnableTx(HAL_UART_2,HAL_UART_CHECK,HAL_UART_ENABLE);
  setstdiouart(HAL_UART_2);
  printf("任务2启动\r\n");
  while(1)
  {
    shell_exec_shellcmd();
    OsTimeDelay(5);
  }
}


extern void * can_rx_handle(void * parameters);

static void can_rxtask( void *pvParameters )
{
   //OsTimeDelay(20000);
  kiss_set(0);
  while(1)
  {
    can_rx_handle(0);
    //driver_can_sendloop();
    //OsTimeDelay(2);
  }
}


static void can_txtask( void *pvParameters )
{
  while(1)
  {
    driver_can_sendloop();
    ///OsTimeDelay(1);
  }
}


void csp_debug_hook(csp_color_t color,csp_debug_level_t level, const char *format, va_list args)
{
     OsPrintf_va(color,OSAPI_DEBUG_ERR,format,args);
}


int process_eps_can_request(csp_packet_t* in)
{
    int erro;
    //接收到自定义包通过串口转发
    driver_uart_send(HAL_UART_4, in->data, in->length,2000,&erro);
    OsPrintf(OSAPI_DEBUG_INFO,"can-tianyi:get frame len=%d\r\n", in->length);
    OsPrintf(OSAPI_DEBUG_INFO,"UART4:put frame len=%d\r\n", in->length);
    return 0;

}


int main(void)
{ 
  GPIO_InitTypeDef giocfg;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_DeInit(GPIOB);
  
  giocfg.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
  giocfg.GPIO_Speed = GPIO_Speed_10MHz;
  giocfg.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &giocfg);
  
  giocfg.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_15;
  giocfg.GPIO_Speed = GPIO_Speed_10MHz;
  giocfg.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &giocfg);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  SystemCoreClockUpdate();
  
  csp_debug_hook_set(csp_debug_hook);
  driver_time_init();
  
  xTaskCreate( shelltask, "shell", SHELL_TASK_STACK_SIZE, NULL, SHELL_TASK_PRIORITY, NULL );
  xTaskCreate( uarttask, "uart", UART_TASK_STACK_SIZE, NULL, UART_TASK_PRIORITY, NULL );
  xTaskCreate( can_rxtask, "canrx", CAN_RXTASK_STACK_SIZE, NULL, CAN_RXTASK_PRIORITY, NULL );
  xTaskCreate( can_txtask, "cantx", CAN_TXTASK_STACK_SIZE, NULL, CAN_TXTASK_PRIORITY, NULL );
  vTaskStartScheduler();
  while(1);
}

void assert_failed(uint8_t* file, uint32_t line)
{
  printf("Wrong parameters value: file %s on line %d\r\n", file, line);
  while(1);
}


void hard_fault_handler_c(unsigned int * hardfault_args) 
{  
    static unsigned int stacked_r0;
    static unsigned int stacked_r1;  
    static unsigned int stacked_r2;  
    static unsigned int stacked_r3;  
    static unsigned int stacked_r12;  
    static unsigned int stacked_lr;  
    static unsigned int stacked_pc;  
    static unsigned int stacked_psr;  
    static unsigned int SHCSR;  
    static unsigned char MFSR;  
    static unsigned char BFSR;   
    static unsigned short int UFSR;  
    static unsigned int HFSR;  
    static unsigned int DFSR;  
    static unsigned int MMAR;  
    static unsigned int BFAR;   
    stacked_r0 = ((unsigned long) hardfault_args[0]);  
    stacked_r1 = ((unsigned long) hardfault_args[1]);  
    stacked_r2 = ((unsigned long) hardfault_args[2]);  
    stacked_r3 = ((unsigned long) hardfault_args[3]);  
    stacked_r12 = ((unsigned long) hardfault_args[4]);  
    /*异常中断发生时，这个异常模式特定的物理R14,即lr被设置成该异常模式将要返回的地址*/  
    stacked_lr = ((unsigned long) hardfault_args[5]);   
    stacked_pc = ((unsigned long) hardfault_args[6]);  
    stacked_psr = ((unsigned long) hardfault_args[7]); 

    MFSR = (*((volatile unsigned char *)(0xE000ED28))); //存储器管理fault状态寄存器   
    BFSR = (*((volatile unsigned char *)(0xE000ED29))); //总线fault状态寄存器   
    UFSR = (*((volatile unsigned short int *)(0xE000ED2A)));//用法fault状态寄存器    
    HFSR = (*((volatile unsigned long *)(0xE000ED2C)));  //硬fault状态寄存器     
    DFSR = (*((volatile unsigned long *)(0xE000ED30))); //调试fault状态寄存器  
    MMAR = (*((volatile unsigned long *)(0xE000ED34))); //存储管理地址寄存器  
    BFAR = (*((volatile unsigned long *)(0xE000ED38))); //总线fault地址寄存器  
    while (1);  
}

void vApplicationMallocFailedHook(void)
{


}