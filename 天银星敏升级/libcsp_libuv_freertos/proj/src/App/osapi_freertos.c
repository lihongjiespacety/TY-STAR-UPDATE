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


#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include "driver.h"
#include "osapi_freertos.h"
#include "task.h"
#include "csp.h"

#define tskSTACK_FILL_BYTE	( 0xa5U )

extern PRIVILEGED_DATA  List_t pxReadyTasksLists[ configMAX_PRIORITIES ];
extern PRIVILEGED_DATA  List_t xDelayedTaskList1;						
extern PRIVILEGED_DATA  List_t xDelayedTaskList2;						
extern PRIVILEGED_DATA  List_t * volatile pxDelayedTaskList;			
extern PRIVILEGED_DATA  List_t * volatile pxOverflowDelayedTaskList;	
extern PRIVILEGED_DATA  List_t xPendingReadyList;					
#if ( INCLUDE_vTaskSuspend == 1 )
extern PRIVILEGED_DATA  List_t xSuspendedTaskList;					/*< Tasks that are currently suspended. */
#endif
#if( INCLUDE_vTaskDelete == 1 )
extern PRIVILEGED_DATA List_t xTasksWaitingTermination;				/*< Tasks that have been deleted - but their memory not yet freed. */
#endif
/*****************************************************************************    
 *                                                                           
 *                             内部数据                                   
 *                                                                            
 ****************************************************************************/

static OSSemObj_p pSemStdio=0;                     /**< 调试信息输出信号量    */
static unsigned int gTaskDelFlag   = 0;          /**< 任务删除请求标志 */
static unsigned int gTaskDebugFlag = 0x0;          /**< 任务调试输出标志 */
static unsigned char gTaskDebugLevel[20] = {0};    /**< 任务调试输出标志 */

/*****************************************************************************    
 *                                                                           
 *                             内部接口函数实现                                   
 *                                                                            
 ****************************************************************************/


/*****************************************************************************    
 *                                                                           
 *                             对外接口函数实现                                   
 *                                                                            
 ****************************************************************************/

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
int OsMemcmp(void *pDst, void *pSrc, unsigned int nNumBytes)
{
    return memcmp(pDst, pSrc, nNumBytes);    
}

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
void OsMemcpy(void *pDst, void *pSrc, unsigned int nNumBytes)
{
    memcpy(pDst, pSrc, nNumBytes);
}

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
void OsMemset(void *pMem, unsigned char iChar, unsigned int nNumBytes)
{
    memset(pMem, iChar, nNumBytes);
}

/**
 *****************************************************************************
 * \fn          void* OsMalloc(unsigned int nNumBytes)
 * \brief       分配动态内存.
 * \param[in]   nNumBytes 需要获取的字节数.
 * \note        .  
 * \return      指向获取的内存 返回0代表获取失败
 *****************************************************************************
 */
void* OsMalloc(unsigned int nNumBytes)
{
    void* p = 0;
    int  err=0;
    OsLock();
    if(err==0)
    {
      p = malloc(nNumBytes);
      OsUnLock();
    }
    return p;
}

/**
 *****************************************************************************
 * \fn          void OsFree(void *pMem)
 * \brief       释放动态内存.
 * \note        .  
 * \param[in]   pMem 指向需要释放的内存.
 *****************************************************************************
 */
void OsFree(void *pMem)
{ 
    OsLock();
    free(pMem);
    OsUnLock();
}

/**
 *****************************************************************************
 * \fn          int OsInit()
 * \brief       初始化系统.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsInit()
{
  return 0;
}

/**
 *****************************************************************************
 * \fn          int OsInit()
 * \brief       启动系统.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsStart()
{
  vTaskStartScheduler();
  return 0;
}

/**
 *****************************************************************************
 * \fn          int OsInitInFirstTask(void);
 * \brief       第一个任务启动相应的初始化操作.
 * \note        初始化系统时钟,统计任务等.  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsInitInFirstTask(void)
{
  vTaskSetTaskNumber(xTaskGetIdleTaskHandle(),0xFF);
  vTaskSetTaskNumber(xTimerGetTimerDaemonTaskHandle(),0xFE);
  return 0;
}

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
int OsTaskCreate(TSK_CFG_t* cfg)
{
  /*本函数有两个注意点
  1.优先级:数值低优先级低与UCOS相反,所以这里人为的进行翻转
  2.堆栈深度以堆栈类型(StackType_t)为单位
  */
#if 0
  TaskHandle_t handle=NULL;  /* 一个void*指针,实际使用时强制转化为TCB_t* */
  handle = xTaskCreateStatic((TaskFunction_t)cfg->taskp,
                             (char * )cfg->name,
                             cfg->tsksize,
                             (void * )cfg->arg,
                             (UBaseType_t)(configMAX_PRIORITIES-cfg->pri),
                             (StackType_t * )cfg->tskbase,
                             (StaticTask_t * )cfg->tcb );
  if(handle==NULL)
  {
    return -1;
  }
  else
  {
    vTaskSetTaskNumber(handle,(UBaseType_t )cfg->id);
    return 0;
  }
