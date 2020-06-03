/**
 *****************************************************************************        
 * \brief       硬件抽象层(HAL)串口(UART)相关接口实现.
 * \details     Copyright (c) 2017,长沙威胜信息技术有限公司.
 *              All rights reserved.    
 * \file        hal_uart.c
 * \author      李红洁W18900
 * \version     1.0 
 * \date        2017-07-24
 * \note        使用前请参考注释.\n
 *              调用BSP实现.\n
 * \since       李红洁      2017-7-24 13:52:37     1.0     新建 
 * \par 修订记录
 * - 2017-07-24 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */
#include "hal.h"
#include "bsp.h"

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
int HAL_UART_Init(HAL_UART_ID_e id)
{
    return BSP_SCI_Init(id);
}

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
int HAL_UART_CheckSend(HAL_UART_ID_e id,unsigned char byte)
{
    return BSP_SCI_CheckSend(id,byte);
}

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
int HAL_UART_CheckRead(HAL_UART_ID_e id,unsigned char* byte)
{
    return BSP_SCI_CheckRead(id,byte);
}

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
int HAL_UART_Send(HAL_UART_ID_e id,unsigned char byte)
{
    return BSP_SCI_Send(id,byte);
}

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
int HAL_UART_Read(HAL_UART_ID_e id,unsigned char* byte)
{
    return BSP_SCI_Read(id,byte);
}

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
int HAL_UART_SetTxISRCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun)
{
    return BSP_SCI_SetTxCallBack(id,callbackfun);
}

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
int HAL_UART_SetTxCmpISRCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun)
{
    return BSP_SCI_SetTxCompleteCallBack(id,callbackfun);
}

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
int HAL_UART_SetRxCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun)
{
    return BSP_SCI_SetRxCallBack(id,callbackfun);
}

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
int HAL_UART_SetRxErrCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun)
{
    return BSP_SCI_SetRxErrCallBack(id,callbackfun);
}

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
int HAL_UART_SetCfg(HAL_UART_ID_e id,HAL_UART_CFG_t* cfg)
{
    return BSP_SCI_SetCfg(id,cfg);
}

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
int HAL_UART_EnableTx(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable)
{
    return BSP_SCI_EnableTx(id,type,enable);
}

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
int HAL_UART_EnableRx(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable)
{
    return BSP_SCI_EnableRx(id,type,enable);
}

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
int HAL_UART_EnableRxErr(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable)
{
    return BSP_SCI_EnableRxErr(id,type,enable);
}

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
int HAL_UART_DeInit(HAL_UART_ID_e id)
{
    return BSP_SCI_DeInit(id);
}