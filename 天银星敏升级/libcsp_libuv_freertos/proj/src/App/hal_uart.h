/**
 *****************************************************************************        
 * \brief       硬件抽象层(HAL)串口模块(UART)相关数据结构和接口描述.
 * \details     Copyright (c) 2017,长沙威胜信息技术有限公司.
 *              All rights reserved.    
 * \file        hal_uart.h  
 * \author      李红洁W18900
 * \version     1.0 
 * \date        2017-07-23
 * \note        使用前请参考注释.\n
 *              HAL只做抽象描述,具体资源由BSP实现.\n
 * \since       李红洁      2017-7-23 14:24:11     1.0     新建 
 * \par 修订记录
 * - 2017-07-23 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */

#ifndef __HAL_UART_H
#define __HAL_UART_H

#ifdef __cplusplus
 extern "C" {
#endif

     
/** \addtogroup HAL
 *  \{
 */    
     
/** \addtogroup HAL_UART 硬件抽象层(HAL)串口模块(UART)
 *  \{
 */
/*****************************************************************************    
 *                                                                           *
 *                             数据结构描述                                  *
 *                                                                          *
 ****************************************************************************/
   

/** \defgroup HAL_UART_data 硬件抽象层(HAL)串口模块(UART)数据结构
  * \{
  */
     
typedef enum  hal_uart_txstate HAL_UART_TXSTATE_e; /**< UART 发送状态枚举定义  */
   	      
/**
 * \enum hal_uart_txstate
 * UART 发送状态.
 */
enum hal_uart_txstate
{
    HAL_UART_TXIDLE=0,      /**< UART 发送空闲 */ 
    HAL_UART_TXBUSY=1,      /**< UART 正在发送 */ 
};


typedef enum  hal_uart_enable HAL_UART_ENABLE_e; /**< UART 使能枚举定义  */
   	      
/**
 * \enum hal_uart_enable
 * UART 使能枚举定义.
 */
enum hal_uart_enable
{
    HAL_UART_DISABLE=0,     /**< UART 不使能 */ 
    HAL_UART_ENABLE=1,      /**< UART 使能 */ 
};

typedef enum  hal_uart_type HAL_UART_TYPE_e; /**< UART 处理方式枚举定义  */
   	      
/**
 * \enum hal_uart_type
 * UART 处理方式枚举定义.
 */
enum hal_uart_type
{
    HAL_UART_INTERRUPT=0,     /**< UART 中断方式 */ 
    HAL_UART_CHECK=1,         /**< UART 查询方式 */ 
};


     
typedef enum  hal_uart_id HAL_UART_ID_e; /**< UART ID枚举定义  */
   	      
/**
 * \enum hal_uart_id
 * UART ID枚举定义.
 * \note id从0开始连续递增,HAL预留足够多的id号\n
 *       实际用到的id号由BSP硬件资源实现决定.
 */
enum hal_uart_id
{
    HAL_UART_0=0,           /**< UART ID0 */ 
    HAL_UART_1=1,           /**< UART ID1 */
    HAL_UART_2=2,           /**< UART ID2 */ 
    HAL_UART_3=3,           /**< UART ID3 */
    HAL_UART_4=4,           /**< UART ID4 */ 
    HAL_UART_5=5,           /**< UART ID5 */
    HAL_UART_6=6,           /**< UART ID6 */ 
    HAL_UART_7=7,           /**< UART ID7 */
    HAL_UART_MAX=8,         /**< UART 个数 */
    HAL_UART_INV=0xFF,      /**< 无效ID */
};

typedef enum  hal_uart_parity HAL_UART_PARITY_e; /**< UART数据奇偶校验枚举定义  */
   	      
/**
 * \enum hal_uart_parity
 * UART数据奇偶校验枚举定义.
 * \note HAL描述了所有标准校验方式,\n
 *       实际用到的校验由BSP硬件资源实现决定.
 */
enum hal_uart_parity
{
    HAL_UART_PARITY_NONE=0,       /**< UART无检验 */ 
    HAL_UART_PARITY_EVEN=1,       /**< UART偶校验 */
    HAL_UART_PARITY_ODD=2,        /**< UART奇校验 */
    HAL_UART_PARITY_HIG=3,        /**< UART校验固定1  */
    HAL_UART_PARITY_LOW=4,        /**< UART校验固定0  */
};

typedef enum  hal_uart_datalen HAL_UART_DATALEN_e; /**< UART数据长度枚举定义  */
   	      
/**
 * \enum hal_uart_datalen
 * UART数据长度枚举定义.
 * \note HAL描述了所有标准数据长度,\n
 *       实际用到的数据长度由BSP硬件资源实现决定.
 */
enum hal_uart_datalen
{
    HAL_UART_DATA_9=9,       /**< UART 9位数据 */ 
    HAL_UART_DATA_8=8,       /**< UART 8位数据 */ 
    HAL_UART_DATA_7=7,       /**< UART 7位数据 */
    HAL_UART_DATA_6=6,       /**< UART 6位数据 */ 
    HAL_UART_DATA_5=5,       /**< UART 5位数据 */
};

typedef enum  hal_uart_stopb HAL_UART_STOPB_e; /**< UART停止位枚举定义  */
   	      
/**
 * \enum hal_uart_stopb
 * UART停止位枚举定义.
 * \note HAL描述了所有标准停止位长度,\n
 *       实际用到的停止位长度由BSP硬件资源实现决定.
 */
enum hal_uart_stopb
{
    HAL_UART_STOPB_1P5=0,       /**< UART 1.5位停止位 */
    HAL_UART_STOPB_1=1,         /**< UART 1位停止位 */ 
    HAL_UART_STOPB_2=2,         /**< UART 2位停止位 */
};


typedef enum  hal_uart_baud HAL_UART_BAUD_e; /**< UART波特率枚举定义  */
   	      
/**
 * \enum hal_uart_baud
 * UART波特率枚举定义.
 * \note HAL描述了常见波特率,\n
 *       实际支持的波特率由BSP硬件资源实现决定.
 */
enum hal_uart_baud
{
    HAL_UART_BAUD_600=600,        /**< UART波特率600 */ 
    HAL_UART_BAUD_1200=1200,      /**< UART波特率1200 */
    HAL_UART_BAUD_2400=2400,      /**< UART波特率2400 */ 
    HAL_UART_BAUD_4800=4800,      /**< UART波特率4800 */
    HAL_UART_BAUD_9600=9600,      /**< UART波特率9600 */ 
    HAL_UART_BAUD_19200=19200,    /**< UART波特率19200 */
    HAL_UART_BAUD_38400=38400,    /**< UART波特率38400 */ 
    HAL_UART_BAUD_57600=57600,    /**< UART波特率38400 */ 
    HAL_UART_BAUD_115200=115200,  /**< UART波特率115200 */
};

typedef struct hal_uart_cfg HAL_UART_CFG_t;  /**< UART配置结构体  */

/**
 * \struct hal_uart_cfg
 * UART配置结构体.
 */
struct hal_uart_cfg
{
    HAL_UART_ID_e       id;         /**< UART id    */
    HAL_UART_DATALEN_e  datalen;    /**< UART数据长度*/  
    HAL_UART_PARITY_e   parity;     /**< UART校验    */ 
    HAL_UART_STOPB_e    stopb;      /**< UART停止位  */ 
    HAL_UART_BAUD_e     baud;       /**< UART波特率  */   
}; 

typedef enum  hal_uart_err HAL_UART_ERR_e; /**< UART错误类型枚举定义  */
   	      
/**
 * \enum hal_uart_err
 * UART错误类型枚举定义.
 */
enum hal_uart_err
{
    HAL_UART_ERR_NONE=0,           /**< UART无错误 */ 
    HAL_UART_ERR_PER=1,            /**< UART校验错误 */ 
    HAL_UART_ERR_FER=2,            /**< UART帧错误 */
    HAL_UART_ERR_ORER=4,           /**< UART溢出错误 */ 
};

/**
 * \typedef HAL_UART_ISRCallBack
 * 中断服务回调函数.
 * - rxerrfun输入错误回调函数时,param指向错误码字节数据,不需要返回值.
 * - rxfun输入缓冲满回调函数时,param指向读到的字节数据,不需要返回值.
 * - txfun发送缓冲空回调函数,需要往param写需要发送的字节,返回值0表示还有数据需要发送 返回其他值标志本次为最后一字节数据.
 * - txcmpfun发送完回调函数,需要往param写需要发送的字节,返回值0表示还有数据需要发送 返回其他值标志本次为最后一字节数据.
 */
typedef int (*HAL_UART_ISRCallBack)(unsigned char* param) ; /**< 中断服务回调函数        */   

typedef struct hal_uart_isrcallback HAL_UART_ISRCALLBACK_t;  /**< 中断服务回调函数结构体  */

/**
 * \struct hal_uart_isrcallback
 * UART回调函数结构体.
 */
struct hal_uart_isrcallback
{
    HAL_UART_ID_e        id;         /**< UART id             */
    HAL_UART_ISRCallBack rxerrfun;   /**< 输入错误回调函数     */
    HAL_UART_ISRCallBack rxfun;      /**< 输入缓冲满回调函数   */
    HAL_UART_ISRCallBack txfun;      /**< 发送缓冲空回调函数   */
    HAL_UART_ISRCallBack txcmpfun;   /**< 发送完回调函数       */
}; 

/**
  * \}
  */
/*****************************************************************************    
 *                                                                           
 *                             接口函数描述                                   
 *                                                                            
 ****************************************************************************/

/** \defgroup HAL_UART_if 硬件抽象层(HAL)串口模块(UART)接口
  * \{
  */

/**
 *****************************************************************************
 * \fn          int HAL_UART_Init(HAL_UART_ID_e id)
 * \brief       初始化UART模块,完成模块相关必要的初始化配置.
 * \note        使用UART前必须调用,所有UART按默认配置(BSP实现).
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \retval      0 初始化成功.
 * \retval      -1 初始化失败.
 *****************************************************************************
 */
int HAL_UART_Init(HAL_UART_ID_e id);

/**
 *****************************************************************************
 * \fn          int HAL_UART_CheckSend(HAL_UART_ID_e id,unsigned char byte)
 * \brief       查询的方式发送一个字节.
 * \note        阻塞发送,直到发送完才会返回.  
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \param[in]   byte 需要发送的字节.
 * \retval      0 发送成功.
 * \retval      其他值 发送失败(实际上没有发送完会阻塞所以不会返回其他值).
 *****************************************************************************
 */
int HAL_UART_CheckSend(HAL_UART_ID_e id,unsigned char byte);

/**
 *****************************************************************************
 * \fn          int HAL_UART_CheckRead(HAL_UART_ID_e id,unsigned char* byte);
 * \brief       查询的方式读一个字节.
 * \note        阻塞读,直到有数据才会返回.  
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \param[in]   byte 存储读到的字节.
 * \retval      0 读成功.
 * \retval      其他值 读失败(实际上读不到数据会阻塞所以不会返回其他值).
 *****************************************************************************
 */
int HAL_UART_CheckRead(HAL_UART_ID_e id,unsigned char* byte);

/**
 *****************************************************************************
 * \fn          int HAL_UART_Send(HAL_UART_ID_e id,unsigned char byte)
 * \brief       直接发送一个字节.
 * \note        直接发送,直接往硬件发送缓冲写字节立即返回.  
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \param[in]   byte 需要发送的字节.
 * \retval      0 发送成功.
 * \retval      其他值 发送失败(实际上不会返回其他值).
 *****************************************************************************
 */
int HAL_UART_Send(HAL_UART_ID_e id,unsigned char byte);

/**
 *****************************************************************************
 * \fn          int HAL_UART_Read(HAL_UART_ID_e id,unsigned char* byte);
 * \brief       直接读一个字节.
 * \note        直接读,直接读硬件缓冲立即返回.  
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \param[in]   byte 存储读到的字节.
 * \retval      0 读成功.
 * \retval      其他值 读失败(实际上不会返回其他值).
 *****************************************************************************
 */
int HAL_UART_Read(HAL_UART_ID_e id,unsigned char* byte);

/**
 *****************************************************************************
 * \fn          int HAL_UART_SetTxISRCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun)
 * \brief       设置发送缓冲区空中断服务回调函数.
 * \note        .  
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \param[in]   callbackfun 回调函数.
 * \retval      0 设置成功.
 * \retval      其他值 设置失败.
 *****************************************************************************
 */
int HAL_UART_SetTxISRCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun);

