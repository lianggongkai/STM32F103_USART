#ifndef _RFIFO_H_
#define _RFIFO_H_
#include "stdint.h"
#include "stm32f10x.h"


#define RFIFO_ENABLE            1
#define RFIFO_DEBUG             1
#define RFIFO_LEN_MAX           256

typedef struct _rfifo_t {
    uint16_t head;
    uint16_t tail;
    uint16_t buf_len;
    uint8_t  buf[RFIFO_LEN_MAX];
	uint8_t start;
	uint16_t valid_len;
	uint16_t max;
	uint16_t max_head;
	uint16_t max_tail;
} rfifo_t;

uint32_t rfifo_init(rfifo_t *fifo);

/*
 * @brief           : ring fifo push data ch to fifo
 * @param fifo      : fifo, where to push data
 * @param ch        : the data to push in fifo
 * @retval          : push in fifo ok return ERR_OK, else ERR_FAIL
 */
uint32_t rfifo_push(rfifo_t *fifo, uint8_t data);

/*
 * @brief           : ring fifo push data ch to fifo
 * @param fifo      : fifo, from where to pop data
 * @param ch        : where to save poped data
 * @retval          : fifo pop ok return ERR_OK, else ERR_FAIL
 */
uint32_t rfifo_pop(rfifo_t *fifo, uint8_t *data);

/*
 * @breif           : get fifo valid data length
 * @param fifo      : get which fifo's valid data length
 * @retval          : return valid data length
 */
uint32_t rfifo_len(rfifo_t *fifo);

#endif

