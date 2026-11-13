/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | test_advisor.c */
#include <stdio.h>
#include <string.h>
#include "scheduler.h"
#include "advisor.h"

int main(void) {
    Job jobs[] = {
        {1, 0, 5, 2},
        {2, 1, 3, 1},
        {3, 2, 1, 3}
    };

    int count = 3;

    ScheduleResult results[3];

    const char *names[] = {
        "FCFS",
        "ROUND ROBIN",
        "PRIORITY"
    };

    char category[80];
    char explanation[200];
    char recommendation[80];

    printf("====================================\n");
    printf(" CampusCompute Workload Intelligence\n");
    printf(" SRN: PES1UG25CS193\n");
    printf("====================================\n");

    printf("\nTEST 1: Workload Classification\n");

    if (!advisor_classify(jobs, count,
                          category, sizeof(category),
                          explanation, sizeof(explanation))) {
        printf("CLASSIFICATION FAILED\n");
        return 1;
    }

    printf("Workload type: %s\n", category);
    printf("Analysis: %s\n", explanation);
    printf("CLASSIFICATION TEST PASSED\n");

    printf("\nTEST 2: Algorithm Comparison\n");

    if (!scheduler_fcfs(jobs, count, &results[0]) ||
        !scheduler_round_robin(jobs, count, 2, &results[1]) ||
        !scheduler_priority(jobs, count, &results[2])) {
        printf("SCHEDULING COMPARISON FAILED\n");
        return 1;
    }

    for (int i = 0; i < 3; i++) {
        double score =
            0.60 * results[i].average_waiting_time +
            0.40 * results[i].average_response_time;

        printf("%s: Waiting = %.2f, Response = %.2f, Score = %.2f\n",
               names[i],
               results[i].average_waiting_time,
               results[i].average_response_time,
               score);
    }

    printf("\nTEST 3: Smart Recommendation\n");

    if (!advisor_recommend(results, names, 3,
                           recommendation,
                           sizeof(recommendation))) {
        printf("RECOMMENDATION FAILED\n");
        return 1;
    }

    printf("Recommended algorithm: %s\n", recommendation);
    printf("Reason: Lowest weighted waiting/response score.\n");

    printf("RECOMMENDATION TEST PASSED\n");

    printf("\nALL ADVISOR TESTS PASSED!\n");

    return 0;
}
