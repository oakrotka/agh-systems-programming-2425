#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Error: Expected only one argument.\n");
    return 1;
  }

  int n = atoi(argv[1]);

  for (int i = 0; i < n; i++) {
    if (fork() == 0) {
      printf("child %d: parent is %d\n", getpid(), getppid());
      return 0;
    }
  }

  for (int i = 0; i < n; i++) wait(NULL);

  printf("parent %d finished executing %d subprocesses\n", getpid(), n);

  return 0;
}

