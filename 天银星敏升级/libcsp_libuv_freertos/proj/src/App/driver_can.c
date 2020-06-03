/**
********************************************************************************        
* \brief       驱动层(DRIVER)CAN模块(CAN)相关接口实现.
* \details     Copyright (c) 2019,深圳/长沙市健科电子有限公司.
*              All rights reserved.    
* \file        driver_can.c 
* \author      李红洁<lihj@jkaec.com>
* \version     1.0 
* \date        2019年5月9日
* \note        使用前请参考注释.\n
* \since       李红洁      2019年5月9日     1.0     新建 
* \par 修订记录
* -  2019年5月9日 初始版本
* \par 资源说明
* - RAM:              
* - ROM:
********************************************************************************
*/

#include "bsp.h"
#include "osapi_freertos.h"
#include "csp_if_can.h"
#include "can.h"
#include "driver_can.h"
#include "stm32f10x.h"
#include "misc.h"
#include <stdint.h>

/*******************************************************************************    
*                                                                           
*                             内部数据                                   
*                                                                            
*******************************************************************************/


driver_can_data_t g_driver_can_rxdata_t[DRIVER_CAN_RXBUFFSIZE];
driver_can_data_t g_driver_can_txdata_t[DRIVER_CAN_TXBUFFSIZE];
driver_can_t g_driver_can_t = {0,0,0,0,0,0,DRIVER_CAN_RXBUFFSIZE,DRIVER_CAN_TXBUFFSIZE,(driver_can_data_t*)(&g_driver_can_rxdata_t[0]),(driver_can_data_t*)(&g_driver_can_txdata_t[0])};
//uint32_t g_driver_idlist_au32[28] = 
//{
//  (uint32_t)1,
//  (uint32_t)2,
//  (uint32_t)3,
//  (uint32_t)4,
//  (uint32_t)5,
//  (uint32_t)6,
//  (uint32_t)7,
//  (uint32_t)8,
//  (uint32_t)9,
//  (uint32_t)10,
//  (uint32_t)11,
//  (uint32_t)12,
//  (uint32_t)13,
//  (uint32_t)14,
//  (uint32_t)15,
//  (uint32_t)16,
//  (uint32_t)17,
//  (uint32_t)18,
//  (uint32_t)19,
//  (uint32_t)20,
//  (uint32_t)21,
//  (uint32_t)22,
//  (uint32_t)23,
//  (uint32_t)24,
//  (uint32_t)25,
//  (uint32_t)26,
//  (uint32_t)27,
//  (uint32_t)28,
//};

uint32_t g_driver_idlist_au32[28] = {0};
/*******************************************************************************    
*                                                                           
*                             内部函数                                   
*                                                                            
*******************************************************************************/


