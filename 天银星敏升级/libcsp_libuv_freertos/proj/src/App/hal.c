/**
 *****************************************************************************        
 * \brief       硬件抽象层(HAL)HAL模块(HAL)相关接口实现.
 * \details     Copyright (c) 2017,长沙威胜信息技术有限公司.
 *              All rights reserved.    
 * \file        hal.c  
 * \author      李红洁W18900
 * \version     1.0 
 * \date        2017-10-17
 * \note        使用前请参考注释.\n
 *              HAL只做抽象描述,具体资源由BSP实现.\n
 * \since       李红洁      2017-10-17 10:56:52    1.0     新建 
 * \par 修订记录
 * - 2017-10-17 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */

#include "hal.h"
#include "bsp.h"

/**
 *****************************************************************************
 * \fn          int HAL_Init(void)
 * \brief       初始化HAL模块,包括必要的BSP初始化.
 * \note        系统调用该函数完成最底层的初始化.
 * \retval      0  初始化成功.
 * \retval      -1 初始化失败.
 *****************************************************************************
 */
int HAL_Init(void)
{
    int res = 0;
    /*1.BSP初始化*/
    res = BSP_Init();
    /*2.HAL初始化*/
    return res;
}

/**
 *****************************************************************************
 * \fn          unsigned int HAL_GetBSPVersion(void)
 * \brief       获取HAL依赖的BSP版本.
 * \note        .  
 * \return      unsigned int 返回版本信息 \ref HAL_VERSION 
 *****************************************************************************
 */
unsigned int HAL_GetBSPVersion(void)
{
    return BSP_GetVersion();
}

/**
 *****************************************************************************
 * \fn          unsigned int HAL_GetHALVersion(void)
 * \brief       获取HAL版本.
 * \note        .  
 * \return      unsigned int 返回版本信息 \ref HAL_VERSION 
 *****************************************************************************
 */
unsigned int HAL_GetHALVersion(void)
{
    return HAL_VERSION;
}

/**
 *****************************************************************************
 * \fn          int HAL_DeInit(void)
 * \brief       解除HAL模块初始化.
 * \note        .
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int HAL_DeInit(void)
{
    int res = 0;
    res = BSP_DeInit();
    return res;
}