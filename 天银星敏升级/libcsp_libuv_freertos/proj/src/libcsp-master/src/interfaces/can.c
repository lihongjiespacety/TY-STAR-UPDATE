/* CAN frames contains at most 8 bytes of data, so in order to transmit CSP
 * packets larger than this, a fragmentation protocol is required. The CAN
 * Fragmentation Protocol (CFP) header is designed to match the 29 bit CAN
 * identifier.
 *
 * The CAN identifier is divided in these fields:
 * src:          8 bits
 * dst:          8 bits
 * type:         4 bit
 * remain:       1 bits
 * identifier:   8 bits
 *
 * Source and Destination addresses must match the CSP packet. The type field
 * is used to distinguish the first and subsequent frames in a fragmented CSP
 * packet. Type is BEGIN (0) for the first fragment and MORE (1) for all other
 * fragments. Remain indicates number of remaining fragments, and must be
 * decremented by one for each fragment sent. The identifier field serves the
 * same purpose as in the Internet Protocol, and should be an auto incrementing
 * integer to uniquely separate sessions.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <csp/csp.h>
#include <csp/csp_interface.h>
#include <csp/csp_endian.h>
#include <csp/interfaces/csp_if_can.h>

#include <csp/arch/csp_semaphore.h>
#include <csp/arch/csp_time.h>
#include <csp/arch/csp_queue.h>
#include <csp/arch/csp_thread.h>

#include <csp/drivers/can.h>
/* EXTENDED FRAME DEFINITIONS */
#define CFP_HOST_SIZE		8
#define CFP_TYPE_SIZE		4
#define CFP_REMAIN_SIZE		1
#define CFP_ID_SIZE		    8

/* Macros for extracting header fields */
#define CFP_FIELD(id,rsiz,fsiz) ((uint32_t)((uint32_t)((id) >> (rsiz)) & (uint32_t)((1 << (fsiz)) - 1)))
#define CFP_SRC(id)		CFP_FIELD(id, CFP_HOST_SIZE + CFP_TYPE_SIZE + CFP_REMAIN_SIZE + CFP_ID_SIZE, CFP_HOST_SIZE)
#define CFP_DST(id)		CFP_FIELD(id, CFP_TYPE_SIZE + CFP_REMAIN_SIZE + CFP_ID_SIZE, CFP_HOST_SIZE)
#define CFP_TYPE(id)		CFP_FIELD(id, CFP_REMAIN_SIZE + CFP_ID_SIZE, CFP_TYPE_SIZE)
#define CFP_REMAIN(id)		CFP_FIELD(id, CFP_ID_SIZE, CFP_REMAIN_SIZE)
#define CFP_ID(id)		CFP_FIELD(id, 0, CFP_ID_SIZE)

/* Macros for building CFP headers */
#define CFP_MAKE_FIELD(id,fsiz,rsiz) ((uint32_t)(((id) & (uint32_t)((uint32_t)(1 << (fsiz)) - 1)) << (rsiz)))
#define CFP_MAKE_SRC(id)	CFP_MAKE_FIELD(id, CFP_HOST_SIZE, CFP_HOST_SIZE + CFP_TYPE_SIZE + CFP_REMAIN_SIZE + CFP_ID_SIZE)
#define CFP_MAKE_DST(id)	CFP_MAKE_FIELD(id, CFP_HOST_SIZE, CFP_TYPE_SIZE + CFP_REMAIN_SIZE + CFP_ID_SIZE)
#define CFP_MAKE_TYPE(id)	CFP_MAKE_FIELD(id, CFP_TYPE_SIZE, CFP_REMAIN_SIZE + CFP_ID_SIZE)
#define CFP_MAKE_REMAIN(id)	CFP_MAKE_FIELD(id, CFP_REMAIN_SIZE, CFP_ID_SIZE)
#define CFP_MAKE_ID(id)		CFP_MAKE_FIELD(id, CFP_ID_SIZE, 0)

