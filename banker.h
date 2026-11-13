/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | banker.h */
#ifndef BANKER_H
#define BANKER_H

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

typedef struct {
    int processes;
    int resources;
    int available[MAX_RESOURCES];
    int allocation[MAX_PROCESSES][MAX_RESOURCES];
    int maximum[MAX_PROCESSES][MAX_RESOURCES];
} BankerSystem;

void calculate_need(
    const BankerSystem *system,
    int need[MAX_PROCESSES][MAX_RESOURCES]
);

int banker_safety_check(const BankerSystem *system);

#endif
