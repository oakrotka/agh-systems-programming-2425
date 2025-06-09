#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Math stuff */
#define START 0
#define STOP  1

double given_function(double x) {
  return 4 / (x*x + 1);
}

double integrate_part(double pstart, double h, int steps) {
  assert(START <= pstart); // may be higher than STOP
  assert(START < h && h <= STOP);
  assert(0 <= steps);

  double total = 0;
  for (int i = 0; i < steps; i++) {
    double x = pstart + i*h + h/2;
    if (x > STOP) break;
    total += given_function(x);
  }

  return total * h;
}

/* Process management stuff */
double area_delta;
int k;

double *results;

void *exec_nth_thread(void *thread_number) {
  int n = *(int*) thread_number;

  int steps = ceil((double) 1 / (area_delta * k));
  if (steps == 0) steps = 1;

  double thread_start = START + n * area_delta * steps;

  assert(results != NULL);
  results[n] = integrate_part(thread_start, area_delta, steps);

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  /* Argument handling */
  if (argc != 3) {
    fprintf(stderr, "Error: this program accepts exactly 2 arguments.\n");
    return EXIT_FAILURE;
  }

  area_delta = atof(argv[1]);
  if (area_delta <= START || area_delta > STOP) {
    fprintf(stderr, "Error: precision must be between %d and %d.\n", START, STOP);
    return EXIT_FAILURE;
  }

  k = atoi(argv[2]);
  if (k <= 0) {
    fprintf(stderr, "Error: number of threads must be at least 1.\n");
    return EXIT_FAILURE;
  }

  /* Allocations */
  results = malloc(k * sizeof(double));
  pthread_t *threads = malloc(k *  sizeof(threads));
  int *range = malloc(k * sizeof(int));
  if (results == NULL || threads == NULL || range == NULL) {
    fprintf(stderr, "Malloc error.\n");
    return EXIT_FAILURE;
  }

  /* Thread creation */
  for (int i = 0; i < k; i++) {
    range[i] = i;
    if (pthread_create(&threads[i], NULL, &exec_nth_thread, &range[i]) != 0) {
      fprintf(stderr, "Error creating a thread, aborting...\n");
      return EXIT_FAILURE;
    }
  }

  /* Collecting the values */
  double res = 0;
  for (int i = 0; i < k; i++) {
    pthread_join(threads[i], NULL);
    res += results[i];
  }

  printf("%f\n", res);

  free(results);
  free(threads);
  free(range);
  return EXIT_SUCCESS;
}
