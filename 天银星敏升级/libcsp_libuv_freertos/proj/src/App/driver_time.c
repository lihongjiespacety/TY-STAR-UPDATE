/**
 *****************************************************************************        
 * \brief       驱动层(DRIVER)时间模块(TIME)相关接口实现.
 * \details     Copyright (c) 2017,长沙威胜信息技术有限公司.
 *              All rights reserved.    
 * \file        driver_time.c 
 * \author      李红洁W18900
 * \version     1.0 
 * \date        2017-08-09
 * \note        使用前请参考注释.\n
 *              调用HAL实现,依赖OSAPI,driver_iicrtc.\n
 * \since       李红洁      2017-8-9 18:18:12     1.0     新建 
 * \par 修订记录
 * - 2017-08-09 初始版本
 * \par 资源说明
 * - RAM:              
 * - ROM:
 *****************************************************************************
 */

#include "driver.h"
#include "osapi_freertos.h"

/*****************************************************************************    
 *                                                                           
 *                             内部数据                                   
 *                                                                            
 ****************************************************************************/


static OSSemObj_p pSemTime=0;              /**<  时间处理信号量指针    */
static time_t gSwTime=0;                   /**<  全局软件时间记录      */   
static time_t gRunTime=0;                  /**<  系统运行时间          */   

/*****************************************************************************    
 *                                                                           
 *                             内部接口函数实现                                   
 *                                                                            
 ****************************************************************************/

/**
 *****************************************************************************
 * \fn          time_t gettime(void)
 * \brief       <time.h>底层接口实现,必须实现该函数标准<time.h>才能使用.
 * \note        .
 *****************************************************************************
 */ 
time_t gettime(void)
{
    return gSwTime;
}


/*****************************************************************************    
 *                                                                           
 *                             对外接口函数实现                                   
 *                                                                            
 ****************************************************************************/

/**
 *****************************************************************************
 * \fn          time_t driver_time_mktime_r(struct tm * t)
 * \brief       将结构体时间struct tm转换成time_t时间戳,线程安全版本.
 * \param[in]   t  指向需要转换的时间结构体.
 * \return      time_t 返回转换后的时间戳,返回0表示操作失败. 
 * \note        .
 *****************************************************************************
 */ 
time_t driver_time_mktime_r(struct tm * t)
{
    if(OsSemPend(pSemTime, DRIVER_TIME_SEMTIMEOUT)==0) 
    {
        time_t ret = mktime(t);
        OsSemPost(pSemTime);
        return ret;
    } 
    else
    {    
        return 0;
    }
}

/**
 *****************************************************************************
 * \fn          int driver_time_sync(void)
 * \brief       同步硬件时间到软件时间.
 * \retval      0 操作成功
 * \retval      其他值 操作失败
 * \note        .
 *****************************************************************************
 */ 
