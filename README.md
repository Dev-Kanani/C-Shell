# MiniProject-1

## Overview

This project is a custom shell implementation with various features such as command execution, job management, process information retrieval, file searching, and logging. The shell supports foreground and background processes and includes several commands to interact with the system.

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

## Assumptions

1) Macro Usage: The macro MAX_LENGTH_OF_NAMES is used to define the maximum length for certain strings.

2) Multiple Arguments Handling: If a command has multiple arguments with a length greater than 2, the argument in the last position is retained and displayed.

3) hop Command Behavior : When using the hop command with the t1 - flag, the shell will ultimately return to the original directory where the command was initially called.
   
4) I am not adding the log execute commands again.
   
5) hop . doesnt change the previous directory

6) hop .. .. .. .. .. The directory previous to the last dot dot library will be the previousdirectory(new) 

