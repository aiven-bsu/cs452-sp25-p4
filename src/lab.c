#include <pthread.h>
#include "lab.h"

struct queue {
    void **data;                // Array of pointers to data
    int capacity;               // Maximum capacity of the queue
    int size;                   // Current size of the queue
    int front;                  // Index of the front element
    int rear;                   // Index of the rear element
    pthread_mutex_t lock;       // Mutex for thread safety
    pthread_cond_t not_empty;   // Condition variable for not empty
    pthread_cond_t not_full;    // Condition variable for not full
    bool shutdown;              // Flag to indicate shutdown state
};

queue_t queue_init(int capacity) {
    // First check if the capacity is valid
    if (capacity <= 0) {
        return NULL; // Invalid capacity
    }

    // Allocate memory for the queue structure
    queue_t q = (queue_t)malloc(sizeof(struct queue));

    // Check if memory allocation was successful
    if (!q) {
        return NULL; // Memory allocation failed
    }
    q->data = (void **)malloc(sizeof(void *) * capacity);
    if (!q->data) {
        free(q);
        return NULL; // Memory allocation failed
    }

    // Initialize queue properties
    q->capacity = capacity;
    q->size = 0;
    q->front = 0;
    q->rear = -1;
    q->shutdown = false;

    // Initialize mutex and condition variables
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);

    return q;
}

void queue_destroy(queue_t q) {
    if (q) {
        // Set shutdown flag and acquire the lock
        pthread_mutex_lock(&q->lock);
        q->shutdown = true;
        
        // Signal(wake up) all threads waiting on condition variables
        pthread_cond_broadcast(&q->not_empty);
        pthread_cond_broadcast(&q->not_full);
        
        // Release the lock
        pthread_mutex_unlock(&q->lock);
        
        // Destroy mutex and condition variables
        pthread_mutex_destroy(&q->lock);
        pthread_cond_destroy(&q->not_empty);
        pthread_cond_destroy(&q->not_full);
        
        // Free the data array
        free(q->data);
        
        // Free the queue structure itself
        free(q);
    }
}

void enqueue(queue_t q, void *data) {
    // Acquire the lock
    pthread_mutex_lock(&q->lock);

    // Wait until the queue is not full
    while (q->size == q->capacity && !q->shutdown) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }

    // Check if shutdown is requested
    if (q->shutdown) {
        pthread_mutex_unlock(&q->lock);
        return; // Exit if shutdown is requested
    }

    // Add the data to the queue
    q->rear = (q->rear + 1) % q->capacity;
    q->data[q->rear] = data;
    q->size++;

    // Signal that the queue is not empty
    pthread_cond_signal(&q->not_empty);

    // Release the lock
    pthread_mutex_unlock(&q->lock);
}

void *dequeue(queue_t q) {
    // Acquire the lock
    pthread_mutex_lock(&q->lock);

    // Wait until the queue is not empty
    while (q->size == 0 && !q->shutdown) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }

    // If queue is empty and in shutdown state, return NULL
    if (q->size == 0 && q->shutdown) {
        pthread_mutex_unlock(&q->lock);
        return NULL;
    }

    // Remove the data from the queue
    void *data = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;

    // Signal that the queue is not full
    pthread_cond_signal(&q->not_full);

    // Release the lock
    pthread_mutex_unlock(&q->lock);

    return data;
}

void queue_shutdown(queue_t q) {
    if (q) {
        // Acquire the lock
        pthread_mutex_lock(&q->lock);
        q->shutdown = true;

        // Signal all waiting threads
        pthread_cond_broadcast(&q->not_empty);
        pthread_cond_broadcast(&q->not_full);

        // Release the lock
        pthread_mutex_unlock(&q->lock);
    }
}

bool is_empty(queue_t q) {
    if (q) {
        pthread_mutex_lock(&q->lock);
        bool empty = (q->size == 0);
        pthread_mutex_unlock(&q->lock);
        return empty;
    }
    return true; // If queue is NULL, consider it empty
}

bool is_shutdown(queue_t q) {
    if (q) {
        pthread_mutex_lock(&q->lock);
        bool shutdown = q->shutdown;
        pthread_mutex_unlock(&q->lock);
        return shutdown;
    }
    return false; // If queue is NULL, consider it not shutdown
}