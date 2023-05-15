#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_ARGUMENTS 10

void display_prompt() {
    printf("$ ");  // Display the prompt
    fflush(stdout);
}

int parse_command(char *command, char **arguments) {
    int num_arguments = 0;
    int i = 0;
    int arg_start_index = 0;
    int in_arg = 0;

    while (command[i] != '\0') {
        if (command[i] == ' ' || command[i] == '\t' || command[i] == '\n') {
            if (in_arg) {
                command[i] = '\0';  // Null-terminate the argument
                arguments[num_arguments++] = &command[arg_start_index];
                in_arg = 0;
            }
        } else {
            if (!in_arg) {
                arg_start_index = i;
                in_arg = 1;
            }
        }
        i++;
    }

    if (in_arg) {
        command[i] = '\0';  // Null-terminate the argument
        arguments[num_arguments++] = &command[arg_start_index];
    }

    arguments[num_arguments] = NULL;  // Set the last argument to NULL

    return num_arguments;
}

int execute_command(char **arguments) {
    pid_t pid = fork();  // Create a child process

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        execvp(arguments[0], arguments);  // Execute the command with arguments
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        wait(&status);  // Wait for the child process to finish
        return status;
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char *arguments[MAX_ARGUMENTS];

    while (1) {
        display_prompt();

        if (fgets(command, sizeof(command), stdin) == NULL) {
            // Handle end of file (Ctrl+D)
            printf("\n");
            break;
        }

        command[strcspn(command, "\n")] = '\0';  // Remove the trailing newline character

        int num_arguments = parse_command(command, arguments);

        if (num_arguments > 0) {
            // Check if the command exists
            if (access(arguments[0], F_OK) == 0) {
                int status = execute_command(arguments);
                if (status != 0) {
                    fprintf(stderr, "Command failed with exit status: %d\n", status);
                }
            } else {
                fprintf(stderr, "Command not found: %s\n", arguments[0]);
            }
        }
    }

    return 0;
}

