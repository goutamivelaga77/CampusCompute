/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | test_logger.c */
#include <stdio.h>
#include <pthread.h>
#include "logger.h"

#define PRODUCER_COUNT 4
#define MESSAGES_PER_PRODUCER 5

typedef struct {
    int producer_id;
} ProducerArgs;

void *producer_thread(void *arg) {
    ProducerArgs *args = (ProducerArgs *)arg;
    char message[160];

    for (int i = 1; i <= MESSAGES_PER_PRODUCER; i++) {
        snprintf(message, sizeof(message),
                 "Producer %d: event %d",
                 args->producer_id, i);

        if (logger_log(message) != 0) {
            fprintf(stderr, "Producer %d failed to log an event.\n",
                    args->producer_id);
            return (void *)1;
        }
    }

    return NULL;
}

int main(void) {
    pthread_t threads[PRODUCER_COUNT];
    ProducerArgs args[PRODUCER_COUNT];
    int created = 0;
    int failed = 0;

    printf("=== CampusCompute Concurrent Logger Test ===\n");
    printf("SRN: PES1UG25CS193\n");
    printf("Producers: %d | Events per producer: %d\n\n",
           PRODUCER_COUNT, MESSAGES_PER_PRODUCER);

    if (logger_start() != 0) {
        return 1;
    }

    for (int i = 0; i < PRODUCER_COUNT; i++) {
        args[i].producer_id = i + 1;

        int rc = pthread_create(&threads[i], NULL,
                                producer_thread, &args[i]);

        if (rc != 0) {
            fprintf(stderr, "Could not create producer %d.\n", i + 1);
            failed = 1;
            break;
        }

        created++;
    }

    for (int i = 0; i < created; i++) {
        void *thread_result = NULL;
        int rc = pthread_join(threads[i], &thread_result);

        if (rc != 0 || thread_result != NULL) {
            failed = 1;
        }
    }

    if (logger_stop() != 0) {
        return 1;
    }

    if (failed) {
        printf("CONCURRENT LOGGER TEST FAILED\n");
        return 1;
    }

    printf("\nAll producer threads completed successfully.\n");
    printf("Expected events: %d\n",
           PRODUCER_COUNT * MESSAGES_PER_PRODUCER);
    printf("CONCURRENT LOGGER TEST PASSED\n");

    return 0;
}
