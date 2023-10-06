#include "shell.h"

void clear_terminal() {
    printf("\033[H\033[J");
}

void help() {
    printf("Simple Shell Help:\n");
    printf("  cd [directory] - Change the current working directory.\n");
    printf("  help - Display this help message.\n");
    printf("  clear - Clear terminal\n");
    printf("  top - Provides real-time information about system processes, resource usage, and system performance.\n");
    printf("  ls - Used to list the files and directories in the current directory. \n");
    printf("  ls -la - Provides a detailed listing of files and directories, including additional information such as file permissions, owner, group, file size, and modification date.\n");
    printf("  mkdir - Allows you to create directories (folders) in your file system from the command line.\n");
    printf("  rm - Allows you to remove files from your file system using the command line.\n");
    printf("  ping - Network utility used to test the reachability of a network host by sending ICMP (Internet Control Message Protocol).\n");
    printf("  ifconfig - Allows you to configure and display information about your system's network interfaces.\n");
    printf("  ps - Display information about running processes.\n");
    printf("  command & - Job control.\n");
    printf("  fg - The fg command in a terminal is used to bring a background job back to the foreground.\n");
    printf("  bg - The bg command in a terminal is used to manage background jobs.\n");
    printf("  echo - Display messages or values to the standard output. \n");
    printf("  pwd - Display the current working directory of the user within the file system hierarchy.\n");
    printf("  exit - Exit the shell.\n");
}

void function(const char* command) {
    int status = system(command);

    if (status == -1) {
        perror("Error executing ifconfig");
        return;
    }
}

void get_current_directory(char *buf, size_t size) {
    if (getcwd(buf, size) == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }
}

int create_directory(const char *path) {
    int status = mkdir(path, 0777);  // 0777 allows full read, write, and execute permissions (for demonstration purposes)
    
    if (status == 0) {
        printf("Directory '%s' created successfully.\n", path);
        return 0;  // Success
    } else {
        perror("Error creating directory");
        return 1;  // Failure
    }
}

int remove_file(const char *filename) {
    if (remove(filename) == 0) {
        printf("File '%s' has been removed.\n", filename);
        return 0;  // Success
    } else {
        perror("Error removing file");
        return 1;  // Failure
    }
}

int change_directory(const char *path) {
    if (chdir(path) == 0) {
        printf("Changed directory to '%s'\n", path);
        return 0;  // Success
    } else {
        perror("Error changing directory");
        return 1;  // Failure
    }
}

