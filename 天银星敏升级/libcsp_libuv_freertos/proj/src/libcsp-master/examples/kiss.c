/**
 * Build this example on linux with:
 * ./waf configure --enable-examples --enable-if-kiss --with-driver-usart=linux --enable-crc32 clean build
 */

#include <stdio.h>
#include <csp/csp.h>
#include <csp/interfaces/csp_if_kiss.h>

#include <csp/drivers/usart.h>
#include <csp/arch/csp_thread.h>
#include "csp_if_can.h"
#include "can.h"
#include "osapi_freertos.h"

#define PORT 10
#define MY_ADDRESS 1
#define SERVER_ADDRESS 0x17

#define SERVER_TIDX 0
#define CLIENT_TIDX 1
#define USART_HANDLE 0

#include "driver_uart.h"
uint8_t kiss_clientaddr = MY_ADDRESS;
uint8_t kiss_serveraddr = SERVER_ADDRESS;
uint8_t kiss_serverport = PORT;

static csp_iface_t csp_if_kiss;
//static csp_kiss_handle_t csp_kiss_driver;
        
uint8_t kiss_get_clientaddr(void)
{
    return kiss_clientaddr;

}

void kiss_set_clientaddr(uint8_t addr)
{
    kiss_clientaddr = addr;

}

uint8_t kiss_get_serveraddr(void)
{
    return kiss_serveraddr;

}

void kiss_set_serveraddr(uint8_t addr)
{
    kiss_serveraddr=addr;

}

uint8_t kiss_get_serverport(void)
{
    return kiss_serverport;

}

void kiss_set_serverport(uint8_t addr)
{
    kiss_serverport =addr; 
}

        
CSP_DEFINE_TASK(task_server) {
    //int running = 1;
    OsTimeDelay(60000);
    csp_socket_t *socket = csp_socket(CSP_SO_NONE);
    csp_conn_t *conn;
    csp_packet_t *packet;
    csp_packet_t *response;

    response = csp_buffer_get(sizeof(csp_packet_t) + 2);
    if( response == NULL ) {
        printf("Could not allocate memory for response packet!\n");
        return CSP_TASK_RETURN;
    }
    response->data[0] = 'O';
    response->data[1] = 'K';
    response->length = 2;

    csp_bind(socket, CSP_ANY);
    csp_listen(socket, 5);

    printf("Server task started\r\n");

    while(1) {
        if( (conn = csp_accept(socket, 10000)) == NULL ) {
            continue;
        }

        while( (packet = csp_read(conn, 100)) != NULL ) {
            switch( csp_conn_dport(conn) ) {
                case PORT:
                    if( packet->data[0] == 'q' )
                        //running = 0;
                    csp_send(conn, response, 1000);
                    break;
                default:
                    csp_service_handler(conn, packet);
                    break;
            }
            csp_buffer_free(packet);  /*用完后释放*/
            packet = NULL;
        }

        csp_close(conn);
    }

    //csp_buffer_free(response);

    //return CSP_TASK_RETURN;
}

