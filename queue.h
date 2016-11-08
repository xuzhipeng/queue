/*
 * Queue.hxx
 *
 *  Created on: Nov 8, 2016
 *      Author: xuzhipeng
 */

#ifndef QUEUE_HXX_
#define QUEUE_HXX_

#include <stdint.h>
#include <pthread.h>

#define MAX_QUEUE_SIZE (1024 * 1024)

typedef struct queue
{
	uint8_t *buffer;
	uint32_t total_size;
	uint32_t free_size;
	uint32_t front;
	uint32_t rear;
	pthread_spinlock_t lock;
} queue_t;

int queue_init(queue_t *queue);

int queue_destroy(queue_t *queue);

int queue_enqueue(queue_t *queue, uint8_t *buffer, uint32_t size);

int queue_dequeue(queue_t *queue, uint8_t *buffer, uint32_t size);


#endif /* QUEUE_HXX_ */
