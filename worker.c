/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | worker.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int run_worker(int simulate_failure) {
    fflush(stdout); fflush(stderr); /* avoid duplicated buffered output in child */
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) {
        printf("[Worker] Child process started. PID=%d\n", getpid());
        fflush(stdout);

        if (simulate_failure) {
            fprintf(stderr, "[Worker] Simulating controlled failure.\n");
            _exit(2);
        }

        printf("[Worker] Job completed successfully.\n");
        fflush(stdout);
        _exit(0);
    }

    int status;
    pid_t result;

    do {
        result = waitpid(pid, &status, 0);
    } while (result < 0);

    if (result < 0) {
        perror("waitpid failed");
        return -1;
    }

    if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);

        printf("[Parent] Child PID=%d exited with code %d.\n",
               pid, code);

        if (code == 0) {
            printf("[Audit] Worker completed normally.\n");
            return 0;
        }

        printf("[Audit] Worker failure detected.\n");
        return 1;
    }

    if (WIFSIGNALED(status)) {
        printf("[Audit] Worker terminated by signal %d.\n",
               WTERMSIG(status));
        return 1;
    }

    return -1;
}
