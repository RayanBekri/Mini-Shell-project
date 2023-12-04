#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "parse.h"
#include "launch.h"
#include "interactive.h"
#include "batch.h"

int main(int argc, char* argv[]) {
    if (argc == 1) {
        return interactive_mode();
    }

    for (int i = 1; i < argc; ++i) {
        printf("Executing batch file: %s\n", argv[i]);
        if (batch_mode(argv[i]) == -1) {
            fprintf(stderr, "Error in executing batch file: %s\n", argv[i]);
        }
    }

    return 0;
}
