/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | banker.c */
#include <stdio.h>
#include "banker.h"

void calculate_need(
    const BankerSystem *system,
    int need[MAX_PROCESSES][MAX_RESOURCES]
) {
    for (int i = 0; i < system->processes; i++) {
        for (int j = 0; j < system->resources; j++) {
            need[i][j] =
                system->maximum[i][j] -
                system->allocation[i][j];
        }
    }
}

int banker_safety_check(const BankerSystem *system) {
    int need[MAX_PROCESSES][MAX_RESOURCES] = {0};
    int work[MAX_RESOURCES];
    int finish[MAX_PROCESSES] = {0};
    int safe_sequence[MAX_PROCESSES];
    int count = 0;

    if (system == NULL ||
        system->processes < 1 ||
        system->processes > MAX_PROCESSES ||
        system->resources < 1 ||
        system->resources > MAX_RESOURCES) {
        fprintf(stderr, "Error: Invalid system dimensions.\n");
        return -1;
    }

    for (int j = 0; j < system->resources; j++) {
        if (system->available[j] < 0) {
            fprintf(stderr, "Error: Negative available resources.\n");
            return -1;
        }
    }

    for (int i = 0; i < system->processes; i++) {
        for (int j = 0; j < system->resources; j++) {
            if (system->allocation[i][j] < 0 ||
                system->maximum[i][j] < system->allocation[i][j]) {
                fprintf(stderr,
                        "Error: Invalid allocation or maximum claim.\n");
                return -1;
            }
        }
    }

    calculate_need(system, need);

    printf("\n=== RESOURCE RISK RADAR ===\n");
    printf("Processes: %d | Resource types: %d\n",
           system->processes, system->resources);

    printf("\nNeed Matrix (Maximum - Allocation):\n");
    for (int i = 0; i < system->processes; i++) {
        printf("P%d: ", i);
        for (int j = 0; j < system->resources; j++) {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }

    for (int j = 0; j < system->resources; j++) {
        work[j] = system->available[j];
    }

    printf("\nAvailable resources: ");
    for (int j = 0; j < system->resources; j++) {
        printf("%d ", work[j]);
    }
    printf("\n\nSafety analysis:\n");

    while (count < system->processes) {
        int progress = 0;

        for (int i = 0; i < system->processes; i++) {
            if (finish[i]) {
                continue;
            }

            int can_finish = 1;

            for (int j = 0; j < system->resources; j++) {
                if (need[i][j] > work[j]) {
                    can_finish = 0;
                    break;
                }
            }

            if (can_finish) {
                printf("P%d can finish; releasing its allocated resources.\n",
                       i);

                for (int j = 0; j < system->resources; j++) {
                    work[j] += system->allocation[i][j];
                }

                finish[i] = 1;
                safe_sequence[count++] = i;
                progress = 1;

                printf("  Resources after P%d finishes: ", i);
                for (int j = 0; j < system->resources; j++) {
                    printf("%d ", work[j]);
                }
                printf("\n");
            } else {
                printf("P%d must wait: its remaining need exceeds "
                       "currently available resources.\n", i);
            }
        }

        if (!progress) {
            break;
        }
    }

    if (count == system->processes) {
        printf("\nRESULT: SAFE STATE\nSafe sequence: ");
        for (int i = 0; i < count; i++) {
            printf("P%d", safe_sequence[i]);
            if (i < count - 1) {
                printf(" -> ");
            }
        }
        printf("\n");
        return 1;
    }

    printf("\nRESULT: UNSAFE STATE\n");
    printf("No safe sequence exists for the supplied state.\n");
    printf("Processes unable to finish: ");

    for (int i = 0; i < system->processes; i++) {
        if (!finish[i]) {
            printf("P%d ", i);
        }
    }

    printf("\n");
    return 0;
}
