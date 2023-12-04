#include "parse.h"

int launch_command(CommandSequence* command);

int launch_compound_command(CompoundCommand* command);

int launch_simple_command(SimpleCommand* command,int read_pipe,int write_pipe);
