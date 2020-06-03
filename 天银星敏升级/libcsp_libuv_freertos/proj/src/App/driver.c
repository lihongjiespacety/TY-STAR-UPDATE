/**
 *****************************************************************************        
 * \brief       驱动层(DRIVER)DRIVER模块(DRIVER)相关接口实现.
 * \details     Copyright (c) 2017,长沙威胜信息技术有限公司.
 *              All rights reserved.    
 * \file        driver.c  
 * \author      李红洁W18900
 * \version     1.0 
 * \date        2017-10-20
 * \note        使用前请参考注释.\n
 *              依赖于HAL和OS实现.\n
 * \since       李红洁      2017-10-20 16:35:35    1.0     新建 
 * \par 修订记录
 * - 2017-10-20 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */
#include "driver.h"


#define DRIVER_VERSION 0x10    /**< DRIVER层版本数据高四位代表大版本号,低四位代表小版本号。比如0x10代表版本1.0 */

/**
 *****************************************************************************
 * \fn          int driver_init(void)
 * \brief       初始化DRIVER层.
 * \note        系统调用该函数完成最驱动的初始化.
 * \retval      0  初始化成功.
 * \retval      -1 初始化失败.
 *****************************************************************************
 */
int driver_init(void)
{
  /*driver_uart和driver_iruart不在此初始化,使用时载初始化*/
  //driver_flashif_init();
  //driver_iap_init(); 
  //driver_adc_init(); 
  //driver_gpio_init();
  //driver_led_init();
  driver_time_init();
  //driver_esam_init();  esam一般不是所有产品都有,而且模块较独立,所以在需要时再调用初始化,不在此调用
  return 0;
}

/**
 *****************************************************************************
 * \fn          unsigned int driver_getversion(void)
 * \brief       获取HAL依赖的BSP版本.
 * \note        .  
 * \return      unsigned int 返回版本信息 \ref unsigned 
 *****************************************************************************
 */
unsigned int driver_getversion(void)
{
    return DRIVER_VERSION;
}