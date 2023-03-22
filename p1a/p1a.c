#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  int level = 0;
  printf("Main Process ID: %d, level: %d\n", getpid(), level);
  int i;
  int n = atoi(argv[1]);
  for (i = 0; i < n; i++) {
    if(fork() == 0){
      level++;
      printf("Process ID: %d, Parent ID: %d, level: %d\n", getpid(), getppid(), level); 
    }
      wait(NULL);
  }
  return 0; 
}