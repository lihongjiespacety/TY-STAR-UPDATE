#include "iorx113.h"
#include "hal.h"
#include "bsp.h"

#define BSP_VERSION 0x11



int BSP_GetVersion(void)
{
    return BSP_VERSION;
}

int BSP_Init(void)
{
    while(1)
    {
        if(BSP_CGC_Init()!=0)
        {
            /*使用外部晶振初始化失败*/
            if(BSP_CGC_SwitchToHOCO()!=0)
            {
                /*切换到内部告诉时钟*/
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    BSP_LPC_Init();
    BSP_IOP_Init();
    BSP_SPI_Init(HAL_SPI_0);
    BSP_IWDT_Init();
    BSP_CMP_Init();
    return 0;
}

/**
 *****************************************************************************
 * \fn          int BSP_DeInit(void)
 * \brief       解除BSP层初始化.
 * \note        .
 * \retval      0  成功.
 * \retval      -1 失败.
 *****************************************************************************
 */
int BSP_DeInit(void)
{
    return 0;
}