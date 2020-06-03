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

#ifndef _CAN_H_
#define _CAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <csp/csp.h>
#include <csp/interfaces/csp_if_can.h>

/* The can_frame_t and can_id_t types intentionally matches the
 * can_frame struct and can_id types in include/linux/can.h
 */

/** CAN Identifier */
typedef uint32_t can_id_t;

/** CAN Frame */
typedef struct {
	/** 32 bit CAN identifier */
	can_id_t id;
	/** Data Length Code */
	uint8_t dlc;
	/**< Frame Data - 0 to 8 bytes */
	union __attribute__((aligned(8))) {
		uint8_t data[8];
		uint16_t data16[4];
		uint32_t data32[2];
	};
} can_frame_t;

typedef enum  {
	CFP_RESET = 0,
	CFP_SINGLE = 1,
	CFP_BEGIN = 2,
	CFP_MORE = 3,
	CFP_TIMESYNC = 4
}cfp_ext_t;

typedef enum
{
	OBC_CANID = 0,
	UV_CANID = 0x05,
    EPS_CANID = 0x17,
	FW_UPDATER_CANID = 0x7F,
} can_id;

typedef enum {
	CAN_ERROR = 0,
	CAN_NO_ERROR = 1,
} can_error_t;

//int can_init(uint32_t id, uint32_t mask, struct csp_can_config *conf);
int can_send(can_id_t id, uint8_t * data, uint8_t dlc);
int csp_can_rx_frame(can_frame_t *frame, CSP_BASE_TYPE *task_woken);
void can_pbuf_cleanup(void);
int can_process_frame(can_frame_t *f);
int process_eps_can_request(csp_packet_t* in);
int tianyi_fw_can_tx(uint8_t *b, int len, int dst_id, int delay, int has_cfpid);
/* 0 ³É¹¦ 1Ê§°Ü
 * 
 */
int8_t  can_tx_raw_data(uint8_t dest, uint8_t *data, uint16_t len,cfp_ext_t type,uint8_t has_cfpid,uint32_t delay);
extern uint8_t kiss_get_clientaddr(void);
extern void kiss_set_clientaddr(uint8_t addr);
extern uint8_t kiss_get_serveraddr(void);
extern void kiss_set_serveraddr(uint8_t addr);
extern uint8_t kiss_get_serverport(void);
extern void kiss_set_serverport(uint8_t addr);

extern unsigned int g_can_send_packet;
extern unsigned int g_can_rcv_packet;
extern unsigned int g_can_rcverr_packet;

#define PBUF_MTU (1024+20)
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _CAN_H_ */
