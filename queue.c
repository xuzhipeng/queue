/*
 * Queue.cxx
 *
 *  Created on: Nov 8, 2016
 *      Author: xuzhipeng
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

int queue_init(queue_t *queue)
{
    queue->buffer = (uint8_t*) malloc(sizeof(uint8_t) * MAX_QUEUE_SIZE);
    if (queue->buffer == NULL)
    {
        printf("queue_init() failed\n");
        return -1;
    }

    queue->total_size = MAX_QUEUE_SIZE;
    queue->used_size = 0;
    queue->free_size = MAX_QUEUE_SIZE;
    queue->front = 0;
    queue->rear = 0;
#if defined __APPLE__
    pthread_mutex_init(&(queue->lock), 0);
#elif defined __linux__
    pthread_spin_init(&(queue->lock), 0);
#endif

    return 0;
}


int queue_destroy(queue_t *queue)
{
    free(queue->buffer);
    queue->total_size = 0;
    queue->used_size = 0;
    queue->free_size = 0;
    queue->front = 0;
    queue->rear = 0;
#if defined __APPLE__
    pthread_mutex_destroy(&(queue->lock));
#elif defined __linux__
    pthread_spin_destroy(&(queue->lock));
#endif

    return 0;
}


int queue_enqueue(queue_t *queue, uint8_t *buffer, uint32_t size)
{
    if (size > queue->total_size)
    {
        printf("queue_enqueue() failed, size > total_size\n");
        return -1;
    }

    if (size > queue->free_size)
    {
        printf("queue_enqueue() failed, size > free_size\n");
        return -1;
    }

    int behind = queue->total_size - queue->rear;
    int ramain = size - behind;
    if (ramain <= 0)
    {
        memcpy(queue->buffer + queue->rear, buffer, size);
        queue->rear += size;
    }
    else
    {
        memcpy(queue->buffer + queue->rear, buffer, behind);
        memcpy(queue->buffer, buffer + ramain, ramain);
        queue->rear = ramain;
    }
#if defined __APPLE__
    pthread_mutex_lock(&(queue->lock));
#elif defined __linux__
    pthread_spin_lock(&(queue->lock));
#endif
    queue->used_size += size;
    queue->free_size -= size;
    printf("after enqueue: front = %d, rear = %d, free_size = %d\n", queue->front, queue->rear, queue->free_size);
#if defined __APPLE__
    pthread_mutex_unlock(&(queue->lock));
#elif defined __linux__
    pthread_spin_unlock(&(queue->lock));
#endif
    return 0;
}


int queue_dequeue(queue_t *queue, uint8_t *buffer, uint32_t size)
{
    if (size > queue->total_size)
    {
        printf("queue_dequeue() failed, size > total_size\n");
        return -1;
    }

    int used = queue->total_size - queue->free_size;
    if (size > used)
    {
        printf("queue_dequeue() failed, size > used_size\n");
        return -1;
    }

    int before = queue->total_size - queue->front;
    int remain = size - before;
    if (remain <= 0)
    {
        memcpy(buffer, queue->buffer + queue->front, size);
        queue->front += size;
    }
    else
    {
        memcpy(buffer, queue->buffer + queue->front, before);
        memcpy(buffer + remain, queue->buffer, remain);
        queue->front = remain;
    }

#if defined __APPLE__
    pthread_mutex_lock(&(queue->lock));
#elif defined __linux__
    pthread_spin_lock(&(queue->lock));
#endif
    queue->used_size -= size;
    queue->free_size += size;
    printf("after dequeue: front = %d, rear = %d, free_size = %d\n", queue->front, queue->rear, queue->free_size);
#if defined __APPLE__
    pthread_mutex_unlock(&(queue->lock));
#elif defined __linux__
    pthread_spin_unlock(&(queue->lock));
#endif

    return 0;
}