/* Mask to uniquely separate connections */
#define CFP_ID_CONN_MASK	(CFP_MAKE_SRC((uint32_t)(1 << CFP_HOST_SIZE) - 1) | \
				 CFP_MAKE_DST((uint32_t)(1 << CFP_HOST_SIZE) - 1) | \
				 CFP_MAKE_REMAIN((uint32_t)(1 << CFP_REMAIN_SIZE) - 1) | \
				 CFP_MAKE_ID((uint32_t)(1 << CFP_ID_SIZE) - 1))

/* Maximum Transmission Unit for CSP over CAN */
#define CSP_CAN_MTU		256

/* Maximum number of frames in RX queue */
#define CSP_CAN_RX_QUEUE_SIZE	100
/* Number of packet buffer elements */
#define PBUF_ELEMENTS		CSP_CONN_MAX
/* Buffer element timeout in ms */
#define PBUF_TIMEOUT_MS		5000


/* CFP identification number */
static int extended_can_id = 0;
static int own_csp_can_id = 0;
int drop_can_tx_data = 0;

unsigned int g_can_send_packet = 0;
unsigned int g_can_rcv_packet = 0;
unsigned int g_can_rcverr_packet = 0;

//extern uint32_t clocks_per_msec;

//struct can_rx_action can_process_maps[256];
//static void init_can_process_maps(void);

/* Packet buffers */
typedef enum {
	BUF_FREE = 0,	/* Buffer element free */
	BUF_USED = 1,	/* Buffer element used */
} can_pbuf_state_t;

typedef struct {
	uint16_t rx_count;	/* Received bytes */
	uint32_t cfpid;		/* Connection CFP identification number */
	csp_packet_t *packet;	/* Pointer to packet buffer */
	can_pbuf_state_t state;	/* Element state */
	uint32_t last_used;	/* Timestamp in ms for last use of buffer */
} can_pbuf_element_t;

static can_pbuf_element_t can_pbuf[PBUF_ELEMENTS];

extern uint8_t ty_starss_flag;
static int
can_pbuf_init(void)
{
	/* Initialize packet buffers */
	int i;
	can_pbuf_element_t *buf;

	for (i = 0; i < PBUF_ELEMENTS; i++) {
		buf = can_pbuf + i;
		buf->rx_count = 0;
		buf->cfpid = 0;
		buf->packet = NULL;
		buf->state = BUF_FREE;
		buf->last_used = 0;
	}

	return 0;
}

static void
can_pbuf_timestamp(can_pbuf_element_t *buf)
{
	if (buf != NULL)
		buf->last_used = csp_get_ms();
}

static int
can_pbuf_free(can_pbuf_element_t *buf)
{
	if (buf != NULL) {
		/* Free CSP packet */
		if (buf->packet != NULL)
			csp_buffer_free(buf->packet);

		/* Mark buffer element free */
		buf->packet = NULL;
		buf->state = BUF_FREE;
		buf->rx_count = 0;
		buf->cfpid = 0;
		buf->last_used = 0;
	}

	return 0;
}

static can_pbuf_element_t *
can_pbuf_new(uint32_t id)
{
	int i;
	can_pbuf_element_t *buf, *ret = NULL;

	for (i = 0; i < PBUF_ELEMENTS; i++) {
		buf = can_pbuf + i;
		if (buf->state == BUF_FREE) {
			buf->state = BUF_USED;
			buf->cfpid = id;
			can_pbuf_timestamp(buf);
			ret = buf;
			break;
		}
	}

	return ret;
}

static can_pbuf_element_t *
can_pbuf_find(uint32_t id, uint32_t mask)
{
	int i;
	can_pbuf_element_t *buf, *ret = NULL;

	for (i = 0; i < PBUF_ELEMENTS; i++) {
		buf = can_pbuf + i;

		if ((buf->state == BUF_USED) && ((buf->cfpid & mask) == (id & mask))) {
			can_pbuf_timestamp(buf);
			ret = buf;
			break;
		}
	}

	return ret;
}

