/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | limits.c */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>
#include "limits.h"

int run_resource_limit_demo(void) {
    fflush(stdout); fflush(stderr); /* avoid duplicated buffered output in child */
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        struct rlimit limit = {1, 1};

        if (setrlimit(RLIMIT_CPU, &limit) == -1) {
            perror("setrlimit");
            _exit(1);
        }

        printf("[Child] CPU time limit set to 1 second.\n");
        fflush(stdout);

        volatile unsigned long long count = 0;
        for (;;) {
            count++;
        }
    }

    int status;
    pid_t result;

    do {
        result = waitpid(pid, &status, 0);
    } while (result == -1 && errno == EINTR);

    if (result == -1) {
        perror("waitpid");
        return -1;
    }

    if (WIFSIGNALED(status)) {
        printf("[Parent] Child terminated by signal %d.\n",
               WTERMSIG(status));
        printf("[Audit] Resource-limit demonstration completed.\n");
        return 0;
    }

    if (WIFEXITED(status)) {
        printf("[Parent] Child exited with code %d.\n",
               WEXITSTATUS(status));
        printf("[Audit] Child exited before the CPU limit signal.\n");
        return 0;
    }

    return -1;
}
