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

void parse_command(char *command, char **arguments) {
    int i = 0;
    arguments[i] = strtok(command, " ");  // Tokenize the command

    while (arguments[i] != NULL && i < MAX_ARGUMENTS - 1) {
        i++;
        arguments[i] = strtok(NULL, " ");  // Tokenize the arguments
    }
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

        parse_command(command, arguments);

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

    return 0;
}

