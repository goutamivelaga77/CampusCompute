/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | scheduler.c */
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "scheduler.h"

static void initialize_result(ScheduleResult *r) {
    memset(r, 0, sizeof(*r));
    for (int i = 0; i < MAX_JOBS; i++) {
        r->response_time[i] = -1;
    }
}

int scheduler_validate_jobs(const Job jobs[], int count) {
    if (jobs == NULL || count < 1 || count > MAX_JOBS) {
        fprintf(stderr, "Error: Job count must be between 1 and %d.\n",
                MAX_JOBS);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        if (jobs[i].id <= 0 ||
            jobs[i].arrival_time < 0 ||
            jobs[i].burst_time <= 0 ||
            jobs[i].priority < 0) {
            fprintf(stderr, "Error: Invalid data for job at index %d.\n", i);
            return 0;
        }

        if (jobs[i].arrival_time > INT_MAX - jobs[i].burst_time) {
            fprintf(stderr, "Error: Job time value is too large.\n");
            return 0;
        }

        for (int j = i + 1; j < count; j++) {
            if (jobs[i].id == jobs[j].id) {
                fprintf(stderr, "Error: Duplicate job ID %d.\n",
                        jobs[i].id);
                return 0;
            }
        }
    }

    return 1;
}

static int add_segment(ScheduleResult *r, int id, int start, int end) {
    if (start == end) {
        return 1;
    }

    if (r->segment_count > 0) {
        GanttSegment *last = &r->timeline[r->segment_count - 1];

        if (last->job_id == id && last->end_time == start) {
            last->end_time = end;
            return 1;
        }
    }

    if (r->segment_count >= MAX_SEGMENTS) {
        fprintf(stderr, "Error: Gantt timeline capacity exceeded.\n");
        return 0;
    }

    GanttSegment *s = &r->timeline[r->segment_count++];
    s->job_id = id;
    s->start_time = start;
    s->end_time = end;

    return 1;
}

static void calculate_metrics(const Job jobs[], int count,
                              ScheduleResult *r) {
    double total_tat = 0;
    double total_wait = 0;
    double total_response = 0;
    int final_time = 0;

    for (int i = 0; i < count; i++) {
        r->turnaround_time[i] =
            r->completion_time[i] - jobs[i].arrival_time;

        r->waiting_time[i] =
            r->turnaround_time[i] - jobs[i].burst_time;

        total_tat += r->turnaround_time[i];
        total_wait += r->waiting_time[i];
        total_response += r->response_time[i];

        if (r->completion_time[i] > final_time) {
            final_time = r->completion_time[i];
        }
    }

    r->average_turnaround_time = total_tat / count;
    r->average_waiting_time = total_wait / count;
    r->average_response_time = total_response / count;
    r->total_completion_time = final_time;
}

/* FCFS: non-preemptive, earliest arrival first. */
int scheduler_fcfs(const Job jobs[], int count,
                   ScheduleResult *r) {
    if (!scheduler_validate_jobs(jobs, count) || r == NULL) {
        return 0;
    }

    initialize_result(r);

    int done[MAX_JOBS] = {0};
    int time = 0;

    for (int completed = 0; completed < count; completed++) {
        int selected = -1;

        for (int i = 0; i < count; i++) {
            if (done[i]) {
                continue;
            }

            if (selected == -1 ||
                jobs[i].arrival_time < jobs[selected].arrival_time ||
                (jobs[i].arrival_time == jobs[selected].arrival_time &&
                 jobs[i].id < jobs[selected].id)) {
                selected = i;
            }
        }

        if (selected == -1) {
            return 0;
        }

        if (time < jobs[selected].arrival_time) {
            if (!add_segment(r, -1, time,
                             jobs[selected].arrival_time)) {
                return 0;
            }
            time = jobs[selected].arrival_time;
        }

        r->response_time[selected] = time - jobs[selected].arrival_time;

        int start = time;
        time += jobs[selected].burst_time;

        if (!add_segment(r, jobs[selected].id, start, time)) {
            return 0;
        }

        r->completion_time[selected] = time;
        done[selected] = 1;
    }

    calculate_metrics(jobs, count, r);
    return 1;
}

/* RR helper: order newly arriving jobs by arrival time, then ID. */
static void enqueue_arrivals(const Job jobs[], int count, int time,
                             int added[], int queue[],
                             int *rear, int *queue_count) {
    for (;;) {
        int selected = -1;

        for (int i = 0; i < count; i++) {
            if (added[i] || jobs[i].arrival_time > time) {
                continue;
            }

            if (selected == -1 ||
                jobs[i].arrival_time < jobs[selected].arrival_time ||
                (jobs[i].arrival_time == jobs[selected].arrival_time &&
                 jobs[i].id < jobs[selected].id)) {
                selected = i;
            }
        }

        if (selected == -1) {
            break;
        }

        queue[*rear] = selected;
        *rear = (*rear + 1) % MAX_JOBS;
        (*queue_count)++;
        added[selected] = 1;
    }
}

