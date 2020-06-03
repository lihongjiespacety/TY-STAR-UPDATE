

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "shell_fun.h"
#include "hal.h"
#include "driver.h"
#include "osapi_freertos.h"
#include "shell.h"
#include "csp_clock.h"

csp_timestamp_t g_clock_t = {(uint32_t)0,(uint32_t)0};

void vApplicationTickHook()
{
    g_clock_t.tv_nsec += (uint32_t)1000000 / configTICK_RATE_HZ;
    while(g_clock_t.tv_nsec >= (uint32_t)1000000)
    {
      g_clock_t.tv_nsec -= (uint32_t)1000000;
      g_clock_t.tv_sec++;
      driver_time_sechandle();
    }
}


void clock_get_time(csp_timestamp_t * time)
{
  time->tv_nsec = g_clock_t.tv_nsec;
  time->tv_sec =  g_clock_t.tv_sec;
}

void clock_set_time(csp_timestamp_t * time)
{
  g_clock_t.tv_nsec = time->tv_nsec;
  g_clock_t.tv_sec = time->tv_sec;
}