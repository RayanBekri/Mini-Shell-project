#ifndef __H_PARSE__
#define __H_PARSE__

#include <stdbool.h>

#define MAX_NB_TOKENS 1000
#define MAX_TOKEN_LEN 50
#define MAX_NB_ARGS 10
#define MAX_NB_CMDS 10

struct SimpleCommand {
    char program_name[MAX_TOKEN_LEN];
    char args[MAX_NB_ARGS][MAX_TOKEN_LEN];
    int nb_args;
    char output_file[MAX_TOKEN_LEN];
};
typedef struct SimpleCommand SimpleCommand;

enum CommandOperator {
    OPERATOR_NONE,
    OPERATOR_AND,
    OPERATOR_OR,
    OPERATOR_PIPE
};
typedef enum CommandOperator CommandOperator;

struct CompoundCommand {
    SimpleCommand cmd1;
    SimpleCommand cmd2;
    CommandOperator op;
};
typedef struct CompoundCommand CompoundCommand;

struct CommandSequence {
    CompoundCommand cmds[MAX_NB_CMDS];
    int nb_cmds;
    bool in_parallel;
};
typedef struct CommandSequence CommandSequence;


int tokenize(const char* input, char tokens[MAX_NB_TOKENS][MAX_TOKEN_LEN]);

int parse(char tokens[MAX_NB_TOKENS][MAX_TOKEN_LEN], int nb_tokens, CommandSequence* command);

int parse_compound_command(char tokens[MAX_NB_TOKENS][MAX_TOKEN_LEN], int nb_tokens, CompoundCommand* command);

int parse_simple_command(char tokens[MAX_NB_TOKENS][MAX_TOKEN_LEN], int nb_tokens, SimpleCommand* command);

#endif