#endif
  return 0;
}

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
int OsTimeDelay( unsigned int milsec )
{
  TickType_t tick = milsec/(1000/configTICK_RATE_HZ);
  if(tick == 0)
  {
    tick = 1;
  }
  vTaskDelay(tick);
  return 0;
}

/**
 *****************************************************************************
 * \fn          unsigned int OsTimeGetTick(void)
 * \brief       获取系统滴答计数器.
 * \note        .  
 * \retval      unsigned int 返回滴答计数器值
 *****************************************************************************
 */
unsigned int OsTimeGetTick(void)
{
  return xTaskGetTickCount();
}

/**
 *****************************************************************************
 * \fn          unsigned int OsTaskGetId(void)
 * \brief       获取当前任务ID.
 * \note        . 
 * \return      unsigned int 返回当前任务的ID.
 *****************************************************************************
 */
unsigned int OsTaskGetId(void)
{
  TaskHandle_t handle=NULL;
  handle = xTaskGetCurrentTaskHandle();
  return ((StaticTask_t*)handle)->uxDummy10[0];
}

/**
 *****************************************************************************
 * \fn          unsigned int OsTaskGetSwCount(unsigned char id)
 * \brief       获取指定ID任务的调度次数.
 * \param[in]   id 任务ID.
 * \note        . 
 * \return      unsigned int 返回任务的调度次数.
 *****************************************************************************
 */
unsigned int OsTaskGetSwCount(unsigned int id)
{
#if 0 
  List_t *pxList;
  UBaseType_t uxTask = 0, uxQueue = configMAX_PRIORITIES;
  StaticTask_t *pxNextTCB, *pxFirstTCB;
  //OSCriticalEnter();
  OsLock();
  /*遍历就绪任务*/
  do
  {
    uxQueue--;
    pxList = &(pxReadyTasksLists[uxQueue]); 
    /*遍历指定队列参照prvListTasksWithinSingleList函数*/
    if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
      do
      {
        listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
        if(pxNextTCB->uxDummy10[1] == id)
        {
          OsUnLock();
          //return pxNextTCB->ulDummy16;  /*Freertos没有切换计数器 使用运行时间代替*/
          return pxNextTCB->ulDummy22;   /*增加任务切换计数器实现*/
        }
        uxTask++;
      } while( pxNextTCB != pxFirstTCB );
    }
    
  }while(uxQueue > (UBaseType_t)tskIDLE_PRIORITY); /*遍历所有优先级队列 参照uxTaskGetSystemState函数*/
  /*遍历pxDelayedTaskList*/             
  pxList = (List_t *)pxDelayedTaskList; 
  if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
  {
    listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
    do
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
      if(pxNextTCB->uxDummy10[1] == id)
      {
        OsUnLock();
        //return pxNextTCB->ulDummy16;  /*Freertos没有切换计数器 使用运行时间代替*/
        return pxNextTCB->ulDummy22;   /*增加任务切换计数器实现*/
      }
      uxTask++;
    } while( pxNextTCB != pxFirstTCB );
  }
  /*遍历pxOverflowDelayedTaskList*/             
  pxList = (List_t *)pxOverflowDelayedTaskList; 
  if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
  {
    listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
    do
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
      if(pxNextTCB->uxDummy10[1] == id)
      {
        OsUnLock();
        //return pxNextTCB->ulDummy16;  /*Freertos没有切换计数器 使用运行时间代替*/
        return pxNextTCB->ulDummy22;   /*增加任务切换计数器实现*/
      }
      uxTask++;
    } while( pxNextTCB != pxFirstTCB );
  }
  
#if( INCLUDE_vTaskDelete == 1 )  /*遍历xTasksWaitingTermination*/  
  pxList = (List_t *)&xTasksWaitingTermination; 
  if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
  {
    listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
    do
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
      if(pxNextTCB->uxDummy10[1] == id)
      {
        OsUnLock();
        //return pxNextTCB->ulDummy16;  /*Freertos没有切换计数器 使用运行时间代替*/
        return pxNextTCB->ulDummy22;   /*增加任务切换计数器实现*/
      }
      uxTask++;
    } while( pxNextTCB != pxFirstTCB );
  }
#endif
  
#if ( INCLUDE_vTaskSuspend == 1 ) /*遍历xSuspendedTaskList*/  
  pxList = (List_t *)&xSuspendedTaskList; 
  if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
  {
    listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
    do
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
      if(pxNextTCB->uxDummy10[1] == id)
      {
        OsUnLock();
        //return pxNextTCB->ulDummy16;  /*Freertos没有切换计数器 使用运行时间代替*/
        return pxNextTCB->ulDummy22;   /*增加任务切换计数器实现*/
      }
      uxTask++;
    } while( pxNextTCB != pxFirstTCB );
  }
#endif
  //OSCriticalExit();
  OsUnLock();
#endif
  return 0;
}

/**
 *****************************************************************************
 * \fn          char* OsGetTaskName(unsigned int id)
 * \brief       获取指定ID的任务名.
 * \param[in]   id 任务id号.
 * \note        .  
 * \return      返回指向任务名字的指针.
 *****************************************************************************
 */
