#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MEDICINE_CAPACITY 6
#define HOSPITAL_CAPACITY 3

char *curtime(void) {
  char formatted[23];

  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);

  strftime(formatted, sizeof(formatted) - 2, "%F %T", localtime(&now.tv_sec));

  char *res;
  if (asprintf(&res, "%s.%d", formatted, (int) now.tv_nsec / 10000000) == -1) {
    res = NULL;
  }

  return res;
}

/* Log a message with the current time prepended.
 * Warning: IO function - may end the current process in case of an allocation error */
void log_msg(char* msg) {
  char *t = curtime();

  if (msg == NULL || t == NULL) {
    fprintf(stderr, "Malloc error.\n");
    exit(EXIT_FAILURE);
  }

  printf("%s - %s", t, msg);

  free(t);
}

int queue[HOSPITAL_CAPACITY], patients_waiting = 0;
int medicine = 0, pharmaceutists_waiting = 0;
pthread_mutex_t hospital_mutex = PTHREAD_MUTEX_INITIALIZER;
// May be waited on only by the doctor (singular) or pharmaceutists
pthread_cond_t hospital_cond = PTHREAD_COND_INITIALIZER;

void *doctor(void *patient_num) {
  int n = *(int*) patient_num;
  char *msg;

  pthread_mutex_lock(&hospital_mutex);
  while (n > 0) {
    log_msg("Lekarz: budzę się");

    if (medicine >= patients_waiting && (patients_waiting == 3 || patients_waiting == n)) {
      asprintf(&msg, "Lekarz: konsultuję pacjentów %d, %d, %d", queue[0], queue[1], queue[2]);
      log_msg(msg);
      free(msg);

      sleep(2 + rand() % 3);

      n -= patients_waiting;
      medicine -= patients_waiting;
      patients_waiting = 0;
    } else if (pharmaceutists_waiting > 0 && medicine < MEDICINE_CAPACITY) {
      log_msg("Lekarz: przyjmuję dostawę leków");

      sleep(1 + rand() % 3);

      medicine = MEDICINE_CAPACITY;
      pharmaceutists_waiting--;
      // We are the only doctor, so this will awaken a pharmaceutist
      pthread_cond_signal(&hospital_cond);
    }

    log_msg("Lekarz: zasypiam");
    pthread_cond_wait(&hospital_cond, &hospital_mutex);
  }
  pthread_mutex_unlock(&hospital_mutex);

  pthread_exit(NULL); // Rest in peace
}

int main(void) {
  printf("%s", curtime());
  return EXIT_SUCCESS;
}

