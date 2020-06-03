/**
 *****************************************************************************        
 * \brief       平台相关(PLATFORM)SHELL模块(SHELL)相关数据结构和接口描述.
 * \details     Copyright (c) 2017,长沙威胜信息技术有限公司.
 *              All rights reserved.    
 * \file        shell.h  
 * \author      李红洁W18900
 * \version     1.0 
 * \date        2017-08-26
 * \note        使用前请参考注释.\n
 * \since       李红洁      2017-8-26 11:10:35     1.0     新建 
 * \par 修订记录
 * - 2017-08-26 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */

#ifndef _SHELL_H_
#define _SHELL_H_

#ifdef __cplusplus
extern "C" {
#endif
     
/** \addtogroup PLATFORM PLATFORM
 *  \{
 */
         
/** \addtogroup PLATFORM_SHELL PLATFORM_SHELL
 *  \{
 */
/*****************************************************************************    
 *                                                                           *
 *                             数据结构描述                                  *
 *                                                                          *
 ****************************************************************************/
     
/** \defgroup PLATFORM_SHELL_data PLATFORM_SHELL_data
  * \{
  */

typedef void ( * CommandFunc )( void *);   /**< shell命令回调函数 */

/**
 * \struct shell_cmd_cfg
 * SHELL命令结构体.
 */
typedef struct
{
    unsigned char const* name;           /**< shell命令名 */
    CommandFunc func;           /**< shell命令回调函数 */
    char const* helpstr;       /**< 帮助字符串 */
}shell_cmd_cfg;

#define SHELL_PASSWORD_STR "DSWYBS"  /**< shell密码固定6个字符  */
#define SHELL_CMDBUF_SIZE  64         /**< shell命令缓冲区大小  */


/**
  * \}
  */
     
/*****************************************************************************    
 *                                                                           
 *                             接口函数描述                                   
 *                                                                            
 ****************************************************************************/

/** \defgroup PLATFORM_SHELL_if PLATFORM_SHELL_if
  * \{
  */
    
/**
 *****************************************************************************
 * \fn          void shell_exec_shellcmd(void)
 * \brief       执行shell命令.
 * \note        任务周期调用该函数. 
 *****************************************************************************
 */
void shell_exec_shellcmd(void);
/**
 *****************************************************************************
 * \fn          int shell_exec_cmdlist(unsigned char* cmd)
 * \brief       搜索命令列表并执行.
 * retval       0 成功
 * retval       -11 失败
 * \note        . 
 *****************************************************************************
 */
int shell_exec_cmdlist(unsigned char* cmd);
/**
  * \}
  */

/** \defgroup PLATFORM_SHELL_samp PLATFORM_SHELL_samp
  * \{
  */

/*****************************************************************************    
 *                                                                           
 *                             使用样例                                   
 *                                                                            
 ****************************************************************************/

/**
 *****************************************************************************
 * \fn          void platform_shell_samp(void)
 * \brief       使用样例.
 * \par   参见  【示例】标签下的【platform_shell_samp.c】
 *****************************************************************************
 */
void platform_shell_samp(void);

/**
 *\example platform_shell_samp.c
 * 平台层(PLATFORM)SHELL模块(SHELL)使用样例.\n
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
