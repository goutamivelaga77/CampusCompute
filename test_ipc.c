/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | test_ipc.c */
#include <stdio.h>
#include "ipc.h"

int main(void) {
    printf("=== CampusCompute IPC Test ===\n");
    printf("SRN: PES1UG25CS193\n\n");

    if (ipc_demo() != 0) {
        printf("IPC TEST FAILED\n");
        return 1;
    }

    printf("\nIPC TEST PASSED\n");
    return 0;
}
