/**
 *****************************************************************************        
 * \brief       驱动层(DRIVER)串口(UART)相关接口实现.
 * \details     Copyright (c) 2017,长沙威胜信息技术有限公司.
 *              All rights reserved.    
 * \file        driver_uart.c 
 * \author      李红洁W18900
 * \version     1.0 
 * \date        2017-07-24
 * \note        使用前请参考注释.\n
 *              调用HAL实现,依赖OSAPI相关接口.\n
 * \since       李红洁      2017-7-24 16:06:52     1.0     新建 
 * \par 修订记录
 * - 2017-07-24 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */

#define DRIVER_UART_GLOBALS /**< 包含driver_uart.h中的数据将会编译*/

#include "driver.h"
#include "osapi_freertos.h"
   
/*****************************************************************************    
 *                                                                           
 *                             内部数据                                   
 *                                                                            
 ****************************************************************************/


/*****************************************************************************    
 *                                                                           
 *                             内部接口函数实现                                   
 *                                                                            
 ****************************************************************************/

/*485方向控制回调函数:
  按照一般需求最多8路串口这里实现8路,如果有更多路需求再添加即可
 */
#if UART0_ENABLE
static void uart0_txhook(void)
{
    //HAL_GPIO_WritePin(UART0_DIRCTRL_GPIO_ID,HAL_GPIO_INACTIVE);  /*无效电平发送*/
}

static void uart0_rxhook(void)
{
    //HAL_GPIO_WritePin(UART0_DIRCTRL_GPIO_ID,HAL_GPIO_ACTIVE);    /*有效电平接收*/
}
#endif

#if UART1_ENABLE
static void uart1_txhook(void)
{
    //HAL_GPIO_WritePin(UART1_DIRCTRL_GPIO_ID,HAL_GPIO_INACTIVE);  /*无效电平发送*/
}

static void uart1_rxhook(void)
{
    //HAL_GPIO_WritePin(UART1_DIRCTRL_GPIO_ID,HAL_GPIO_ACTIVE);    /*有效电平接收 */
}
#endif

#if UART2_ENABLE
static void uart2_txhook(void)
{
    //HAL_GPIO_WritePin(UART2_DIRCTRL_GPIO_ID,HAL_GPIO_INACTIVE);  /*无效电平发送*/
}

static void uart2_rxhook(void)
{
    //HAL_GPIO_WritePin(UART2_DIRCTRL_GPIO_ID,HAL_GPIO_ACTIVE);    /*有效电平接收 */
}
#endif

#if UART3_ENABLE
static void uart3_txhook(void)
{
    //HAL_GPIO_WritePin(UART3_DIRCTRL_GPIO_ID,HAL_GPIO_INACTIVE);  /*无效电平发送*/
}

static void uart3_rxhook(void)
{
    //HAL_GPIO_WritePin(UART3_DIRCTRL_GPIO_ID,HAL_GPIO_ACTIVE);    /*有效电平接收 */
}
#endif

#if UART4_ENABLE
static void uart4_txhook(void)
{
    //HAL_GPIO_WritePin(UART4_DIRCTRL_GPIO_ID,HAL_GPIO_INACTIVE);  /*无效电平发送*/
}

static void uart4_rxhook(void)
{
    //HAL_GPIO_WritePin(UART4_DIRCTRL_GPIO_ID,HAL_GPIO_ACTIVE);    /*有效电平接收 */
}
#endif

#if UART5_ENABLE
static void uart5_txhook(void)
{
    //HAL_GPIO_WritePin(UART5_DIRCTRL_GPIO_ID,HAL_GPIO_INACTIVE);  /*无效电平发送*/
}

static void uart5_rxhook(void)
{
    //HAL_GPIO_WritePin(UART5_DIRCTRL_GPIO_ID,HAL_GPIO_ACTIVE);    /*有效电平接收 */
}
#endif



/**
 *****************************************************************************
 * \fn          static int rx_fifo_in(driver_uart_t *p, unsigned char data)
 * \brief       写数据到接收缓冲区.
 * \note        接收中断服务函数调用该函数写数据到接收缓冲区,缓冲区满不写入. 
 * \param[in]   p 指向uart结构体实例的指针 \ref driver_uart_t
 * \param[in]   data 指向要写到缓冲区的数据
 *****************************************************************************
 */
static int rx_fifo_in(driver_uart_t *p, unsigned char data)
{
    if(p==0)
    {
        return 0;
    }
    /*缓冲区未满写入数据*/
    if (p->rx_len < (p->rxbuf_len/sizeof(p->rx_buf[0]))) 
    {   
        p->rx_buf[p->rx_in++] = data;                   /*写入数据,写入指针递增*/
        p->rx_in %= p->rxbuf_len/sizeof(p->rx_buf[0]); /*边界处理,实现圆形缓冲区*/
        p->rx_len++;                                    /*数据个数递增*/
        return 0;
    }
    else
    {
        return -1;
    }
}


/**
 *****************************************************************************
 * \fn          static unsigned char rx_fifo_out(driver_uart_t *p, unsigned int timeout, unsigned char *perro)
 * \brief       从接收缓冲区读数据.
 * \note        应用函数调用该函数从接收缓冲区读数据,可以指定超时时间. 
 * \param[in]   p 指向uart结构体实例的指针 \ref driver_uart_t
 * \param[in]   timeout 指定超时时间 0表示一直等待 
 * \param[in]   perro 错误码0表示读到了数据 1表示没有读到数据 
 * \retval      unsigned char 读到的字节数据.
 *****************************************************************************
 */
