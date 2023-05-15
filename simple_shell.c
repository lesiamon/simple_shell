#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 100

void display_prompt() {
    printf("$ ");  // Display the prompt
    fflush(stdout);
}

void execute_command(char *command) {
    pid_t pid = fork();  // Create a child process

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        execlp(command, command, NULL);  // Execute the command
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        wait(NULL);  // Wait for the child process to finish
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];

    while (1) {
        display_prompt();
        
        if (fgets(command, sizeof(command), stdin) == NULL) {
            // Handle end of file (Ctrl+D)
            printf("\n");
            break;
        }

        command[strcspn(command, "\n")] = '\0';  // Remove the trailing newline character

        execute_command(command);
    }

    return 0;
}
