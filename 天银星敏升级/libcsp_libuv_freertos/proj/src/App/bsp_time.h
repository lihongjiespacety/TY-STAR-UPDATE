#ifndef BSP_TIME_H
#define BSP_TIME_H

#ifdef __cplusplus
 extern "C" {
#endif
   
#include <stdint.h>
   
typedef void (*bsp_time_callback)(void);
void bsp_time6_init(uint32_t frq,uint32_t period);
void bsp_time6_setcallback(bsp_time_callback callbackfun);

#ifdef __cplusplus
}
#endif

#endif
   