static unsigned char rx_fifo_out(driver_uart_t *p, unsigned int timeout, int *perro)
{
    unsigned char data = 0;
    if(p==0)
    {
        *perro = 1;
        return 0;
    }
    OSCriticalAlloc();
    /*timeout==0 表示一直等待直到有数据*/
    if (timeout == 0) 
    {
        while(p->rx_len == 0)
        {
            OsTimeDelay(DRIVER_UART_DELAY_CHECK);  /*延时*/
        }
    } 
    else /*timeout!=0 表示等待指定超时时间*/
    {
        while (timeout != 0)
        {
            if (p->rx_len == 0) 
            {
                /*如果无数据,延时指定时间间隔*/
                if (timeout >= DRIVER_UART_DELAY_CHECK)
                {
                    timeout -= DRIVER_UART_DELAY_CHECK;
                    OsTimeDelay(DRIVER_UART_DELAY_CHECK);
                }
                else
                {
                     /*不足一个时间间隔延时剩余时间*/
                    OsTimeDelay(timeout);
                    timeout = 0;
                }
            } 
            else
            {
                break;
            }
        }
    }
    /*临界段处理 防止此时中断在操作缓冲区*/
    OSCriticalEnter();
    if (p->rx_len != 0) 
    {
        /*从缓冲区读出数据*/
        p->rx_len--;
        data = p->rx_buf[p->rx_out++];
        p->rx_out %= p->rxbuf_len/sizeof(p->rx_buf[0]);
        *perro = 0;
    }
    else
    {
        *perro = 1;
    }
    OSCriticalExit();
    return data;
}


/**
 *****************************************************************************
 * \fn          static int tx_fifo_in(driver_uart_t *p, unsigned int timeout, unsigned char data,unsigned char *perro)
 * \brief       写数据到发送缓冲区.
 * \note        应用调用该函数调写数据到发送缓冲区,缓冲区满等待. 
 * \param[in]   p 指向uart结构体实例的指针 \ref driver_uart_t
 * \param[in]   timeout 设定超时时间 0表示一直等待
 * \param[in]   data 指向要写到缓冲区的数据
 * \retval      0 写成功
 * \retval      1 写失败
 *****************************************************************************
 */
static int tx_fifo_in(driver_uart_t *p, unsigned char data, unsigned int timeout)
{
    /*注意确保p缓冲区的读写处于临界段保护 */
    int ret=0;
    if(p==0)
    {
        return 1;
    }
    OSCriticalAlloc();
    if (timeout == 0) 
    {
       
        while(1)
        {
            OSCriticalEnter();
            if(p->tx_len < (p->txbuf_len/sizeof(p->tx_buf[0])))
            {
                /*没有满则写入数据*/
                p->tx_buf[p->tx_in++] = data;
                p->tx_in %= p->txbuf_len/sizeof(p->tx_buf[0]);
                p->tx_len++;
                OSCriticalExit();
                break;
            }
            else
            {
                /*满则等待*/
                OSCriticalExit();
                OsTimeDelay(DRIVER_UART_DELAY_CHECK);  /*延时*/
            }
        }
    } 
    while (timeout != 0)
    {
        OSCriticalEnter();
        if (p->tx_len < (p->txbuf_len/sizeof(p->tx_buf[0]))) 
        {   
            /*没有满则写入数据*/
            p->tx_buf[p->tx_in++] = data;
            p->tx_in %= p->txbuf_len/sizeof(p->tx_buf[0]);
            p->tx_len++;
            OSCriticalExit();
            ret = 0; /*设置正确返回值*/
            break;
        } 
        else 
        {
            OSCriticalExit();
            /*如果无数据,延时指定时间间隔*/
            if (timeout >= DRIVER_UART_DELAY_CHECK)
            {
                timeout -= DRIVER_UART_DELAY_CHECK;
                OsTimeDelay(DRIVER_UART_DELAY_CHECK);
                ret = 1; /*设置超时错误 之前这里没设置ret=1 在timeout为DRIVER_UART_DELAY_CHECK的整数倍时总是会返回0*/
            }
            else
            {
                /*不足一个时间间隔延时剩余时间*/
                OsTimeDelay(timeout);
                timeout = 0;
                ret = 1; /*设置超时错误*/
            }
        }
    }
    return ret;
}


/**
 *****************************************************************************
 * \fn          static unsigned char tx_fifo_out(driver_uart_t *p, unsigned char *perro)
 * \brief       从发送缓冲区读数据.
 * \note        发送中断服务函数调用该函数获取发送缓冲区数据,用于发送. 
 * \param[in]   p 指向uart结构体实例的指针 \ref driver_uart_t
 * \param[out]  perro 错误码 0表示读到数据 1表示没有数据
 * \return      unsigned char perro=0时表示读到的数据  
 *****************************************************************************
 */