void
can_pbuf_cleanup(void)
{
	int i;
	can_pbuf_element_t *buf;

	for (i = 0; i < PBUF_ELEMENTS; i++) {
		buf = can_pbuf + i;

		/* Skip if not used */
		if (buf->state != BUF_USED)
			continue;

		/* Check timeout */
		uint32_t now = csp_get_ms();
		if (now - buf->last_used > PBUF_TIMEOUT_MS) {
			if (buf->packet != NULL) {
				csp_log_warn("CPBUF TIMEOUT ERROR, CANID 0x%x, CFPID 0x%x\n", (unsigned int) CFP_SRC(buf->packet->id.ext), (unsigned int) CFP_ID(buf->packet->id.ext));
			}
			/* Recycle packet buffer */
			can_pbuf_free(buf);
		}
	}
}

uint8_t
get_dst_canid(can_id_t id)
{
	return (CFP_DST(id) & 0xff);
}

int
is_libcsp(can_id_t id)
{
	return CFP_REMAIN(id);
}

uint32_t
get_can_extid(int dst, int type, int libcsp, int cfp_id)
{
	uint32_t id;

	id = 0;
	id |= CFP_MAKE_DST(dst);
	id |= CFP_MAKE_TYPE(type);
	id |= CFP_MAKE_REMAIN(libcsp);
	id |= CFP_MAKE_ID(cfp_id);

	return id;
}

/* EXTENED CAN FRAME */
/* Identification number */
static int
extended_can_id_init(void)
{
	/* Init ID field to random number */
	srand((int)csp_get_ms());
	extended_can_id = rand() & ((1 << CFP_ID_SIZE) - 1);

	return 0;
}

int
extended_can_id_get(void)
{
	int id;

	id = extended_can_id ++;
	extended_can_id = extended_can_id & ((1 << CFP_ID_SIZE) - 1);
    id = 0;
	return id;
}


