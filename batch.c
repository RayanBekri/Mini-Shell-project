#include "batch.h"
#include "parse.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_INPUT_LENGTH 1024 

/**
 * Executes commands from a batch file.
 *
 * Opens a specified file and reads commands line by line, executing each one.
 * This function is used for running the shell in batch mode, where commands
 * are read from a file instead of an interactive prompt.
 *
 * @param filename The name of the file containing the batch commands.
 * @return 0 on successful execution of all commands, or -1 if an error occurs
 *         during file opening, tokenizing, parsing, or command execution.
 */
int batch_mode(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    char input_line[MAX_INPUT_LENGTH];
    char tokens[MAX_NB_TOKENS][MAX_TOKEN_LEN];
    CommandSequence command_sequence;

    while (fgets(input_line, MAX_INPUT_LENGTH, file) != NULL) {
        input_line[strcspn(input_line, "\n")] = 0; 

        int nb_tokens = tokenize(input_line, tokens); 
        if (nb_tokens == -1) {
            fprintf(stderr, "Error: Failed to tokenize input\n");
            continue;
        }

        if (parse(tokens, nb_tokens, &command_sequence) == -1) {
            fprintf(stderr, "Error: Failed to parse command\n");
            continue;
        }

        if (launch_command(&command_sequence) == -1) {
            fprintf(stderr, "Error: Failed to execute command\n");
        }
    }

    fclose(file);
    return 0;
}
