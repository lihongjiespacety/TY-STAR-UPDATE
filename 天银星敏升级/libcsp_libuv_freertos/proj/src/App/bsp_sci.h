#include "hal.h"
typedef enum  sci_port SCI_PORT_e; /**< SCI端口枚举定义  */
   	      
/**
 * \enum sci_port
 * SCI端口枚举定义枚举.
 */
enum sci_port
{
    SCI_0=0,           /**< SCI0 */ 
    SCI_1=1,           /**< SCI1 */
    SCI_2=2,           /**< SCI2 */ 
    SCI_5=3,           /**< SCI5 */
    SCI_6=4,           /**< SCI6 */ 
    SCI_8=5,           /**< SCI8 */
    SCI_9=6,           /**< SCI9 */ 
    SCI_12=7,          /**< SCI12 */

};

int BSP_SCI_Init(HAL_UART_ID_e id);
int BSP_SCI_Send(HAL_UART_ID_e id,unsigned char byte);
int BSP_SCI_CheckSend(HAL_UART_ID_e id,unsigned char byte);
int BSP_SCI_Read(HAL_UART_ID_e id,unsigned char* byte);
int BSP_SCI_CheckRead(HAL_UART_ID_e id,unsigned char* byte);
int BSP_SCI_Test(void);

int BSP_SCI_SetTxCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun);
int BSP_SCI_SetTxCompleteCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun);
int BSP_SCI_SetRxCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun);
int BSP_SCI_SetRxErrCallBack(HAL_UART_ID_e id,HAL_UART_ISRCallBack callbackfun);
int BSP_SCI_SetCfg(HAL_UART_ID_e id,HAL_UART_CFG_t* cfg);
int BSP_SCI_EnableTx(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable);
int BSP_SCI_EnableRx(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable);
int BSP_SCI_EnableRxErr(HAL_UART_ID_e id,HAL_UART_TYPE_e type,HAL_UART_ENABLE_e enable);
int BSP_SCI_DeInit(HAL_UART_ID_e id);