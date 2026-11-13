/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | test_worker.c */
#include <stdio.h>
#include "worker.h"

int main(void) {
    printf("=== CampusCompute Worker Tests ===\n");
    printf("SRN: PES1UG25CS193\n");

    printf("\nTEST 1: Normal execution\n");
    int result = run_worker(0);

    if (result != 0) {
        printf("TEST 1 FAILED\n");
        return 1;
    }
    printf("TEST 1 PASSED\n");

    printf("\nTEST 2: Simulated failure\n");
    result = run_worker(1);

    if (result != 1) {
        printf("TEST 2 FAILED\n");
        return 1;
    }
    printf("TEST 2 PASSED\n");

    printf("\nAll worker tests passed.\n");
    return 0;
}
