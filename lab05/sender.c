#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void forced_shutdown(int signo) {
  static const char msg[] = "Warning: Program did not exit the intended way.\n";
  write(2, msg, sizeof(msg) - sizeof(char));
  _exit(2);
}

void intended_shutdown(int signo) {
  assert(signo == SIGUSR1);
  _exit(0);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Error: Expected exactly 2 arguments.\n");
    printf("%d\n");
    return 1;
  }

  pid_t catcher_pid = atoi(argv[1]);
  if (catcher_pid <= 0) {
    fprintf(stderr, "Error: Incorrect PID given.\n");
    return 1;
  }

  int mode = atoi(argv[2]);
  if (mode < 1 || 5 < mode) {
    fprintf(stderr, "Error: Incorrect mode given.\n");
    return 1;
  }

  sigset_t sigusr1_set;
  sigfillset(&sigusr1_set);
  sigdelset(&sigusr1_set, SIGUSR1);
  signal(SIGUSR1, &intended_shutdown);

  /* For debugging purposes */
  sigdelset(&sigusr1_set, SIGINT); 
  signal(SIGINT, &forced_shutdown);

  union sigval wrapped_mode = {.sival_int = mode};
  sigqueue(catcher_pid, SIGUSR1, wrapped_mode);

  sigsuspend(&sigusr1_set);
  return -1; // this point should probably never be reached
}
