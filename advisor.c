/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | advisor.c */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "advisor.h"

int advisor_classify(const Job jobs[], int count,
                     char *category, size_t category_size,
                     char *explanation, size_t explanation_size) {

    if (!scheduler_validate_jobs(jobs, count) ||
        category == NULL || explanation == NULL ||
        category_size == 0 || explanation_size == 0) {
        return 0;
    }

    double total = 0;
    double min_burst = jobs[0].burst_time;
    double max_burst = jobs[0].burst_time;

    int min_arrival = jobs[0].arrival_time;
    int max_arrival = jobs[0].arrival_time;

    int min_priority = jobs[0].priority;
    int max_priority = jobs[0].priority;

    for (int i = 0; i < count; i++) {
        double burst = jobs[i].burst_time;

        total += burst;

        if (burst < min_burst) min_burst = burst;
        if (burst > max_burst) max_burst = burst;

        if (jobs[i].arrival_time < min_arrival)
            min_arrival = jobs[i].arrival_time;

        if (jobs[i].arrival_time > max_arrival)
            max_arrival = jobs[i].arrival_time;

        if (jobs[i].priority < min_priority)
            min_priority = jobs[i].priority;

        if (jobs[i].priority > max_priority)
            max_priority = jobs[i].priority;
    }

    double average = total / count;
    double variance = 0;

    for (int i = 0; i < count; i++) {
        double difference = jobs[i].burst_time - average;
        variance += difference * difference;
    }

    variance /= count;

    double coefficient = average > 0
                         ? sqrt(variance) / average : 0;

    if (max_priority > min_priority) {
        snprintf(category, category_size, "Priority-sensitive");
        snprintf(explanation, explanation_size,
                 "Jobs have different priority levels. "
                 "Priority scheduling deserves evaluation.");
    }
    else if (coefficient >= 0.50) {
        snprintf(category, category_size, "Mixed-burst workload");
        snprintf(explanation, explanation_size,
                 "Burst times vary considerably. "
                 "Compare response and waiting times carefully.");
    }
    else if (max_arrival > min_arrival) {
        snprintf(category, category_size, "Staggered workload");
        snprintf(explanation, explanation_size,
                 "Jobs arrive at different times. "
                 "Arrival-aware scheduling is important.");
    }
    else {
        snprintf(category, category_size, "Balanced workload");
        snprintf(explanation, explanation_size,
                 "Jobs have relatively similar burst times "
                 "and priority levels.");
    }

    return 1;
}

/*
 * Lower score is better.
 * Waiting time receives 60% weight.
 * Response time receives 40% weight.
 */
int advisor_recommend(const ScheduleResult results[],
                      const char *names[], int count,
                      char *best_algorithm, size_t output_size) {

    if (results == NULL || names == NULL ||
        best_algorithm == NULL || count <= 0 ||
        output_size == 0) {
        return 0;
    }

    int best = -1;
    double best_score = 0;

    for (int i = 0; i < count; i++) {
        if (names[i] == NULL) {
            return 0;
        }

        if (!isfinite(results[i].average_waiting_time) ||
            !isfinite(results[i].average_response_time) ||
            results[i].average_waiting_time < 0 ||
            results[i].average_response_time < 0) {
            return 0;
        }

        double score =
            0.60 * results[i].average_waiting_time +
            0.40 * results[i].average_response_time;

        if (best == -1 || score < best_score) {
            best = i;
            best_score = score;
        }
    }

    int written = snprintf(best_algorithm, output_size,
                           "%s", names[best]);

    return written >= 0 && (size_t)written < output_size;
}