int can_process_frame(can_frame_t *f)
{
	can_pbuf_element_t *buf = NULL;
	uint8_t offset, ext_type;
	int r = 0;
	uint32_t id;
	uint8_t dst_id = 0, src_id = 0, cfp_id = 0;

	if (f == NULL)
		return 1;

	id = f->id;

	/* Bind incoming frame to a packet buffer */
	dst_id = CFP_DST(id);
	src_id = CFP_SRC(id);
	cfp_id = CFP_ID(id);

	/* don't process can frame from myself, or not our business */
	//if (src_id == EPS_CANID || src_id == own_csp_can_id || (dst_id != own_csp_can_id && dst_id != EPS_CANID))
		//return 1;

	buf = can_pbuf_find(id, CFP_ID_CONN_MASK);
	/* Check returned buffer */
	ext_type = CFP_TYPE(id);
	if (buf == NULL) {
		if (ext_type == CFP_BEGIN || ext_type == CFP_RESET || ext_type == CFP_TIMESYNC || ext_type == CFP_SINGLE) {
			buf = can_pbuf_new(id);
			if (buf == NULL) {
				csp_log_error("NO FREE CAN PBUF\n");
				csp_if_can.rx_error ++;
                                g_can_rcverr_packet++;
				return 1;
			}
		} else {
			csp_log_error("CFP_MORE FRAME WITHOUT PBUF, CANID 0x%x, CFPID 0x%x\n", src_id, cfp_id);
			csp_if_can.frame ++;
                        g_can_rcverr_packet++;
			return 1;
		}
	}

	/* Reset frame data offset */
	offset = 0;
	switch (ext_type) {
	case CFP_RESET:
	case CFP_TIMESYNC:
	case CFP_SINGLE:
		if (buf->packet == NULL) {
			buf->packet = csp_buffer_get(PBUF_MTU);
			if (buf->packet == NULL) {
				csp_log_error("NO FREE CSP BUFFER, CANID 0x%x, CFPID 0x%x\n", src_id, cfp_id);
				csp_if_can.frame ++;
				can_pbuf_free(buf);
                                g_can_rcverr_packet++;
				break;
			}
		}
		buf->packet->id.ext = id;
		buf->packet->length = f->dlc;
		memcpy(buf->packet->data, f->data, f->dlc);
		buf->rx_count = f->dlc;
		r = 1;
		break;
	case CFP_BEGIN:
		/* Check for incomplete frame */
		if (buf->packet != NULL) {
			/* Reuse the buffer */
			csp_if_can.frame ++;
			csp_buffer_free(buf->packet);
		}

		/* Allocate memory for frame */
		buf->packet = csp_buffer_get(PBUF_MTU);

		if (buf->packet == NULL) {
			csp_log_error("NO FREE CSP BUFFER, CANID 0x%x, CFPID 0x%x\n", src_id, cfp_id);
			csp_if_can.frame ++;
			can_pbuf_free(buf);
                        g_can_rcverr_packet++;
			break;
		}

		if (CFP_REMAIN(id)) { /* libcsp packet */
			/* Copy CSP identifier and length*/
			memcpy(&(buf->packet->id), f->data, sizeof(csp_id_t));
			buf->packet->id.ext = csp_ntoh32(buf->packet->id.ext);
			memcpy(&(buf->packet->length), f->data + sizeof(csp_id_t), sizeof(uint16_t));
			buf->packet->length = csp_ntoh16(buf->packet->length);
			/* Set offset to prevent CSP header from being copied to CSP data */
			offset = sizeof(csp_id_t) + sizeof(uint16_t);
		} else { /* normal packet */
			buf->packet->id.ext = id;
			buf->packet->length = (f->data[0] << 8) + f->data[1]; /* network order / big endian */
			///if(((CFP_SRC(id)) == STAR_SENSOR_CANID) && (ty_starss_flag == 1))
				///buf->packet->length += 2; /* 2b length + sumofbuffer byte */
			///else
				buf->packet->length += 3; /* 2b length + sumofbuffer byte */
			offset = 0;
		}

		/* Reset RX count */
		buf->rx_count = 0;

		/* Set offset to prevent CSP header from being copied to CSP data */
		/* FALL THROUGH */
	case CFP_MORE:
		/* double check buf->packet because we may encounter out of order CFP_MORE */
		if (buf->packet == NULL) {
			csp_log_error("CFP_MORE FRAME WITHOUT PBUF, CANID 0x%x, CFPID 0x%x\n", src_id, cfp_id);
			csp_if_can.frame ++;
			can_pbuf_free(buf);
                        g_can_rcverr_packet++;
			break;
		}

		/* Check for overflow */
		if ((buf->rx_count + f->dlc - offset) > buf->packet->length) {
			csp_log_error("RX BUFFER OVERFLOW #%d > #%d, CANID 0x%x, CFPID 0x%x\n", buf->rx_count + f->dlc - offset, buf->packet->length, src_id, cfp_id);
			can_pbuf_free(buf);
			csp_if_can.frame++;
                        g_can_rcverr_packet++;
			break;
		}

		/* Copy dlc bytes into buffer */
		memcpy(buf->packet->data + buf->rx_count, f->data + offset, f->dlc - offset);
		buf->rx_count += f->dlc - offset;

		/* Check if all data is received */
		if (buf->rx_count >= buf->packet->length) {
			r = 1;
		}
		break;
	default:
		csp_log_error("UNKOWN CAN TYPE 0x%x, CANID 0x%x, CFPID 0x%x\n", ext_type, src_id, cfp_id);
		can_pbuf_free(buf);
                g_can_rcverr_packet++;
		break;
	}

	if (r) {
		/* new data */
		if (CFP_REMAIN(buf->cfpid)) {
			/* libcsp packet */
			csp_new_packet(buf->packet, &csp_if_can, NULL);
		} else {
			/* normal packet */
			//tianyi_can_handler(buf->packet);  ！！！调用自定义处包处理函数
                        process_eps_can_request(buf->packet);
                        csp_buffer_free(buf->packet);
		}
		/* Drop packet buffer reference */
		buf->packet = NULL;
		/* Free packet buffer */
		can_pbuf_free(buf);
                g_can_rcv_packet++;
	}

	return 0;
}



/* return 0 if OK
 * return 1 if failed.
 * no matter tx result is OK or fail, make sure that csp_packet is freed before exit.
 */
