#include "job.h"

Job jobs[MAX_JOBS];
int num_jobs = 0;

// Function to add a job to the job list
void add_job(pid_t pid, const char *command) {
    if (num_jobs < MAX_JOBS) {
        jobs[num_jobs].pid = pid;
        strncpy(jobs[num_jobs].command, command, sizeof(jobs[num_jobs].command));
        jobs[num_jobs].command[sizeof(jobs[num_jobs].command) - 1] = '\0'; // Ensure null-termination
        num_jobs++;
    } else {
        fprintf(stderr, "Maximum number of jobs reached.\n");
    }
}

// command &
void run_command_in_background(const char* command) {
    // Fork a child process
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Child process
        // Redirect standard error to /dev/null
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull == -1) {
            perror("open");
            exit(1);
        }
        dup2(devnull, STDERR_FILENO);
        close(devnull);

        // Execute the command in the background
        char* args[] = { (char*)command, NULL };
        if (execvp(args[0], args) == -1) {
            perror("execvp"); // This will not be displayed
            exit(1);
        }
    } else {
        add_job(pid, command);
        printf("[%d] %d\n", num_jobs, pid);
    }
}

void remove_job(int index) {
    if (index < 0 || index >= num_jobs) {
        return;
    }
    
    // Overwrite the job to be removed with the last job in the list
    jobs[index] = jobs[num_jobs - 1];
    num_jobs--;
}

// Function to bring a background job to the foreground
void bring_to_foreground(pid_t pid) {
    for (int i = 0; i < num_jobs; ++i) {
        if (jobs[i].pid == pid) {
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid");
            } else {
                printf("[%d]+   Done          %s\n", i + 1, jobs[i].command);
                remove_job(i);
            }
            return;
        }
    }
    printf("Job with PID %d not found.\n", pid);
}   

// Resume a background job by sending SIGCONT signal
void resume_background_job(pid_t pid) {
    for (int i = 0; i < num_jobs; ++i) {
        if (jobs[i].pid == pid) {

            // Remove the job from the list
            for (int j = i; j < num_jobs - 1; j++) {
                jobs[j] = jobs[j + 1];
            }

            num_jobs--;
            printf("[%d]+   Done          command\n", i + 1);

            if (kill(pid, SIGCONT) == -1) {
                perror("kill");
                return;
            }
            return;
        }
    }
    // If the PID is not found in the job list
    printf("Job with PID %d not found.\n", pid);
}

void jobss() {
    for (int i = 0; i < num_jobs; i++) {
        int status;
        pid_t pid = jobs[i].pid;

        if (waitpid(pid, &status, WNOHANG) == 0) {
            // The job is still running
            printf("[%d]+   Running       %s (PID %d)\n", i + 1, jobs[i].command, pid);
        } else if (WIFEXITED(status)) {
            // The job has exited
            printf("[%d]+   Done          %s (PID %d)\n", i + 1, jobs[i].command, pid);
            // Remove the job from the list
            remove_job(i);
        } else if (WIFSIGNALED(status)) {
            // The job was terminated by a signal
            printf("[%d]+   Terminated    %s (PID %d)\n", i + 1, jobs[i].command, pid);
            // Remove the job from the list
            remove_job(i);
        }
    }
}
