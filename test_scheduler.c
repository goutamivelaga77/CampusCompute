/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | test_scheduler.c */
#include <stdio.h>
#include "scheduler.h"

int main(void) {
    Job jobs[] = {
        {1, 0, 5, 2},
        {2, 1, 3, 1},
        {3, 2, 1, 3}
    };

    int count = 3;
    ScheduleResult result;

    printf("====================================\n");
    printf(" CampusCompute Scheduling Test\n");
    printf(" SRN: PES1UG25CS193\n");
    printf("====================================\n");

    printf("\nInput workload:\n");
    printf("ID\tArrival\tBurst\tPriority\n");

    for (int i = 0; i < count; i++) {
        printf("P%d\t%d\t%d\t%d\n",
               jobs[i].id,
               jobs[i].arrival_time,
               jobs[i].burst_time,
               jobs[i].priority);
    }

    printf("\nTEST 1: FCFS Scheduling\n");

    if (!scheduler_fcfs(jobs, count, &result)) {
        printf("FCFS FAILED\n");
        return 1;
    }

    scheduler_print_result(jobs, count, &result, "FCFS");

    if (result.completion_time[0] != 5 ||
        result.completion_time[1] != 8 ||
        result.completion_time[2] != 9) {
        printf("FCFS RESULT INCORRECT\n");
        return 1;
    }

    printf("FCFS TEST PASSED\n");

    printf("\nTEST 2: Round Robin (Quantum = 2)\n");

    if (!scheduler_round_robin(jobs, count, 2, &result)) {
        printf("ROUND ROBIN FAILED\n");
        return 1;
    }

    scheduler_print_result(jobs, count, &result, "ROUND ROBIN");

    if (result.completion_time[0] != 9 ||
        result.completion_time[1] != 8 ||
        result.completion_time[2] != 5) {
        printf("ROUND ROBIN RESULT INCORRECT\n");
        return 1;
    }

    printf("ROUND ROBIN TEST PASSED\n");

    printf("\nTEST 3: Non-preemptive Priority\n");

    if (!scheduler_priority(jobs, count, &result)) {
        printf("PRIORITY FAILED\n");
        return 1;
    }

    scheduler_print_result(jobs, count, &result, "PRIORITY");

    if (result.completion_time[0] != 5 ||
        result.completion_time[1] != 8 ||
        result.completion_time[2] != 9) {
        printf("PRIORITY RESULT INCORRECT\n");
        return 1;
    }

    printf("PRIORITY TEST PASSED\n");

    printf("\nTEST 4: Invalid time quantum\n");

    if (scheduler_round_robin(jobs, count, 0, &result)) {
        printf("INVALID QUANTUM TEST FAILED\n");
        return 1;
    }

    printf("INVALID QUANTUM TEST PASSED\n");

    printf("\nALL SCHEDULING TESTS PASSED!\n");
    return 0;
}
