/**
 *****************************************************************************        
 * \brief       驱动层(DRIVER)串口模块(UART)相关数据结构和接口描述.
 * \details     Copyright (c) 2017,xxx.
 *              All rights reserved.    
 * \file        driver_uart.h 
 * \author      李红洁
 * \version     1.0 
 * \date        2017-07-24
 * \note        使用前请参考注释.\n
 *              调用HAL实现,依赖OSAPI相关接口.\n
 * \since       李红洁      2017-7-24 16:07:47     1.0     新建 
 * \par 修订记录
 * - 2017-07-24 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */

#ifndef __DRIVER_UART_H
#define __DRIVER_UART_H

#ifdef __cplusplus
 extern "C" {
#endif
 
#include "hal_uart.h"
/** \addtogroup DRIVER 驱动层(DRIVER)
 *  \{
 */
         
/** \addtogroup DRIVER_UART 驱动层(DRIVER)串口模块(UART)
 *  \{
 */
/*****************************************************************************    
 *                                                                           *
 *                             数据结构描述                                  *
 *                                                                          *
 ****************************************************************************/
/** \defgroup DRIVER_UART_data 驱动层(DRIVER)串口模块(UART)数据结构 
  * \{
  */
/*
UART0  HAL_UART_0 下行2
UART1  HAL_UART_1 从MBUS/维护
UART2  HAL_UART_2 下行3
UART5  HAL_UART_3 下行1
UART6  HAL_UART_4 主MBUS
UART8  HAL_UART_5 下行485      
UART9  HAL_UART_6 上行载波
UART12 HAL_UART_7 上行485
 */
     
#define UART0_ENABLE          1  /**< 通道0 使能 */ 
#define UART1_ENABLE          1  /**< 通道1 使能 */ 
#define UART2_ENABLE          1  /**< 通道2 使能 */ 
#define UART3_ENABLE          1  /**< 通道3 使能 */ 
#define UART4_ENABLE          1  /**< 通道4 使能 */ 
#define UART5_ENABLE          1  /**< 通道5 使能 */ 

     

     


#define DRIVER_UART0_TXBUF_SIZE 1      /**< UART0发送缓冲区大小  */    
#define DRIVER_UART0_RXBUF_SIZE 1      /**< UART0接收缓冲区大小  */ 
#define DRIVER_UART1_TXBUF_SIZE 1      /**< UART1发送缓冲区大小  */    
#define DRIVER_UART1_RXBUF_SIZE 1      /**< UART1接收缓冲区大小  */ 
#define DRIVER_UART2_TXBUF_SIZE 64       /**< UART2发送缓冲区大小  */    
#define DRIVER_UART2_RXBUF_SIZE 128       /**< UART2接收缓冲区大小  */ 
#define DRIVER_UART3_TXBUF_SIZE 1       /**< UART3发送缓冲区大小  */    
#define DRIVER_UART3_RXBUF_SIZE 1       /**< UART3接收缓冲区大小  */ 
#define DRIVER_UART4_TXBUF_SIZE (4096+32)       /**< UART4发送缓冲区大小  */    
#define DRIVER_UART4_RXBUF_SIZE (4096+32)       /**< UART4接收缓冲区大小  */ 
#define DRIVER_UART5_TXBUF_SIZE (1)       /**< UART5发送缓冲区大小  */    
#define DRIVER_UART5_RXBUF_SIZE (1)       /**< UART5接收缓冲区大小  */ 

#define DRIVER_UART_DELAY_FIFO    (20)    /**< 缓冲区操作延时时间       */   
#define DRIVER_UART_DELAY_CHECK   (1)      /**< 缓冲区延时查询时间间隔   */
#define DRIVER_UART_DELAY_FRAME   (20)    /**< 字节间距大于该值认为帧结束   */
         
typedef void (*driver_uart_hook)(void);      /**< 发送/接收回调函数   */   

/**
 * \struct driver_uart_t
 * UART驱动配置结构体.
 */
typedef struct 
{
    HAL_UART_ID_e id;                 /**< UART id   */   
    unsigned int rx_in;              /**< 接收缓冲区写入指针       */ 
    unsigned int rx_out;             /**< 接收缓冲区读出指针       */ 
    unsigned int rx_len;             /**< 接收缓冲区有效数据大小   */ 
    unsigned int tx_in;              /**< 发送缓冲区写入指针       */ 
    unsigned int tx_out;             /**< 发送缓冲区读出指针      */ 
    unsigned int tx_len;             /**< 发送缓冲区有效数据大小   */ 
    unsigned short rxbuf_len;        /**< 接收缓冲区大小           */   
    unsigned short txbuf_len;        /**< 发送缓冲区大小          */ 
    driver_uart_hook tx_hook;         /**< 准备发送回调函数          */ 
    driver_uart_hook rx_hook;         /**< 发送完转接收回调函数          */ 
    unsigned char *rx_buf;           /**< 接收缓冲区              */ 
    unsigned char *tx_buf;           /**< 发送缓冲区                */ 
    unsigned int bit_len;            /**< 用于发送完延时(有发送完中断不需要)    */ 
    HAL_UART_TXSTATE_e state;         /**< 发送状态标志是否在发送                */ 
    unsigned char rxhavedata;        /**< 是否有接收到数据                */ 
    unsigned char txhavedata;        /**< 是否有发送数据                */ 
    unsigned int err;                /**< 错误码计数器               */ 
}driver_uart_t;


/*****************************************************************************    
 *                                                                           *
 *                             模块数据定义                                  *
 *                                                                          *
 ****************************************************************************/

#ifdef   DRIVER_UART_GLOBALS

/*485方向控制回调函数:
  按照一般需求最多8路串口这里实现8路,如果有更多路需求再添加即可
 */
void uart5_txhook(void);
void uart5_rxhook(void);
void uart4_txhook(void);
void uart4_rxhook(void);
void uart3_txhook(void);
void uart3_rxhook(void);
void uart2_txhook(void);
void uart2_rxhook(void);
void uart1_txhook(void);
void uart1_rxhook(void);
void uart0_txhook(void);
void uart0_rxhook(void);

static unsigned char driver_uart0_rxbuf[DRIVER_UART0_RXBUF_SIZE];
static unsigned char driver_uart0_txbuf[DRIVER_UART0_TXBUF_SIZE];
static unsigned char driver_uart1_rxbuf[DRIVER_UART1_RXBUF_SIZE];
static unsigned char driver_uart1_txbuf[DRIVER_UART1_TXBUF_SIZE];
static unsigned char driver_uart2_rxbuf[DRIVER_UART2_RXBUF_SIZE];
static unsigned char driver_uart2_txbuf[DRIVER_UART2_TXBUF_SIZE];
static unsigned char driver_uart3_rxbuf[DRIVER_UART3_RXBUF_SIZE];
static unsigned char driver_uart3_txbuf[DRIVER_UART3_TXBUF_SIZE];
static unsigned char driver_uart4_rxbuf[DRIVER_UART4_RXBUF_SIZE];
static unsigned char driver_uart4_txbuf[DRIVER_UART4_TXBUF_SIZE];
static unsigned char driver_uart5_rxbuf[DRIVER_UART5_RXBUF_SIZE];
static unsigned char driver_uart5_txbuf[DRIVER_UART5_TXBUF_SIZE];


static driver_uart_t driver_uart0 = {HAL_UART_0, 0,0,0, 0,0,0, sizeof(driver_uart0_rxbuf), sizeof(driver_uart0_txbuf),uart0_txhook,uart0_rxhook,driver_uart0_rxbuf, driver_uart0_txbuf, 0,HAL_UART_TXIDLE,0};
static driver_uart_t driver_uart1 = {HAL_UART_1, 0,0,0, 0,0,0, sizeof(driver_uart1_rxbuf), sizeof(driver_uart1_txbuf),uart1_txhook,uart1_rxhook,driver_uart1_rxbuf, driver_uart1_txbuf, 0,HAL_UART_TXIDLE,0};
static driver_uart_t driver_uart2 = {HAL_UART_2, 0,0,0, 0,0,0, sizeof(driver_uart2_rxbuf), sizeof(driver_uart2_txbuf),uart2_txhook,uart2_rxhook,driver_uart2_rxbuf, driver_uart2_txbuf, 0,HAL_UART_TXIDLE,0};
static driver_uart_t driver_uart3 = {HAL_UART_3, 0,0,0, 0,0,0, sizeof(driver_uart3_rxbuf), sizeof(driver_uart3_txbuf),uart3_txhook,uart3_rxhook,driver_uart3_rxbuf, driver_uart3_txbuf, 0,HAL_UART_TXIDLE,0};
static driver_uart_t driver_uart4 = {HAL_UART_4, 0,0,0, 0,0,0, sizeof(driver_uart4_rxbuf), sizeof(driver_uart4_txbuf),uart4_txhook,uart4_rxhook,driver_uart4_rxbuf, driver_uart4_txbuf, 0,HAL_UART_TXIDLE,0};
static driver_uart_t driver_uart5 = {HAL_UART_5, 0,0,0, 0,0,0, sizeof(driver_uart5_rxbuf), sizeof(driver_uart5_txbuf),uart5_txhook,uart5_rxhook,driver_uart5_rxbuf, driver_uart5_txbuf, 0,HAL_UART_TXIDLE,0};


static driver_uart_t *driver_uart_tab[] = {&driver_uart0, &driver_uart1, &driver_uart2,&driver_uart3, &driver_uart4, &driver_uart5};


#endif

/**
  * \}
  */
/*****************************************************************************    
 *                                                                           
 *                             接口函数描述                                   
 *                                                                            
 ****************************************************************************/

/** \defgroup DRIVER_UART_if 驱动层(DRIVER)串口模块(UART)接口
  * \{
  */
      
/**
 *****************************************************************************
 * \fn          unsigned int driver_uart_getrxlen(HAL_UART_ID_e id)
 * \brief       获取接收缓冲区有效数据长度.
 * \note        . 
 * \param[in]   id 串口id \ref HAL_UART_ID_e 
 * \return      unsigned int 接收缓冲区长度
 *****************************************************************************
 */
unsigned int driver_uart_getrxlen(HAL_UART_ID_e id);

/**
 *****************************************************************************
 * \fn          unsigned int driver_uart_flush(HAL_UART_ID_e id)
 * \brief       清除缓冲区.
 * \note        . 
 * \param[in]   id 串口id \ref HAL_UART_ID_e 
 * \return      int 总是返回0
 *****************************************************************************
 */
unsigned int driver_uart_flush(HAL_UART_ID_e id);

/**
 *****************************************************************************
 * \fn          int driver_uart_set(HAL_UART_CFG_t* cfg)
 * \brief       配置串口参数.
 * \note        . 
 * \param[in]   cfg 配置参数
 * \return      int \ref HAL_HWT_SetCfg() 的返回值
 *****************************************************************************
 */
int driver_uart_set(HAL_UART_CFG_t* cfg);

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
int driver_uart_recv(HAL_UART_ID_e id, unsigned char *pdata, unsigned int len, unsigned int timeout, int *erro);

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
int driver_uart_send(HAL_UART_ID_e id, unsigned char *pdata, unsigned int len,unsigned int timeout,int *erro);

/**
 *****************************************************************************
 * \fn          int driver_uart_haverxdata(HAL_UART_ID_e id)
 * \brief       读串口是否曾经接收到过数据.
 * \note        应用周期调用该函数用于判断是否点亮接收指示灯.读会清除状态.
 * \param[in]   id 指定的串口id.
 * \retval      1 串口曾经有接收数据
 * \retval      0 串口没有接收数据
 *****************************************************************************
 */
int driver_uart_haverxdata(HAL_UART_ID_e id);

/**
 *****************************************************************************
 * \fn          int driver_uart_havetxdata(HAL_UART_ID_e id)
 * \brief       读串口是否曾经发送过数据.
 * \note        应用周期调用该函数用于判断是否点亮发送指示灯.读会清除状态.
 * \param[in]   id 指定的串口id.
 * \retval      1 串口曾经有发送数据
 * \retval      0 串口没有发送数据
 *****************************************************************************
 */
int driver_uart_havetxdata(HAL_UART_ID_e id);

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
int driver_uart_geterr(HAL_UART_ID_e id,unsigned int* err);
/**
  * \}
  */

/** \defgroup DRIVER_UART_samp 驱动层(DRIVER)串口模块(UART)使用样例
  * \{
  */

/*****************************************************************************    
 *                                                                           
 *                             使用样例                                   
 *                                                                            
 ****************************************************************************/

/**
 *****************************************************************************
 * \fn          void driver_uart_samp(void)
 * \brief       使用样例.
 * \par   参见  【示例】标签下的【driver_uart_samp.c】
 *****************************************************************************
 */
void driver_uart_samp(void);

/**
 *\example driver_uart_samp.c
 * 驱动层(DRIVER)串口模块(UART)使用样例.\n
 */

/**
  * \}
  */

/**
  * \}
  */

/**
  * \}
  */

#ifdef __cplusplus
}
#endif

#endif


