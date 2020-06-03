#include <stdio.h>
#include <csp/csp.h>
#include <csp/drivers/usart.h>
#include "driver.h"
#include "hal.h"

usart_callback_t  usart_callback = NULL;

/*接收任务循环调用接收回调函数*/
unsigned  prvRxTask(void* params) 
{
  
  uint8_t buff[100];
  uint16_t len;
  int32_t erro;
  len = driver_uart_recv(HAL_UART_4, buff, sizeof(buff), 100, &erro);
  if(len)
  {
    if( usart_callback != NULL )
    {
      usart_callback(buff, len, NULL);
    }
  }
  return 0;
}

void usart_shutdown(void) 
{

}



void usart_putstr(char* buf, int bufsz) 
{
  int32_t erro;
  driver_uart_send(HAL_UART_4, (unsigned char*)buf, bufsz, 100, &erro);
}

void usart_putc(char c) 
{
  int32_t erro;
  unsigned char tmp=c;
  driver_uart_send(HAL_UART_4, &tmp, 1, 10, &erro);
}


void usart_insert(char c, void *pxTaskWoken) 
{
    /* redirect debug output to stdout */
    printf("%c", c);
}

void usart_set_callback(usart_callback_t callback) 
{
    usart_callback = callback;
}

void usart_init(struct usart_conf * conf) 
{

}


