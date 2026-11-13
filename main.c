/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | main.c */
#include <stdio.h>
#include <string.h>

#include "scheduler.h"
#include "advisor.h"
#include "banker.h"
#include "worker.h"
#include "logger.h"
#include "ipc.h"
#include "limits.h"

void scheduling_demo(void) {
    Job jobs[] = {
        {1, 0, 5, 2},
        {2, 1, 3, 1},
        {3, 2, 1, 3}
    };

    int count = 3;
    ScheduleResult results[3];
    const char *names[] = {"FCFS", "ROUND ROBIN", "PRIORITY"};

    printf("\n===== CPU SCHEDULING =====\n");

    if (scheduler_fcfs(jobs, count, &results[0]))
        scheduler_print_result(jobs, count, &results[0], names[0]);

    if (scheduler_round_robin(jobs, count, 2, &results[1]))
        scheduler_print_result(jobs, count, &results[1], names[1]);

    if (scheduler_priority(jobs, count, &results[2]))
        scheduler_print_result(jobs, count, &results[2], names[2]);

    printf("\n===== WORKLOAD INTELLIGENCE =====\n");

    char category[100];
    char explanation[200];
    char recommendation[100];

    if (advisor_classify(jobs, count, category, sizeof(category),
                         explanation, sizeof(explanation))) {
        printf("Workload: %s\n", category);
        printf("Analysis: %s\n", explanation);
    }

    if (advisor_recommend(results, names, 3,
                          recommendation, sizeof(recommendation))) {
        printf("Recommended algorithm: %s\n", recommendation);
    }
}

void banker_demo(void) {
    printf("\n===== BANKER'S ALGORITHM =====\n");

    BankerSystem system = {0};

    system.processes = 5;
    system.resources = 3;

    int available[] = {3, 3, 2};

    int allocation[5][3] = {
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };

    int maximum[5][3] = {
        {7, 5, 3},
        {3, 2, 2},
        {9, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };

    for (int j = 0; j < 3; j++)
        system.available[j] = available[j];

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            system.allocation[i][j] = allocation[i][j];
            system.maximum[i][j] = maximum[i][j];
        }
    }

    if (banker_safety_check(&system))
        printf("System is in a SAFE state.\n");
    else
        printf("System is in an UNSAFE state.\n");
}

void worker_demo(void) {
    printf("\n===== WORKER MANAGEMENT =====\n");
    run_worker(0);
}

void logger_demo(void) {
    printf("\n===== CONCURRENT LOGGER =====\n");

    if (logger_start() == 0) {
        logger_log("CampusCompute integrated application started.");
        logger_log("Scheduling and resource management modules active.");
        logger_stop();
        printf("Logging completed successfully.\n");
    } else {
        printf("Logger could not start.\n");
    }
}

void ipc_module(void) {
    printf("\n===== INTER PROCESS COMMUNICATION =====\n");
    ipc_demo();
}

void limits_demo(void) {
    printf("\n===== RESOURCE LIMITS =====\n");
    run_resource_limit_demo();
}


void run_all(void) {
    scheduling_demo();
    banker_demo();
    worker_demo();
    ipc_module();
    logger_demo();
    limits_demo();
}

int main(void) {
    int choice;

    printf("\n========================================\n");
    printf("       CAMPUSCOMPUTE SYSTEM\n");
    printf("       SRN: PES1UG25CS193\n");
    printf("========================================\n");

    while (1) {
        printf("\n========== MAIN MENU ==========\n");
        printf("1. CPU Scheduling\n");
        printf("2. Workload Intelligence\n");
        printf("3. Banker's Algorithm\n");
        printf("4. Worker Management\n");
        printf("5. Inter-Process Communication\n");
        printf("6. Concurrent Logger\n");
        printf("7. Resource Limits\n");
        printf("8. Run All Modules\n");
        printf("0. Exit\n");

        printf("\nEnter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            continue;
        }

        switch (choice) {
            case 1:
            case 2:
                scheduling_demo();
                break;

            case 3:
                banker_demo();
                break;

            case 4:
                worker_demo();
                break;

            case 5:
                ipc_module();
                break;

            case 6:
                logger_demo();
                break;

            case 7:
                limits_demo();
                break;

            case 8:
                run_all();
                break;

            case 0:
                printf("\nThank you for using CampusCompute!\n");
                return 0;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}
