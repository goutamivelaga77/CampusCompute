/* SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute | advisor.h */
#ifndef ADVISOR_H
#define ADVISOR_H

#include <stddef.h>
#include "scheduler.h"

int advisor_classify(const Job jobs[], int count,
                     char *category, size_t category_size,
                     char *explanation, size_t explanation_size);

int advisor_recommend(const ScheduleResult results[],
                      const char *names[], int count,
                      char *best_algorithm, size_t output_size);

#endif
