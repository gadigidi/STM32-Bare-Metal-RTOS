#include "buffer.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f446xx.h"

void buf_init (ring_buf_t *buf){
    buf->empty = 1;
    buf->full = 0;
    buf->head = 0;
    buf->tail = 0;
    buf->size = 0;
    return;
}

bool push_buf (ring_buf_t *buf, uint8_t data){
    if (!buf->full){
        buf->array[buf->head] = data;
        buf->head = (buf->head+1) & (BUF_SIZE-1); //Equivalent to "% BUF_SIZE". Saving cycles
        buf->size++;
        buf->empty = 0;
        if (buf->size == 4){
            buf->full = 1;
        }
        else{
            buf->full = 0;
        }
        return 1;
    }
    else{
        return 0; //ERROR
    }
}

uint8_t pop_buf (ring_buf_t *buf){
    if (!buf->empty){
        uint8_t data = buf->array[buf->tail];
        buf->tail = (buf->tail+1) & (BUF_SIZE-1);//Equivalent to "% BUF_SIZE". Saving cycles
        buf->size--;
        buf->full = 0;
        if (buf->size == 0){
            buf->empty = 1;
        }
        else{
            buf->empty = 0;
        }
        return data;
    }
    else{
        return BUF_EMPTY;
    }
}
