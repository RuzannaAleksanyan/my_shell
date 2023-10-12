#ifndef JOB
#define JOB

#include <fcntl.h> // Include this header for file-related constants
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_JOBS 10
#define MAX_CMD_LENGTH 1024

// Structure to store job information
typedef struct {
    pid_t pid;
    char command[MAX_CMD_LENGTH];
} Job;

void add_job(pid_t pid, const char *command);
void run_command_in_background(const char* command);
void remove_job(int index);
void bring_to_foreground(pid_t pid);
void resume_background_job(pid_t pid);
void jobss();


#endif
