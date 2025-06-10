#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
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


/* Log a message with the current time (and optionally person information) prepended.
 * Warning: IO functions - may end the current process in case of an allocation error */
void log_msg(char* msg) {
  char *t = curtime();

  if (msg == NULL || t == NULL) {
    fprintf(stderr, "Malloc error.\n");
    exit(EXIT_FAILURE);
  }

  printf("%s - %s\n", t, msg);

  free(t);
}

void log_msg_with_id(size_t id, const char* person_name, char* msg) {
  char* t = curtime();

  if (msg == NULL || t == NULL) { // person_name is assumed to be static
    fprintf(stderr, "Malloc error.\n");
    exit(EXIT_FAILURE);
  }

  printf("%s - %s(%zu): %s\n", t, person_name, id, msg);

  free(t);
}

int patients_left;

int queue[HOSPITAL_CAPACITY], patients_waiting = 0;
int medicine = MEDICINE_CAPACITY, pharmaceutists_waiting = 0;
bool delivering = false;
pthread_mutex_t hospital_mutex = PTHREAD_MUTEX_INITIALIZER;
// May be waited on only by the doctor (singular) or pharmaceutists
pthread_cond_t medicine_cond = PTHREAD_COND_INITIALIZER;
// May only be waited on by the patients currently in the queue
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;


// Note: Different threads are only created for pharmaceutists and patients,
// doctor runs on the main thread
void doctor() {
  char *msg;

  pthread_mutex_lock(&hospital_mutex);

  while (patients_left > 0) {
    // This was supposed to be the doctor's last message in the loop,
    // but it's convenient for it to be here
    log_msg("Lekarz: zasypiam");
    pthread_cond_wait(&medicine_cond, &hospital_mutex);
    log_msg("Lekarz: budzę się");

patients_check:
    if (medicine >= patients_waiting 
      && (patients_waiting == HOSPITAL_CAPACITY || patients_waiting == patients_left)) {
      // This can print garbage (uninitialized memory or ids of previously consulted patients)
      // but handling this properly would require a lot of code 
      // as for a condition that wasn't even considered by the author of this laboratory
      asprintf(&msg, "Lekarz: konsultuję pacjentów %d, %d, %d", queue[0], queue[1], queue[2]);
      log_msg(msg);
      free(msg);

      sleep(2 + rand() % 3);

      patients_left -= patients_waiting;
      medicine -= patients_waiting;
      patients_waiting = 0;

      pthread_cond_broadcast(&queue_cond);
    } else if (pharmaceutists_waiting > 0 && medicine < MEDICINE_CAPACITY) {
      log_msg("Lekarz: przyjmuję dostawę leków");

      sleep(1 + rand() % 3);
      medicine = MEDICINE_CAPACITY;

      int dbg = pharmaceutists_waiting--;
      // We are the only doctor, so this will awaken a pharmaceutist
      pthread_cond_broadcast(&medicine_cond);
      assert(pharmaceutists_waiting == dbg - 1);

      // This kind of goes against the specifiaction of the exercise 
      // but is required not to create a deadlock
      goto patients_check;
    } else {
      printf("fucked up %d farmów %d medycyny\n", pharmaceutists_waiting, medicine);
    }
  }

  pthread_mutex_unlock(&hospital_mutex);
}