/********************************************************************************
* \fn          static void driver_can_ioinit(void)
* \brief       初始化引脚.
* \note        . 
********************************************************************************
*/
static void driver_can_ioinit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
  //AFIO->MAPR = (AFIO->MAPR & (uint32_t)(~((uint32_t)3<<13))) | (uint32_t)((uint32_t)2<<13);  //PB8 PB9 CAN1
  GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
  /*使能时钟*/

  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

  /*引脚 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PB9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//PB8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PA12
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  
}




/**
********************************************************************************
* \fn          static int8_t driver_can_writemailbox(uint32_t id,uint8_t ide,uint8_t rtr,uint8_t *pdata,uint8_t len)
* \brief       CAN写消息邮箱.
* \note        .
* \param[in]   id ID.
* \param[in]   ide 0:标准帧 1 扩展帧
* \param[in]   rtr 0:数据帧 1 远程帧
* \param[in]   pdata   数据区域.
* \param[out]  len 数据长度
* \return      uint8_t 0成功 1失败.
* \note        .
********************************************************************************
*/
static int8_t driver_can_writemailbox(driver_can_data_t data)
{
  int8_t res = 0;
  uint8_t dlc = 0;
  if(data.len>8)
  {
     dlc = (uint8_t)8;
  }
  else
  {
     dlc = data.len;
  }
  if(CAN1->TSR & (uint32_t)((uint32_t)1<<26))
  {
    CAN1->sTxMailBox[0].TIR = (data.id<<3)  & (uint32_t)0xFFFFFFFE;  /*避免使能一开始就发送上一次的数据*/
    if(0 == (data.type & (uint8_t)((uint8_t)1<<CAN_DATA_RTR_BIT)))
    {
       CAN1->sTxMailBox[0].TIR &= (uint32_t)(~(uint32_t)((uint32_t)1<<1));
    }
    else
    {
       CAN1->sTxMailBox[0].TIR |= (uint32_t)((uint32_t)1<<1);
    }
    if(0 == (data.type & (uint8_t)((uint8_t)1<<CAN_DATA_IDE_BIT)))
    {
       CAN1->sTxMailBox[0].TIR &= (uint32_t)(~(uint32_t)((uint32_t)1<<2));
       //CAN1->sTxMailBox[0].TIR |= (uint32_t)((data.id & (uint32_t)0x7FF)<<21);
    }
    else
    {
       CAN1->sTxMailBox[0].TIR |= (uint32_t)((uint32_t)1<<2);
       //CAN1->sTxMailBox[0].TIR |= (uint32_t)((data.id & (uint32_t)0x1FFFFFFF)<<3);
    }
    /*填充数据*/
    CAN1->sTxMailBox[0].TDTR = (uint32_t)dlc;
    CAN1->sTxMailBox[0].TDLR = (uint32_t)data.buff[0] | ((uint32_t)data.buff[1]<<8) | ((uint32_t)data.buff[2]<<16) | ((uint32_t)data.buff[3]<<24);
    CAN1->sTxMailBox[0].TDHR = (uint32_t)data.buff[4] | ((uint32_t)data.buff[5]<<8) | ((uint32_t)data.buff[6]<<16) | ((uint32_t)data.buff[7]<<24);
    /*发送*/
    CAN1->sTxMailBox[0].TIR |= (uint32_t)0x01;

  }
#if 0  /*只使用一个FIFO发送 避免不按序发送*/
  else if(CAN1->TSR & (uint32_t)((uint32_t)1<<27))
  {
    CAN1->sTxMailBox[1].TIR = (data.id<<3)  & (uint32_t)0xFFFFFFFE;  /*避免使能一开始就发送上一次的数据*/
    if(0 == (data.type & (uint8_t)((uint8_t)1<<CAN_DATA_RTR_BIT)))
    {
       CAN1->sTxMailBox[1].TIR &= (uint32_t)(~(uint32_t)((uint32_t)1<<1));
    }
    else
    {
       CAN1->sTxMailBox[1].TIR |= (uint32_t)((uint32_t)1<<1);
    }
    if(0 == (data.type & (uint8_t)((uint8_t)1<<CAN_DATA_IDE_BIT)))
    {
       CAN1->sTxMailBox[1].TIR &= (uint32_t)(~(uint32_t)((uint32_t)1<<2));
       //CAN1->sTxMailBox[1].TIR |= (uint32_t)((data.id & (uint32_t)0x7FF)<<21);
    }
    else
    {
       CAN1->sTxMailBox[1].TIR |= (uint32_t)((uint32_t)1<<2);
       //CAN1->sTxMailBox[1].TIR |= (uint32_t)((data.id & (uint32_t)0x1FFFFFFF)<<3);
    }
    /*填充数据*/
    CAN1->sTxMailBox[1].TDTR = (uint32_t)dlc;
    CAN1->sTxMailBox[1].TDLR = (uint32_t)data.buff[0] | ((uint32_t)data.buff[1]<<8) | ((uint32_t)data.buff[2]<<16) | ((uint32_t)data.buff[3]<<24);
    CAN1->sTxMailBox[1].TDHR = (uint32_t)data.buff[4] | ((uint32_t)data.buff[5]<<8) | ((uint32_t)data.buff[6]<<16) | ((uint32_t)data.buff[7]<<24);
    /*发送*/
    CAN1->sTxMailBox[1].TIR |= (uint32_t)0x01;
  }
  else if(CAN1->TSR & (uint32_t)((uint32_t)1<<28))
  {
    CAN1->sTxMailBox[2].TIR = (data.id<<3)  & (uint32_t)0xFFFFFFFE;  /*避免使能一开始就发送上一次的数据*/
    if(0 == (data.type & (uint8_t)((uint8_t)1<<CAN_DATA_RTR_BIT)))
    {
       CAN1->sTxMailBox[2].TIR &= (uint32_t)(~(uint32_t)((uint32_t)1<<1));
    }
    else
    {
       CAN1->sTxMailBox[2].TIR |= (uint32_t)((uint32_t)1<<1);
    }
    if(0 == (data.type & (uint8_t)((uint8_t)1<<CAN_DATA_IDE_BIT)))
    {
       CAN1->sTxMailBox[2].TIR &= (uint32_t)(~(uint32_t)((uint32_t)1<<2));
       //CAN1->sTxMailBox[2].TIR |= (uint32_t)((data.id & (uint32_t)0x7FF)<<21);
    }
    else
    {
       CAN1->sTxMailBox[2].TIR |= (uint32_t)((uint32_t)1<<2);
       //CAN1->sTxMailBox[2].TIR |= (uint32_t)((data.id & (uint32_t)0x1FFFFFFF)<<3);
    }
    /*填充数据*/
    CAN1->sTxMailBox[2].TDTR = (uint32_t)dlc;
    CAN1->sTxMailBox[2].TDLR = (uint32_t)data.buff[0] | ((uint32_t)data.buff[1]<<8) | ((uint32_t)data.buff[2]<<16) | ((uint32_t)data.buff[3]<<24);
    CAN1->sTxMailBox[2].TDHR = (uint32_t)data.buff[4] | ((uint32_t)data.buff[5]<<8) | ((uint32_t)data.buff[6]<<16) | ((uint32_t)data.buff[7]<<24);
    /*发送*/
    CAN1->sTxMailBox[2].TIR |= (uint32_t)0x01;
  }
#endif
  else
  {
    res = 1;
  
  }
  return res;
}


