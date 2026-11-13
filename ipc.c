/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | ipc.c */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>

#include "ipc.h"

#define SHM_NAME "/cc_shm307"
#define SEM_NAME "/cc_sem307"
#define MESSAGE_SIZE 256

typedef struct {
    char message[MESSAGE_SIZE];
    int ready;
} SharedData;

int ipc_demo(void) {
    int shm_fd = -1;
    SharedData *shared = MAP_FAILED;
    sem_t *semaphore = SEM_FAILED;
    pid_t pid = -1;
    int status = 0;
    int success = 0;

    /* Remove stale resources from an earlier interrupted run. */
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (shm_fd == -1) {
        perror("shm_open");
        goto cleanup;
    }

    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        perror("ftruncate");
        goto cleanup;
    }

    shared = mmap(NULL, sizeof(SharedData),
                  PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shared == MAP_FAILED) {
        perror("mmap");
        goto cleanup;
    }

    memset(shared, 0, sizeof(*shared));

    semaphore = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0600, 0);
    if (semaphore == SEM_FAILED) {
        perror("sem_open");
        goto cleanup;
    }

    printf("IPC: Shared memory and semaphore created.\n");
    fflush(stdout);

    fflush(stdout); fflush(stderr); /* avoid duplicated buffered output in child */
    pid = fork();

    if (pid == -1) {
        perror("fork");
        goto cleanup;
    }

    if (pid == 0) {
        const char *message =
            "Worker report: CampusCompute job completed.";

        snprintf(shared->message, sizeof(shared->message), "%s", message);
        shared->ready = 1;

        printf("[Child] Message written to shared memory.\n");
        fflush(stdout);

        if (sem_post(semaphore) == -1) {
            perror("[Child] sem_post");
            _exit(1);
        }

        _exit(0);
    }

    /* Wait until the child signals that the message is ready. */
    while (sem_wait(semaphore) == -1) {
        if (errno != EINTR) {
            perror("sem_wait");
            goto cleanup;
        }
    }

    if (shared->ready == 1) {
        printf("[Parent] Received through shared memory:\n%s\n",
               shared->message);
    } else {
        fprintf(stderr, "[Parent] Shared message was not marked ready.\n");
        goto cleanup;
    }

    while (waitpid(pid, &status, 0) == -1) {
        if (errno != EINTR) {
            perror("waitpid");
            goto cleanup;
        }
    }
    pid = -1;

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "IPC child process failed.\n");
        goto cleanup;
    }

    success = 1;

cleanup:
    /* Reap the child if an earlier operation failed. */
    if (pid > 0) {
        while (waitpid(pid, &status, 0) == -1 && errno == EINTR) {
        }
    }

    if (semaphore != SEM_FAILED) {
        sem_close(semaphore);
    }

    if (shared != MAP_FAILED) {
        munmap(shared, sizeof(SharedData));
    }

    if (shm_fd != -1) {
        close(shm_fd);
    }

    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);

    printf("IPC: Shared memory and semaphore cleanup completed.\n");
    return success ? 0 : -1;
}
