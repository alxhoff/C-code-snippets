#ifndef __RBUF_H__
#define __RBUF_H__

#include <stdio.h>

typedef struct ring_buf ring_buf_t;

typedef ring_buf_t *rbuf_handle_t;

//Init
rbuf_handle_t rbuf_init(size_t item_size, size_t item_count);
void rbuf_init_static(ring_buf_t *rbuf, void *buffer, size_t size);

//Destroy
void rbuf_free(rbuf_handle_t rbuf);

//Reset
void rbuf_reset(rbuf_handle_t rbuf);

//Add data
int rbuf_put(rbuf_handle_t rbuf, void *data);

//Add and overwrite
void rbuf_fput(rbuf_handle_t rbuf, void *data);

//Get data
int rbuf_get(rbuf_handle_t rbuf, void *data);

//Check empty or full
unsigned char rbuf_empty(rbuf_handle_t rbuf);
unsigned char rbug_full(rbuf_handle_t rbuf);

//Num of elements
size_t rbuf_size(rbuf_handle_t rbuf);

//Get max capacity
size_t rbuf_capacity(rbuf_handle_t rbuf);

#endif //__RBUF_H__
