/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 GomSpace ApS (http://www.gomspace.com)
Copyright (C) 2012 AAUSAT3 Project (http://aausat3.space.aau.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* SocketCAN driver */

#include <stdint.h>
#include <stdio.h>

#include <csp/csp.h>
#include <csp/interfaces/csp_if_can.h>
#include <csp/drivers/can.h>

#include "driver_can.h"
#include "can.h"
#include "csp_if_can.h"

void * can_rx_handle(void * parameters)
{
  static uint8_t s_rx_idlenum =0;
  uint8_t i;
  can_frame_t frame;
  int8_t ret = 0;
  driver_can_data_t tmp_can_recvdata;
  do
  {
    if((int8_t)0 == (ret=driver_can_recv(&tmp_can_recvdata, (uint32_t)1000)))
    {
      frame.id = tmp_can_recvdata.id;
      if(tmp_can_recvdata.len > (uint8_t)8)
      {
        frame.dlc = (uint8_t)8;
      }
      else
      {
        frame.dlc = tmp_can_recvdata.len;
      }
      for(i=0;i<frame.dlc;i++)
      {
        frame.data[i] = tmp_can_recvdata.buff[i];
        
      }
      /* Call RX callback */
      can_process_frame(&frame);
      s_rx_idlenum=0;
    }
    else
    {
      s_rx_idlenum++;
      if(s_rx_idlenum>=10)
      {
        s_rx_idlenum=0;
        can_pbuf_cleanup();
      }
    }
  }while(ret == 0);
  return 0;
}

int can_send(can_id_t id, uint8_t data[], uint8_t dlc)
{
  driver_can_data_t senddata;
  uint8_t i;
  uint8_t tries = 10;
  int8_t res;
  if (dlc > 8)
  {
    return -1;
  }
  else
  {
    senddata.type = 0x02;
    senddata.len = dlc;
    senddata.id = id;
    for(i=0;i<dlc;i++)
    {
      senddata.buff[i] = data[i];
    }
    do
    {
      res = driver_can_send(senddata, (uint32_t)500);
    }
    while((res != 0) && (tries--));
    return res;
  }
}

int can_init(uint32_t id, uint32_t mask, struct csp_can_config *conf)
{
  driver_can_init(conf->bitrate);
  driver_can_filter(0,0,id,mask);
  return 0;
}
