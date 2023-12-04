#include "parse.h"
#include "ctype.h"
#include "string.h"
#include <stdio.h>

/**
 * Tokenizes the input string into separate tokens.
 * @param input The input string to tokenize.
 * @param tokens Array to store the tokenized output.
 * @return The number of tokens generated, or -1 on error.
 */
int tokenize(const char* input, char tokens[MAX_NB_TOKENS][MAX_TOKEN_LEN])
{
    int input_len = strlen(input);
    if (input_len == 0) {
        return 0;
    }

    int token_i = 0;
    int token_len = 0;
    for (int i = 0; i < input_len; ++i) {
        if (isspace(input[i])) {
            if (token_len == 0) {
                continue;
            } else {
                if (token_len >= MAX_TOKEN_LEN) {
                    return -1;
                }
                tokens[token_i][token_len] = '\0';
                token_len = 0;
                token_i++;
            }
        } else {
            if (token_i >= MAX_NB_TOKENS) {
                return -1;
            }
            tokens[token_i][token_len] = input[i];
            token_len++;
        }
    }

    if (token_len > 0) {
        if (token_len >= MAX_TOKEN_LEN) {
            return -1;
        }
        tokens[token_i][token_len] = '\0';
        token_i++;
    }

    return token_i;
}

/**
 * Parses an array of tokens into a CommandSequence.
 * @param tokens Array of tokenized inputs.
 * @param nb_tokens Number of tokens.
 * @param command Pointer to CommandSequence to store parsed commands.
 * @return Total number of tokens consumed, or -1 on error.
 */
int parse(
    char tokens[MAX_NB_TOKENS][MAX_TOKEN_LEN],
    int nb_tokens,
    CommandSequence* command)
{
    int cursor = 0;
    command->nb_cmds = 0;
    bool sequential = false;
    bool parallel = false;

    while (nb_tokens - cursor) {
        if (command->nb_cmds >= MAX_NB_CMDS) {
            return -1;
        }
        int consumed = parse_compound_command(
            &tokens[cursor], nb_tokens - cursor, &command->cmds[command->nb_cmds]);
        if (consumed == -1) {
            return -1;
        }
        command->nb_cmds++;
        cursor += consumed;

        if (nb_tokens - cursor == 0) {
            break;
        }

        if (strcmp(tokens[cursor], ";") == 0) {
            if (parallel) {
                return -1;
            }
            sequential = true;
            cursor++;
        } else if (strcmp(tokens[cursor], "&") == 0) {
            if (sequential) {
                return -1;
            }
            parallel = true;
            cursor++;
        } else {
            return -1;
        }
    }

    command->in_parallel = parallel;
    return nb_tokens;
}

/**
 * Parses tokens into a CompoundCommand.
 * @param tokens Array of tokenized inputs.
 * @param nb_tokens Number of tokens.
 * @param command Pointer to CompoundCommand to store parsed command.
 * @return Number of tokens consumed, or -1 on error.
 */
int parse_compound_command(
    char tokens[MAX_NB_TOKENS][MAX_TOKEN_LEN],
    int nb_tokens,
    CompoundCommand* command)
{
    int consumed = parse_simple_command(
        tokens, nb_tokens, &command->cmd1);
    if (consumed == -1) {
        return -1;
    }

    if (strcmp(tokens[consumed], "&&") == 0) {
        command->op = OPERATOR_AND;
        consumed++;
    } else if (strcmp(tokens[consumed], "||") == 0) {
        command->op = OPERATOR_OR;
        consumed++;
    } else if(strcmp(tokens[consumed],"|")== 0){
        command->op = OPERATOR_PIPE;
        consumed++;
    } else {
        command->op = OPERATOR_NONE;
        return consumed;
    }

    int consumed_cmd2 = parse_simple_command(
        &tokens[consumed], nb_tokens - consumed, &command->cmd2);
    if (consumed_cmd2 == -1) {
        return -1;
    }
    return consumed + consumed_cmd2;
}

/**
 * Parses tokens into a SimpleCommand.
 * @param tokens Array of tokenized inputs.
 * @param nb_tokens Number of tokens.
 * @param command Pointer to SimpleCommand to store parsed command.
 * @return Number of tokens consumed, or -1 on error.
 */
int parse_simple_command(
    char tokens[MAX_NB_TOKENS][MAX_TOKEN_LEN],
    int nb_tokens,
    SimpleCommand* command)
{
    if (nb_tokens < 1 || 
        strcmp(tokens[0], "&&") == 0 || 
        strcmp(tokens[0], "||") == 0 || 
        strcmp(tokens[0], "|") == 0 || 
        strcmp(tokens[0], ";") == 0 || 
        strcmp(tokens[0], "&") == 0) {
        return -1;
    }

    strcpy(command->program_name, tokens[0]);
    command->nb_args = 0;
    command->output_file[0] = '\0';

    for (int i = 1; i < nb_tokens; ++i) {
        if (strcmp(tokens[i], ">") == 0) {
            if (i + 1 < nb_tokens) {
                strcpy(command->output_file, tokens[i + 1]);
                command->nb_args = i - 1;
                return i + 2;
            } else {
                fprintf(stderr, "Error: No output file specified for redirection\n");
                return -1;
            }
        }

        if (command->nb_args < MAX_NB_ARGS) {
            strcpy(command->args[command->nb_args], tokens[i]);
            command->nb_args++;
        } else {
            fprintf(stderr, "Error: Too many arguments\n");
            return -1;
        }
    }

    return nb_tokens;
}