void echo(char* message[], int arg_count) {
    for(int i = 1; i < arg_count; ++i) {
        printf("%s", message[i]);
        if(i < arg_count - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

// Global variable to keep track of whether Ctrl+C was pressed
volatile int ctrlC_pressed = 0;

// Signal handler for SIGINT (Ctrl+C)
void sigint_handler() {
    if (ctrlC_pressed == 1) {
        // The program has already received Ctrl+C once, exit the program
        printf("Ctrl+C pressed. Exiting the shell...\n");
        exit(0);
    } else {
        // The program has not yet received Ctrl+C, stop the ping function
        printf("Ctrl+C pressed. Stopping the ping command...\n");
        ctrlC_pressed = 1;
    }
}

int ping(char *command) {
    // Set up the signal handler for SIGINT
    signal(SIGINT, sigint_handler);

    // Create a child process to execute the command
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return -1;
    } else if (pid == 0) {
        // In the child process
        char *args[] = { "/bin/sh", "-c", command, NULL };
        execvp(args[0], args);
        perror("Execution failed");
        exit(1);
    } else {
        // In the parent process, wait for the child to complete
        int status;
        waitpid(pid, &status, 0);

        if (ctrlC_pressed) {
            // If Ctrl+C was pressed, terminate the child process
            kill(pid, SIGTERM);
            return -1;
        } else {
            return WEXITSTATUS(status);
        }
    }
}

void run_pipeline(char **commands) {
    int num_commands = 0;
    while (commands[num_commands] != NULL) {
        num_commands++;
    }

    int pipe_fds[2];
    int prev_pipe = -1;

    for (int i = 0; i < num_commands; i++) {
        if (pipe(pipe_fds) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (child_pid == 0) {
            // Child process
            close(pipe_fds[0]); // Close the read end of the pipe

            if (i > 0) {
                dup2(prev_pipe, STDIN_FILENO); // Redirect input from the previous pipe
            }

            if (i < num_commands - 1) {
                dup2(pipe_fds[1], STDOUT_FILENO); // Redirect output to the next pipe
            }

            close(pipe_fds[1]); // Close the write end of the pipe

            execvp(commands[i], &commands[i]);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            close(prev_pipe); // Close the previous pipe (if any)
            close(pipe_fds[1]); // Close the write end of the pipe
            prev_pipe = pipe_fds[0]; // Save the read end of the current pipe for the next iteration
        }
    }

    // Wait for all child processes to finish
    while (wait(NULL) > 0) {}
}

void list_files_detailed(const char *path) {
    DIR *directory;
    struct dirent *entry;

    directory = opendir(path);
    if (directory == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    printf("%-15s %-15s %-21s %-14s %-15s %-15s\n", "Name", "Size", "Owner", "Group", "Permissions", "Last Modified");
    while ((entry = readdir(directory))) {
        char full_path[PATH_MAX];
        struct stat file_stat;

        // Build the full path to the file
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (stat(full_path, &file_stat) == 0) {
            struct passwd *user_info = getpwuid(file_stat.st_uid);
            struct group *group_info = getgrgid(file_stat.st_gid);
            struct tm *time_info = localtime(&file_stat.st_mtime);

            printf("%-15s %-15ld bytes ", entry->d_name, file_stat.st_size);
            printf("%-15s %-15s", user_info ? user_info->pw_name : "N/A", group_info ? group_info->gr_name : "N/A");
            printf("%-15o %-15s", file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO), asctime(time_info));
        } else {
            perror("stat");
        }
    }

    closedir(directory);
}

int append_output_to_file(const char *command, const char *filename) {
    // Open the file for appending (or create it if it doesn't exist)
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (fd == -1) {
        perror("open"); // Error opening the file 
        return 1; 
    }

    // Duplicate the file descriptor for stdout (1)
    int stdout_copy = dup(STDOUT_FILENO);

    if (stdout_copy == -1) {
        perror("dup");
        close(fd);
        return 1; // Error duplicating the file descriptor
    }

    // Redirect stdout to the file descriptor of the opened file
    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        close(fd);
        return 1; // Error redirecting stdout
    }

    // Close the file descriptor for the opened file (no longer needed)
    close(fd);

    // Execute the command
    int status = system(command);

    if (status == -1) {
        perror("Error executing command");
        return 1; // Error executing the command
    }

    // Restore the original stdout
    if (dup2(stdout_copy, STDOUT_FILENO) == -1) {
        perror("dup2");
        return 1; // Error restoring stdout
    }

    // Close the duplicated stdout file descriptor (no longer needed)
    close(stdout_copy);

    return 0; // Success
}

void execute_command(char* args[], int arg_count) {
    if (arg_count >= 3 && strcmp(args[arg_count - 2], ">>") == 0) {
        const char *filename = args[arg_count - 1];
        args[arg_count - 2] = NULL; // Remove ">>" and the filename from the arguments
        int result = append_output_to_file(args[0], filename);

        if (result == 0) {
            printf("Output appended to '%s'.\n", filename);
        }
    } else if(arg_count > 0 && strcmp(args[0], "exit") == 0) {
        exit(EXIT_SUCCESS);
    } else if(arg_count > 0 && strcmp(args[0], "help") == 0) {
        help();
    } else if(arg_count > 0 && strcmp(args[0], "clear") == 0) {
        printf("\003");
        clear_terminal();
    } else if(arg_count == 2 && strcmp(args[0], "ls") == 0 && strcmp(args[1], "-la") == 0) { 
        const char* path = ".";

        list_files_detailed(path);
    } else if(arg_count > 0 && strcmp(args[0], "mkdir") == 0) {
        if (arg_count < 2) {
            printf("Usage: mkdir <directory_name>\n");
        } else {
            char* directory_name = args[1];
            if (strcmp(directory_name, "") == 0) {
                printf("Invalid input: Empty directory name.\n");
            } else {
                create_directory(directory_name);
            }
        }
    } else if(arg_count > 0 && strcmp(args[0], "rm") == 0) {
        const char *filename = args[1];
        remove_file(filename);
    } else if(arg_count > 0 && strcmp(args[0], "cd") == 0) {
        change_directory(args[1]);
    } else if (arg_count > 0 && strcmp(args[0], "ping") == 0) { // ctrl c
        if (arg_count < 2) {
            printf("Usage: ping <hostname_or_ip>\n");
        } else {
            // Build the ping command with the provided arguments
            char ping_command[MAX_INPUT_SIZE];
            snprintf(ping_command, sizeof(ping_command), "ping %s", args[1]);

            // Execute the ping command
            int result = ping(ping_command);

            if (result == -1) {
                printf("Failed to execute the ping command.\n");
            }
        }

        // Set up the signal handler for SIGINT
        signal(SIGINT, sigint_handler);
    } else if(arg_count == 2 && strcmp(args[0], "command") == 0 && strcmp(args[1], "&") == 0){
        run_command_in_background(args[0]);
    } else if (arg_count == 2 && strcmp(args[0], "fg") == 0) {
        pid_t pid = atoi(args[1]);
        bring_to_foreground(pid);
    } else if (arg_count == 2 && strcmp(args[0], "bg") == 0) {
        pid_t job_number = atoi(args[1]);
        resume_background_job(job_number);
    } else if (arg_count == 1 && (strcmp(args[0], "top") == 0 || strcmp(args[0], "ls") == 0|| strcmp(args[0], "ps") == 0 || strcmp(args[0], "ifconfig") == 0 || strcmp(args[0], "pwd") == 0)){
        function(args[0]);
    } else if(arg_count > 0 && strcmp(args[0], "echo") == 0) {
        echo(args, arg_count);
    } else {
        printf("gd: command not found\n");
    }
}

void pipe_commands(const char *cmd1, const char *cmd2) {
    int pipe_fd[2]; // File descriptors for the pipe

    // Create a pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork the first child process to execute cmd1
    pid_t child1_pid = fork();

    if (child1_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child1_pid == 0) {
        // In the first child process
        close(pipe_fd[0]); // Close the read end of the pipe
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
        close(pipe_fd[1]); // Close the write end of the pipe

        // Execute cmd1
        execl("/bin/sh", "sh", "-c", cmd1, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    // Fork the second child process to execute cmd2
    pid_t child2_pid = fork();

    if (child2_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child2_pid == 0) {
        // In the second child process
        close(pipe_fd[1]); // Close the write end of the pipe
        dup2(pipe_fd[0], STDIN_FILENO); // Redirect stdin to the read end of the pipe
        close(pipe_fd[0]); // Close the read end of the pipe

        // Execute cmd2
        execl("/bin/sh", "sh", "-c", cmd2, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    // In the parent process
    close(pipe_fd[0]); // Close both ends of the pipe
    close(pipe_fd[1]);

    // Wait for both child processes to complete
    waitpid(child1_pid, NULL, 0);
    waitpid(child2_pid, NULL, 0);
}

void run_shell() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];
    int arg_count = 0;
    char current_directory[PATH_MAX];

    clear_terminal();

    while (1) {
        get_current_directory(current_directory, sizeof(current_directory));  // Get the current directory
        printf("Shell/%s ", current_directory);  // Include it in the prompt
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("fgets");
            exit(EXIT_FAILURE);
        }

        char *token = strtok(input, " \t\n");
        arg_count = 0;

        while (token != NULL) {
            args[arg_count] = token;
            arg_count++;

            if (arg_count >= MAX_ARGS) {
                fprintf(stderr, "Too many arguments.\n");
                exit(EXIT_FAILURE);
            }

            token = strtok(NULL, " \t\n");
        }
        args[arg_count] = NULL;

        int pipe_position = -1;

        // Find the position of the pipe character "|"
        for (int i = 0; i < arg_count; i++) {
            if (strcmp(args[i], "|") == 0) {
                pipe_position = i;
                break;
            }
        }

        if (pipe_position != -1) {
            // If the pipe character is found, split the commands into two parts
            args[pipe_position] = NULL; // Terminate the first command
            execute_command(args, pipe_position); // Execute the first command
            execute_command(args + pipe_position + 1, arg_count - pipe_position - 1); // Execute the second command
        } else {
            // If no pipe character is found, execute the command as usual
            execute_command(args, arg_count);
        }
    }
}
