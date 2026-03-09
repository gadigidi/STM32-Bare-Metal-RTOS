#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BUF_SIZE            4
#define BUF_EMPTY           0xFF

typedef struct {
    uint8_t array[BUF_SIZE];
    uint8_t size;
    bool empty;
    bool full;
    volatile uint8_t head;
    volatile uint8_t tail;
} ring_buf_t;

void buf_init (ring_buf_t *buf);
bool push_buf (ring_buf_t *buf, uint8_t data);
uint8_t pop_buf (ring_buf_t *buf);

#endif /* BUFFER_H_ */
