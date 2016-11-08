#include <stdio.h>
#include <unistd.h>
#include "queue.h"

void* enqueue_thread_fun(void* args)
{
    queue_t* queue = (queue_t*) args;
    while (1)
    // int i = 0;
    // for (i = 0; i < 20; i++)
    {
        uint8_t buffer[1024] = "aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffffgggggggggghhhhhhhhhhiiiiiiiiiijjjjjjjjjj";
        queue_enqueue(queue, buffer, 100);
        usleep(100);
    }
}

void* dequeue_thread_fun(void* args)
{
    queue_t* queue = (queue_t*) args;
    while (1)
    {
        uint8_t buffer[1024];
        int ret = queue_dequeue(queue, buffer, 100);
        if (ret == 0)
        {
            printf("%s\n", buffer);
        }
        usleep(100);
    }
}

int main(int argc, char* argv[])
{
    queue_t queue;
    queue_init(&queue);

    pthread_t enqueue;
    pthread_t dequeue;

    pthread_create(&enqueue, NULL, &enqueue_thread_fun, &queue);
    pthread_create(&dequeue, NULL, &dequeue_thread_fun, &queue);

    pthread_join(enqueue, NULL);
    pthread_join(dequeue, NULL);

    queue_destroy(&queue);
    return 0;
}