/**
 *****************************************************************************
 * \fn          static int8_t is_txmail_empty(void)
 * \brief       判断发送邮箱是否可以写数据.
 * \note        . 
 * \return      1 可以写数据 0不可以
 *****************************************************************************
 */
static int8_t is_txmail_empty(void)
{
   if(CAN1->TSR & (uint32_t)((uint32_t)1<<26))
   {
       return (int8_t)1;
   }
#if 0
   else if(CAN1->TSR & (uint32_t)((uint32_t)1<<27))
   {
       return (int8_t)1;
   }
   else if((CAN1->TSR & (uint32_t)((uint32_t)1<<28)))
   {
       return (int8_t)1;
   }
#endif
   else
   {
       return (int8_t)0;
   }
}

/**
 *****************************************************************************
 * \fn          static int8_t rx_fifo_in(driver_can_t *p, driver_can_data_t data)
 * \brief       写数据到接收缓冲区.
 * \note        接收中断服务函数调用该函数写数据到接收缓冲区,缓冲区满不写入. 
 * \param[in]   p 指向can结构体实例的指针 \ref driver_can_t
 * \param[in]   data 指向要写到缓冲区的数据
 * \return      0成功 其他值失败
 *****************************************************************************
 */
static int8_t rx_fifo_in(driver_can_t *p, driver_can_data_t data)
{
    uint8_t i = 0;
    if((p==0) || (data.len>8))
    {
        return -1;
    }
    /*缓冲区未满写入数据*/
    if (p->rx_len < p->rxbuf_len) 
    {   
        p->rx_buf[p->rx_in].len = data.len;             /*写入数据,写入指针递增*/
        p->rx_buf[p->rx_in].type = data.type;
        p->rx_buf[p->rx_in].id = data.id;
        for(i=0;i<data.len;i++)
        {
          p->rx_buf[p->rx_in].buff[i] = data.buff[i];
        }
        p->rx_in++;
        p->rx_in %= p->rxbuf_len;                       /*边界处理,实现圆形缓冲区*/
        p->rx_len++;                                    /*数据个数递增*/
        return 0;
    }
    else
    {
        return -1;
    }
}


/**
 *****************************************************************************
 * \fn          static int8_t rx_fifo_out(driver_can_t *p, driver_can_data_t* buff,uint32_t timeout)
 * \brief       从接收缓冲区读数据.
 * \note        应用函数调用该函数从接收缓冲区读数据,可以指定超时时间. 
 * \param[in]   p 指向can结构体实例的指针 \ref driver_can_t
 * \param[out]  buff 指向can_data结构体实例的指针 \ref driver_can_data_t
 * \param[in]   timeout 指定超时时间 0表示一直等待 
 * \retval      int8_t 0成功 其他值失败.
 *****************************************************************************
 */