CSP_DEFINE_TASK(task_client) {

    ///static uint8_t buff[PBUF_MTU]={0}; 
    uint16_t buff_offset=0;
    //char outbuf[] = "1234567890";
    //char inbuf[3] = {0};
    int pingResult;
    int erro;
    int uartlen;
    int csplen;
    csp_packet_t * packet = 0;
    OsTimeDelay(20);
//    for(int i = 0; i <= 10; i++) {
//      pingResult = csp_ping(kiss_get_serveraddr(), 1000, 200, CSP_O_NONE);
//      OsPrintf(OSAPI_DEBUG_INFO,"Ping with payload of %d bytes, took %d ms\r\n", 200, pingResult);
//      csp_sleep_ms(100);
//    }
    
//    for(int i = 0; i <= 30; i++) {
//      csp_ps(kiss_get_serveraddr(), 1000);
//      csp_sleep_ms(1000);
//    }
//    for(int i = 0; i <= 30; i++) {
//      csp_memfree(kiss_get_serveraddr(), 1000);
//      csp_sleep_ms(1000);
//    }
//    for(int i = 0; i <= 30; i++) {
//      csp_buf_free(kiss_get_serveraddr(), 1000);
//      csp_sleep_ms(1000);
//    }
//    for(int i = 0; i <= 30; i++) {
//      csp_uptime(kiss_get_serveraddr(), 1000);
//      csp_sleep_ms(1000);
//    }
    csp_conn_t * conn = csp_connect(0, kiss_get_serveraddr(), kiss_get_serverport(), 0, CSP_CONNECTION_SO);
    while(1)
    {
        //csp_transaction(0, kiss_get_serveraddr(), kiss_get_serverport(), 3000, &outbuf, 1, inbuf, 2);
        //int status = csp_transaction_persistent(conn, 2000, outbuf, 10, inbuf, 2);
        //printf("Quit response from server: %s\n", inbuf);
      /*UART5  -> CSP*/
//      do
//      {
//        uartlen = driver_uart_recv(HAL_UART_5, buff+buff_offset, sizeof(buff)-buff_offset, 30, &erro);
//        if(uartlen)
//        {
//            buff_offset+=uartlen;
//            if((buff_offset>=PBUF_MTU) || (buff[buff_offset-1] == '\r') || (buff[buff_offset-1] == '\n'))               
//            {
//              OsPrintf(OSAPI_DEBUG_INFO,"UART5:get frame len=%d\r\n", buff_offset);
//              csplen = csp_transaction_persistent(conn, 30, buff, buff_offset, 0, 0);
//              if(csplen)
//              {
//                  OsPrintf(OSAPI_DEBUG_INFO,"can-csp:put frame len=%d ok\r\n", buff_offset);
//              }
//              else
//              {
//                  OsPrintf(OSAPI_DEBUG_INFO,"can-csp:put frame len=%d err\r\n", buff_offset);
//              }
//              buff_offset=0;
//            }
//        }
//      }
//      while(uartlen);
      
      /*CSP->UART5*/
      do
      {
	packet = csp_read(conn, 50);
	if (packet == NULL)
        {
            csplen = 0;
            break;
        }
        else
        {
            csplen = packet->length;
        }
        if(csplen)
        {
            OsPrintf(OSAPI_DEBUG_INFO,"can-csp:get frame len=%d\r\n", csplen);
            if(0!=driver_uart_send(HAL_UART_5, packet->data, csplen, 30, &erro))
            {
                OsPrintf(OSAPI_DEBUG_INFO,"UART5:put frame len=%d ok\r\n", csplen);
            }
            else
            {
                OsPrintf(OSAPI_DEBUG_INFO,"UART5:put frame len=%d err\r\n", csplen);
            }
        }
        csp_buffer_free(packet);
      }
      while(csplen);
      //csp_sleep_ms(10);
    }
    //return CSP_TASK_RETURN;
}

void my_usart_rx(uint8_t * buf, int len, void * pxTaskWoken) 
{
  csp_kiss_rx(&csp_if_kiss, buf, len, pxTaskWoken);
}

int kiss_set(int argc) 
{
  struct csp_can_config canconf;
  canconf.bitrate = (uint32_t)500000;
  canconf.clock_speed = 0;
  canconf.ifc=0;
    csp_debug_toggle_level(CSP_PACKET);
    csp_debug_toggle_level(CSP_INFO);

    csp_buffer_init(5, PBUF_MTU);
    csp_init(kiss_get_clientaddr());

    struct usart_conf conf;


    /* Run USART init */
    usart_init(&conf);

    /* Setup CSP interface */

    //csp_kiss_init(&csp_if_kiss, &csp_kiss_driver, usart_putc, usart_insert, "KISS");
    csp_can_init(CSP_CAN_PROMISC, &canconf);	//    CSP_CAN_PROMISC CSP_CAN_MASKED
    /* Setup callback from USART RX to KISS RS */
    //usart_set_callback(my_usart_rx);
    csp_route_set(kiss_get_serveraddr(), &csp_if_can, CSP_NODE_MAC);
    csp_route_set(kiss_get_clientaddr(), &csp_if_can, CSP_NODE_MAC);
    //csp_route_set(kiss_get_clientaddr(), &csp_if_kiss, CSP_NODE_MAC);
    csp_route_start_task(256, 3);
#if CSP_DEBUG
    csp_conn_print_table();
    csp_route_print_table();
    csp_route_print_interfaces();
#endif
    if(argc)
    {
      ///csp_thread_handle_t handle_server;
      ///csp_thread_create(task_server, "SERVER", 256, NULL, 3, &handle_server);
    }
    else
    {
      ///csp_thread_handle_t handle_client;
      ///csp_thread_create(task_client, "CLIENT", 256, NULL, 5, &handle_client);
    }
    /* Wait for program to terminate (ctrl + c) */
//    while(1) {
//    	csp_sleep_ms(1000000);
//    }
    return 0;
}
