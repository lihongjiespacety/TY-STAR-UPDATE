/**
 *****************************************************************************        
 * \brief       标准输入输出底层接口重定向.
 * \details     Copyright (c) 2017,长沙威胜信息技术有限公司.
 *              All rights reserved.    
 * \file        retarget.c 
 * \author      李红洁W18900
 * \version     1.0 
 * \date        2017-07-26
 * \note        使用前请参考注释.\n
 *              实现供printf scanf调用的底层接口.\n
 * \since       李红洁      2017-7-26 08:40:34     1.0     新建 
 * \par 修订记录
 * - 2017-07-26 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */
#include "hal.h"
#include "driver.h"
#include "retarget.h"
#include <stddef.h>
static HAL_UART_ID_e stdiouart=HAL_UART_MAX;
///**
// *****************************************************************************
// * \fn          int putchar(int ch)
// * \brief       字符输出接口实现.
// * \note        供printf调用,调用driver_uart实现. 
// * \param[in]   ch 需要输出的字符
// * \return      int 实际输出的字符
// *****************************************************************************
// */
//int putchar(int ch)
//{
//    int erro=0;
//    unsigned char data=ch;
//    driver_uart_send(stdiouart, &data, 1,0,&erro);
//    return ch;   
//}

/**
 *****************************************************************************
 * \fn          size_t __write(int handle,const unsigned char *buf,size_t bufSize)
 * \brief       标准输出接口实现.
 * \note        供printf等函数调用,调用driver_uart实现. 
 * \param[in]   handle 命令
 * \param[in]   buf 输出缓冲区
 * \param[in]   bufSize 需要输出的字节数
 * \return      size_t 实际输出的长度
 *****************************************************************************
 */
size_t __write(int handle,const unsigned char *buf,size_t bufSize)
{ 
    size_t nChars = 0;   /* Check for the command to flush all handles */ 
    //int erro=0;
    if (handle == -1)  
    {    
        return 0;  
    }    /* Check for stdout and stderr (only necessary if FILE descriptors are enabled.) */  
    if (handle != 1 && handle != 2)  
    {    
        return -1;  
    }  
    for (/* Empty */; bufSize > 0; --bufSize)  
    {    
        //driver_uart_send(stdiouart,(unsigned char *)buf, 1,0,&erro); 
        /*改为查询方式发送(中断方式不能在中断服务函数中调用)*/
        //HAL_UART_EnableTx(stdiouart,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
        HAL_UART_EnableTx(stdiouart,HAL_UART_CHECK,HAL_UART_ENABLE);
        HAL_UART_CheckSend(stdiouart,(unsigned char)*buf); 
        //HAL_UART_EnableTx(stdiouart,HAL_UART_INTERRUPT,HAL_UART_ENABLE);
        ++buf;    
        ++nChars;  
    } 
    return nChars;
}

///**
// *****************************************************************************
// * \fn          int getchar(void)
// * \brief       字符输入接口实现.
// * \note        供scanf调用,调用driver_uart实现. 
// * \return      int 接收到的字符
// *****************************************************************************
// */
//int getchar(void)
//{
//    int erro=0;
//    unsigned char ch=0;
//    driver_uart_recv(stdiouart, &ch, 1,0,&erro);
//    putchar(ch);
//    if (ch == '\r')
//    {
//        putchar('\n');
//    }
//    return ch;
//}

/**
 *****************************************************************************
 * \fn          size_t __read(int handle,unsigned char *buf,size_t bufSize)
 * \brief       标准输入接口实现.
 * \note        供scanf等函数调用,调用driver_uart实现. 
 * \param[in]   handle 命令
 * \param[in]   buf 输入缓冲区
 * \param[in]   bufSize 需要输入的字节数
 * \return      size_t 实际输入的长度
 *****************************************************************************
 */
size_t __read(int handle,unsigned char *buf,size_t bufSize)
{  
    size_t nChars = 0;  
    int erro=0;
    unsigned char ch=0;
    if (handle != 0)  
    {    
        return -1;  
    }   
    for (/*Empty*/; bufSize > 0; --bufSize)  
    {      
        driver_uart_recv(stdiouart, &ch, 1,0,&erro);
        *buf++ = ch;    
        ++nChars;  
        //driver_uart_send(stdiouart,&ch, 1,0,&erro);
        HAL_UART_CheckSend(stdiouart,(unsigned char)ch); 
        if (ch == '\r')
        {
            ch = '\n'; 
            HAL_UART_CheckSend(stdiouart,(unsigned char)ch); 
        }
    }   
    return nChars;
}
/**
 *****************************************************************************
 * \fn          void setstdiouart(HAL_UART_ID_e id)
 * \brief       设置stdio使用的串口.
 * \note        默认端口无效,需要调用该函数去映射实际对应的端口. 
 * \param[in]   id stdio映射到的物理串口号,如果不需要使用stdio函数,端口设置为HAL_UART_MAX
 *****************************************************************************
 */
void setstdiouart(HAL_UART_ID_e id)
{
    stdiouart = id; 
}

/**
 *****************************************************************************
 * \fn          HAL_UART_ID_e getstdiouart(void)
 * \brief       获取stdio使用的串口.
 * \note        默认端口无效,需要调用setstdiouart去映射实际对应的端口. 
 * \return      返回使用的串口ID \ref HAL_UART_ID_e
 *****************************************************************************
 */
HAL_UART_ID_e getstdiouart(void)
{
    return stdiouart; 
}