#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

const struct timespec one_second = {.tv_sec = 1};

unsigned long long signal_count = 0;
bool timer_enabled = false;

void sigint_handler(int signo) {
  assert(signo == SIGINT);
  static const char msg[] = "CTRL+C pressed.\n";
  write(1, msg, sizeof(msg) - sizeof(char));
}

void safe_sigcount_print() {
  static const char init_msg[] = "Received ";
  static const char end_msg[] = " SIGUSR1 signals so far.\n";

  char num_str[32];
  int i = 31;
  for (unsigned long long n = signal_count; n > 0; n /= 10) {
    if (i < 0) exit(2);
    num_str[i--] = '0' + n % 10;
  }
  if (signal_count == 0) num_str[i--] = '0';

  write(1, init_msg, sizeof(init_msg) - sizeof(char));
  write(1, num_str + i + 1, (31 - i) * sizeof(char));
  write(1, end_msg, sizeof(end_msg) - sizeof(char));
}

void sigusr1_handler(int signo, siginfo_t *info, void *_extra) {
  assert(signo == SIGUSR1);

  ++signal_count;
  timer_enabled = false;
  kill(info->si_pid, SIGUSR1);

  switch (info->si_value.sival_int) {
    case 1:
      safe_sigcount_print();
      break;

    case 2:
      timer_enabled = true;
      break;

    case 3:
      signal(SIGINT, SIG_IGN);
      break;

    case 4:
      signal(SIGINT, &sigint_handler);
      break;

    case 5:
      exit(0);

    default:
      exit(1);
  }
}

int main(void) {
  printf("%d\n", getpid());
  
  struct sigaction action = {
    .sa_flags = SA_SIGINFO,
    .sa_sigaction = &sigusr1_handler 
  };
  sigaction(SIGUSR1, &action, NULL);
  
  for (;;) {
    if (timer_enabled) {
      for (int i = 0; timer_enabled; ++i) {
        printf("%d\n", i);

        if (nanosleep(&one_second, NULL) == -1) {
          break; // reset the timer if it has been interrupted
        }
      }
    } else {
      pause();
    }
  }
}