char* OsGetTaskName(unsigned int id)
{
  TaskStatus_t *pxTaskStatusArray;
  volatile UBaseType_t uxArraySize, x;
  uxArraySize = uxTaskGetNumberOfTasks();
  pxTaskStatusArray = OsMalloc( uxArraySize * sizeof( TaskStatus_t ) );
  if( pxTaskStatusArray != NULL )
  {
    uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, NULL );
    for( x = 0; x < uxArraySize; x++ )
    {
      if(pxTaskStatusArray[x].xTaskNumber == id)
      {
          OsFree(pxTaskStatusArray);
          return (char*)pxTaskStatusArray[x].pcTaskName;
      }
    }
    OsFree(pxTaskStatusArray);
  }
  return 0;
}

/**
 *****************************************************************************
 * \fn          int OsGetTaskId(char* name,unsigned int* id)
 * \brief       获取指定名字任务的ID.
 * \param[in]   name 任务名字.
 * \param[out]  id   获取到的ID.
 * \note        .  
 * \retval      0 有效.
 * \retval      -1 无效.
 *****************************************************************************
 */
int OsGetTaskId(char* name,unsigned int* id)
{
  TaskStatus_t *pxTaskStatusArray;
  volatile UBaseType_t uxArraySize, x;
  uxArraySize = uxTaskGetNumberOfTasks();
  pxTaskStatusArray = OsMalloc( uxArraySize * sizeof( TaskStatus_t ) );
  if( pxTaskStatusArray != NULL )
  {
    uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, NULL );
    for( x = 0; x < uxArraySize; x++ )
    {
      if(memcmp(name, pxTaskStatusArray[x].pcTaskName, strlen(name)) == 0)
      {
          OsFree(pxTaskStatusArray);
          *id = pxTaskStatusArray[x].xTaskNumber;
          return 0;
      }
    }
    OsFree(pxTaskStatusArray);
  }
  return -1;
}

/*根据ID获取句柄*/
static TaskHandle_t OsGetTaskHandle(unsigned int id)
{
  List_t *pxList;
  UBaseType_t uxTask = 0, uxQueue = configMAX_PRIORITIES;
  StaticTask_t *pxNextTCB, *pxFirstTCB;
  OsLock();
  /*遍历就绪任务*/
  do
  {
    uxQueue--;
    pxList = (List_t *)&(pxReadyTasksLists[uxQueue]); 
    /*遍历指定队列参照prvListTasksWithinSingleList函数*/
    if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
      do
      {
        listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
        if(pxNextTCB->uxDummy10[1] == id)
        {
          OsUnLock();
          return (TaskHandle_t)pxNextTCB;  
        }
        uxTask++;
      } while( pxNextTCB != pxFirstTCB );
    }
    
  }while(uxQueue > (UBaseType_t)tskIDLE_PRIORITY); /*遍历所有优先级队列 参照uxTaskGetSystemState函数*/
  /*遍历pxDelayedTaskList*/             
  pxList = (List_t *)pxDelayedTaskList; 
  if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
  {
    listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
    do
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
      if(pxNextTCB->uxDummy10[1] == id)
      {
        OsUnLock();
        return (TaskHandle_t)pxNextTCB; 
      }
      uxTask++;
    } while( pxNextTCB != pxFirstTCB );
  }
  /*遍历pxOverflowDelayedTaskList*/             
  pxList = (List_t *)pxOverflowDelayedTaskList; 
  if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
  {
    listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
    do
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
      if(pxNextTCB->uxDummy10[1] == id)
      {
        OsUnLock();
        return (TaskHandle_t)pxNextTCB; 
      }
      uxTask++;
    } while( pxNextTCB != pxFirstTCB );
  }
  
#if( INCLUDE_vTaskDelete == 1 )  /*遍历xTasksWaitingTermination*/  
  pxList = (List_t *)&xTasksWaitingTermination; 
  if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
  {
    listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
    do
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
      if(pxNextTCB->uxDummy10[1] == id)
      {
        OsUnLock();
        return (TaskHandle_t)pxNextTCB; 
      }
      uxTask++;
    } while( pxNextTCB != pxFirstTCB );
  }
#endif
  
#if ( INCLUDE_vTaskSuspend == 1 ) /*遍历xSuspendedTaskList*/  
  pxList = (List_t *)&xSuspendedTaskList; 
  if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
  {
    listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
    do
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
      if(pxNextTCB->uxDummy10[1] == id)
      {
        OsUnLock();
        return (TaskHandle_t)pxNextTCB; 
      }
      uxTask++;
    } while( pxNextTCB != pxFirstTCB );
  }
