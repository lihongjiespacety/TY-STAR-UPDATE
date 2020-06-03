/**
 *****************************************************************************        
 * \brief       平台层(PLATFORM)SHELL命令模块(SHELL_FUN)相关数据结构和接口描述.
 * \details     Copyright (c) 2017,长沙威胜信息技术有限公司.
 *              All rights reserved.    
 * \file        shell_fun.h 
 * \author      李红洁W18900
 * \version     1.0 
 * \date        2017-11-02
 * \note        平台相关命令.\n
 * \since       李红洁     2017年11月2日13:27:57   1.0     新建 
 * \par 修订记录
 * - 2017-11-02 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */

#ifndef __SHELL_FUN_H
#define __SHELL_FUN_H

#ifdef __cplusplus
 extern "C" {
#endif

/*****************************************************************************    
 *                                                                           
 *                             帮助相关                                  
 *                                                                            
 ****************************************************************************/   
void HelpFun(void* param);

/*****************************************************************************    
 *                                                                           
 *                             操作系统相关                                  
 *                                                                            
 ****************************************************************************/
void ResetFun(void* cmdbuf);
void SetCanidFun(void* cmdbuf);
void ShowPsFun(void * cmdbuf);
void ShowSemFun(void * cmdbuf);
void ShowMsgFun(void * cmdbuf);
void ShowMutexFun(void * cmdbuf);
void ShowTimerFun(void * cmdbuf);
void ShowHeapInfoFun(void * cmdbuf);
void ShowMem(void * cmdbuf);
void SetdClrdFun(void * cmdbuf);
void SetCspDebugFun(void * cmdbuf);
void SetTimeFun(void * cmdbuf);
void ShowTimeFun(void * cmdbuf);
void RunTimeFun(void * cmdbuf);
#ifdef __cplusplus
}
#endif

#endif