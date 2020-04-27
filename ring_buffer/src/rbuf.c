
#include <string.h>
#include <assert.h>

#include "rbuf.h"

struct ring_buf {
	void *buffer;
	size_t head; // Next free slot
	size_t tail; // Last stored value
	size_t size;
	size_t item_size;
	unsigned char full;
};

#pragma mark - private

static void inc_buf(rbuf_handle_t rbuf)
{
	assert(rbuf);

	if (rbuf->full) {
		rbuf->tail += 1;
		rbuf->tail %= rbuf->size;

		rbuf->head = rbuf->tail;
	} else {
		rbuf->head += 1;
		rbuf->head %= rbuf->size;
	}

	rbuf->full = (rbuf->head == rbuf->tail);
}

static void dec_buf(rbuf_handle_t rbuf)
{
	assert(rbuf);

	rbuf->full = 0;
	rbuf->tail += 1;
	rbuf->tail %= rbuf->size;
}

#pragma mark - public

//Init
rbuf_handle_t rbuf_init(size_t item_size, size_t item_count)
{
	rbuf_handle_t ret = (rbuf_handle_t)calloc(1, sizeof(ring_buf_t));

	assert(ret);

	ret->buffer = calloc(item_count, item_size);

	assert(ret->buffer);

	ret->size = item_count;
	ret->item_size = item_size;

	assert(rbuf_empty(ret));

	return ret;
}
/** void rbuf_init_static(ring_buf_t *rbuf, void *buffer, size_t size); */

//Destroy
void rbuf_free(rbuf_handle_t rbuf)
{
	assert(rbuf);

	free(rbuf->buffer);
	free(rbuf);
}

//Reset
void rbuf_reset(rbuf_handle_t rbuf)
{
	assert(rbuf);

	rbuf->head = 0;
	rbuf->tail = 0;
	rbuf->full = 0;
}

//Add data
int rbuf_put(rbuf_handle_t rbuf, void *data)
{
	assert(rbuf && rbuf->buffer);

	if (rbuf->full)
		return -1;

	memcpy(rbuf->buffer + rbuf->head * rbuf->item_size, data,
	       rbuf->item_size);
	inc_buf(rbuf);

	return 0;
}

//Add and overwrite
void rbuf_fput(rbuf_handle_t rbuf, void *data)
{
	assert(rbuf && rbuf->buffer);

	memcpy(rbuf->buffer + rbuf->head * rbuf->item_size, data,
	       rbuf->item_size);
	inc_buf(rbuf);
}

//Get data
int rbuf_get(rbuf_handle_t rbuf, void *data)
{
	assert(rbuf && rbuf->buffer);

	if (rbuf_empty(rbuf))
		return -1;

	memcpy(data, rbuf->buffer + rbuf->tail * rbuf->item_size,
	       rbuf->item_size);
	dec_buf(rbuf);

	return 0;
}

//Check empty or full
unsigned char rbuf_empty(rbuf_handle_t rbuf)
{
	assert(rbuf);

	return !rbuf->full && rbuf->head == rbuf->tail;
}

unsigned char rbug_full(rbuf_handle_t rbuf)
{
	assert(rbuf);

	return rbuf->full;
}

//Num of elements
size_t rbuf_size(rbuf_handle_t rbuf)
{
	assert(rbuf);

	ssize_t ret = rbuf->size;

	if (!rbuf->full) {
		ret = rbuf->head - rbuf->tail;
		if (rbuf->tail > rbuf->head)
			ret += rbuf->size;
	}

	return (size_t)ret;
}

//Get max capacity
size_t rbuf_capacity(rbuf_handle_t rbuf)
{
	assert(rbuf);

	return rbuf->size;
}
