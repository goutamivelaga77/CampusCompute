/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | test_limits.c */
#include <stdio.h>
#include "limits.h"

int main(void) {
    printf("=== CampusCompute Resource Limits Test ===\n");
    printf("SRN: PES1UG25CS193\n\n");

    if (run_resource_limit_demo() != 0) {
        printf("RESOURCE LIMIT TEST FAILED\n");
        return 1;
    }

    printf("RESOURCE LIMIT TEST COMPLETED\n");
    return 0;
}