#endif
  OsUnLock();
  return 0;
}

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
int OsCheckTaskId(unsigned int id)
{
  if(OsGetTaskHandle(id) ==0)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

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
int OsGetTaskFreeStack(unsigned int id)
{
  List_t *pxList;
  unsigned char* pstk;
  unsigned int ulCount=0;
  UBaseType_t uxTask = 0, uxQueue = configMAX_PRIORITIES;
  StaticTask_t *pxNextTCB, *pxFirstTCB;
  OsLock();
  /*遍历就绪任务*/
  do
  {
    uxQueue--;
    pxList = (List_t *)&(pxReadyTasksLists[uxQueue]); 
    /*遍历指定队列参照prvListTasksWithinSingleList函数*/
    if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
      do
      {
        listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
        if(pxNextTCB->uxDummy10[1] == id)
        {
          pstk = (unsigned char*)pxNextTCB->pxDummy6;
          while( *pstk == ( unsigned char)tskSTACK_FILL_BYTE )
          {
            pstk -= portSTACK_GROWTH;
            ulCount++;
          }
          OsUnLock();
          return ulCount;  
        }
        uxTask++;
      } while( pxNextTCB != pxFirstTCB );
    }
    
  }while(uxQueue > (UBaseType_t)tskIDLE_PRIORITY); /*遍历所有优先级队列 参照uxTaskGetSystemState函数*/
  /*遍历pxDelayedTaskList*/             
  pxList = (List_t *)pxDelayedTaskList; 
  if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
  {
    listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
    do
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
      if(pxNextTCB->uxDummy10[1] == id)
      {
        pstk = (unsigned char*)pxNextTCB->pxDummy6;
        while( *pstk == ( unsigned char)tskSTACK_FILL_BYTE )
        {
          pstk -= portSTACK_GROWTH;
          ulCount++;
        }
        OsUnLock();
        return ulCount; 
      }
      uxTask++;
    } while( pxNextTCB != pxFirstTCB );
  }
  /*遍历pxOverflowDelayedTaskList*/             
  pxList = (List_t *)pxOverflowDelayedTaskList; 
  if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
  {
    listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
    do
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
      if(pxNextTCB->uxDummy10[1] == id)
      {
        pstk = (unsigned char*)pxNextTCB->pxDummy6;
        while( *pstk == ( unsigned char)tskSTACK_FILL_BYTE )
        {
          pstk -= portSTACK_GROWTH;
          ulCount++;
        }
        OsUnLock();
        return ulCount; 
      }
      uxTask++;
    } while( pxNextTCB != pxFirstTCB );
  }
  
#if( INCLUDE_vTaskDelete == 1 )  /*遍历xTasksWaitingTermination*/  
  pxList = (List_t *)&xTasksWaitingTermination; 
  if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
  {
    listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
    do
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
      if(pxNextTCB->uxDummy10[1] == id)
      {
        pstk = (unsigned char*)pxNextTCB->pxDummy6;
        while( *pstk == ( unsigned char)tskSTACK_FILL_BYTE )
        {
          pstk -= portSTACK_GROWTH;
          ulCount++;
        }
        OsUnLock();
        return ulCount; 
      }
      uxTask++;
    } while( pxNextTCB != pxFirstTCB );
  }
#endif
  
#if ( INCLUDE_vTaskSuspend == 1 ) /*遍历xSuspendedTaskList*/  
  pxList = (List_t *)&xSuspendedTaskList; 
  if(listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t)0)
  {
    listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
    do
    {
      listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
      if(pxNextTCB->uxDummy10[1] == id)
      {
        pstk = (unsigned char*)pxNextTCB->pxDummy6;
        while( *pstk == ( unsigned char)tskSTACK_FILL_BYTE )
        {
          pstk -= portSTACK_GROWTH;
          ulCount++;
        }
        OsUnLock();
        return ulCount; 
      }
      uxTask++;
    } while( pxNextTCB != pxFirstTCB );
  }
#endif
  OsUnLock();
  return -1;
}

/**
 *****************************************************************************
 * \fn          void OsSetTaskKillReq(unsigned char id)
 * \brief       设置删除任务请求.
 * \param[in]   id 任务id号,不能超过31.
 * \note        .  
 *****************************************************************************
 */
void OsSetTaskKillReq(unsigned char id)
{
	gTaskDelFlag |= (1<<id);
}

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
int OsGetTaskKillReq(unsigned char id)
{
	return (gTaskDelFlag & (1<<id));
}

/**
 *****************************************************************************
 * \fn          int OsTaskKillSelf(void)
 * \brief       任务删除自己.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsTaskKillSelf(void)
{
    TaskHandle_t xTask;
	unsigned char id = OsTaskGetId();
	if((gTaskDelFlag & (1<<id)) != 0)
	{
		gTaskDelFlag &= ~(1<<id);
        xTask = xTaskGetCurrentTaskHandle(); 
		vTaskDelete(xTask);
	}
    return 0;
}

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
int OsSuspendTask(unsigned int id)
{
  TaskHandle_t handle;
  handle = OsGetTaskHandle(id);
  if(handle != 0)
  {
    vTaskSuspend(handle);
    return 0;
  }
  else
  {
    return -1;
  }
}

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
int OsResumeTask(unsigned int id)
{
#if 0
  TaskHandle_t handle;
  handle = OsGetTaskHandle(id);
  if(handle != 0)
  {
    if(IsIntContext())
    {
      xTaskResumeFromISR(handle);
    }
    else
    {
      vTaskResume(handle);
    }
    return 0;
  }
  else
  {
    return -1;
  }
#endif
  return 0;
}

/**
 *****************************************************************************
 * \fn          OSSemObj_p OsSemCreate(unsigned char *name, unsigned char count)
 * \brief       创建信号量.
 * \param[in]   name 信号量名字 必须传入全局数据或者const数据.
 * \param[in]   count 信号量初始值.
 * \note        信号量实例由malloc而来.  
 * \return      OSSemObj*  返回创建的信号量指针,如果为0表示创建失败.
 *****************************************************************************
 */
