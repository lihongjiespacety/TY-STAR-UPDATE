/**
 *****************************************************************************        
 * \brief       操作系统应用程序接口(OSAPI)相关数据结构和接口描述.
 * \details     Copyright (c) 2017,长沙威胜信息技术有限公司.
 *              All rights reserved.    
 * \file        osapi.h  
 * \author      李红洁W18900
 * \version     1.0 
 * \date        2017-08-23
 * \note        使用前请参考注释.\n
 *              对常见OS接口函数进行封装.\n
 * \since       李红洁      2017-8-23 09:49:20     1.0     新建 
 * \par 修订记录
 * - 2017-08-23 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */

#ifndef __OSAPI_H
#define __OSAPI_H

#ifdef __cplusplus
 extern "C" {
#endif
     
#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"
#include "task.h"
#include <stdarg.h>
#include "csp.h"
/** \addtogroup OSAPI 操作系统层(OSAPI)
 *  \{
 */    
     
/*****************************************************************************    
 *                                                                           *
 *                             数据结构描述                                  *
 *                                                                          *
 ****************************************************************************/
   

/** \defgroup OSAPI_data 操作系统层(OSAPI)相关数据结构
  * \{
  */
     
#define TSKNAMELEN 16                       /**< 任务名字长度           */
#define TICKS_PER_SEC configTICK_RATE_HZ    /**< 系统滴答频率           */
     
   typedef struct 
   {
     StaticQueue_t queue;
     void* data;
   }OSMSGQObj,*OSMSGQObj_p;  /*继承自StaticQueue_t 增加data区域存储队列数据*/
   
typedef StaticTask_t   TSK_TCB;            /**< ostcb数据类型           */
typedef StackType_t    STK_TYPE;           /**< 堆栈数据类型           */
typedef void (*TASK_ENTRY_fp)(void *arg); /**< 指向任务函数入口的指针  */
typedef struct tsk_cfg TSK_CFG_t;         /**< 任务创建参数结构体      */
typedef struct QueueDefinition     OSSemObj;    /**< 信号量对象              */
typedef QueueHandle_t    OSSemObj_p;  /**< 信号量对象指针类型      */
typedef struct QueueDefinition     OSMutexObj;  /**< 互斥量对象             */
typedef QueueHandle_t *   OSMutexObj_p;/**< 互斥量对象指针类型      */
typedef StaticTimer_t         OSTimerObj;  /**< 定时器对象             */
typedef StaticTimer_t *       OSTimerObj_p;/**< 定时器对象指针类型      */
//typedef StaticQueue_t         OSMSGQObj;   /**< 消息队列对象             */
//typedef StaticQueue_t *       OSMSGQObj_p; /**< 消息队列对象指针类型      */


typedef void(*OSAPI_TIMER_CALLBACK)(void*,void*);   /**< 定时器回调函数  */   
typedef enum  osapi_timer_type OSAPI_TIMER_TYPE_e;   /**< 定时器处理方式枚举定义  */      
/**
 * \enum osapi_timer_type
 * 定时器处理方式枚举定义.
 */
enum osapi_timer_type
{
    OSAPI_TIMIER_SINGLE=0,      /**< TIMER 单次方式 */ 
    OSAPI_TIMIER_CONTINU=1,     /**< TIMER 连续方式 */ 
};


/**
 * \enum osapi_debug_e
 * 调试输出等级.
 */
typedef enum 
{
    OSAPI_DEBUG_OFF =0,      /**< */ 
    OSAPI_DEBUG_INFO=1,     /**<  */ 
    OSAPI_DEBUG_DEBUG=2,
    OSAPI_DEBUG_WARM=3,
    OSAPI_DEBUG_ERR=4,
}osapi_debug_e;


/**
 * \struct tsk_cfg
 * 任务创建参数结构体定义.
 * 用于传入给任务创建函数.
 */
struct tsk_cfg
{
    unsigned int   id;         /**< 任务id         */
    unsigned int   pri;        /**< 任务优先级     */
    STK_TYPE*       tskbase;    /**< 任务堆栈地址   */
    unsigned int   tsksize;    /**< 任务堆栈大小   */
    TASK_ENTRY_fp   taskp;      /**< 任务入口       */
    unsigned int   robin;      /**< 时间片       */
    void*           arg;        /**< 任务入口参数   */
    unsigned int   opt;        /**< 可选配置     */
    char name[TSKNAMELEN];      /**< 任务名字       */
    StaticTask_t*   tcb;        /**< 指向任务控制块  */
}; 

typedef struct 
{
    unsigned int addr;
	unsigned int len;
}OSAPI_MSGQ_FORM;
/**
  * \}
  */
/*****************************************************************************    
 *                                                                           
 *                             接口函数描述                                   
 *                                                                            
 ****************************************************************************/

/** \defgroup OSAPI_if 操作系统层(OSAPI)相关接口
  * \{
  */

#define OSCriticalAlloc()         /**< 准备进入临界段       */
#define OSCriticalEnter() taskENTER_CRITICAL()    /**< 进入临界段           */
#define OSCriticalExit()  taskEXIT_CRITICAL()	  /**< 退出临界段           */

/**
 *****************************************************************************
 * \fn          int OsMemcmp(void *pDst, void *pSrc, unsigned int nNumBytes)
 * \brief       内存比较.
 * \param[in]   pDst 目的地址.
 * \param[in]   pSrc 源地址.
 * \param[in]   nNumBytes 比较字节数.
 * \note        .  
 * \retval      0 相等
 * \retval      >0 目的大于源
 * \retval      <0 目的小于源
 *****************************************************************************
 */
int OsMemcmp(void *pDst, void *pSrc, unsigned int nNumBytes);


/**
 *****************************************************************************
 * \fn          void OsMemcpy(void *pDst, void *pSrc, unsigned int nNumBytes)
 * \brief       内存复制.
 * \param[in]   pDst 目的地址.
 * \param[in]   pSrc 源地址.
 * \param[in]   nNumBytes 复制的字节数.
 * \note        .  
 *****************************************************************************
 */
void OsMemcpy(void *pDst, void *pSrc, unsigned int nNumBytes);

/**
 *****************************************************************************
 * \fn          void OsMemset(void *pMem, unsigned char iChar, unsigned int nNumBytes)
 * \brief       填充内存.
 * \param[in]   pMem 需要填充的地址.
 * \param[in]   iChar 需要填充的值.
 * \param[in]   nNumBytes 需要填充的字节数.
 * \note        .  
 *****************************************************************************
 */
void OsMemset(void *pMem, unsigned char iChar, unsigned int nNumBytes);

/**
 *****************************************************************************
 * \fn          void* OsMalloc(unsigned int nNumBytes)
 * \brief       分配动态内存.
 * \param[in]   nNumBytes 需要获取的字节数.
 * \note        .  
 * \return      指向获取的内存 返回0代表获取失败
 *****************************************************************************
 */
void* OsMalloc(unsigned int nNumBytes);

/**
 *****************************************************************************
 * \fn          void OsFree(void *pMem)
 * \brief       释放动态内存.
 * \note        .  
 * \param[in]   pMem 指向需要释放的内存.
 *****************************************************************************
 */
void OsFree(void *pMem);

/**
 *****************************************************************************
 * \fn          int OsInit()
 * \brief       初始化系统.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsInit();

/**
 *****************************************************************************
 * \fn          int OsInit()
 * \brief       启动系统.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsStart();
/**
 *****************************************************************************
 * \fn          int OsTaskCreate(TSK_CFG_t* cfg)
 * \brief       创建任务.
 * \note        .  
 * \param[in]   cfg 指向任务参数结构体的指针 \ref TSK_CFG_t.
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsTaskCreate(TSK_CFG_t* cfg);

/**
 *****************************************************************************
 * \fn          int OsTimeDelay(unsigned int milsec);
 * \brief       任务延时.
 * \note        .  
 * \param[in]   milsec 延时时间单位mS.
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsTimeDelay( unsigned int milsec );


/**
 *****************************************************************************
 * \fn          unsigned int OsTimeGetTick(void)
 * \brief       获取系统滴答计数器.
 * \note        .  
 * \retval      unsigned int 返回滴答计数器值
 *****************************************************************************
 */
unsigned int OsTimeGetTick(void);

/**
 *****************************************************************************
 * \fn          int OsInitInFirstTask(void);
 * \brief       第一个任务启动相应的初始化操作.
 * \note        初始化系统时钟,统计任务等.  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsInitInFirstTask(void);

/**
 *****************************************************************************
 * \fn          unsigned int OsTaskGetId(void)
 * \brief       获取当前任务ID.
 * \note        . 
 * \return      unsigned int 返回当前任务的ID.
 *****************************************************************************
 */
unsigned int OsTaskGetId(void);

/**
 *****************************************************************************
 * \fn          unsigned int OsTaskGetSwCount(unsigned char id)
 * \brief       获取指定ID任务的调度次数.
 * \param[in]   id 任务ID.
 * \note        . 
 * \return      unsigned int 返回任务的调度次数.
 *****************************************************************************
 */
unsigned int OsTaskGetSwCount(unsigned int id);

/**
 *****************************************************************************
 * \fn          void OsSetTaskKillReq(unsigned char id)
 * \brief       设置删除任务请求.
 * \param[in]   id 任务id号,不能超过31.
 * \note        .  
 *****************************************************************************
 */
void OsSetTaskKillReq(unsigned char id);

/**
 *****************************************************************************
 * \fn          int OsGetTaskKillReq(unsigned char id)
 * \brief       获取删除任务请求.
 * \param[in]   id 任务id号,不能超过31.
 * \note        . 
 * \retval      0  无请求.
 * \retval      1  有请求.
 *****************************************************************************
 */
int OsGetTaskKillReq(unsigned char id);

/**
 *****************************************************************************
 * \fn          int OsTaskKillSelf(void)
 * \brief       任务删除自己.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsTaskKillSelf(void);

/**
 *****************************************************************************
 * \fn          int OsSuspendTask(unsigned int id)
 * \brief       挂起指定ID的任务.
 * \param[in]   id 任务ID.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsSuspendTask(unsigned int id);

/**
 *****************************************************************************
 * \fn          int OsResumeTask(unsigned int id)
 * \brief       恢复指定ID的任务.
 * \param[in]   id 任务ID.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsResumeTask(unsigned int id);

/**
 *****************************************************************************
 * \fn          OSSemObj* OsSemCreate(unsigned char *name, unsigned char count)
 * \brief       创建信号量.
 * \param[in]   name 信号量名字 必须传入全局数据或者const数据.
 * \param[in]   count 信号量初始值.
 * \note        信号量实例由malloc而来.  
 * \return      OSSemObj*  返回创建的信号量指针,如果为0表示创建失败.
 *****************************************************************************
 */
OSSemObj_p OsSemCreate(unsigned char *name, unsigned int count);

/**
 *****************************************************************************
 * \fn          int OsSemPend(OSSemObj* psem, unsigned int timeout )
 * \brief       等待信号量.
 * \param[in]   psem 信号量指针 必须是已经创建的信号量.
 * \param[in]   timeout 等待超时时间单位mS.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsSemPend(OSSemObj_p psem, unsigned int timeout);

/**
 *****************************************************************************
 * \fn          int OsSemPost(OSSemObj* psem )
 * \brief       发送信号量.
 * \param[in]   psem 信号量指针 必须是已经创建的信号量.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsSemPost(OSSemObj_p psem );

/**
 *****************************************************************************
 * \fn          int OsSemDel(OSSemObj* psem )
 * \brief       删除信号量.
 * \param[in]   psem 信号量指针 必须是已经创建的信号量.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsSemDel(OSSemObj_p psem );

/**
 *****************************************************************************
 * \fn          OSMutexObj_p OsMutexCreate(char *name)
 * \brief       创建互斥量.
 * \param[in]   name 互斥量名字 必须传入全局数据或者const数据.
 * \note        互斥量量实例由malloc而来.  
 * \return      OSMutexObj_p  返回创建的互斥量指针,如果为0表示创建失败.
 *****************************************************************************
 */
OSMutexObj_p OsMutexCreate(char *name);

/**
 *****************************************************************************
 * \fn          int OsMutexPend(OSMutexObj_p pmutex, unsigned int timeout)
 * \brief       等待互斥量.
 * \param[in]   pmutex 互斥量指针 必须是已经创建的互斥量.
 * \param[in]   timeout 等待超时时间单位mS.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsMutexPend(OSMutexObj_p pmutex, unsigned int timeout);

/**
 *****************************************************************************
 * \fn          int OsMutexPost(OSMutexObj_p pmutex)
 * \brief       发送互斥量.
 * \param[in]   pmutex 互斥量指针 必须是已经创建的互斥量.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsMutexPost(OSMutexObj_p pmutex);

/**
 *****************************************************************************
 * \fn          int OsMutexDel(OSMutexObj_p pmutex)
 * \brief       删除互斥量.
 * \param[in]   pmutex 互斥量量指针 必须是已经创建的互斥量.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsMutexDel(OSMutexObj_p pmutex);

/**
 *****************************************************************************
 * \fn          OSMSGQObj_p OsMsgQCreate(char *name,unsigned int size)
 * \brief       创建消息队列.
 * \param[in]   name 消息队列名字 必须传入全局数据或者const数据.
 * \param[in]   size 消息队列最大消息条数.
 * \note        消息队列实例由malloc而来.  
 * \return      OSMSGQObj_p  返回创建的消息队列指针,如果为0表示创建失败.
 *****************************************************************************
 */
OSMSGQObj_p OsMsgQCreate(char *name,unsigned int size);

/**
 *****************************************************************************
 * \fn          int OsMsgQRcv(OSMSGQObj_p pMsgQ, unsigned char *buffer, unsigned int bufsize,unsigned int* size,unsigned int timeout)
 * \brief       从消息队列接收消息.
 * \param[in]   pMsgQ 消息队列指针 必须指向已经创建的消息队列.
 * \param[in]   buffer 接收缓冲区.
 * \param[in]   bufsize 接收缓冲区大小.
 * \param[in]   timeout 超时时间单位mS.
 * \param[out]  size 实际接收的消息内容大小.
 * \note        消息队列存储由malloc而来.  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */ 
int OsMsgQRcv(OSMSGQObj_p pMsgQ, unsigned char *buffer, unsigned int bufsize,unsigned int* size,unsigned int timeout);


/**
 *****************************************************************************
 * \fn          int OsMsgQSnd(OSMSGQObj_p pMsgQ, unsigned char *buffer, unsigned size)
 * \brief       往消息队列发送消息.
 * \param[in]   pMsgQ 消息队列指针 必须指向已经创建的消息队列.
 * \param[in]   buffer 消息内容.
 * \param[in]   size   消息内容的长度.
 * \note        消息队列存储由malloc而来.  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */ 
int OsMsgQSnd(OSMSGQObj_p pMsgQ, unsigned char *buffer, unsigned size);

   /**
 *****************************************************************************
 * \fn          int OsMsgQDel(OSMSGQObj_p pMsgQ)
 * \brief       删除消息队列.
 * \param[in]   pMsgQ 消息队列指针 必须指向已经创建的消息队列.
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */ 
int OsMsgQDel(OSMSGQObj_p pMsgQ);
/**
 *****************************************************************************
 * \fn          OSTimerObj_p OsTimerCreate(unsigned int time,OSAPI_TIMER_TYPE_e mode,OSAPI_TIMER_CALLBACK callback,char *name)
 * \brief       创建定时器.
 * \param[in]   time 定时器周期
 * \param[in]   mode 定时器模式 \ref OSAPI_TIMER_TYPE_e.
 * \param[in]   callback 定时器回调函数 \ref OSAPI_TIMER_CALLBACK.
 * \param[in]   name 定时器名字.
 * \note        定时器实例由malloc而来.  
 * \return      OSMutexObj_p  返回创建的互斥量指针,如果为0表示创建失败.
 *****************************************************************************
 */
OSTimerObj_p OsTimerCreate(unsigned int time,OSAPI_TIMER_TYPE_e mode,OSAPI_TIMER_CALLBACK callback,char *name);

/**
 *****************************************************************************
 * \fn          int OsTimerStart(OSTimerObj_p ptimer)
 * \brief       启动定时器.
 * \param[in]   ptimer 定时器指针 必须是已经创建的定时器.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsTimerStart(OSTimerObj_p ptimer);

/**
 *****************************************************************************
 * \fn          int OsTimerStop(OSTimerObj_p ptimer)
 * \brief       停止定时器.
 * \param[in]   ptimer 定时器指针 必须是已经创建的定时器.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsTimerStop(OSTimerObj_p ptimer);

/**
 *****************************************************************************
 * \fn          int OsTimerDelete(OSTimerObj_p ptimer)
 * \brief       删除定时器.
 * \param[in]   ptimer 定时器指针 必须是已经创建的定时器.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsTimerDelete(OSTimerObj_p ptimer);

/**
 *****************************************************************************
 * \fn          char* OsGetTaskName(unsigned int id)
 * \brief       获取指定ID的任务名.
 * \param[in]   id 任务id号.
 * \note        .  
 * \return      返回指向任务名字的指针.
 *****************************************************************************
 */
char* OsGetTaskName(unsigned int id);

/**
 *****************************************************************************
 * \fn          int OsGetTaskId(char* name,unsigned int* id)
 * \brief       获取指定名字任务的ID.
 * \param[in]   name 任务名字.
 * \param[out]  id   获取到的ID.
 * \note        .  
 * \return      unsigned int 返回任务的ID.
 *****************************************************************************
 */
int OsGetTaskId(char* name,unsigned int* id);

/**
 *****************************************************************************
 * \fn          int OsCheckTaskId(unsigned int id);
 * \brief       检查任务的ID是否有效.
 * \param[out]  id   任务的ID.
 * \note        .  
 * \retval      0 有效.
 * \retval      -1 无效.
 *****************************************************************************
 */
int OsCheckTaskId(unsigned int id);

/**
 *****************************************************************************
 * \fn          int OsGetTaskFreeStack(unsigned int id)
 * \brief       获取指定ID任务的堆栈剩余量.
 * \param[in]   id 任务ID.
 * \retval      >=0 返回任务剩余堆栈.
 * \retval      -1 指定ID任务不存在.
 * \note        .  
 *****************************************************************************
 */
int OsGetTaskFreeStack(unsigned int id);

/**
*****************************************************************************
* \fn          void OsSetTaskDebug(unsigned int flag,osapi_debug_e level)
* \brief       设置任务调试输出标志.
* \param[in]   id 任务号,某一位为1表示指定ID任务允许输出.
* \param[in]   level \ref osapi_debug_e.
* \note        .  
*****************************************************************************
*/
void OsSetTaskDebug(unsigned int id,osapi_debug_e level);
/**
*****************************************************************************
* \fn          void OsClrTaskDebug(unsigned int flag,osapi_debug_e level)
* \brief       清除任务调试输出标志.
* \param[in]   id 任务号,某一位为1表示指定ID任务不允许输出.
* \param[in]   level \ref osapi_debug_e.
* \note        .  
*****************************************************************************
*/
void OsClrTaskDebug(unsigned int id,osapi_debug_e level);



/**
 *****************************************************************************
 * \fn          int OsPrintf(osapi_debug_e level,const char *format, ...)
 * \brief       打印任务调试信息:打印格式化输出信息.
 * \param[in]   format 格式化字符串, 与printf参数一致.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsPrintf(osapi_debug_e level,const char *format, ...);

/**
 *****************************************************************************
 * \fn          int OsPrintf_va(csp_color_t color,osapi_debug_e level,const char *format, va_list arg)
 * \brief       打印任务调试信息:打印格式化输出信息.
 * \param[in]   format 格式化字符串, 与printf参数一致.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsPrintf_va(csp_color_t color,osapi_debug_e level,const char *format, va_list arg);

/**
 *****************************************************************************
 * \fn          int OsPrintfBuf(char *name,osapi_debug_e level, unsigned char * buff, unsigned int length)
 * \brief       打印任务调试信息:打印内存信息.
 * \param[in]   name 打印标签.
 * \param[in]   buff 内存区域.
 * \param[in]   length 缓冲区长度.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsPrintfBuf(char *name,osapi_debug_e level, unsigned char * buff, unsigned int length);


/**
 *****************************************************************************
 * \fn          int OsPrintfTaskInfo(void)
 * \brief       打印任务信息.
 * \note        .  
 *****************************************************************************
 */
void OsPrintfTaskInfo(void);

/**
 *****************************************************************************
 * \fn          void OsPrintfSemInfo(void)
 * \brief       打印信号量信息.
 * \note        .  
 *****************************************************************************
 */
void OsPrintfSemInfo(void);

/**
 *****************************************************************************
 * \fn          void OsPrintfMsgInfo(void)
 * \brief       打印消息队列信息.
 * \note        .  
 *****************************************************************************
 */
void OsPrintfMsgInfo(void);

/**
 *****************************************************************************
 * \fn          void OsPrintfMutexInfo(void)
 * \brief       打印互斥量信息.
 * \note        .  
 *****************************************************************************
 */
void OsPrintfMutexInfo(void);

/**
 *****************************************************************************
 * \fn          void OsPrintfTimerInfo(void)
 * \brief       打印定时器信息.
 * \note        .  
 *****************************************************************************
 */
void OsPrintfTimerInfo(void);

/**
  * \}
  */

/**
 *****************************************************************************
 * \fn          unsigned int OsGetTs(void)
 * \brief       获取时间戳.
 * \note        .  
 *****************************************************************************
 */
unsigned int OsGetTs(void);

/**
 *****************************************************************************
 * \fn          void OsLock(void)
 * \brief       任务调度锁定.
 * \note        .  
 *****************************************************************************
 */
void OsLock(void);

/**
 *****************************************************************************
 * \fn          void OsUnLock(void)
 * \brief       解除任务调度锁定.
 * \note        .  
 *****************************************************************************
 */
void OsUnLock(void);

/**
 *****************************************************************************
 * \fn          void OsEnterInt(void)
 * \brief       进入中断.
 * \note        .  
 *****************************************************************************
 */
void OsEnterInt(void);

/**
 *****************************************************************************
 * \fn          void OsExitInt(void)
 * \brief       退出中断.
 * \note        .  
 *****************************************************************************
 */
void OsExitInt(void);

/** \defgroup OSAPI_samp 操作系统层(OSAPI)使用样例
  * \{
  */

/*****************************************************************************    
 *                                                                           
 *                             使用样例                                   
 *                                                                            
 ****************************************************************************/

/**
 *****************************************************************************
 * \fn          void osapi_samp(void)
 * \brief       使用样例.
 * \par   参见  【示例】标签下的【osapi_samp.c】
 *****************************************************************************
 */
void osapi_samp(void);

/**
 *\example osapi_samp.c
 * 操作系统层(OSAPI)使用样例.\n
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