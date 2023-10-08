# Simple Shell

## General Overview

A shell is a basic command-line shell program implemented in the C programming language. It provides a simple interface for running commands and managing files and information. In the modern computer world, using a simple shell is preferable to make user work more accessible.

## Table of Contents

1. [Commands' Structure](#commands-structure)
2. [Commands](#commands)
3. [Algorithms](#algorithms)
   - [Execution of Commands](#execution-of-commands)
   - [Job Control](#job-control)
   - [File and Directory Operations](#file-and-directory-operations)
   - [Handling Signals](#handling-signals)
4. [Usage](#usage)

## Commands' Structure

The Simple Shell accepts various commands and arguments with the following structure:
            command [argument]

- `command`: The action to be performed (ls, cd, help, etc.).
- `[arguments]`: Optional arguments for the command.

## Commands

- `ls`: List files and directories in the current directory.
- `ls -la`: List files and directories in long format (-l) and include hidden files (-a).
- `mkdir`: Create a new directory.
- `rm`: Remove files and directories.
- `cd`: Change the current directory.
- `help`: Request help (specific command).
- `exit`: Exit the current shell session.
- `clear`: Clear the terminal screen.
- `ping`: Test network connectivity to a host.
- `ps`: List currently running processes.
- `ifconfig`: Display information about network interfaces.
- `echo`: Display messages or values to the standard output.
- `pwd`: Print the current working directory.
- `command &`: Run a command in the background.
- `fg`: Bring a background job to the foreground.
- `bg`: Resume a stopped background job.
- `top`: Display dynamic information about system processes.
- `|`: Gives the first command's output to the second one and does the second command.
- `>>`: Append the output of a command to a file.

## Algorithms

### Execution of Commands

- The Simple Shell reads user inputs and analyzes the command and its components.
- It checks for special commands like exit, help, clear, etc.
- For commands not listed above, it uses the `execvp` function to run external applications.

### Job Control

- The shell supports background commands using the syntax `command &`.
- It manages background processes and their Process IDs (PIDs) and associated commands.
- The `bg` command restarts background jobs that have been stopped.
- The `fg` command brings a job to the foreground.
- The shell manages job lists and control signals.

### File and Directory Operations

- The shell provides commands for file and directory operations such as `ls`, `mkdir` and `rm`.
- The `ls -la` command displays detailed information about files and directories.

### Handling Signals

- The shell handles signals like SIGINT (Ctrl+C) to interrupt processes.
- SIGINT allows users to stop a command's operation with Ctrl+C and exit the shell with a double Ctrl+C.
- Managing signals improves control over processes.

## Usage

1. **Build MyShell:**

   - Before using MyShell, ensure you have GCC (GNU Compiler Collection) installed on your system.
   - Navigate to the directory containing the MyShell source code and the Makefile.
   - Open a terminal window and run the following command to build MyShell:
     ```
     make
     ```

2. **Run MyShell:**

   - After successfully building MyShell, run it by executing the following command:
     ```
     ./myshell
     ```
     
3. **Customization:**

   - You can customize MyShell by modifying the source code in `test.c`. Add new commands or functionality to suit your specific needs. You can also customize compiler flags and options in the Makefile.