OSSemObj_p OsSemCreate(unsigned char *name, unsigned int count)
{
  
  return xSemaphoreCreateCounting(0xFFFFFFFF,count);
}

/**
 *****************************************************************************
 * \fn          int OsSemPend(OSSemObj_p psem, unsigned int timeout )
 * \brief       等待信号量.
 * \param[in]   psem 信号量指针 必须是已经创建的信号量.
 * \param[in]   timeout 等待超时时间单位mS.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsSemPend(OSSemObj_p psem, unsigned int timeout)
{	
  BaseType_t xYieldRequired;
  if(( * ( ( volatile uint32_t * ) 0xe000ed04 ) ) & 0xFFUL)
  {
    if(xSemaphoreTakeFromISR(psem,&xYieldRequired) == pdTRUE)
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    if(xSemaphoreTake(psem,timeout) == pdTRUE)
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
}

/**
 *****************************************************************************
 * \fn          int OsSemPost(OSSemObj_p psem )
 * \brief       发送信号量.
 * \param[in]   psem 信号量指针 必须是已经创建的信号量.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsSemPost(OSSemObj_p psem )
{
  BaseType_t xYieldRequired;
  if(( * ( ( volatile uint32_t * ) 0xe000ed04 ) ) & 0xFFUL)
  {
    if(xSemaphoreGiveFromISR(psem,&xYieldRequired) == pdTRUE)
    {
      portYIELD_FROM_ISR( xYieldRequired );
      return 0;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    if(xSemaphoreGive(psem) == pdTRUE)
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
}

/**
 *****************************************************************************
 * \fn          int OsSemDel(OSSemObj_p psem )
 * \brief       删除信号量.
 * \param[in]   psem 信号量指针 必须是已经创建的信号量.
 * \note        .  
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int OsSemDel(OSSemObj_p psem )
{ 
  vSemaphoreDelete(psem);
  //OsFree(psem);
  return 0;
}

/**
 *****************************************************************************
 * \fn          OSMutexObj_p OsMutexCreate(char *name)
 * \brief       创建互斥量.
 * \param[in]   name 互斥量名字 必须传入全局数据或者const数据.
 * \note        互斥量量实例由malloc而来.  
 * \return      OSMutexObj_p  返回创建的互斥量指针,如果为0表示创建失败.
 *****************************************************************************
 */