/**
 *****************************************************************************
 * \fn          int HAL_UART_SetTxCmpISRCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun)
 * \brief       设置发送完中断服务回调函数.
 * \note        .  
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \param[in]   callbackfun 回调函数.
 * \retval      0 设置成功.
 * \retval      其他值 设置失败.
 *****************************************************************************
 */
int HAL_UART_SetTxCmpISRCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun);

/**
 *****************************************************************************
 * \fn          int HAL_UART_SetRxCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun)
 * \brief       设置接收缓冲满中断服务回调函数.
 * \note        .  
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \param[in]   callbackfun 回调函数.
 * \retval      0 设置成功.
 * \retval      其他值 设置失败.
 *****************************************************************************
 */
int HAL_UART_SetRxCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun);

/**
 *****************************************************************************
 * \fn          int HAL_UART_SetRxErrCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun)
 * \brief       设置接收错误中断服务回调函数.
 * \note        .  
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \param[in]   callbackfun 回调函数.
 * \retval      0 设置成功.
 * \retval      其他值 设置失败.
 *****************************************************************************
 */
int HAL_UART_SetRxErrCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun);

/**
 *****************************************************************************
 * \fn          int HAL_UART_SetCfg(HAL_UART_ID_e id,HAL_UART_CFG_t* cfg)
 * \brief       配置UART参数.
 * \note        .  
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \param[in]   cfg 指向配置参数结构体 \ref HAL_UART_CFG_t.
 * \retval      0 设置成功.
 * \retval      其他值 设置失败.
 *****************************************************************************
 */
