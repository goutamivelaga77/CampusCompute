# SRN: PES1UG25CS193 | OS Mini Project (UE25CS242A) - CampusCompute
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
LIBS    = -pthread -lm
TARGET  = campuscompute
SOURCES = main.c scheduler.c advisor.c banker.c worker.c logger.c ipc.c limits.c
TESTS   = test_scheduler test_advisor test_banker test_worker test_logger test_ipc test_limits

all: $(TARGET)

$(TARGET): $(SOURCES) $(wildcard *.h)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LIBS)

test_scheduler: scheduler.c test_scheduler.c scheduler.h
	$(CC) $(CFLAGS) scheduler.c test_scheduler.c -o $@ $(LIBS)
test_advisor: scheduler.c advisor.c test_advisor.c
	$(CC) $(CFLAGS) scheduler.c advisor.c test_advisor.c -o $@ $(LIBS)
test_banker: banker.c test_banker.c banker.h
	$(CC) $(CFLAGS) banker.c test_banker.c -o $@
test_worker: worker.c test_worker.c worker.h
	$(CC) $(CFLAGS) worker.c test_worker.c -o $@
test_logger: logger.c test_logger.c logger.h
	$(CC) $(CFLAGS) logger.c test_logger.c -o $@ -pthread
test_ipc: ipc.c test_ipc.c ipc.h
	$(CC) $(CFLAGS) ipc.c test_ipc.c -o $@ -pthread
test_limits: limits.c test_limits.c limits.h
	$(CC) $(CFLAGS) limits.c test_limits.c -o $@

tests: $(TESTS)

test: tests
	@for t in $(TESTS); do ./$$t || exit 1; done

clean:
	rm -f $(TARGET) $(TESTS) campuscompute.log

run: $(TARGET)
	./$(TARGET)

.PHONY: all tests test clean run