static int8_t rx_fifo_out(driver_can_t *p, driver_can_data_t* buff,uint32_t timeout)
{
    uint8_t i = (uint8_t)0;
    if(p==0)
    {
        return 1;
    }
    OSCriticalAlloc();
    /*timeout==0 表示一直等待直到有数据*/
    if (timeout == 0) 
    {
        while(p->rx_len == 0)
        {
            OsTimeDelay(DRIVER_CAN_DELAY_CHECK);  /*延时*/
        }
    } 
    else /*timeout!=0 表示等待指定超时时间*/
    {
        while (timeout != 0)
        {
            if (p->rx_len == 0) 
            {
                /*如果无数据,延时指定时间间隔*/
                if (timeout >= DRIVER_CAN_DELAY_CHECK)
                {
                    timeout -= DRIVER_CAN_DELAY_CHECK;
                    OsTimeDelay(DRIVER_CAN_DELAY_CHECK);
                }
                else
                {
                     /*不足一个时间间隔延时剩余时间*/
                    OsTimeDelay(timeout);
                    timeout = 0;
                }
            } 
            else
            {
                break;
            }
        }
    }
    /*临界段处理 防止此时中断在操作缓冲区*/
    OSCriticalEnter();
    if (p->rx_len != 0) 
    {
        /*从缓冲区读出数据*/
        p->rx_len--;
        buff->type = p->rx_buf[p->rx_out].type;
        buff->id = p->rx_buf[p->rx_out].id;
        if(p->rx_buf[p->rx_out].len > (uint8_t)8)
        {
            buff->len = (uint8_t)8;
        }
        else
        {
            buff->len = p->rx_buf[p->rx_out].len;
        }
        for(i=0;i<buff->len;i++)
        {
          buff->buff[i] = p->rx_buf[p->rx_out].buff[i];
        }
        p->rx_out++;
        p->rx_out %= p->rxbuf_len;
    }
    else
    {
        OSCriticalExit();
        return 1;
    }
    OSCriticalExit();
    return 0;
}


/**
 *****************************************************************************
 * \fn          static int8_t tx_fifo_in(driver_can_t *p, driver_can_data_t data, uint32_t timeout)
 * \brief       写数据到发送缓冲区.
 * \note        应用调用该函数调写数据到发送缓冲区,缓冲区满等待. 
 * \param[in]   p 指向can结构体实例的指针 \ref driver_can_t
 * \param[in]   data 指向can_data结构体实例的指针 \ref driver_can_data_t
 * \param[in]   timeout 设定超时时间 0表示一直等待
 * \retval      0 写成功
 * \retval      1 写失败
 *****************************************************************************
 */
static int8_t tx_fifo_in(driver_can_t *p, driver_can_data_t data, uint32_t timeout)
{
    /*注意确保p缓冲区的读写处于临界段保护 */
    int8_t ret = (int8_t)0;
    uint8_t i = (uint8_t)0;
    if((p==0) || (data.len>8))
    {
        return 1;
    }
    OSCriticalAlloc();
    if (timeout == 0) 
    {
       
        while(1)
        {
            OSCriticalEnter();
            if(p->tx_len < p->txbuf_len)
            {
                /*没有满则写入数据*/
                p->tx_buf[p->tx_in].len = data.len;             
                p->tx_buf[p->tx_in].type = data.type;
                p->tx_buf[p->tx_in].id = data.id;
                for(i=0;i<data.len;i++)
                {
                  p->tx_buf[p->tx_in].buff[i] = data.buff[i];
                }
                p->tx_in++;
                p->tx_in %= p->txbuf_len;
                p->tx_len++;
                OSCriticalExit();
                ret = (int8_t)0;
                break;
            }
            else
            {
                /*满则等待*/
                OSCriticalExit();
                OsTimeDelay(DRIVER_CAN_DELAY_CHECK);  /*延时*/
            }
        }
    } 
    while (timeout != 0)
    {
        OSCriticalEnter();
        if(p->tx_len < p->txbuf_len)
        {
          /*没有满则写入数据*/
          p->tx_buf[p->tx_in].len = data.len;             
          p->tx_buf[p->tx_in].type = data.type;
          p->tx_buf[p->tx_in].id = data.id;
          for(i=0;i<data.len;i++)
          {
            p->tx_buf[p->tx_in].buff[i] = data.buff[i];
          }
          p->tx_in++;
          p->tx_in %= p->txbuf_len;
          p->tx_len++;
          OSCriticalExit();
          ret = (int8_t)0;
          break;
        }
        else 
        {
          OSCriticalExit();
          /*如果无数据,延时指定时间间隔*/
          if (timeout >= DRIVER_CAN_DELAY_CHECK)
          {
            timeout -= DRIVER_CAN_DELAY_CHECK;
            OsTimeDelay(DRIVER_CAN_DELAY_CHECK);
            ret = (uint8_t)1; /*设置超时错误 之前这里没设置ret=1 在timeout为DRIVER_CAN_DELAY_CHECK的整数倍时总是会返回0*/
          }
          else
          {
            /*不足一个时间间隔延时剩余时间*/
            OsTimeDelay(timeout);
            timeout = (uint8_t)0;
            ret = 1; /*设置超时错误*/
          }
        }
    }
    return ret;
}


