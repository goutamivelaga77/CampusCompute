/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | test_banker.c */
#include <stdio.h>
#include "banker.h"

int main(void) {
    BankerSystem s = {
        .processes = 5,
        .resources = 3,
        .available = {3, 3, 2},
        .allocation = {
            {0, 1, 0},
            {2, 0, 0},
            {3, 0, 2},
            {2, 1, 1},
            {0, 0, 2}
        },
        .maximum = {
            {7, 5, 3},
            {3, 2, 2},
            {9, 0, 2},
            {2, 2, 2},
            {4, 3, 3}
        }
    };

    printf("====================================\n");
    printf(" CampusCompute - Banker Test Suite\n");
    printf(" SRN: PES1UG25CS193\n");
    printf("====================================\n");

    printf("\nTEST 1: Known safe resource state\n");
    int result = banker_safety_check(&s);
    if (result != 1) {
        printf("TEST 1 FAILED\n");
        return 1;
    }
    printf("TEST 1 PASSED\n");

    printf("\nTEST 2: Unsafe resource state\n");
    s.available[0] = 0;
    s.available[1] = 0;
    s.available[2] = 0;

    result = banker_safety_check(&s);
    if (result != 0) {
        printf("TEST 2 FAILED\n");
        return 1;
    }
    printf("TEST 2 PASSED\n");

    printf("\nAll Banker tests passed.\n");
    return 0;
}