int
tianyi_can_tx(csp_packet_t *packet)
{
	uint16_t tx_count;
	uint8_t bytes, overhead, avail, dest;
	uint8_t frame_buf[9];
	can_id_t id, id2;
	int ext_type, ident;

	if (packet == NULL) return 1;

	/* Get CFP identification number */
	ident = extended_can_id_get(); // CFP_ID(packet->id.ext);
	if (ident < 0) {
		csp_log_error("Failed to get CFP identification number\n");
		//csp_buffer_free(packet);
		return 1;
	}

	/* Calculate overhead */
	overhead = 0;

	dest = CFP_DST(packet->id.ext);
	//if (dest == own_csp_can_id || dest == EPS_CANID) { /* to OBC data? should not happen */
		//csp_buffer_free(packet);
		//return 1;
	//}

	/* Create CAN identifier */
	id = 0;
	id |= CFP_MAKE_SRC(kiss_get_clientaddr());
	id |= CFP_MAKE_DST(dest);
	id |= CFP_MAKE_ID(ident);
	id |= CFP_MAKE_REMAIN(0); /* not libcsp packet */
	id |= (uint32_t)((uint32_t)1<<31);

	id2 = id;
	ext_type = CFP_TYPE(packet->id.ext);
	if (ext_type == CFP_RESET || ext_type == CFP_TIMESYNC) {
		/* single frame */
		if (packet->length > 8) {
			//csp_buffer_free(packet);
			return 1;
		}
		id |= CFP_MAKE_TYPE(ext_type);
	} else if (packet->length <= 8) {
		/* single frame */
		id |= CFP_MAKE_TYPE(CFP_SINGLE);
	} else {
		overhead = 0;//sizeof(uint16_t);
		id |= CFP_MAKE_TYPE(CFP_BEGIN);
		/* network order / big endian */
	}

	/* Calculate first frame data bytes */
	avail = 8 - overhead;
	bytes = (packet->length <= avail) ? packet->length : avail;

	memcpy(frame_buf + overhead, packet->data, bytes);

	/* Increment tx counter */
	tx_count = bytes;

	/* Send first frame */
	frame_buf[overhead + bytes] = '\0';
	if (can_send(id, frame_buf, overhead + bytes)) {
		csp_log_error("Failed to send CAN frame to CANID 0x%x in extended_can_tx()\n", id);
		//csp_buffer_free(packet);
		return 1;
	}

	/* Send next frames if not complete */
	while (tx_count < packet->length) {
		/* Calculate frame data bytes */
		bytes = (packet->length - tx_count >= 8) ? 8 : packet->length - tx_count;

		id = id2 | CFP_MAKE_TYPE(CFP_MORE);

		/* Increment tx counter */
		tx_count += bytes;

		/* Send frame */
		if (can_send(id, packet->data + tx_count - bytes, bytes)) {
			csp_log_error("Failed to send more CAN frames in extended_can_tx()\n");
			//csp_buffer_free(packet);
			return 1;
		}
	}

	//csp_buffer_free(packet);

	return 0;
}


/* return 1 if OK
 * return 0 if failed.
 */
int
tianyi_fw_can_tx(uint8_t *b, int len, int dst_id, int delay, int has_cfpid)
{
	int tx_count, bytes;
	can_id_t id, id2;

	//if (b == NULL || len <= 8 || dst_id <= 0) return 0;
	if (b == NULL) return 0;
	/* Create CAN identifier */
	id = 0;
	id |= CFP_MAKE_SRC(kiss_get_clientaddr());
	id |= CFP_MAKE_DST(dst_id);
	id |= CFP_MAKE_REMAIN(0); /* not libcsp packet */
	if (has_cfpid)
		id |= CFP_MAKE_ID(extended_can_id_get());
	else
		id |= CFP_MAKE_ID(0);

	id2 = id;
	id |= CFP_MAKE_TYPE(CFP_BEGIN);
	tx_count = 8;
	/* Send first frame */
	if (can_send(id, b, 8)) {
		csp_log_error("FAILED TO SEND CAN DATA IN TIANYI_FW_UPDATE_CAN_TX()");
		return 0;
	}

	if (delay > 0)
		csp_sleep_ms(delay);
	/* Send next frames if not complete */
	while (tx_count < len) {
		/* Calculate frame data bytes */
		bytes = (len - tx_count >= 8) ? 8 : len - tx_count;

		id = id2 | CFP_MAKE_TYPE(CFP_MORE);

		/* Increment tx counter */
		tx_count += bytes;

		/* Send frame */
		if (can_send(id, b + tx_count - bytes, bytes)) {
			csp_log_error("FAILED TO SEND CAN DATA IN TIANYI_FW_UPDATE_CAN_TX()");
			return 0;
		}

		//if (delay > 0)
			//csp_sleep_ms(delay);
	}

	return 1;
}