int driver_time_sync(void)
{
    struct tm t;
    int res;
    time_t temp;
    if(OsSemPend(pSemTime, DRIVER_TIME_SEMTIMEOUT)==0) 
    {
        res = -1;//driver_iicrtc_gettime(&t);  /*获取硬件时间*/
        OsSemPost(pSemTime);
    } 
    else
    {    
        return -1;
    }
    if(res == 0) 
    {
        temp =  driver_time_mktime_r(&t);  /*转换成时间戳*/    
        if(temp != 0)
        {
            gSwTime = temp;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}


/**
 *****************************************************************************
 * \fn          struct tm * driver_time_localtime_r(const time_t *pt, struct tm *ptm)
 * \brief       将time_t时间戳转换成结构体时间struct tm,线程安全版本.
 * \param[in]   pt  指向需要转换的时间戳.
 * \param[out]  ptm 保存转换结果,调用者提供存储空间.
 * \return      struct tm* 返回指向转换结果的指针,如果返回0表示操作失败.
 * \note        .
 *****************************************************************************
 */ 
struct tm * driver_time_localtime_r(const time_t *pt, struct tm *ptm)
{
    struct tm *ret;
    if(OsSemPend(pSemTime, DRIVER_TIME_SEMTIMEOUT)==0) 
    {
        ret = localtime(pt);
        if(ret != (void *)0)
        {
            *ptm = *ret;  /*注意这里一定要用值拷贝,因为实际上ptm指向的是一个全局变量,线程不安全,所以才用信号量保护*/
        }
        OsSemPost(pSemTime);
        return ptm;
    }
    else
    {
        return 0;
    }
}



/**
 *****************************************************************************
 * \fn          int driver_time_init(void)
 * \brief       初始化时间驱动.
 * \retval      0 操作成功
 * \retval      其他值 操作失败
 * \note        .
 *****************************************************************************
 */ 
int driver_time_init(void)
{
    //driver_iicrtc_init();   
    if(pSemTime == 0)
    {
        pSemTime = OsSemCreate("semtime", 1);
    }      
    return 0;//driver_time_sync();      /*初始化时加载硬件时间*/	        
}


/**
 *****************************************************************************
 * \fn          void driver_time_sechandle(void)
 * \brief       软件时间更新回调函数,1秒钟调用一次.
 * \note        .
 *****************************************************************************
 */ 
void driver_time_sechandle(void)
{
    gSwTime++;
    gRunTime++;
    if(gSwTime%DRIVER_TIME_SYNCPERIOD==0) 
    {  
        //driver_time_sync();   /*同步时间*/
    }
}

/**
 *****************************************************************************
 * \fn          int driver_time_settime(time_t t)
 * \brief       设置软硬件时间.
 * \param[in]   t  要设置的时间戳.
 * \retval      0 操作成功
 * \retval      其他值 操作失败
 * \note        .
 *****************************************************************************
 */ 
int driver_time_settime(time_t t)
{
    int res=0;
    struct tm tmtm;
    struct tm *ptm = driver_time_localtime_r(&t, &tmtm);
    if(ptm != 0) 
    {
        if(OsSemPend(pSemTime, DRIVER_TIME_SEMTIMEOUT)==0) 
        {
            res = -1;//driver_iicrtc_settime(&tmtm);
            gSwTime = t;
            OsSemPost(pSemTime);
        } 
        else
        {    
            res = -1;
        }	
    } 
    else
    {
        res = -1;
    }
    return res;
}

/**
 *****************************************************************************
 * \fn          int driver_time_gettm(struct tm *p)
 * \brief       获取软件结构体时间.
 * \param[out]  p  存储获取的时间结构体.
 * \retval      0 操作成功
 * \retval      其他值 操作失败
 * \note        .
 *****************************************************************************
 */ 
int driver_time_gettm(struct tm *p)
{
    time_t t = gettime();
    struct tm *ptm = driver_time_localtime_r(&t, p);
    if(ptm != 0)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 *****************************************************************************
 * \fn          int driver_time_settm(struct tm *p)
 * \brief       设置软硬件结构体时间.
 * \param[out]  p  存储获取的时间结构体.
 * \retval      0 操作成功
 * \retval      其他值 操作失败
 * \note        .
 *****************************************************************************
 */ 
int driver_time_settm(struct tm *p)
{
    time_t temp=0;
    int res=0;
    if(OsSemPend(pSemTime, DRIVER_TIME_SEMTIMEOUT)==0) 
    {
        res = -1;//driver_iicrtc_settime(p);
        OsSemPost(pSemTime);
    } 
    else
    {    
        res = -1;
    }	
    if(res == 0)
    {
        temp = driver_time_mktime_r(p);
        if(temp !=0)
        {
            gSwTime = temp;
            res =0;
        }
        else
        {
            res = -1;
        }
    }
    return res;
}

/**
 *****************************************************************************
 * \fn          time_t driver_time_gettime(void)
 * \brief       获取软件时间戳时间.
 * \return      time_t 返回软件时间戳
 * \note        .
 *****************************************************************************
 */ 
time_t driver_time_gettime(void)
{
    return gettime(); 
}

/**
 *****************************************************************************
 * \fn          time_t driver_time_getruntime(void)
 * \brief       获取系统运行时间.
 * \return      time_t 返回系统运行时间
 * \note        .
 *****************************************************************************
 */ 
time_t driver_time_getruntime(void)
{
    return gRunTime; 
}

/**
 *****************************************************************************
 * \fn          int driver_time_getrtctm(struct tm *p)
 * \brief       获取硬件RTC结构体时间.
 * \retval      0 操作成功
 * \retval      其他值 操作失败
 * \note        .
 *****************************************************************************
 */ 
int driver_time_getrtctm(struct tm *p)
{
    int res;
    if(OsSemPend(pSemTime, DRIVER_TIME_SEMTIMEOUT)==0) 
    {
        res = -1;//driver_iicrtc_gettime(p);
        OsSemPost(pSemTime);
        return res;
    } 
    else
    {    
        return -1;
    }	
}