OSMutexObj_p OsMutexCreate(char *name)
{
#if 0
  OSMutexObj_p pMutex = (OSMutexObj_p)OsMalloc(sizeof(OSMutexObj));
  if(pMutex != 0)
  {
    if (xSemaphoreCreateMutexStatic(pMutex) != NULL )
    {
      return pMutex;
    }
    else
    {
      OsFree(pMutex);
    }
  }    
#endif
  return 0; 
}

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
int OsMutexPend(OSMutexObj_p pmutex, unsigned int timeout)
{	
#if 0 
  BaseType_t xYieldRequired;
  if(IsIntContext())
  {
    if(xSemaphoreTakeFromISR(pmutex,&xYieldRequired) == pdTRUE)
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    if(xSemaphoreTake(pmutex,timeout) == pdTRUE)
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
#endif
  return 0;
}

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
int OsMutexPost(OSMutexObj_p pmutex)
{
#if 0
  BaseType_t xYieldRequired;
  if(IsIntContext())
  {
    if(xSemaphoreGiveFromISR(pmutex,&xYieldRequired) == pdTRUE)
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    if(xSemaphoreGive(pmutex) == pdTRUE)
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
#endif
  return 0;
}

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
int OsMutexDel(OSMutexObj_p pmutex)
{
  vSemaphoreDelete(pmutex);
  //OsFree(pmutex);
  return 0;
}

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
OSMSGQObj_p OsMsgQCreate(char *name,unsigned int size)
{
#if 0
  OSMSGQObj_p pMsgQ=0; 
  void* pData=0; 
  pMsgQ = (OSMSGQObj_p)OsMalloc(sizeof(OSMSGQObj));
  if (pMsgQ != 0)
  {
    pData = (void*)OsMalloc(sizeof(void*)*size); /*分配数据区*/
    if(pData ==0)
    {
      OsFree(pMsgQ);
    }
    else
    {
      if(xQueueCreateStatic( size,sizeof(void*),pData,&(pMsgQ->queue)) != NULL)
      {
        pMsgQ->data = pData;                       /*指向数据区*/
        return pMsgQ;
      }
      else
      {
        OsFree(pMsgQ);
        OsFree(pData);
      }
    }
  }    
  return 0; 
#endif
  return 0;
}

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
int OsMsgQRcv(OSMSGQObj_p pMsgQ, unsigned char *buffer, unsigned int bufsize,unsigned int* size,unsigned int timeout)
{
#if 0
    BaseType_t xYieldRequired;
    BaseType_t res;
    unsigned int pmsg=NULL;
	OSAPI_MSGQ_FORM msgform;
    if(IsIntContext())
    {
      res = xQueueReceiveFromISR(&(pMsgQ->queue),&pmsg,&xYieldRequired);
    }
    else
    {
      res = xQueueReceive(&(pMsgQ->queue),&pmsg,timeout);
    }
    if(pdTRUE  == res)
	{
      if(pmsg != NULL)
      {
        msgform.addr = ((OSAPI_MSGQ_FORM*)pmsg)->addr;
        msgform.len = ((OSAPI_MSGQ_FORM*)pmsg)->len;
        if(bufsize>=msgform.len)
        {
          *size = msgform.len;
        }
        else
        {
          *size = bufsize; 
        }
		OsMemcpy(buffer, (unsigned char *)msgform.addr, *size);
		OsFree((unsigned char *)(msgform.addr));
		OsFree((void*)pmsg);
        return 0;
      }
      else
      {
		*size = 0;
        return -1;
      }
	}
	else
	{
		*size = 0;
        return -1;
	}
#endif
    return 0;
}


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
int OsMsgQSnd(OSMSGQObj_p pMsgQ, unsigned char *buffer, unsigned size)
{
#if 0
	unsigned char *pmsg;
	OSAPI_MSGQ_FORM *msgform;
    BaseType_t xYieldRequired;
	pmsg = (unsigned char *)OsMalloc(size);
	if(pmsg == 0) 
	{
		return -1;
	}
	msgform = (OSAPI_MSGQ_FORM *)OsMalloc(sizeof(OSAPI_MSGQ_FORM));	
	if(msgform == NULL) 
	{
		OsFree(pmsg);
		return -1;	
	}
	OsMemcpy(pmsg, buffer, size );
	msgform->addr = (unsigned int)pmsg;
	msgform->len = size;
    if(IsIntContext())
    {
      if(pdTRUE != xQueueSendFromISR(&(pMsgQ->queue),(void*)&msgform,&xYieldRequired))
      {
	    OsFree(pmsg);
	    OsFree(msgform);
        return -1;
      }	 
    }
    else
    {
      if(pdTRUE != xQueueSend(&pMsgQ->queue,(void*)&msgform,0))
      {
	    OsFree(pmsg);
	    OsFree(msgform);
        return -1;
      }	
    }	
#endif
	return 0;
}

/**
 *****************************************************************************
 * \fn          OSTimerObj_p OsTimerCreate(unsigned int time,OSAPI_TIMER_TYPE_e mode,OSAPI_TIMER_CALLBACK callback,char *name)
 * \brief       创建定时器.
 * \param[in]   time 定时器周期,精度依赖于OS_CFG_TMR_TASK_RATE_HZ设置(目前设置为100mS)
 * \param[in]   mode 定时器模式 \ref OSAPI_TIMER_TYPE_e.
 * \param[in]   callback 定时器回调函数 \ref OSAPI_TIMER_CALLBACK.
 * \param[in]   name 定时器名字.
 * \note        定时器实例由malloc而来.  
 * \return      OSMutexObj_p  返回创建的互斥量指针,如果为0表示创建失败.
 *****************************************************************************
 */
OSTimerObj_p OsTimerCreate(unsigned int time,OSAPI_TIMER_TYPE_e mode,OSAPI_TIMER_CALLBACK callback,char *name)
{
#if 0
  OSTimerObj_p ptimer = (OSTimerObj_p)OsMalloc(sizeof(OSTimerObj));
  if(ptimer != NULL)
  {
    time = time/(1000/TICKS_PER_SEC);
    if(time == 0)
    {
      time = 1;
    }
    if(mode == OSAPI_TIMIER_SINGLE)
    {
      xTimerCreateStatic(name,time,pdFALSE,0,callback,ptimer); 
    }
    else
    {
      xTimerCreateStatic(name,time,pdTRUE,0,callback,ptimer);
    }
    return ptimer;
  }
  else
  {
    return 0; 
  }
#endif
  return 0;
}

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
int OsTimerStart(OSTimerObj_p ptimer)
{
#if 0
  BaseType_t xYieldRequired;
  if(IsIntContext())
  {
    if(pdPASS == xTimerStartFromISR(ptimer,&xYieldRequired))
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    if(pdPASS == xTimerStart(ptimer,1000))
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
#endif
  return 0;
}

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
int OsTimerStop(OSTimerObj_p ptimer)
{
#if 0
  BaseType_t xYieldRequired;
  if(IsIntContext())
  {
    if(pdPASS == xTimerStopFromISR(ptimer,&xYieldRequired))
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    if(pdPASS == xTimerStop(ptimer,1000))
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
#endif
  return 0;
}

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
int OsTimerDelete(OSTimerObj_p ptimer)
{
#if 0
  if(pdPASS == xTimerDelete(ptimer,1000))
  {
      OsFree(ptimer);
      return 0;
  }
  else
  {
    return -1;
  }
#endif
  return 0;
}



/**
*****************************************************************************
* \fn          void OsSetTaskDebug(unsigned int flag,osapi_debug_e level)
* \brief       设置任务调试输出标志.
* \param[in]   id 任务号,某一位为1表示指定ID任务允许输出.
* \param[in]   level \ref osapi_debug_e.
* \note        .  
*****************************************************************************
*/
void OsSetTaskDebug(unsigned int id,osapi_debug_e level)
{
  uint8_t i;
  if(id == (uint32_t)0xFFFFFFFF)
  {
      gTaskDebugFlag =  (uint32_t)0xFFFFFFF;
      for(i=0;i<sizeof(gTaskDebugLevel)/sizeof(gTaskDebugLevel[0]);i++)
      {
        gTaskDebugLevel[id] = level;  
      }
  }
  else
  {
    gTaskDebugFlag |= 1<<id;
    gTaskDebugLevel[id] = level;
    
  }
}

/**
*****************************************************************************
* \fn          void OsClrTaskDebug(unsigned int flag,osapi_debug_e level)
* \brief       清除任务调试输出标志.
* \param[in]   id 任务号,某一位为1表示指定ID任务不允许输出.
* \param[in]   level \ref osapi_debug_e.
* \note        .  
*****************************************************************************
*/
void OsClrTaskDebug(unsigned int id,osapi_debug_e level)
{
  if(id == (uint32_t)0xFFFFFFFF)
  {
      gTaskDebugFlag = (uint32_t)0x0;
    
  }
  else
  {
    gTaskDebugFlag &= ~(1<<id);
    gTaskDebugLevel[id] = level;
    
  }
}


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
int OsPrintf(osapi_debug_e level,const char *format, ...)
{
    unsigned int id = OsTaskGetId();
	struct tm tm_time; 
	int ret;
    if(pSemStdio == 0)
    {
        pSemStdio = OsSemCreate("semstdio", 1);
        if(pSemStdio==0)
        {
            return -1;
        }
    }    

    if((gTaskDebugFlag & (1 << id)) && (level>gTaskDebugLevel[id]))
    {
        if(OsSemPend(pSemStdio, 2000)==0) 
        {
            ret = driver_time_gettm(&tm_time);
            if (ret == 0)
            {
                printf("[%02d-%02d %02d:%02d:%02d %s]:", 
                       tm_time.tm_mon+1, tm_time.tm_mday,    
                       tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                       OsGetTaskName(id));
            }
            else
            {
                printf("[%s]:", OsGetTaskName(id));
            }
            va_list arg;
            va_start(arg, format);      
            vprintf(format, arg);
            va_end(arg);  
            printf("\r\n");
            OsSemPost(pSemStdio);
        }
    }
    return 0;
}

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
int OsPrintf_va(csp_color_t color,osapi_debug_e level,const char *format, va_list arg)
{
    unsigned int id = OsTaskGetId();
	struct tm tm_time; 
	int ret;
    if(pSemStdio == 0)
    {
        pSemStdio = OsSemCreate("semstdio", 1);
    }    

    if((gTaskDebugFlag & (1 << id)) && (level>gTaskDebugLevel[id]))
    {
        if(OsSemPend(pSemStdio, 1000)==0) 
        {
            csp_sys_set_color(color);
            ret = driver_time_gettm(&tm_time);
            if (ret == 0)
            {
                printf("[%02d-%02d %02d:%02d:%02d %s]:", 
                       tm_time.tm_mon+1, tm_time.tm_mday,    
                       tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                       OsGetTaskName(id));
            }
            else
            {
                printf("[%s]:", OsGetTaskName(id));
            }
            
            vprintf(format, arg);
            printf("\r\n");
            csp_sys_set_color(COLOR_RESET);
            OsSemPost(pSemStdio);
        }
    }
    return 0;
}

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
int OsPrintfBuf(char *name,osapi_debug_e level, unsigned char * buff, unsigned int length)
{  
    unsigned int i;
    unsigned int j;
    unsigned char* p;
    int dat;
    struct tm tm_time; 
    unsigned int lines = length / 16;          /* 打印行数          */
    unsigned int lastline = length % 16;       /* 最后一行数据个数  */
    unsigned int id = OsTaskGetId();
    if(pSemStdio == 0)
    {
        pSemStdio = OsSemCreate("semstdio", 1);
        if(pSemStdio==NULL)
        {
            return -1;
        }
    }   

    if((gTaskDebugFlag & (1 << id)) && (level>gTaskDebugLevel[id]))
    {
        if(OsSemPend(pSemStdio, 1000)==0) 
        {
            int ret = driver_time_gettm(&tm_time);
            if (ret == 0)
            {
                printf("[%02d-%02d %02d:%02d:%02d %s]:", 
                       tm_time.tm_mon+1, tm_time.tm_mday,    
                       tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                       OsGetTaskName(id));
            }
            else
            {
                printf("[%s]:", OsGetTaskName(id));
            }
            printf("%s length = %d, data: \r\n", name, length);
            p = buff;
            for(j=0;j<lines;j++)
            {
                 printf("%#10x:",(uint32_t)p);
                for (i = 0; i< 16; i++)
                {
                    dat = p[i]; 
                    printf("%02x ", dat);
                }
                printf("| ");
                for (i = 0; i< 16; i++)
                {
                    dat = p[i]; 
                    if(isprint(dat))
                    {
                        printf("%c ", dat);
                    }
                    else
                    {
                        printf(".");    
                    }
                }
                p += 16;
                printf("\r\n");
            }
            printf("%#10x:",(uint32_t)p);
            for (i = 0; i< lastline; i++)
            {
                dat = p[i]; 
                printf("%02x ", dat);
            }
            printf("| ");
            for (i = 0; i< lastline; i++)
            {
                dat = p[i]; 
                if(isprint(dat))
                {
                    printf("%c ", dat);
                }
                else
                {
                    printf(".");    
                }
            }
            printf("\r\n");
            OsSemPost(pSemStdio);
        }
    }
    return 0;	
}

extern uint32_t port_get_runtime(void);
/**
 *****************************************************************************
 * \fn          int OsPrintfTaskInfo(void)
 * \brief       打印任务信息.
 * \note        .  
 *****************************************************************************
 */
void OsPrintfTaskInfo(void)
{
  unsigned int usage=0;
  TaskStatus_t *pxTaskStatusArray=NULL;
  volatile UBaseType_t uxArraySize, x;
  char cStatus;
  uxArraySize = uxTaskGetNumberOfTasks();
  pxTaskStatusArray = OsMalloc( uxArraySize * sizeof( TaskStatus_t ) );
  if( pxTaskStatusArray != NULL )
  {
    uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, NULL );
    printf("\r\nname\t\t\tID\tprio\ttskbase\t\ttmsize\tmfree\tusage\t\truntime\trunnum      \tDescp.\r\n");
    printf("------------------------------------------------------------------------------------\r\n");
    for( x = 0; x < uxArraySize; x++ )
    {
      switch( pxTaskStatusArray[ x ].eCurrentState )
      {
      case eRunning:    cStatus = '-';
      break;
      case eReady:		cStatus = 'R';
      break;
      case eBlocked:		cStatus = 'B';
      break;
      case eSuspended:	cStatus = 'S';
      break;
      case eDeleted:		cStatus = 'D';
      break;
      default:	
        cStatus = 0x00;
        break;
      }
      if(portGET_RUN_TIME_COUNTER_VALUE() !=0)
      {
        usage = (pxTaskStatusArray[x].ulRunTimeCounter*10*100)/portGET_RUN_TIME_COUNTER_VALUE();  
      }
      printf("%-024s%d\t%d\t%#x\t%d\t",pxTaskStatusArray[x].pcTaskName,pxTaskStatusArray[x].xTaskNumber,pxTaskStatusArray[x].uxCurrentPriority,pxTaskStatusArray[x].pxStackBase,(((uint32_t)((StaticTask_t*)(pxTaskStatusArray[x].xHandle))->pxDummy8)-(uint32_t)(((StaticTask_t*)(pxTaskStatusArray[x].xHandle))->pxDummy6))+8);
      printf("%d\t%03d/1000\t%#x\t",pxTaskStatusArray[x].usStackHighWaterMark*4,usage,pxTaskStatusArray[x].ulRunTimeCounter);
      printf("%c|",cStatus);
      printf("%s\r\n","");
    }
    OsFree(pxTaskStatusArray);
  }
  printf("%s\r\n","");
}

/**
 *****************************************************************************
 * \fn          void OsPrintfSemInfo(void)
 * \brief       打印信号量信息.
 * \note        .  
 *****************************************************************************
 */
void OsPrintfSemInfo(void)
{
 
}

/**
 *****************************************************************************
 * \fn          void OsPrintfMsgInfo(void)
 * \brief       打印消息队列信息.
 * \note        .  
 *****************************************************************************
 */
void OsPrintfMsgInfo(void)
{

}

/**
 *****************************************************************************
 * \fn          void OsPrintfMutexInfo(void)
 * \brief       打印互斥量信息.
 * \note        .  
 *****************************************************************************
 */
void OsPrintfMutexInfo(void)
{

}

/**
 *****************************************************************************
 * \fn          void OsPrintfTimerInfo(void)
 * \brief       打印定时器信息.
 * \note        .  
 *****************************************************************************
 */
void OsPrintfTimerInfo(void)
{

}

/**
 *****************************************************************************
 * \fn          unsigned int OsGetTs(void)
 * \brief       获取时间戳.
 * \note        .  
 *****************************************************************************
 */
unsigned int OsGetTs(void)
{
  //return ulMainGetRunTimeCounterValue(); /*单位mS*/
  return 0;
}


/**
 *****************************************************************************
 * \fn          void OsLock(void)
 * \brief       任务调度锁定.
 * \note        .  
 *****************************************************************************
*/
void OsLock(void)
{
#if 0
  if(IsIntContext()==0)
  {
    vTaskSuspendAll(); /*关调度(中断未关闭)*/
  }
#endif
}

/**
 *****************************************************************************
 * \fn          void OsUnLock(void)
 * \brief       解除任务调度锁定.
 * \note        .  
 *****************************************************************************
 */
void OsUnLock(void)
{
#if 0
  if(IsIntContext()==0)
  {
    xTaskResumeAll(); /*开调度*/
  }
#endif
}
/**
 *****************************************************************************
 * \fn          void OsEnterInt(void)
 * \brief       进入中断.
 * \note        .  
 *****************************************************************************
 */
void OsEnterInt(void)
{
}

/**
 *****************************************************************************
 * \fn          void OsExitInt(void)
 * \brief       退出中断.
 * \note        .  
 *****************************************************************************
 */
void OsExitInt(void)
{
}