void *pharmaceutist(void *pharm_id) {
  int id = *(int*) pharm_id;
  char *msg;

  for (;;) { // The grind never stops.
    int timeout = 5 + rand() % 11;

    asprintf(&msg, "ide do szpitala, bede za %d s", timeout);
    log_msg_with_id(id, "Farmaceuta", msg);
    free(msg);

    sleep(timeout);

    pthread_mutex_lock(&hospital_mutex);

    pharmaceutists_waiting++;
    if (medicine >= HOSPITAL_CAPACITY) {
      log_msg_with_id(id, "Farmaceuta", "czekam na oproznienie apteczki");

      // TODO: multiple pharmaceutists can deliver for some fucking reason
      do {
        pthread_cond_wait(&medicine_cond, &hospital_mutex);
      } while (medicine >= HOSPITAL_CAPACITY || delivering == true);
    }

    assert(medicine < HOSPITAL_CAPACITY);

    delivering = true;
    log_msg_with_id(id, "Farmaceuta", "dostarczam leki");

    log_msg_with_id(id, "Farmaceuta", "budzę lekarza");
    pthread_cond_broadcast(&medicine_cond);
    pthread_cond_wait(&medicine_cond, &hospital_mutex);
    delivering = false;

    log_msg_with_id(id, "Farmaceuta", "zakończyłom dostawę");
    printf("%d pacjentów %d leków %d farmaceutów\n",
           patients_waiting, medicine, pharmaceutists_waiting);
    pthread_mutex_unlock(&hospital_mutex);
  }
}

void *patient(void *patient_id) {
  int id = *(int*) patient_id;
  char* msg;

  int timeout = 2 + rand() % 4;

  asprintf(&msg, "ide do szpitala, bede za %d s", timeout);
  log_msg_with_id(id, "Pacjent", msg);
  free(msg);

  sleep(timeout);

  pthread_mutex_lock(&hospital_mutex);
  while (patients_waiting >= HOSPITAL_CAPACITY) {
    int timeout = 2 + rand() % 4;

    asprintf(&msg, "za dużo pacjentów, wracam później za %d s", timeout);
    log_msg_with_id(id, "Pacjent", msg);
    free(msg);

    pthread_mutex_unlock(&hospital_mutex);
    sleep(timeout);
    pthread_mutex_lock(&hospital_mutex);
  } 

  queue[patients_waiting++] = id;

  if (patients_waiting < HOSPITAL_CAPACITY && patients_waiting < patients_left) {
    asprintf(&msg, "czeka %d pacjentów na lekarza", patients_waiting);
    log_msg_with_id(id, "Pacjent", msg);
    free(msg);

    pthread_cond_wait(&queue_cond, &hospital_mutex);
  } else {
    log_msg_with_id(id, "Pacjent", "budzę lekarza");
    assert(patients_waiting == HOSPITAL_CAPACITY || patients_waiting == patients_left);

    pthread_cond_broadcast(&medicine_cond);
    // Why do I have to do this - shouldn't just broadcasting be enough
    // for the doctor to reclaim the mutex?
    pthread_cond_wait(&queue_cond, &hospital_mutex);
  }

  log_msg_with_id(id, "Pacjent", "kończę wizytę");
  pthread_mutex_unlock(&hospital_mutex);

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Błąd: Program oczekuje 2 argumentów\n");
    return EXIT_FAILURE;
  }

  srand(time(NULL));

  int patients_count = patients_left = atoi(argv[1]);
  int pharms_count  = atoi(argv[2]);
  if (patients_left < 0 || pharms_count <= 0) {
    fprintf(stderr, "Błąd: nieprawidłowe liczby pacjentów/farmaceutów wprowadzone\n");
    return EXIT_FAILURE;
  }

  // We need an array with static numbers to pass as ids to prevent race conditions
  size_t ids_len = patients_left > pharms_count ? patients_left : pharms_count;
  int *ids = malloc(ids_len * sizeof(int));

  // Patient's threads
  pthread_t *pathr = malloc(patients_left * sizeof(pthread_t));

  if (ids == NULL || pathr == NULL) {
    fprintf(stderr, "Malloc error\n");
    return EXIT_FAILURE;
  }
  for (int i = 0; i < ids_len; i++) ids[i] = i;

  pthread_t _ignored;
  for (int i = 0; i < pharms_count;  i++) pthread_create(&_ignored, NULL, pharmaceutist, &ids[i]);
  for (int i = 0; i < patients_count; i++) pthread_create(&pathr[i], NULL, patient, &ids[i]);

  doctor();

  for (int i = 0; i < patients_count; i++) pthread_join(pathr[i], NULL);

  free(ids);
  free(pathr);
  // Automatically kill the pharmaceutists' threads
  return EXIT_SUCCESS;
}

