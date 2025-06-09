#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int global = 0;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Error: Expected only one argument.\n");
    return 1;
  }

  printf("hello, my name is %s\n", argv[0]);

  int local = 10;

  int child_pid = fork();
  if (child_pid == 0) {
    printf("child process\n");

    local += 1;
    global += 1;

    printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
    printf("child's local = %d, child's global = %d\n", local, global);

    return execl("/bin/ls", "ls", argv[1], NULL);
  }

  printf("parent process\n");
  printf("parent pid = %d, child pid = %d\n", getpid(), child_pid);

  int child_exit_status;
  wait(&child_exit_status);
  printf("Child exit code: %d\n", child_exit_status);

  printf("Parent's local = %d, parent's global = %d\n", local, global);

  return (child_exit_status == 0) ? 0 : 2;
}