/**
 *****************************************************************************
 * \fn          static int8_t tx_fifo_out(driver_can_t *p, driver_can_data_t *buff)
 * \brief       从发送缓冲区读数据.
 * \note        发送函数调用该函数获取发送缓冲区数据,用于发送. 
 * \param[in]   p 指向can结构体实例的指针 \ref driver_can_t
 * \param[out]  buff 指向can_data结构体实例的指针 \ref driver_can_data_t
 * \return      int8_t 0成功 其他值失败  
 *****************************************************************************
 */
static int8_t tx_fifo_out(driver_can_t *p, driver_can_data_t *buff)
{
    uint8_t i = (uint8_t)0;
    if(p==0)
    {
        return 1;
    }
    OSCriticalAlloc();
    OSCriticalEnter();
    if (p->tx_len != 0) 
    {
        p->tx_len--;
        
        buff->type = p->tx_buf[p->tx_out].type;
        buff->id = p->tx_buf[p->tx_out].id;
        if(p->tx_buf[p->tx_out].len > (uint8_t)8)
        {
            buff->len = (uint8_t)8;
        }
        else
        {
            buff->len = p->tx_buf[p->tx_out].len;
        }
        for(i=0;i<buff->len;i++)
        {
          buff->buff[i] = p->tx_buf[p->tx_out].buff[i];
        }
        p->tx_out++;
        p->tx_out %= p->txbuf_len;
        OSCriticalExit();
    } 
    else
    {
        OSCriticalExit();
        return 1;
    }
    return 0;
}


void USB_LP_CAN1_RX0_IRQHandler(void)
{
   /*接收数据*/
   driver_can_data_t tmp;
   tmp.type = (uint8_t)0;
   uint8_t len = (uint8_t)0;
   uint32_t readdata;
   len = (uint8_t)(CAN1->RF0R & (uint8_t)0x03);  /*读清除状态*/
   while(len--)
   {
     if(0 != (CAN1->sFIFOMailBox[0].RIR & (uint32_t)((uint32_t)1<<1)))
     {
       tmp.type |= (uint8_t)((uint8_t)1<<CAN_DATA_RTR_BIT);
     }
     else
     {
       tmp.type &= (uint8_t)(~(uint8_t)((uint8_t)1<<CAN_DATA_RTR_BIT));
     }
     if(0 !=  (CAN1->sFIFOMailBox[0].RIR & (uint32_t)((uint32_t)1<<2)))
     {
       tmp.type |= (uint8_t)((uint8_t)1<<CAN_DATA_IDE_BIT);
     }
     else
     {
       tmp.type &= (uint8_t)(~(uint8_t)((uint8_t)1<<CAN_DATA_IDE_BIT));
     }
     tmp.id = (CAN1->sFIFOMailBox[0].RIR>>3) & (uint32_t)0x1FFFFFFF;
     tmp.len = (uint8_t)(CAN1->sFIFOMailBox[0].RDTR & (uint8_t)0x0F);
     if(tmp.len > (uint8_t)8)
     {
       tmp.len = (uint8_t)8;
     }
     else
     {}
     readdata = CAN1->sFIFOMailBox[0].RDLR;
     tmp.buff[0] = (uint8_t)(readdata & (uint32_t)0x00FF);
     tmp.buff[1] = (uint8_t)((readdata>>8) & (uint32_t)0x00FF);
     tmp.buff[2] = (uint8_t)((readdata>>16) & (uint32_t)0x00FF);
     tmp.buff[3] = (uint8_t)((readdata>>24) & (uint32_t)0x00FF);
     readdata = CAN1->sFIFOMailBox[0].RDHR;
     tmp.buff[4] = (uint8_t)(readdata & (uint32_t)0x00FF);
     tmp.buff[5] = (uint8_t)((readdata>>8) & (uint32_t)0x00FF);
     tmp.buff[6] = (uint8_t)((readdata>>16) & (uint32_t)0x00FF);
     tmp.buff[7] = (uint8_t)((readdata>>24) & (uint32_t)0x00FF);
     CAN1->RF0R |= (uint32_t)((uint32_t)1<<5);  /*释放FIFO 读取下一条*/
     if(rx_fifo_in(&g_driver_can_t, tmp) != 0)
     {
         return;   /*缓冲区不能写直接退出*/
     }
   }
}

