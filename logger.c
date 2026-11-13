/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | logger.c */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "logger.h"

#define QUEUE_SIZE 8
#define MESSAGE_SIZE 160

typedef struct {
    char messages[QUEUE_SIZE][MESSAGE_SIZE];
    int front;
    int rear;
    int count;
    int stopping;
    int started;
    FILE *file;
    pthread_t consumer;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} Logger;

static Logger logger = {
    .front = 0,
    .rear = 0,
    .count = 0,
    .stopping = 0,
    .started = 0,
    .file = NULL,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .not_empty = PTHREAD_COND_INITIALIZER,
    .not_full = PTHREAD_COND_INITIALIZER
};

static void *consumer_thread(void *arg) {
    (void)arg;

    for (;;) {
        char message[MESSAGE_SIZE];

        pthread_mutex_lock(&logger.mutex);

        while (logger.count == 0 && !logger.stopping) {
            pthread_cond_wait(&logger.not_empty, &logger.mutex);
        }

        if (logger.count == 0 && logger.stopping) {
            pthread_mutex_unlock(&logger.mutex);
            break;
        }

        snprintf(message, sizeof(message), "%s",
                 logger.messages[logger.front]);

        logger.front = (logger.front + 1) % QUEUE_SIZE;
        logger.count--;

        pthread_cond_signal(&logger.not_full);
        pthread_mutex_unlock(&logger.mutex);

        time_t now = time(NULL);
        struct tm time_info;
        char timestamp[32];

        if (localtime_r(&now, &time_info) != NULL &&
            strftime(timestamp, sizeof(timestamp),
                     "%Y-%m-%d %H:%M:%S", &time_info) > 0) {
            fprintf(logger.file, "[%s] %s\n", timestamp, message);
            fflush(logger.file);
            printf("[LOGGER] [%s] %s\n", timestamp, message);
        }
    }

    return NULL;
}

int logger_start(void) {
    pthread_mutex_lock(&logger.mutex);

    if (logger.started) {
        pthread_mutex_unlock(&logger.mutex);
        fprintf(stderr, "Logger is already running.\n");
        return -1;
    }

    logger.file = fopen("campuscompute.log", "a");
    if (logger.file == NULL) {
        pthread_mutex_unlock(&logger.mutex);
        perror("Could not open campuscompute.log");
        return -1;
    }

    logger.front = 0;
    logger.rear = 0;
    logger.count = 0;
    logger.stopping = 0;

    int rc = pthread_create(&logger.consumer, NULL,
                            consumer_thread, NULL);

    if (rc != 0) {
        fclose(logger.file);
        logger.file = NULL;
        pthread_mutex_unlock(&logger.mutex);
        fprintf(stderr, "Could not create logger consumer thread.\n");
        return -1;
    }

    logger.started = 1;
    pthread_mutex_unlock(&logger.mutex);

    printf("Concurrent logger started.\n");
    return 0;
}

int logger_log(const char *message) {
    if (message == NULL) {
        return -1;
    }

    pthread_mutex_lock(&logger.mutex);

    if (!logger.started || logger.stopping) {
        pthread_mutex_unlock(&logger.mutex);
        fprintf(stderr, "Cannot log: logger is not running.\n");
        return -1;
    }

    while (logger.count == QUEUE_SIZE && !logger.stopping) {
        pthread_cond_wait(&logger.not_full, &logger.mutex);
    }

    if (logger.stopping) {
        pthread_mutex_unlock(&logger.mutex);
        return -1;
    }

    snprintf(logger.messages[logger.rear], MESSAGE_SIZE, "%s", message);
    logger.rear = (logger.rear + 1) % QUEUE_SIZE;
    logger.count++;

    pthread_cond_signal(&logger.not_empty);
    pthread_mutex_unlock(&logger.mutex);

    return 0;
}

int logger_stop(void) {
    pthread_mutex_lock(&logger.mutex);

    if (!logger.started) {
        pthread_mutex_unlock(&logger.mutex);
        fprintf(stderr, "Logger is not running.\n");
        return -1;
    }

    logger.stopping = 1;
    pthread_cond_broadcast(&logger.not_empty);
    pthread_cond_broadcast(&logger.not_full);

    pthread_mutex_unlock(&logger.mutex);

    int rc = pthread_join(logger.consumer, NULL);
    if (rc != 0) {
        fprintf(stderr, "Could not join logger thread.\n");
        return -1;
    }

    pthread_mutex_lock(&logger.mutex);

    fclose(logger.file);
    logger.file = NULL;
    logger.started = 0;

    pthread_mutex_unlock(&logger.mutex);

    printf("Concurrent logger stopped; queued events flushed.\n");
    return 0;
}