int
csp_can_tx(csp_iface_t *interface, csp_packet_t *packet, uint32_t timeout)
{
	uint16_t tx_count;
	uint8_t bytes, overhead, avail, dest;
	uint8_t frame_buf[8];
	can_id_t id = 0, id2 = 0;

	/* Get CFP identification number */
	int ident = extended_can_id_get();
	if (ident < 0) {
		csp_log_error("Failed to get CFP identification number");
		return CSP_ERR_INVAL;
	}

	/* Calculate overhead */
	overhead = sizeof(csp_id_t) + sizeof(uint16_t);

	/* Insert destination node mac address into the CFP destination field */
	dest = csp_rtable_find_mac(packet->id.dst);
	if (dest == CSP_NODE_MAC)
		dest = packet->id.dst;

	/* Create CAN identifier */
	id |= CFP_MAKE_SRC(packet->id.src);
	id |= CFP_MAKE_DST(dest);
	id |= CFP_MAKE_ID(ident);
	id |= CFP_MAKE_REMAIN(1); /* libcsp packet */
	id |= (uint32_t)((uint32_t)1<<31);

	id2 = id;

	id |= CFP_MAKE_TYPE(CFP_BEGIN);
	/* Calculate first frame data bytes */
	avail = 8 - overhead;
	bytes = (packet->length <= avail) ? packet->length : avail;

	/* Copy CSP headers and data */
	uint32_t csp_id_be = csp_hton32(packet->id.ext);
	uint16_t csp_length_be = csp_hton16(packet->length);

	memcpy(frame_buf, &csp_id_be, sizeof(csp_id_be));
	memcpy(frame_buf + sizeof(csp_id_be), &csp_length_be, sizeof(csp_length_be));
	memcpy(frame_buf + overhead, packet->data, bytes);

	/* Increment tx counter */
	tx_count = bytes;

	/* Send first frame */
	if (can_send(id, frame_buf, overhead + bytes)) {
		csp_log_error("Failed to send CAN frame to CANID 0x%x in csp_tx_can()\n", id);
		return CSP_ERR_DRIVER;
	}

	/* Send next frames if not complete */
	while (tx_count < packet->length) {
		/* Calculate frame data bytes */
		bytes = (packet->length - tx_count >= 8) ? 8 : packet->length - tx_count;

		/* Prepare identifier */
		id = id2;
		id |= CFP_MAKE_TYPE(CFP_MORE);

		/* Increment tx counter */
		tx_count += bytes;

		/* Send frame */
		if (can_send(id, packet->data + tx_count - bytes, bytes)) {
			csp_log_error("Failed to send CAN frame in Tx callback\n");
			csp_if_can.tx_error++;
			return CSP_ERR_DRIVER;
		}
	}

	//csp_buffer_free(packet);
        //g_can_send_packet++;
	return CSP_ERR_NONE;
}

