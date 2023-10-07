#ifndef SHELL
#define SHELL

#include "job.h"

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>

#include <time.h> // Include this header for localtime and asctime
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h> // Include this header for file-related constants
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 100
#define PATH_MAX 4096

void clear_terminal();
void help();
void function(const char* command);
void get_current_directory(char *buf, size_t size);
int create_directory(const char *path);
int remove_file(const char *filename);
int change_directory(const char *path);
void echo(char* message[], int arg_count);
void sigint_handler();
int ping(char *command);
void list_files_detailed(const char *path);
int append_output_to_file(const char *command, const char *filename);
void execute_command(char* args[], int arg_count);
void run_shell();

#endif 
