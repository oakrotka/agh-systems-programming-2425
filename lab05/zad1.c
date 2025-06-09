#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

void sigusr1_handler(int signum) {
  assert(signum == SIGUSR1);
  static const char msg[] = "Received signal SIGUSR1.\n";
  write(1, msg, sizeof(msg) - sizeof(char));
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Error: This program expects exactly one argument.\n");
    return 1;
  }

  bool sig_is_mask = false;

  if (strcmp(argv[1], "none") == 0) {
    /* do not change the reaction to the signal. nothing ever happens. */
  } else if (strcmp(argv[1], "ignore") == 0) {
    signal(SIGUSR1, SIG_IGN);
  } else if (strcmp(argv[1], "handler") == 0) {
    signal(SIGUSR1, &sigusr1_handler);
  } else if (strcmp(argv[1], "mask") == 0) {
    sig_is_mask = true;

    sigset_t newmask;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, &newmask, NULL) < 0) {
      fprintf(stderr, "Error: Masking the signal failed.\n");
      return 2;
    }
  } else {
    fprintf(stderr, "Error: Unknown option.\n");
    return 1;
  }

  raise(SIGUSR1);

  if (sig_is_mask) {
    sigset_t pendings;
    sigpending(&pendings);
    if (sigismember(&pendings, SIGUSR1)) {
      printf("SIGUSR1 is currently waiting.\n");
    } else {
      printf("SIGUSR1 is NOT currently waiting.\n");
    }
  }

  // printf("The program has reached its end normally.\n");
  return 0;
}
