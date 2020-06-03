/**
 *****************************************************************************        
 * \brief       标准输入输出底层接口重定向.
 * \details     Copyright (c) 2017,长沙威胜信息技术有限公司.
 *              All rights reserved.    
 * \file        retarget.h 
 * \author      李红洁W18900
 * \version     1.0 
 * \date        2017-07-26
 * \note        使用前请参考注释.\n
 *              实现供printf scanf调用的底层接口.\n
 * \since       李红洁      2017-7-26 09:58:25     1.0     新建 
 * \par 修订记录
 * - 2017-07-26 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */

#ifndef __RETARGET_H
#define __RETARGET_H

#ifdef __cplusplus
 extern "C" {
#endif
     
#include "hal.h"
/** \addtogroup PLATFORM PLATFORM
 *  \{
 */
     
/** \addtogroup PLATFORM_RETARGET PLATFORM_RETARGET
 *  \{
 */ 
     
/*****************************************************************************    
 *                                                                           
 *                             数据结构描述                                   
 *                                                                            
 ****************************************************************************/

/** \defgroup PLATFORM_RETARGET_data PLATFORM_RETARGET_data
  * \{
  */
    
 /**
  * \}
  */
   
   
/*****************************************************************************    
 *                                                                           
 *                             接口函数描述                                   
 *                                                                            
 ****************************************************************************/

/** \defgroup PLATFORM_RETARGET_if PLATFORM_RETARGET_if
  * \{
  */
     
/**
 *****************************************************************************
 * \fn          void setstdiouart(HAL_UART_ID_e id)
 * \brief       设置stdio使用的串口.
 * \note        默认端口无效,需要调用该函数去映射实际对应的端口. 
 * \param[in]   id stdio映射到的物理串口号,如果不需要使用stdio函数,端口设置为HAL_UART_MAX
 *****************************************************************************
 */
void setstdiouart(HAL_UART_ID_e id);

/**
 *****************************************************************************
 * \fn          HAL_UART_ID_e getstdiouart(void)
 * \brief       获取stdio使用的串口.
 * \note        默认端口无效,需要调用setstdiouart去映射实际对应的端口. 
 * \return      返回使用的串口ID \ref HAL_UART_ID_e
 *****************************************************************************
 */
HAL_UART_ID_e getstdiouart(void);

/**
  * \}
  */

/** \defgroup PLATFORM_RETARGET_samp PLATFORM_RETARGET_samp
  * \{
  */

/*****************************************************************************    
 *                                                                           
 *                             使用样例                                   
 *                                                                            
 ****************************************************************************/

/**
 *****************************************************************************
 * \fn          void platform_retarget_samp(void)
 * \brief       使用样例.
 * \par   参见  platform_retarget_samp.c】
 *****************************************************************************
 */
void platform_retarget_samp(void);

/**
 *\example platform_retarget_samp.c
 * 平台层(PLATFORM)标准输入输出重定向模块(RETARGET)使用样例.\n
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