void USB_HP_CAN1_TX_IRQHandler(void)
{


}


 void CAN1_RX1_IRQHandler(void)
 {
   /*接收数据*/
   driver_can_data_t tmp;
   tmp.type = (uint8_t)0;
   uint8_t len = (uint8_t)0;
   uint32_t readdata;
   len = (uint8_t)(CAN1->RF1R & (uint8_t)0x03);  /*读清除状态*/
   while(len--)
   {
     if(0 != (CAN1->sFIFOMailBox[1].RIR & (uint32_t)((uint32_t)1<<1)))
     {
       tmp.type |= (uint8_t)((uint8_t)1<<CAN_DATA_RTR_BIT);
     }
     else
     {
       tmp.type &= (uint8_t)(~(uint8_t)((uint8_t)1<<CAN_DATA_RTR_BIT));
     }
     if(0 !=  (CAN1->sFIFOMailBox[1].RIR & (uint32_t)((uint32_t)1<<2)))
     {
       tmp.type |= (uint8_t)((uint8_t)1<<CAN_DATA_IDE_BIT);
     }
     else
     {
       tmp.type &= (uint8_t)(~(uint8_t)((uint8_t)1<<CAN_DATA_IDE_BIT));
     }
     tmp.id = (CAN1->sFIFOMailBox[1].RIR>>3) & (uint32_t)0x1FFFFFFF;
     tmp.len = (uint8_t)(CAN1->sFIFOMailBox[1].RDTR & (uint8_t)0x0F);
     if(tmp.len > (uint8_t)8)
     {
       tmp.len = (uint8_t)8;
     }
     else
     {}
     readdata = CAN1->sFIFOMailBox[1].RDLR;
     tmp.buff[0] = (uint8_t)(readdata & (uint32_t)0x00FF);
     tmp.buff[1] = (uint8_t)((readdata>>8) & (uint32_t)0x00FF);
     tmp.buff[2] = (uint8_t)((readdata>>16) & (uint32_t)0x00FF);
     tmp.buff[3] = (uint8_t)((readdata>>24) & (uint32_t)0x00FF);
     readdata = CAN1->sFIFOMailBox[1].RDHR;
     tmp.buff[4] = (uint8_t)(readdata & (uint32_t)0x00FF);
     tmp.buff[5] = (uint8_t)((readdata>>8) & (uint32_t)0x00FF);
     tmp.buff[6] = (uint8_t)((readdata>>16) & (uint32_t)0x00FF);
     tmp.buff[7] = (uint8_t)((readdata>>24) & (uint32_t)0x00FF);
     CAN1->RF1R |= (uint32_t)((uint32_t)1<<5);  /*释放FIFO 读取下一条*/
     if(rx_fifo_in(&g_driver_can_t, tmp) != 0)
     {
         return;   /*缓冲区不能写直接退出*/
     }
   }
 }

 void CAN1_SCE_IRQHandler(void)
 {
    /*发送 错误数据*/
   CAN1->MSR |= CAN1->MSR; /*清除标志*/
   CAN1->TSR |= CAN1->TSR;
   
 }



/*******************************************************************************    
*                                                                           
*                             对外接口函数实现                                   
*                                                                            
*******************************************************************************/

