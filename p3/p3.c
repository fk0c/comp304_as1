#define _POSIX_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "You must enter: %s <number> <n>\n", argv[0]);
        exit(1);
    }

    int x = atoi(argv[1]);
    int pnum = atoi(argv[2]);
    if (x < 1 || x > 1000) {
        fprintf(stderr, "number must be between 1 and 1000\n");
        exit(1);
    }

    //sequence
    int numbers[1000];
    int num_count = 0;
    char input[10];
    while (fgets(input, sizeof(input), stdin) != NULL && num_count < 1000) {
        int num = atoi(input);
        numbers[num_count++] = num;
    }

    int segment_size = num_count / pnum;
    int remaining_size = num_count % pnum;
    int i, j, start, end;

    for (i = 0; i < pnum; i++) {
        start = i * segment_size;
        end = start + segment_size;
        if (i == pnum - 1) {
            end += remaining_size;
        }
        int pid = fork();
        if (pid == -1) {
            fprintf(stderr, "Failed to fork child process\n");
            exit(1);
        }
        if (pid == 0) {
            //child process
            int found = 0;
            for (j = start; j < end; j++) {
                if (numbers[j] == x) {
                    found = 1;
                    printf("Found at index %d\n", j);
                    exit(0);
                }
            }
            if (!found) {
                exit(-1);
            }
        }
    }

    int status;
    for (i = 0; i < pnum; i++) {
        wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            //if a child has found x, kill all other children and exit
            j = 0;
            while (j < pnum) {
                kill(0, SIGTERM);
                j++;
            }
            exit(0);
        }
    }
    
    //if none of the children found the number
    printf("Number not found\n");
    return 0;
}