static unsigned char tx_fifo_out(driver_uart_t *p, int *perro)
{
    unsigned char data = 0;
    if(p==0)
    {
        *perro = 1;
        return 0;
    }
    if (p->tx_len != 0) 
    {
        if(p->tx_hook != 0)
        {
            (*(p->tx_hook))();
        }
        p->tx_len--;
        data = p->tx_buf[p->tx_out++];
        p->tx_out %= p->txbuf_len/sizeof(p->tx_buf[0]);
        *perro = 0;
    } 
    else
    {
        *perro = 1;
    }
    return data;
}



/**
 *****************************************************************************
 * \fn          static int driver_uart0_send_isr(unsigned char *pdata)
 * \brief       发送处理.
 * \note        发送中断服务函数调用该函数,如果缓冲区空则停止发送并调用回调函数. 
 * \param[out]  pdata 回写需要发送的数据
 * \retval      0 pdata指向的数据有效(需要发送) 1无后续发送数据
 *****************************************************************************
 */
#if UART0_ENABLE
static int driver_uart0_send_isr(unsigned char *pdata)
{
    int erro=0; 
    unsigned char data;
    if(driver_uart_tab[0]==0)
    {
        return 1;
    }
    data = tx_fifo_out(driver_uart_tab[0], &erro);
    if (erro != 0) 
    {  
        /*没有获取到数据,停止发送*/
        HAL_UART_EnableTx(driver_uart_tab[0]->id,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
        driver_uart_tab[0]->state = HAL_UART_TXIDLE;
        if(driver_uart_tab[0]->rx_hook != 0)
        {
            driver_uart_tab[0]->rx_hook();
        }   
        return 1;
    } 
    else
    {
        *pdata = data;
        return 0;
    }  
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart1_send_isr(unsigned char *pdata)
 * \brief       发送处理.
 * \note        发送中断服务函数调用该函数,如果缓冲区空则停止发送并调用回调函数. 
 * \param[out]  pdata 回写需要发送的数据
 * \retval      0 pdata指向的数据有效(需要发送) 1无后续发送数据
 *****************************************************************************
 */
#if UART1_ENABLE
static int driver_uart1_send_isr(unsigned char *pdata)
{
    int erro=0; 
    unsigned char data;
    if(driver_uart_tab[1]==0)
    {
        return 1;
    }
    data = tx_fifo_out(driver_uart_tab[1], &erro);
    if (erro != 0) 
    {  
        /*没有获取到数据,停止发送*/
        HAL_UART_EnableTx(driver_uart_tab[1]->id,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
        driver_uart_tab[1]->state = HAL_UART_TXIDLE;
        if(driver_uart_tab[1]->rx_hook != 0)
        {
            driver_uart_tab[1]->rx_hook();
        }   
        return 1;
    } 
    else
    {
        *pdata = data;
        return 0;
    }  
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart2_send_isr(unsigned char *pdata)
 * \brief       发送处理.
 * \note        发送中断服务函数调用该函数,如果缓冲区空则停止发送并调用回调函数. 
 * \param[out]  pdata 回写需要发送的数据
 * \retval      0 pdata指向的数据有效(需要发送) 1无后续发送数据
 *****************************************************************************
 */
#if UART2_ENABLE
static int driver_uart2_send_isr(unsigned char *pdata)
{
    int erro=0; 
    unsigned char data;
    if(driver_uart_tab[2]==0)
    {
        return 1;
    }
    data = tx_fifo_out(driver_uart_tab[2], &erro);
    if (erro != 0) 
    {  
        /*没有获取到数据,停止发送*/
        HAL_UART_EnableTx(driver_uart_tab[2]->id,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
        driver_uart_tab[2]->state = HAL_UART_TXIDLE;
        if(driver_uart_tab[2]->rx_hook != 0)
        {
            driver_uart_tab[2]->rx_hook();
        } 
        return 1;
    } 
    else
    {
        *pdata = data;
        return 0;
    }  
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart3_send_isr(unsigned char *pdata)
 * \brief       发送处理.
 * \note        发送中断服务函数调用该函数,如果缓冲区空则停止发送并调用回调函数. 
 * \param[out]  pdata 回写需要发送的数据
 * \retval      0 pdata指向的数据有效(需要发送) 1无后续发送数据
 *****************************************************************************
 */
#if UART3_ENABLE
static int driver_uart3_send_isr(unsigned char *pdata)
{
    int erro=0; 
    unsigned char data;
    if(driver_uart_tab[3]==0)
    {
        return 1;
    }
    data = tx_fifo_out(driver_uart_tab[3], &erro);
    if (erro != 0) 
    {  
        /*没有获取到数据,停止发送*/
        HAL_UART_EnableTx(driver_uart_tab[3]->id,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
        driver_uart_tab[3]->state = HAL_UART_TXIDLE;
        if(driver_uart_tab[3]->rx_hook != 0)
        {
            driver_uart_tab[3]->rx_hook();
        }   
        return 1;
    } 
    else
    {
        *pdata = data;
        return 0;
    }  
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart4_send_isr(unsigned char *pdata)
 * \brief       发送处理.
 * \note        发送中断服务函数调用该函数,如果缓冲区空则停止发送并调用回调函数. 
 * \param[out]  pdata 回写需要发送的数据
 * \retval      0 pdata指向的数据有效(需要发送) 1无后续发送数据
 *****************************************************************************
 */
#if UART4_ENABLE
static int driver_uart4_send_isr(unsigned char *pdata)
{
    int erro=0; 
    unsigned char data;
    if(driver_uart_tab[4]==0)
    {
        return 1;
    }
    data = tx_fifo_out(driver_uart_tab[4], &erro);
    if (erro != 0) 
    {  
        /*没有获取到数据,停止发送*/
        HAL_UART_EnableTx(driver_uart_tab[4]->id,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
        driver_uart_tab[4]->state = HAL_UART_TXIDLE;
        if(driver_uart_tab[4]->rx_hook != 0)
        {
            driver_uart_tab[4]->rx_hook();
        }  
        return 1;
    } 
    else
    {
      *pdata = data;
      return 0;
    }  
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart5_send_isr(unsigned char *pdata)
 * \brief       发送处理.
 * \note        发送中断服务函数调用该函数,如果缓冲区空则停止发送并调用回调函数. 
 * \param[out]  pdata 回写需要发送的数据
 * \retval      0 pdata指向的数据有效(需要发送) 1无后续发送数据
 *****************************************************************************
 */
#if UART5_ENABLE
static int driver_uart5_send_isr(unsigned char *pdata)
{
    int erro=0; 
    unsigned char data;
    if(driver_uart_tab[5]==0)
    {
        return 1;
    } 
    data = tx_fifo_out(driver_uart_tab[5], &erro);
    if (erro != 0) 
    {  
        /*没有获取到数据,停止发送*/
        HAL_UART_EnableTx(driver_uart_tab[5]->id,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
        driver_uart_tab[5]->state = HAL_UART_TXIDLE;
        if(driver_uart_tab[5]->rx_hook != 0)
        {
            driver_uart_tab[5]->rx_hook();
        }   
        return 1;
    } 
    else
    {
        *pdata = data;
        return 0;
    }  
}
#endif




/**
 *****************************************************************************
 * \fn          static int driver_uart0_recv_isr(unsigned char* data)
 * \brief       接收处理.
 * \note        接收中断服务函数调用该函数,如果缓冲区满则丢弃新的数据. 
 * \param[out]  pdata 新的数据.
 * \retval      总是返回0
 *****************************************************************************
 */
#if UART0_ENABLE
 static int driver_uart0_recv_isr(unsigned char* data)
{
    if(driver_uart_tab[0]==0)
    {
        return 0;
    }
    driver_uart_tab[0]->rxhavedata =1;
    return rx_fifo_in(driver_uart_tab[0], *data);
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart1_recv_isr(unsigned char* data)
 * \brief       接收处理.
 * \note        接收中断服务函数调用该函数,如果缓冲区满则丢弃新的数据. 
 * \param[out]  pdata 新的数据.
 * \retval      总是返回0
 *****************************************************************************
 */
#if UART1_ENABLE
static int driver_uart1_recv_isr(unsigned char* data)
{
    if(driver_uart_tab[1]==0)
    {
        return 0;
    }
    driver_uart_tab[1]->rxhavedata =1;
    return rx_fifo_in(driver_uart_tab[1], *data);
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart2_recv_isr(unsigned char* data)
 * \brief       接收处理.
 * \note        接收中断服务函数调用该函数,如果缓冲区满则丢弃新的数据. 
 * \param[out]  pdata 新的数据.
 * \retval      总是返回0
 *****************************************************************************
 */
#if UART2_ENABLE
static int driver_uart2_recv_isr(unsigned char* data)
{
    if(driver_uart_tab[2]==0)
    {
        return 0;
    }
    driver_uart_tab[2]->rxhavedata =1;
    return rx_fifo_in(driver_uart_tab[2], *data);
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart3_recv_isr(unsigned char* data)
 * \brief       接收处理.
 * \note        接收中断服务函数调用该函数,如果缓冲区满则丢弃新的数据. 
 * \param[out]  pdata 新的数据.
 * \retval      总是返回0
 *****************************************************************************
 */
#if UART3_ENABLE
static int driver_uart3_recv_isr(unsigned char* data)
{
    if(driver_uart_tab[3]==0)
    {
        return 0;
    }
    driver_uart_tab[3]->rxhavedata =1;
    return rx_fifo_in(driver_uart_tab[3], *data);
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart4_recv_isr(unsigned char* data)
 * \brief       接收处理.
 * \note        接收中断服务函数调用该函数,如果缓冲区满则丢弃新的数据. 
 * \param[out]  pdata 新的数据.
 * \retval      总是返回0
 *****************************************************************************
 */
#if UART4_ENABLE
static int driver_uart4_recv_isr(unsigned char* data)
{
    if(driver_uart_tab[4]==0)
    {
        return 0;
    }
    driver_uart_tab[4]->rxhavedata =1;
    return rx_fifo_in(driver_uart_tab[4], *data);
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart5_recv_isr(unsigned char* data)
 * \brief       接收处理.
 * \note        接收中断服务函数调用该函数,如果缓冲区满则丢弃新的数据. 
 * \param[out]  pdata 新的数据.
 * \retval      总是返回0
 *****************************************************************************
 */
#if UART5_ENABLE
static int driver_uart5_recv_isr(unsigned char* data)
{
    if(driver_uart_tab[5]==0)
    {
        return 0;
    }
    driver_uart_tab[5]->rxhavedata =1;
    return rx_fifo_in(driver_uart_tab[5], *data);
}
#endif



/**
 *****************************************************************************
 * \fn          static int driver_uart_rxerr_isr(HAL_UART_ID_e id,unsigned char *pdata)
 * \brief       接收错误处理.
 * \note        接收错误中断服务函数调用该函数. 
 * \param[in]   pdata 错误码
 * \retval      0 总是返回0
 *****************************************************************************
 */
static int driver_uart_rxerr_isr(HAL_UART_ID_e id,unsigned char *pdata)
{
    if(driver_uart_tab[id]==0)
    {
        return 0;
    }
    if(*pdata & HAL_UART_ERR_PER)
    {
        if((driver_uart_tab[id]->err & 0x000000FF) != 0x000000FF)
        {
            driver_uart_tab[id]->err = driver_uart_tab[id]->err + 1;
        }
        else
        {
            HAL_UART_EnableRxErr(id,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
            HAL_UART_EnableRx(id,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
        }
    }
    if(*pdata & HAL_UART_ERR_FER)
    {
        if((driver_uart_tab[id]->err & 0x0000FF00) != 0x0000FF00)
        {
            driver_uart_tab[id]->err = driver_uart_tab[id]->err + 0x100;
        }
        else
        {
            HAL_UART_EnableRxErr(id,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
            HAL_UART_EnableRx(id,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
        }
    }
    if(*pdata & HAL_UART_ERR_ORER)
    {
        if((driver_uart_tab[id]->err & 0x00FF0000) != 0x00FF0000)
        {
            driver_uart_tab[id]->err = driver_uart_tab[id]->err + 0x10000;
        }
        else
        {
            HAL_UART_EnableRxErr(id,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
            HAL_UART_EnableRx(id,HAL_UART_INTERRUPT,HAL_UART_DISABLE);
        }
    }
    return 0;
}

/**
 *****************************************************************************
 * \fn          static int driver_uart0_rxerr_isr(unsigned char *pdata)
 * \brief       接收错误处理.
 * \note        接收错误中断服务函数调用该函数. 
 * \param[in]   pdata 错误码
 * \retval      0 总是返回0
 *****************************************************************************
 */
#if UART0_ENABLE
static int driver_uart0_rxerr_isr(unsigned char *pdata)
{
    return driver_uart_rxerr_isr(HAL_UART_0,pdata);
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart1_rxerr_isr(unsigned char *pdata)
 * \brief       接收错误处理.
 * \note        接收错误中断服务函数调用该函数. 
 * \param[in]   pdata 错误码
 * \retval      0 总是返回0
 *****************************************************************************
 */
#if UART1_ENABLE
static int driver_uart1_rxerr_isr(unsigned char *pdata)
{
    return driver_uart_rxerr_isr(HAL_UART_1,pdata);
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart2_rxerr_isr(unsigned char *pdata)
 * \brief       接收错误处理.
 * \note        接收错误中断服务函数调用该函数. 
 * \param[in]   pdata 错误码
 * \retval      0 总是返回0
 *****************************************************************************
 */
#if UART2_ENABLE
static int driver_uart2_rxerr_isr(unsigned char *pdata)
{
    return driver_uart_rxerr_isr(HAL_UART_2,pdata);
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart3_rxerr_isr(unsigned char *pdata)
 * \brief       接收错误处理.
 * \note        接收错误中断服务函数调用该函数. 
 * \param[in]   pdata 错误码
 * \retval      0 总是返回0
 *****************************************************************************
 */
#if UART3_ENABLE
static int driver_uart3_rxerr_isr(unsigned char *pdata)
{
    return driver_uart_rxerr_isr(HAL_UART_3,pdata);
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart4_rxerr_isr(unsigned char *pdata)
 * \brief       接收错误处理.
 * \note        接收错误中断服务函数调用该函数. 
 * \param[in]   pdata 错误码
 * \retval      0 总是返回0
 *****************************************************************************
 */
#if UART4_ENABLE
static int driver_uart4_rxerr_isr(unsigned char *pdata)
{
    return driver_uart_rxerr_isr(HAL_UART_4,pdata);
}
#endif

/**
 *****************************************************************************
 * \fn          static int driver_uart5_rxerr_isr(unsigned char *pdata)
 * \brief       接收错误处理.
 * \note        接收错误中断服务函数调用该函数. 
 * \param[in]   pdata 错误码
 * \retval      0 总是返回0
 *****************************************************************************
 */
#if UART5_ENABLE
static int driver_uart5_rxerr_isr(unsigned char *pdata)
{
    return driver_uart_rxerr_isr(HAL_UART_5,pdata);
}
#endif


/*****************************************************************************    
 *                                                                           
 *                             对外接口函数实现                                   
 *                                                                            
 ****************************************************************************/

/**
 *****************************************************************************
 * \fn          static int driver_uart_init(HAL_UART_ID_e id)
 * \brief       初始化串口.
 * \note        . 
 * \param[in]   id 串口id \ref HAL_UART_ID_e 
 * \retval      0 设置成功
 * \retval      -1参数错误
 *****************************************************************************
 */
static int driver_uart_init(HAL_UART_ID_e id)
{	
    /*初始化端口和默认参数*/
    HAL_UART_Init(id);
    /*设置回调函数*/
    switch(id)
    {
#if UART0_ENABLE
    case HAL_UART_0:
        HAL_UART_SetTxCmpISRCallBack(HAL_UART_0,driver_uart0_send_isr);
        HAL_UART_SetRxCallBack(HAL_UART_0,driver_uart0_recv_isr);
        HAL_UART_SetRxErrCallBack(HAL_UART_0,driver_uart0_rxerr_isr);
        break;
#endif
#if UART1_ENABLE
    case HAL_UART_1:
        HAL_UART_SetTxCmpISRCallBack(HAL_UART_1,driver_uart1_send_isr);
        HAL_UART_SetRxCallBack(HAL_UART_1,driver_uart1_recv_isr);
        HAL_UART_SetRxErrCallBack(HAL_UART_1,driver_uart1_rxerr_isr);
        break;
#endif
#if UART2_ENABLE   
    case HAL_UART_2:
        HAL_UART_SetTxCmpISRCallBack(HAL_UART_2,driver_uart2_send_isr);
        HAL_UART_SetRxCallBack(HAL_UART_2,driver_uart2_recv_isr);
        HAL_UART_SetRxErrCallBack(HAL_UART_2,driver_uart2_rxerr_isr);
        break;
#endif
#if UART3_ENABLE
    case HAL_UART_3:
        HAL_UART_SetTxCmpISRCallBack(HAL_UART_3,driver_uart3_send_isr);
        HAL_UART_SetRxCallBack(HAL_UART_3,driver_uart3_recv_isr);
        HAL_UART_SetRxErrCallBack(HAL_UART_3,driver_uart3_rxerr_isr);
        break;
#endif
#if UART4_ENABLE
    case HAL_UART_4:
        HAL_UART_SetTxCmpISRCallBack(HAL_UART_4,driver_uart4_send_isr);
        HAL_UART_SetRxCallBack(HAL_UART_4,driver_uart4_recv_isr);
        HAL_UART_SetRxErrCallBack(HAL_UART_4,driver_uart4_rxerr_isr);
        break;
#endif
#if UART5_ENABLE 
    case HAL_UART_5:
        HAL_UART_SetTxCmpISRCallBack(HAL_UART_5,driver_uart5_send_isr);
        HAL_UART_SetRxCallBack(HAL_UART_5,driver_uart5_recv_isr);
        HAL_UART_SetRxErrCallBack(HAL_UART_5,driver_uart5_rxerr_isr);
        break;
#endif
#if UART6_ENABLE
    case HAL_UART_6:
        HAL_UART_SetTxCmpISRCallBack(HAL_UART_6,driver_uart6_send_isr);
        HAL_UART_SetRxCallBack(HAL_UART_6,driver_uart6_recv_isr);
        HAL_UART_SetRxErrCallBack(HAL_UART_6,driver_uart6_rxerr_isr);
        break;
#endif
#if UART7_ENABLE
    case HAL_UART_7:
        HAL_UART_SetTxCmpISRCallBack(HAL_UART_7,driver_uart7_send_isr);
        HAL_UART_SetRxCallBack(HAL_UART_7,driver_uart7_recv_isr);
        HAL_UART_SetRxErrCallBack(HAL_UART_7,driver_uart7_rxerr_isr);
        break;
#endif
    default:
        return -1;
        break;
    }
    return 0;
}

/**
 *****************************************************************************
 * \fn          unsigned int driver_uart_getrxlen(HAL_UART_ID_e id)
 * \brief       获取接收缓冲区有效数据长度.
 * \note        . 
 * \param[in]   id 串口id \ref HAL_UART_ID_e 
 * \return      unsigned int 接收缓冲区长度
 *****************************************************************************
 */
unsigned int driver_uart_getrxlen(HAL_UART_ID_e id)
{
    if(driver_uart_tab[id]==0)
    {
        return 0;
    }
    return driver_uart_tab[id]->rx_len;
}


/**
 *****************************************************************************
 * \fn          unsigned int driver_uart_flush(HAL_UART_ID_e id)
 * \brief       清除缓冲区.
 * \note        . 
 * \param[in]   id 串口id \ref HAL_UART_ID_e 
 * \return      int 总是返回0
 *****************************************************************************
 */
unsigned int driver_uart_flush(HAL_UART_ID_e id)
{
    if(driver_uart_tab[id]!=0)
    {
        driver_uart_tab[id]->rx_in  = 0;
        driver_uart_tab[id]->rx_out = 0;
        driver_uart_tab[id]->rx_len = 0;
        driver_uart_tab[id]->tx_in  = 0;
        driver_uart_tab[id]->tx_out = 0;
        driver_uart_tab[id]->tx_len = 0;
        driver_uart_tab[id]->err = 0x00;
    }
    return 0;
}

/**
 *****************************************************************************
 * \fn          int driver_uart_set(HAL_UART_CFG_t* cfg)
 * \brief       配置串口参数.
 * \note        . 
 * \param[in]   cfg 配置参数
 * \return      int \ref HAL_UART_SetCfg 的返回值
 *****************************************************************************
 */
int driver_uart_set(HAL_UART_CFG_t* cfg)
{
    int ret;
    if(driver_uart_tab[cfg->id]==0)
    {
        return 0;
    }
    driver_uart_init(cfg->id);
    ret = HAL_UART_SetCfg(cfg->id,cfg);
    HAL_UART_EnableRx(cfg->id,HAL_UART_INTERRUPT,HAL_UART_ENABLE);
    driver_uart_flush(cfg->id);
    return ret;
}

/**
 *****************************************************************************
 * \fn          int driver_uart_recv(HAL_UART_ID_e id, unsigned char *pdata, unsigned int len, unsigned int timeout, int *perro)
 * \brief       串口接收指定长度数据,可以设置超时时间.
 * \note        应用调用该函数接收数据,如果缓冲区无数据就等待.
 * \param[in]   id 指定的串口id.
 * \param[in]   pdata 需存储接收数据的缓冲区.
 * \param[in]   len   接收的数据长度.
 * \param[in]   timeout 设置为0表示一直等到接收到指定长度数据,设置为其他值表示等待指定超时时间(单位mS)
 * \param[out]  perro 错误码 0表示成功 -1表示参数错误 1表示超时
 * \return      int 返回实际读到的数据长度
 *****************************************************************************
 */
int driver_uart_recv(HAL_UART_ID_e id, unsigned char *pdata, unsigned int len, unsigned int timeout, int *erro)
{
    unsigned char data=0;
    unsigned int readlen=0;
    unsigned int tmptimeout=0;
    if (id >= HAL_UART_MAX)
    {
        *erro = -1;
        return 0;
    }
    if(driver_uart_tab[id]==0)
    {
        *erro = -1;
        return 0;
    }
    if(len ==0)
    {
        *erro = -1;
        return 0;
    }
    tmptimeout = timeout; 
    while(len !=0)
    {
        data = rx_fifo_out(driver_uart_tab[id], tmptimeout, erro);
        if (*erro == 0) 
        {       
            tmptimeout = DRIVER_UART_DELAY_FRAME;
            *pdata++ = data;   
            len--;
            readlen++;
        } 
        else
        {
            *erro = 1;
            break;
        }
    }
    return readlen; 
}

/**
 *****************************************************************************
 * \fn          int driver_uart_send(HAL_UART_ID_e id, unsigned char *pdata, unsigned int len,unsigned int timeout,int *perro)
 * \brief       串口发送指定长度数据,可以设置超时时间.
 * \note        应用调用该函数发送数据,如果缓冲区满就等待.
 * \param[in]   id 指定的串口id.
 * \param[in]   pdata 需要发送的数据缓冲区.
 * \param[in]   len   需要发送的数据长度.
 * \param[in]   timeout 设置为0表示一直等到全部写入缓冲区,设置为其他值表示等待指定超时时间(单位mS)
 * \param[out]  perro 错误码 0表示成功 -1表示参数错误 1表示超时
 * \return      int 返回实际发送的数据
 *****************************************************************************
 */
int driver_uart_send(HAL_UART_ID_e id, unsigned char *pdata, unsigned int len,unsigned int timeout,int *erro)
{
    unsigned char full=0;
    driver_uart_t *p=0;
    unsigned char data=0;
    unsigned int sendlen=0;
    OSCriticalAlloc();
    if (id >= HAL_UART_MAX)
    {
        *erro = -1;
        return 0;
    }
    if(driver_uart_tab[id]==0)
    {
        *erro = -1;
        return 0;
    }
    if(len ==0)
    {
         *erro = -1;
        return 0;
    }
    p = (driver_uart_t *)driver_uart_tab[id];
    if(p->tx_hook !=0)
    {
        p->tx_hook();   /*调用准备发送回调函数*/
    }
    driver_uart_tab[id]->txhavedata =1;
    if(timeout == 0)
    {
        while(len !=0)
        {
            full = tx_fifo_in(p,*pdata,DRIVER_UART_DELAY_CHECK);
            if (full == 0) 
            {
                pdata++;
                len--;
                sendlen++;
            } 
            else 
            {
                OSCriticalEnter();
                /*如果缓冲区满而且当前串口没有处于发送状态要先发送一个字节触发发送中断流*/
                if(p->state == HAL_UART_TXIDLE)
                {
                    data = tx_fifo_out(p, erro);
                    if(*erro == 0)
                    {
                        if(p->tx_hook !=0)
                        {
                            p->tx_hook();   /*调用准备发送回调函数*/
                        }
                        p->state = HAL_UART_TXBUSY; /*注意:p->state的读写需要临界保护*/
#ifdef FM33A
                        HAL_UART_EnableTx(id,HAL_UART_INTERRUPT,HAL_UART_ENABLE);
                        HAL_UART_Send(p->id,data);   /*注意:必须放在使能TX后,否则使能TE会马上清除刚写如的数据,不产生中断 这里与k20等芯片不同*/
#else
                        HAL_UART_Send(p->id,data);  /*注意:必须放在使能TX前,写TDR会清除TEND标志,否则使能TE会马上产生发送完中断*/
                        HAL_UART_EnableTx(id,HAL_UART_INTERRUPT,HAL_UART_ENABLE);      
#endif
                    }
                }
                OSCriticalExit();
                OsTimeDelay(DRIVER_UART_DELAY_CHECK);
            }
        }
        *erro = 0;
    }
    else
    {
        while ((len != 0) && (timeout!=0)) 
        {
            full = tx_fifo_in(p,*pdata,DRIVER_UART_DELAY_CHECK);
            if (full == 0) 
            {
                pdata++;
                len--;
                sendlen++;
            } 
            else 
            {
                OSCriticalEnter();
                /*如果缓冲区满而且当前串口没有处于发送状态要先发送一个字节触发发送中断流*/
                if(p->state == HAL_UART_TXIDLE)
                {
                    data = tx_fifo_out(p, erro);
                    if(*erro == 0)
                    {
                        if(p->tx_hook !=0)
                        {
                            p->tx_hook();   /*调用准备发送回调函数*/
                        }
                        p->state = HAL_UART_TXBUSY; /*注意:p->state的读写需要临界保护*/
#ifdef FM33A
                        HAL_UART_EnableTx(id,HAL_UART_INTERRUPT,HAL_UART_ENABLE);
                        HAL_UART_Send(p->id,data);   /*注意:必须放在使能TX后,否则使能TE会马上清除刚写如的数据,不产生中断 这里与k20等芯片不同*/
#else
                        HAL_UART_Send(p->id,data);  /*注意:必须放在使能TX前,写TDR会清除TEND标志,否则使能TE会马上产生发送完中断*/
                        HAL_UART_EnableTx(id,HAL_UART_INTERRUPT,HAL_UART_ENABLE);      
#endif
                    }
                    OSCriticalExit();
                }
                else /*如果当前处于发送状态*/
                {
                    OSCriticalExit();
                    /*如果缓冲区满而且串口处于发送状态则延时指定时间间隔等待中断发送完*/
                    if (timeout >= DRIVER_UART_DELAY_CHECK)
                    {
                        timeout -= DRIVER_UART_DELAY_CHECK;
                        OsTimeDelay(DRIVER_UART_DELAY_CHECK);
                    }
                    else
                    {
                        /*不足一个时间间隔延时剩余时间*/
                        OsTimeDelay(timeout);
                        timeout = 0;
                    }
                }
            }
        }
        /*如果没发送完设置超时错误码*/
        if(len != 0)
        {
            *erro = 1;
        }
    }
    

    OSCriticalEnter();
    /*如果缓冲区有数据而且当前串口没有处于发送状态要先发送一个字节触发发送中断流*/
    if((p->state == HAL_UART_TXIDLE) && (p->tx_len!=0))
    {
        data = tx_fifo_out(p, erro);
        if(*erro == 0)
        {
            if(p->tx_hook !=0)
            {
                p->tx_hook();   /*调用准备发送回调函数*/
            }
            p->state = HAL_UART_TXBUSY; /*注意:p->state的读写需要临界保护*/
#ifdef FM33A
            HAL_UART_EnableTx(id,HAL_UART_INTERRUPT,HAL_UART_ENABLE);
            HAL_UART_Send(p->id,data);   /*注意:必须放在使能TX后,否则使能TE会马上清除刚写如的数据,不产生中断 这里与k20等芯片不同*/
#else
            HAL_UART_Send(p->id,data);  /*注意:必须放在使能TX前,写TDR会清除TEND标志,否则使能TE会马上产生发送完中断*/
            HAL_UART_EnableTx(id,HAL_UART_INTERRUPT,HAL_UART_ENABLE);      
#endif
        }
    }
    OSCriticalExit();
    return sendlen;
}

/**
 *****************************************************************************
 * \fn          int driver_uart_haverxdata(HAL_UART_ID_e id)
 * \brief       读串口是否曾经接收到过数据.
 * \note        应用调用该函数用于判断是否点亮接收指示灯.读会清除状态.
 * \param[in]   id 指定的串口id.
 * \retval      1 串口曾经有接收数据
 * \retval      0 串口没有接收数据
 *****************************************************************************
 */
int driver_uart_haverxdata(HAL_UART_ID_e id)
{
    unsigned char havedata=0;
    if(driver_uart_tab[id]==0)
    {
        return 0;
    }
    havedata = driver_uart_tab[id]->rxhavedata;
    driver_uart_tab[id]->rxhavedata =0;
    return havedata;
}

/**
 *****************************************************************************
 * \fn          int driver_uart_havetxdata(HAL_UART_ID_e id)
 * \brief       读串口是否曾经发送过数据.
 * \note        应用调用该函数用于判断是否点亮发送指示灯.读会清除状态.
 * \param[in]   id 指定的串口id.
 * \retval      1 串口曾经有发送数据
 * \retval      0 串口没有发送数据
 *****************************************************************************
 */
int driver_uart_havetxdata(HAL_UART_ID_e id)
{
    unsigned char havedata=0;
    if(driver_uart_tab[id]==0)
    {
        return 0;
    }
    havedata = driver_uart_tab[id]->txhavedata;
    driver_uart_tab[id]->txhavedata =0;
    return havedata;
}

/**
 *****************************************************************************
 * \fn          int driver_uart_geterr(HAL_UART_ID_e id,unsigned int* err)
 * \brief       读串口错误状态字.
 * \note        应用调用该函数用于判断串口是否异常,如果err的低三个字节有任意个字节为FF则表示异常需要处理.
 * \param[in]   id 指定的串口id \ref HAL_UART_ID_e .
 * \param[out]  err 存储错误字
                0xxxxxxxFF表示校验错误  
                0xxxxxFFxx表示帧错误   
                0xxxFF00xx表示溢出错误  
 * \retval      0 总是返回0
 *****************************************************************************
 */
int driver_uart_geterr(HAL_UART_ID_e id,unsigned int* err)
{
    if(driver_uart_tab[id]==0)
    {
        return 0;
    }
    *err = driver_uart_tab[id]->err;
    return 0;
}

