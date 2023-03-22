#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    pid_t pid = fork();
    
    if(pid > 0) {
        sleep(5);
    } 

    else {
        exit(0);
    }

    return 0;
    
}