/* Round Robin: preemptive time-sliced scheduling. */
int scheduler_round_robin(const Job jobs[], int count,
                          int quantum, ScheduleResult *r) {
    if (!scheduler_validate_jobs(jobs, count) ||
        r == NULL || quantum <= 0) {
        fprintf(stderr, "Error: Time quantum must be positive.\n");
        return 0;
    }

    initialize_result(r);

    int remaining[MAX_JOBS];
    int added[MAX_JOBS] = {0};
    int queue[MAX_JOBS];
    int front = 0, rear = 0, queue_count = 0;
    int completed = 0;
    int time = 0;

    for (int i = 0; i < count; i++) {
        remaining[i] = jobs[i].burst_time;
    }

    enqueue_arrivals(jobs, count, time, added,
                     queue, &rear, &queue_count);

    while (completed < count) {
        if (queue_count == 0) {
            int next = -1;

            for (int i = 0; i < count; i++) {
                if (!added[i] &&
                    (next == -1 ||
                     jobs[i].arrival_time < jobs[next].arrival_time ||
                     (jobs[i].arrival_time == jobs[next].arrival_time &&
                      jobs[i].id < jobs[next].id))) {
                    next = i;
                }
            }

            if (next == -1) {
                return 0;
            }

            if (time < jobs[next].arrival_time) {
                if (!add_segment(r, -1, time,
                                 jobs[next].arrival_time)) {
                    return 0;
                }
                time = jobs[next].arrival_time;
            }

            enqueue_arrivals(jobs, count, time, added,
                             queue, &rear, &queue_count);
        }

        int current = queue[front];
        front = (front + 1) % MAX_JOBS;
        queue_count--;

        if (r->response_time[current] == -1) {
            r->response_time[current] =
                time - jobs[current].arrival_time;
        }

        int run_time = remaining[current] < quantum
                       ? remaining[current] : quantum;

        int start = time;
        time += run_time;
        remaining[current] -= run_time;

        if (!add_segment(r, jobs[current].id, start, time)) {
            return 0;
        }

        enqueue_arrivals(jobs, count, time, added,
                         queue, &rear, &queue_count);

        if (remaining[current] == 0) {
            r->completion_time[current] = time;
            completed++;
        } else {
            queue[rear] = current;
            rear = (rear + 1) % MAX_JOBS;
            queue_count++;
        }
    }

    calculate_metrics(jobs, count, r);
    return 1;
}

/* Priority: non-preemptive; lower number means higher priority. */
int scheduler_priority(const Job jobs[], int count,
                       ScheduleResult *r) {
    if (!scheduler_validate_jobs(jobs, count) || r == NULL) {
        return 0;
    }

    initialize_result(r);

    int done[MAX_JOBS] = {0};
    int time = 0;

    for (int completed = 0; completed < count; completed++) {
        int selected = -1;

        for (int i = 0; i < count; i++) {
            if (done[i] || jobs[i].arrival_time > time) {
                continue;
            }

            if (selected == -1 ||
                jobs[i].priority < jobs[selected].priority ||
                (jobs[i].priority == jobs[selected].priority &&
                 (jobs[i].arrival_time < jobs[selected].arrival_time ||
                  (jobs[i].arrival_time == jobs[selected].arrival_time &&
                   jobs[i].id < jobs[selected].id)))) {
                selected = i;
            }
        }

        if (selected == -1) {
            int next = -1;

            for (int i = 0; i < count; i++) {
                if (!done[i] &&
                    (next == -1 ||
                     jobs[i].arrival_time < jobs[next].arrival_time ||
                     (jobs[i].arrival_time == jobs[next].arrival_time &&
                      jobs[i].id < jobs[next].id))) {
                    next = i;
                }
            }

            if (next == -1) {
                return 0;
            }

            if (time < jobs[next].arrival_time) {
                if (!add_segment(r, -1, time,
                                 jobs[next].arrival_time)) {
                    return 0;
                }
                time = jobs[next].arrival_time;
            }

            continue;
        }

        r->response_time[selected] = time - jobs[selected].arrival_time;

        int start = time;
        time += jobs[selected].burst_time;

        if (!add_segment(r, jobs[selected].id, start, time)) {
            return 0;
        }

        r->completion_time[selected] = time;
        done[selected] = 1;
    }

    calculate_metrics(jobs, count, r);
    return 1;
}

void scheduler_print_result(const Job jobs[], int count,
                            const ScheduleResult *r,
                            const char *algorithm_name) {
    if (jobs == NULL || r == NULL || algorithm_name == NULL) {
        return;
    }

    printf("\n========== %s ==========\n", algorithm_name);
    printf("%-6s %-8s %-8s %-9s %-11s %-8s %-8s %-8s\n",
           "ID", "Arrival", "Burst", "Priority",
           "Completion", "TAT", "Waiting", "Response");

    for (int i = 0; i < count; i++) {
        printf("P%-5d %-8d %-8d %-9d %-11d %-8d %-8d %-8d\n",
               jobs[i].id,
               jobs[i].arrival_time,
               jobs[i].burst_time,
               jobs[i].priority,
               r->completion_time[i],
               r->turnaround_time[i],
               r->waiting_time[i],
               r->response_time[i]);
    }

    printf("\nGantt timeline:\n");
    for (int i = 0; i < r->segment_count; i++) {
        const GanttSegment *s = &r->timeline[i];

        if (s->job_id == -1) {
            printf("| IDLE %d-%d ", s->start_time, s->end_time);
        } else {
            printf("| P%d %d-%d ", s->job_id,
                   s->start_time, s->end_time);
        }
    }
    printf("|\n");

    printf("\nAverage waiting time    : %.2f\n",
           r->average_waiting_time);
    printf("Average turnaround time : %.2f\n",
           r->average_turnaround_time);
    printf("Average response time   : %.2f\n",
           r->average_response_time);
    printf("Total completion time   : %d\n",
           r->total_completion_time);
}