int HAL_UART_SetCfg(HAL_UART_ID_e id,HAL_UART_CFG_t* cfg);

/**
 *****************************************************************************
 * \fn          int HAL_UART_EnableTx(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable)
 * \brief       发送使能.
 * \note        .  
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \param[in]   type 处理方式 \ref HAL_UART_TYPE_e.
 * \param[in]   enable 使能或者禁能 \ref HAL_UART_ENABLE_e.
 * \retval      0 设置成功.
 * \retval      其他值 设置失败.
 *****************************************************************************
 */
int HAL_UART_EnableTx(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable);

/**
 *****************************************************************************
 * \fn          int HAL_UART_EnableRx(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable)
 * \brief       接收使能.
 * \note        .  
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \param[in]   type 处理方式 \ref HAL_UART_TYPE_e.
 * \param[in]   enable 使能或者禁能 \ref HAL_UART_ENABLE_e.
 * \retval      0 设置成功.
 * \retval      其他值 设置失败.
 *****************************************************************************
 */
int HAL_UART_EnableRx(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable);

/**
 *****************************************************************************
 * \fn          int HAL_UART_EnableRxErr(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable)
 * \brief       接收错误中断使能.
 * \note        .  
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \param[in]   type 处理方式 \ref HAL_UART_TYPE_e.
 * \param[in]   enable 使能或者禁能 \ref HAL_UART_ENABLE_e.
 * \retval      0 设置成功.
 * \retval      其他值 设置失败.
 *****************************************************************************
 */
int HAL_UART_EnableRxErr(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable);

/**
 *****************************************************************************
 * \fn          int HAL_UART_DeInit(HAL_UART_ID_e id)
 * \brief       解除UART模块,关闭模块.
 * \note        .
 * \param[in]   id UARTid,依赖于BSP实现,\ref HAL_UART_ID_e.
 * \retval      0 成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int HAL_UART_DeInit(HAL_UART_ID_e id);

/**
  * \}
  */

/** \defgroup HAL_UART_samp 硬件抽象层(HAL)串口模块(UART)使用样例
  * \{
  */

/*****************************************************************************    
 *                                                                           
 *                             使用样例                                   
 *                                                                            
 ****************************************************************************/

/**
 *****************************************************************************
 * \fn          void hal_uart_samp(void)
 * \brief       使用样例.
 * \par   参见  【示例】标签下的【hal_uart_samp.c】
 *****************************************************************************
 */
void hal_uart_samp(void);

/**
 *\example hal_uart_samp.c
 * 硬件抽象层(HAL)串口模块(UART)使用样例.\n
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