/**
********************************************************************************
* \fn          void driver_can_init(uint32_t baudrate)
* \brief       初始化CAN.
* \param[in]   baudrate 波特率.
* \note        . 
********************************************************************************
*/
void driver_can_init(uint32_t baudrate)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  uint32_t timeout = (uint32_t)10000;
  uint32_t baudset = 0;
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  
  driver_can_ioinit();
  /*中断配置*/
  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=14 ;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);  
   
  NVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=14 ;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);  
  
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=14 ;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);  
  
  NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=14 ;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);    
  
  /*1.进入初始化模式:复位后是SLEEP模式 ->SLEEP=0 INRQ=1 进入初始化模式*/
  CAN1->MCR &= ~(1<<1); /*SLEEP=0*/
  CAN1->MCR |=  (1<<0); /*INAR=1*/
  while((CAN1->MSR & (1<<0)) ==0);  /*等待INAK置1  初始化完成*/
  /*2.配置*/
  CAN1->MCR &= ~(1<<16); /*禁用时间触发模式*/
  CAN1->MCR &= ~(1<<7); /*禁用时间触发模式*/
  CAN1->MCR |=  (1<<6); /*BUS-off 自动恢复*/
  CAN1->MCR |=  (1<<5); /*使能自动唤醒*/
  CAN1->MCR &= ~(1<<4); /*使能自动重发*/
  CAN1->MCR &= ~(1<<3); /*接收缓冲区溢出则覆盖*/
  CAN1->MCR &= ~(1<<2); /*发送优先级按ID确认*/
  
  CAN1->MSR |= (1<<4) | (1<<3) | (1<<2); /*清除标志*/
  CAN1->TSR |= (uint32_t)0x000F0F0F;/*清除标志*/
  CAN1->RF0R |= (1<<4) | (1<<3); /*清除标志*/
  CAN1->RF1R |= (1<<4) | (1<<3); /*清除标志*/
  
  
  CAN1->IER = 0x00038F7E; /*使能除发送以外的所有中断所有中断*/
  
  CAN1->ESR = (uint32_t)0x70;
  
  /*
  BaudRate= 1/NominalBitTime
  NominalBitTime =1xtq + tBS1 + tBS2
  tBS1 = tq x (TS1[3:0] + 1)
  tBS2 = tq x (TS2[2:0] + 1)
  tq = (BRP[9:0] + 1) x tPCLK
   ts1 ts2设置为0
  BaudRate = 1/3tq
  */
  baudset = RCC_Clocks.PCLK1_Frequency/((uint32_t)8 * baudrate) -1;
  //CAN1->BTR = (uint32_t)0xC1230000 | (uint32_t)baudset;  /*回环模式  设置波特率*/
  CAN1->BTR = (uint32_t)0x01230000 | (uint32_t)baudset;  /*正常模式  设置波特率*/

  /*4.进入正常模式:INRQ=0*/
  CAN1->MCR &=  ~(1<<0); /*INRQ=0*/
  while((CAN1->MSR & (1<<0)) && (timeout--));  /*等待INAK清0  进入完成*/
  
  /*3.设置接收过滤*/
  CAN1->FMR |= (uint32_t)0x01;         /* FINIT=1才能设置后续寄存器*/
  CAN1->FM1R = (uint32_t)0x00000000;  /*所有过滤器使用MASK模式*/
  CAN1->FS1R = (uint32_t)0x0FFFFFFF;  /*所有过滤器使用32位匹配模式*/
  CAN1->FFA1R = (uint32_t)0x0FFFFFFF;  /*放在一个FIFO避免接收时不按序*/
  CAN1->FA1R = 0;  /*关闭才能配置*/
  /*过滤ID设置*/
  CAN1->sFilterRegister[0].FR1 = (g_driver_idlist_au32[0]<<3) | (uint32_t)0x04;  /*扩展数据帧0x04 标准数据帧0x00  扩展远程帧0x03 标准远程帧0x02  标准帧<<21位*/
  CAN1->sFilterRegister[0].FR2 = (g_driver_idlist_au32[1]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[1].FR1 = (g_driver_idlist_au32[2]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[1].FR2 = (g_driver_idlist_au32[3]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[2].FR1 = (g_driver_idlist_au32[4]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[2].FR2 = (g_driver_idlist_au32[5]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[3].FR1 = (g_driver_idlist_au32[6]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[3].FR2 = (g_driver_idlist_au32[7]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[4].FR1 = (g_driver_idlist_au32[8]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[4].FR2 = (g_driver_idlist_au32[9]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[5].FR1 = (g_driver_idlist_au32[10]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[5].FR2 = (g_driver_idlist_au32[11]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[6].FR1 = (g_driver_idlist_au32[12]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[6].FR2 = (g_driver_idlist_au32[13]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[7].FR1 = (g_driver_idlist_au32[14]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[7].FR2 = (g_driver_idlist_au32[15]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[8].FR1 = (g_driver_idlist_au32[16]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[8].FR2 = (g_driver_idlist_au32[17]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[9].FR1 = (g_driver_idlist_au32[18]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[9].FR2 = (g_driver_idlist_au32[19]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[10].FR1 = (g_driver_idlist_au32[20]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[10].FR2 = (g_driver_idlist_au32[21]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[11].FR1 = (g_driver_idlist_au32[22]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[11].FR2 = (g_driver_idlist_au32[23]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[12].FR1 = (g_driver_idlist_au32[24]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[12].FR2 = (g_driver_idlist_au32[25]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[13].FR1 = (g_driver_idlist_au32[26]<<3) | (uint32_t)0x04;
  CAN1->sFilterRegister[13].FR2 = (g_driver_idlist_au32[27]<<3) | (uint32_t)0x04;
  //CAN1->FA1R = (uint32_t)0xFFFFFFF;  /*不使能*/
  CAN1->FMR &= (uint32_t)~0x01;         /* 使能过滤*/
}



/**
********************************************************************************
* \fn          void driver_can_filter(uint_t mode,uint32_t id,uint32_t mask)
* \brief       设置CAN过滤.
* \param[in]   ch 通道0-13.
* \param[in]   mode 0:MASK模式 1:list模式.
* \param[in]   id id.
* \param[in]   mask 掩码.
* \note        . 
********************************************************************************
*/
void driver_can_filter(uint8_t ch,uint8_t mode,uint32_t id,uint32_t mask)
{
  /*3.设置接收过滤*/
  CAN1->FMR |= (uint32_t)0x01;         /* FINIT=1才能设置后续寄存器*/
  if(mode == (uint8_t)1)
  {
    CAN1->FM1R |= (uint32_t)((uint32_t)1<<ch);  /*使用LIST模式*/
  }
  else
  {
    CAN1->FM1R &= (uint32_t)(~(uint32_t)((uint32_t)1<<ch));  /*使用MSK模式*/
  }

  CAN1->FA1R &= (uint32_t)(~(uint32_t)((uint32_t)1<<ch));  /*关闭才能配置*/
  /*过滤ID设置*/
  CAN1->sFilterRegister[ch].FR1 = id<<3;  /*扩展数据帧0x04 标准数据帧0x00  扩展远程帧0x03 标准远程帧0x02  标准帧<<21位*/
  CAN1->sFilterRegister[ch].FR2 = mask<<3;
  CAN1->FA1R |= (uint32_t)((uint32_t)1<<ch);   /*使能*/
  CAN1->FMR &= (uint32_t)~0x01;         /* 使能过滤*/
}

/**
 *****************************************************************************
 * \fn          int8_t driver_can_send(driver_can_data_t data, uint32_t timeout)
 * \brief       发送CAN数据(写入缓冲区).
 * \note        应用调用该函数调写数据到发送缓冲区,缓冲区满等待. 
 * \param[in]   data 指向can_data结构体实例的指针 \ref driver_can_data_t
 * \param[in]   timeout 设定超时时间 0表示一直等待
 * \retval      0 写成功
 * \retval      1 写失败
 *****************************************************************************
 */
int8_t driver_can_send(driver_can_data_t data, uint32_t timeout)
{
    return tx_fifo_in(&g_driver_can_t, data, timeout); 
}

/**
 *****************************************************************************
 * \fn          int8_t driver_can_recv(driver_can_data_t* data, uint32_t timeout)
 * \brief       从接收缓冲区读数据.
 * \note        应用函数调用该函数从接收缓冲区读数据,可以指定超时时间. 
 * \param[out]  data 指向can_data结构体实例的指针 \ref driver_can_data_t
 * \param[in]   timeout 指定超时时间 0表示一直等待 
 * \retval      int8_t 0成功 其他值失败.
 *****************************************************************************
 */
int8_t driver_can_recv(driver_can_data_t* data, uint32_t timeout)
{
  return rx_fifo_out(&g_driver_can_t, data, timeout);
}

/**
 *****************************************************************************
 * \fn          int8_t driver_can_sendloop(void)
 * \brief       如果发送消息邮箱有空,且有发送数据,则将发送缓冲区数据写入发送消息邮箱.
 * \note        应用周期调用该函数进行发送处理. 
 * \retval      0 有发送
 * \retval      1 无发送
 *****************************************************************************
 */
//int8_t driver_can_sendloop(void)
//{
//  int8_t res = (int8_t)0;
//  driver_can_data_t tmp;
//  do
//  {
//    if(is_txmail_empty() != (uint8_t)0)
//    {
//      if(tx_fifo_out(&g_driver_can_t, &tmp) == (int8_t)0)
//      {
//        res = driver_can_writemailbox(tmp);
//      }
//      else
//      {
//        res = (int8_t)1;
//      }
//    }
//    else
//    {
//      res = (int8_t)1;
//    }
//  }while(res == 0);
//  return res;
//}

int8_t driver_can_sendloop(void)
{
    static uint32_t g_check_empty_u32 = 0;
    int8_t res = (int8_t)0;
    driver_can_data_t tmp;
    do
    {
        if(is_txmail_empty() != (uint8_t)0)
        {
            g_check_empty_u32 = 0;
            if(tx_fifo_out(&g_driver_can_t, &tmp) == (int8_t)0)
            {
              res = driver_can_writemailbox(tmp);
            }
            else
            {
              res = (int8_t)1;
            }
        }
        else
        {
            g_check_empty_u32++;
            if(g_check_empty_u32 >= 100)  /*如果100次查询发送邮箱均非空说明异常重新初始化(邮箱发送一帧不可能超过100ms)*/
            {
                g_check_empty_u32 = 0;
                res = (int8_t)1;
                driver_can_init((uint32_t)500000);
                driver_can_filter(0,0,(uint32_t)kiss_get_clientaddr()<<13,(uint32_t)0xFF<<13);
                driver_can_filter(1,0,(uint32_t)kiss_get_clientaddr()<<19,(uint32_t)0x1F<<19);
            }
            OsTimeDelay(1);
        }
    }
    while(res == 0);
    return res;
}

