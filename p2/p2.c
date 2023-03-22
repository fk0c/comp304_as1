#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "You must enter: %s <command> <arg1> <arg2> ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pnum = atoi(argv[1]);
    char *command = argv[2];

    struct timeval start_tv, end_tv;

    int fd[pnum][2];
    pid_t pid[pnum];

    //n child processes
    for(int i = 0; i < pnum; ++i){
        //create pipe
        if(pipe(fd[i]) == -1){
            fprintf(stderr, "Pipe failed");
            exit(EXIT_FAILURE);
        }
        
        pid[i] = fork();

        if(pid[i] < 0){
            fprintf(stderr, "Fork failed");
            exit(EXIT_FAILURE);
        }

        if(pid[i] == 0){
            //child process
            //close unused read end
            close(fd[i][READ_END]);

            //get current time
            gettimeofday(&start_tv, NULL);
            pid_t pid2 = fork();

            if (pid2 == -1) {
                fprintf(stderr, "Fork failed");
                exit(EXIT_FAILURE);
            }

            if (pid2 == 0){
                //child of child
                //suppress stdout and stderr
                freopen("/dev/null", "w", stdout);
                freopen("/dev/null", "w", stderr);
                //execute command
                char *args[argc - 2];
                for(int j = 0; j < argc - 2; ++j){
                    args[j] = argv[j + 3];
                }

                execvp(command, args);   
            }
            else {
                //child
                wait(NULL);
                gettimeofday(&end_tv, NULL);
                double time_passed = (end_tv.tv_sec-start_tv.tv_sec) + (end_tv.tv_usec-start_tv.tv_usec)/1000000.0;
                //write time elapsed to parent
                write(fd[i][WRITE_END], &time_passed, sizeof(double));
                close(fd[i][WRITE_END]);
                exit(EXIT_SUCCESS);
            }

        }

    }

    //parent process
    
    double min = 0.0;
    double max = 0.0;
    double sum = 0.0;
    
    //read time elapsed from child
    double time_passed;
    
    for(int i = 0; i < pnum; ++i){
        read(fd[i][READ_END], &time_passed, sizeof(double));
        printf("Child %d executed in %.2f millis\n", i+1, 1000*time_passed);
        sum += time_passed;
        if(time_passed < min || min == 0.0){
            min = time_passed;
        }
        if(time_passed > max){
            max = time_passed;
        }

        //close read end
        close(fd[i][READ_END]);

        wait(NULL);
    }
    
    printf("Max: %.2f millis\n", 1000*max);
    printf("Min: %.2f millis\n", 1000*min);
    printf("Avg: %.2f millis\n", 1000*sum/pnum);

    return 0;

}