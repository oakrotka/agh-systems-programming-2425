#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define QUEUE_LEN 10
#define TEXT_LEN  11

sem_t *printer_count, *user_count;
void unlink_and_exit(int _ignored) {
  // sem_close(printer_count);
  // sem_close(user_count);

  static const char exitmsg[] = "Received exit signal, quitting...\n";
  write(1, exitmsg, sizeof(exitmsg));

  _exit(0);
}

void spawn_user(int id) {
  if (fork() != 0) return;
  for (;;) {
    // TODO
  }
}

void spawn_printer(int id) {
  if (fork() != 0) return;
  for (;;) {
    // TODO
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) goto argerr;

  int n = atoi(argv[1]);
  int m = atoi(argv[2]);
  if (n == 0 || m == 0) {
  argerr:
    fprintf(stderr, "Error: The program expects 2 nonzero numbers as arguments.\n");
    return EXIT_FAILURE;
  }

  for (int i = 0; i < n; i++) spawn_user(i);
  for (int i = 0; i < m; i++) spawn_printer(i);

  if (signal(SIGINT, unlink_and_exit) == SIG_ERR) {
    fprintf(stderr, "Error linking exit function to sigint.\n");
    return EXIT_FAILURE;
  }

  for (;;);
}

