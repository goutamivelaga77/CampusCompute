/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | scheduler.h */
#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_JOBS 20
#define MAX_SEGMENTS 200

typedef struct {
    int id;
    int arrival_time;
    int burst_time;
    int priority;
} Job;

typedef struct {
    int job_id;       /* -1 represents CPU idle time */
    int start_time;
    int end_time;
} GanttSegment;

typedef struct {
    int completion_time[MAX_JOBS];
    int turnaround_time[MAX_JOBS];
    int waiting_time[MAX_JOBS];
    int response_time[MAX_JOBS];

    GanttSegment timeline[MAX_SEGMENTS];
    int segment_count;

    double average_turnaround_time;
    double average_waiting_time;
    double average_response_time;
    int total_completion_time;
} ScheduleResult;

/* Validate the submitted workload. */
int scheduler_validate_jobs(const Job jobs[], int count);

/* Required scheduling algorithms. */
int scheduler_fcfs(const Job jobs[], int count,
                   ScheduleResult *result);

int scheduler_round_robin(const Job jobs[], int count,
                          int quantum, ScheduleResult *result);

int scheduler_priority(const Job jobs[], int count,
                       ScheduleResult *result);

/* Display actual calculated results. */
void scheduler_print_result(const Job jobs[], int count,
                            const ScheduleResult *result,
                            const char *algorithm_name);

#endif
