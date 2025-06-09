#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

double given_function(double x) {
  return 4 / (x*x + 1);
}

void calc_and_report(double start, double h, int steps, int pipe) {
  assert(0 <= start);
  assert(0 < h && h <= 1);
  assert(0 <= steps);

  double total = 0;
  for (int i = 0; i < steps; i++) {
    double x = start + i*h + h/2;
    if (x > 1) break;
    total += given_function(x);
  }
  total *= h;
  
  write(pipe, &total, sizeof(double));
  close(pipe);
}

double delegate_calculation(int k, double h) {
  assert(k > 0);
  assert(h > 0 && h <= 1);

  // Array to store pipes to read from
  int* pipes = malloc(k * sizeof(int));
  if (pipes == NULL) {
    fprintf(stderr, "Allocation error.\n");
    exit(EXIT_FAILURE);
  }

  // Calculating the number of steps a subprocess should do
  int total_steps = ceil(1 / h);
  int subpr_steps = (total_steps > k) ? ceil((float) total_steps / k) : 1;

  // Delegating calculations to the subprocesses
  for (int i = 0; i < k; i++) {
    int fd[2];
    if (pipe(fd) != 0) {
      fprintf(stderr, "Error opening pipe\n");
      exit(EXIT_FAILURE);
    }

    if (fork() == 0) {
      close(fd[0]);
      calc_and_report(i * subpr_steps * h, h, subpr_steps, fd[1]);
      exit(EXIT_SUCCESS);
    } else {
      close(fd[1]);
      pipes[i] = fd[0];
    }
  }

  // Reading the values from subprocessess
  double total = 0;
  for (int i = 0; i < k; i++) {
    double val;
    read(pipes[i], &val, sizeof(double));

    total += val;

    close(pipes[i]);
  }

  free(pipes);
  return total;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Error: this program accepts exactly 2 arguments.\n");
    return EXIT_FAILURE;
  }

  double h = atof(argv[1]);
  if (h <= 0 || h > 1) {
    fprintf(stderr, "Error: precision must be between 0 and 1.\n");
    return EXIT_FAILURE;
  }

  int n = atoi(argv[2]);
  if (n <= 0) {
    fprintf(stderr, "Error: number of processes must be at least 1.\n");
    return EXIT_FAILURE;
  }


  printf("k\tval\t\ttime\n");
  for (int k = 1; k <= n; k++) {
    struct timespec start, stop;
    if (clock_gettime(CLOCK_REALTIME, &start) != 0) {
      fprintf(stderr, "Error getting the time.\n");
      return EXIT_FAILURE;
    }

    double val = delegate_calculation(k, h);

    if (clock_gettime(CLOCK_REALTIME, &stop) != 0) {
      fprintf(stderr, "Error getting the time.\n");
      return EXIT_FAILURE;
    }
    double diff = (double)(stop.tv_sec - start.tv_sec) 
      + ((double)(stop.tv_nsec - start.tv_nsec)/1000000000L);

    printf("%d\t%f\t%f\n", k, val, diff);
  }

  return EXIT_SUCCESS;
}

