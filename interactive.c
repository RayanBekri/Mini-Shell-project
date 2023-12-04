#include "interactive.h"
#include "parse.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_INPUT_LENGTH 1024

/**
 * Runs the shell in interactive mode.
 * Reads commands from stdin, executes them, and logs to 'history.txt'.
 * Exits on 'exit' command or EOF.
 *
 * @return 0 on normal exit, -1 on file error.
 */

int interactive_mode() {
    char input_line[MAX_INPUT_LENGTH];
    char tokens[MAX_NB_TOKENS][MAX_TOKEN_LEN];
    CommandSequence command_sequence;
    FILE *history_file = fopen("history.txt", "a");
    if (!history_file) {
        perror("Error opening history file");
        return -1;
    }

    while (1) {
        printf("shell> ");
        if (fgets(input_line, MAX_INPUT_LENGTH, stdin) == NULL) {
            if (feof(stdin)) {
                printf("\nExiting shell.\n");
                break;
            }
            perror("Error reading input");
            continue;
        }

        input_line[strcspn(input_line, "\n")] = 0;

        fprintf(history_file, "%s\n", input_line);
        fflush(history_file);

        int nb_tokens = tokenize(input_line, tokens);
        if (nb_tokens == -1) {
            fprintf(stderr, "Error: Failed to tokenize input\n");
            continue;
        }

        if (nb_tokens > 0 && strcmp(tokens[0], "exit") == 0) {
            printf("Exiting shell.\n");
            break;
        }

        if (parse(tokens, nb_tokens, &command_sequence) == -1) {
            fprintf(stderr, "Error: Failed to parse command\n");
            continue;
        }

        if (launch_command(&command_sequence) == -1) {
            fprintf(stderr, "Error: Failed to execute command\n");
        }
    }

    fclose(history_file);
    return 0;
}
