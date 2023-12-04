#include "launch.h"
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

/**
 * Executes a sequence of commands.
 * @param command_sequence Pointer to a series of commands.
 * @return Status of the command execution.
 */
int launch_command(CommandSequence* command_sequence) {
    int status = 0;

    for (int i = 0; i < command_sequence->nb_cmds; i++) {
        status = launch_compound_command(&command_sequence->cmds[i]);

        if (!command_sequence->in_parallel && status != 0) {
            break;
        }
    }

    return status;
}

/**
 * Executes a compound command based on the operator.
 * @param command Pointer to a compound command.
 * @return Status of the command execution.
 */
int launch_compound_command(CompoundCommand* command){
    int error1;
    int fd[2];
    if(command->op==OPERATOR_PIPE){
        pipe(fd);
        error1=launch_simple_command(&command->cmd1,-1,fd[1]);
        close(fd[1]);
    }else{
        error1=launch_simple_command(&command->cmd1,-1,-1);
    }

    if(command->op==OPERATOR_NONE){
        return error1;
    }
    else if(command->op==OPERATOR_AND){
        if(error1==0){
            int error2=launch_simple_command(&command->cmd2,-1,-1);
            return error2;
        }else{
            return error1;
        }
    }else if(command->op==OPERATOR_OR){
        if(error1==0){
            return error1;
        }else{
            int error2=launch_simple_command(&command->cmd2,-1,-1);
            return error2;
        }
    }else if(command->op==OPERATOR_PIPE){
        int error2=launch_simple_command(&command->cmd2,fd[0],-1);
        close(fd[0]);
        return error2;
    }

    return -1;
}

/**
 * Executes a simple command, handling I/O redirection and piping.
 * @param command Command to execute.
 * @param read_pipe Pipe for reading input.
 * @param write_pipe Pipe for writing output.
 * @return Status of the command execution.
 */
int launch_simple_command(SimpleCommand* command, int read_pipe, int write_pipe) {
    pid_t pid = fork();

    if (pid == 0) { // Child process
        if (read_pipe != -1) {
            dup2(read_pipe, STDIN_FILENO);
            close(read_pipe);
        }

        if (write_pipe != -1) {
            dup2(write_pipe, STDOUT_FILENO);
            close(write_pipe);
        }

        if (command->output_file[0] != '\0') {
            int fd = open(command->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                fprintf(stderr, "Error opening file %s\n", command->output_file);
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        char* argv[MAX_NB_ARGS + 2];
        argv[0] = command->program_name;
        for (int i = 0; i < command->nb_args; i++) {
            argv[i + 1] = command->args[i];
        }
        argv[command->nb_args + 1] = NULL;

        execvp(command->program_name, argv);
        fprintf(stderr, "Error: Command not found - %s\n", command->program_name);
        exit(EXIT_FAILURE);
    }

    if (read_pipe != -1) {
        close(read_pipe);
    }
    if (write_pipe != -1) {
        close(write_pipe);
    }

    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status) ? -1 : 0;
    } else {
        return -1;
    }
}