int csp_can_init(uint8_t mode, struct csp_can_config *conf)
{
    uint32_t mask;
	//init_can_process_maps();
  
	/* Initialize packet buffer */
	if (can_pbuf_init() != 0) {
		csp_log_error("Failed to initialize CAN packet buffers\n");
		return 1;
	}

	/* Initialize CFP identifier */
	if (extended_can_id_init() != 0) {
		csp_log_error("Failed to initialize CAN identification number\n");
		return 1;
	}

	if (mode == CSP_CAN_MASKED) {
		mask = CFP_MAKE_DST((1 << CFP_HOST_SIZE) - 1);
	} else if (mode == CSP_CAN_PROMISC) {
		mask = 0;
	} else {
		csp_log_error("Unknown CAN mode");
		return CSP_ERR_INVAL;
	}

	//own_csp_can_id = own_node;

	/* Initialize CAN driver */
	if (can_init(CFP_MAKE_DST(csp_get_address()), mask, conf) != 0) {
		csp_log_error("Failed to initialize CAN driver");
		return CSP_ERR_DRIVER;
	}

	/* Regsiter interface */
	csp_iflist_add(&csp_if_can);

	return CSP_ERR_NONE;
}

/* return sum of buffer */
uint8_t
sum_buffer(uint8_t *buf, int len)
{
	int i;
	uint8_t j = 0;

	if (buf == NULL || len <= 0) return 0;

	for (i = 0; i < len; i ++)
		j += buf[i];
	return j;
}

/* 0 成功 1失败
 * 
 */
int8_t  can_tx_raw_data(uint8_t dest, uint8_t *data, uint16_t len,cfp_ext_t type,uint8_t has_cfpid,uint32_t delay)
{
	uint16_t tx_count;
	uint8_t bytes, overhead=0, avail;
	uint8_t frame_buf[9];
	can_id_t id;
	uint8_t ident;
	if ((data == NULL) || (len <= 0))
        {
            return 1;
        }

	/* 组CAN ID */
	id = 0;
	id |= CFP_MAKE_SRC(kiss_get_clientaddr());  /*源地址*/
	id |= CFP_MAKE_DST(dest);  /*目的地址*/
        if(has_cfpid)
        {
	    /* 获取包ID */
            ident = extended_can_id_get(); /*8位ID*/
            if(type == CFP_SINGLE)
            {
                ident=0;
            }
	    id |= CFP_MAKE_ID(ident);  /*包序号*/
	    id |= CFP_MAKE_TYPE(type); /*类型*/
	    id |= CFP_MAKE_REMAIN(0);  /* 自定义包 */
	    id |= (uint32_t)((uint32_t)1<<31);
        }


	if ((type == CFP_RESET) || (type == CFP_TIMESYNC) || (type == CFP_SINGLE)) 
        {
            /*单帧*/
          if(len>8)
          {
              return 1;
          }
          else
          {
            
          }
	} 
        else
        {
            /*多帧*/
          if(len<=8)
          {
              return 1;
          }
          else
          {
            
          }
	} 

	/*填充首帧 */
	avail = 8 - overhead;
	bytes = (len <= avail) ? len : avail;
	memcpy(frame_buf + overhead, data,bytes);
	/*已经发送字节数 */
	tx_count = bytes;

	/* 发送第一帧 */
	frame_buf[overhead + bytes] = '\0';
    //id = id | CFP_MAKE_TYPE(CFP_BEGIN);
	if (can_send(id, frame_buf, overhead + bytes)) 
        {
		csp_log_error("Failed to send CAN frame to CANID 0x%x in extended_can_tx()\n", id);
		return 1;
	}

	/* 发送后续帧 */
	while (tx_count < len) 
        {
		/* Calculate frame data bytes */
		bytes = (len - tx_count >= 8) ? 8 : len - tx_count;  /*本帧长度*/
        id &= ~CFP_MAKE_TYPE((uint32_t)(1 << CFP_TYPE_SIZE) - 1);
		id = id | CFP_MAKE_TYPE(CFP_MORE);  /*后续帧*/
		tx_count += bytes;  /*发送计数递增*/

		/* 发送帧 */
		if (can_send(id, data + tx_count - bytes, bytes)) 
                {
			csp_log_error("Failed to send more CAN frames in extended_can_tx()\n");
			return 1;
		}
	}
        g_can_send_packet++;
	return 0;
}


/** Interface definition */
csp_iface_t csp_if_can = {
	.name = "CAN",
	.nexthop = csp_can_tx,
	.mtu = PBUF_MTU,
};
