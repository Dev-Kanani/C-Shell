# MiniProject-1

## Overview

This project is a custom shell implementation designed to handle various system-level tasks such as command execution, job management, process control, and input/output redirection. The shell supports both foreground and background processes and includes several built-in commands to interact with the system.

## Files

### Source Files

- **`foreground.c`**
  - Implements functionality for managing foreground processes.

- **`proclore.c`**
  - Implements functions to retrieve and display process information, including PID, status, memory, and executable path.

- **`log.c`**
  - Implements command logging functionality and allows execution of commands from the log.

- **`parse_command.c`**
  - Implements the parsing of commands and handles different types of input from the user.

- **`queue.c`**
  - Implements the queue management for handling multiple processes or commands.

- **`reveal.c`**
  - Implements the `reveal` command to display files and directories with options for color coding and sorting.

- **`seek.c`**
  - Implements the `seek` command to search for files and directories, with options to filter results.

- **`main.c`**
  - Contains the main function that initializes the shell and handles user input and command execution.

### Header Files

- **`neonate.h`**
  - Declares the functions and variables for handling the `neonate` command, which prints the PID of the most recent process created.

- **`iman.h`**
  - Contains the declarations for the `iMan` command, which fetches man pages from the internet using sockets.

- **`aliases.h`**
  - Defines structures and functions for managing command aliases, including dynamic creation and persistence through a `.myshrc` file.

- **`ioredirect.h`**
  - Declares functions for handling input/output redirection, ensuring that commands properly redirect their input and output as specified by the user.

## Compilation

1. **Build the Project**
   - Use `make` to compile the source files and build the executable:
     ```sh
     make
     ```

2. **Clean Up**
   - To remove the compiled files and clean the project directory, use:
     ```sh
     make clean
     ```

## Running the Shell

- Execute the compiled shell:
  ```sh
  ./a.out

## Assumptions for Shell Implementation

## Piping and Redirection Rules

- **Pipes at Start or End**: If a pipe (`|`) is placed at the start or end of a command, it will result in an error.
- **Consecutive Pipes with Space**: If there are two pipes (`|`) with spaces between them in a command, this will trigger an error, invalidating the entire command.
- **Double Pipes (`||`) Behavior**: In commands containing double pipes (`||`), any part of the command that follows `||` will be ignored. No syntax errors will be reported for the ignored part, even if they exist.

## Command Behavior

- **Background Process Checks**: The `sleep` command may not reliably indicate the status of background processes on certain systems. Use `gedit` or `emacs` to check background processes instead.
- **Macro Usage**: The macro `MAX_LENGTH_OF_NAMES` defines the maximum allowable length for specific string values and names within the shell.

## Argument Handling

- **Handling Multiple Arguments**: For commands with multiple arguments that exceed a length of 2, the argument in the last position will be retained and used. This is specifically relevant to commands like `sleep`.

## `hop` Command

- **Behavior of the `hop` Command**:
   - Using the `hop` command with the `t1` flag will return the shell to the original directory where the command was first executed.
   - The `hop .` command does not modify the current working directory.
   - In commands like `hop .. .. .. ..`, the shell will set the directory previous to the last `..` as the new "previous directory."

## Logging

- **Log Execution**: The shell does not log commands that have been executed, so commands cannot be replayed from logs.
- If at anywhere in my command there is `log`, then the whole log.txt file will get printed.

## Printing

- **Output Redirection with Background Processes**: When executing commands like sleep 4 & > a.txt, the output, including the PID of the background process and the success message, will be redirected entirely to a.txt.

- **`hop .. &` Command Behavior**:Using the hop .. & command will not function as expected when running in the background. The process will not change directories or work as intended. Only the PID will be printed, but no actual execution will occur, leading to an error in execvp.
