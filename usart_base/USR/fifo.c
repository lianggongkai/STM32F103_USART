#include "fifo.h"
#include "util.h"
#include "g_errno.h"
#include "string.h"

#if RFIFO_DEBUG
    #define rfifo_dbg(fmt, ...)
#else
    #define rfifo_dbg(fmt, ...)
#endif

uint32_t rfifo_init(rfifo_t *fifo)
{
    uint32_t retv = ERR_OK;

    if (!fifo) {
        retv = __LINE__;
        goto ret;
    }

    fifo->head = 0;
    fifo->tail = 0;
    fifo->start = 1;
    fifo->buf_len = RFIFO_LEN_MAX;
ret:
    rfifo_dbg("%s-ret:%u\n", __FUNCTION__, retv);
    return retv;
}

uint32_t rfifo_len(rfifo_t *fifo)
{
    uint16_t valid_len;

    if (!fifo) {
        rfifo_dbg("[rfifo]:Warning, para null pointer\n");
        return 0;
    }
    G_ASSERT(fifo->tail >= fifo->head);
    valid_len = fifo->tail - fifo->head;
    return valid_len;
}

uint32_t rfifo_push(rfifo_t *fifo, uint8_t data)
{
    uint32_t retv = ERR_OK;
    uint16_t tail = fifo->tail;
    if (!fifo) {
        retv = __LINE__;
        goto ret;
    }

    if (rfifo_len(fifo) >= fifo->buf_len) {
        retv = __LINE__;
        goto ret;
    }

    if (tail >= fifo->head + fifo->buf_len) {
        tail = fifo->head;
    }
    fifo->buf[tail++ % fifo->buf_len] = data;
    fifo->tail = tail;
ret:
    rfifo_dbg("%s-ret:%u, data:%u\n", __FUNCTION__, retv, data);
    return retv;
}

uint32_t rfifo_pop(rfifo_t *fifo, uint8_t *data)
{
    uint32_t retv = ERR_OK;
    uint16_t head = fifo->head;

    if (!fifo) {
        retv = __LINE__;
        goto ret;
    }

    if (rfifo_len(fifo) == 0 ) {
        rfifo_init(fifo);
        retv = __LINE__;
        goto ret;
    } else {
        fifo->start = 0;
    }

    *data = fifo->buf[head++ % fifo->buf_len];
    fifo->head = head;
ret:
    rfifo_dbg("%s-ret:%u, data:%u\n", __FUNCTION__, retv, *data);
    return retv;
}
