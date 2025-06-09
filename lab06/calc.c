#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

double given_function(double x) {
  return 4 / (x*x + 1);
}

double integrate(double l, double r, double h) {
  assert(l < r);
  assert(h < r - l);

  int steps = ceil((r - l) / h);

  double total = 0;
  for (int i = 0; i < steps; i++) {
    double x = l + i*h + h/2;
    if (x > r) x = r;
    total += given_function(x);
  }
  return total * h;
}

int main(void) {
  double args[3];
  
  int reader = open("./input", O_RDONLY);
  if (reader < 0) {
    fprintf(stderr, "Error opening the pipe to read arguments.\n");
    return EXIT_FAILURE;
  }
  for (int n = 0; n < sizeof(args); n += read(reader, args+n, sizeof(args) - n));

  close(reader);

  double res = integrate(args[0], args[1], args[2]);

  int writer = open("./output", O_WRONLY);
  if (reader < 0) {
    fprintf(stderr, "Error opening the pipe to pass the result.\n");
    return EXIT_FAILURE;
  }
  for (int n = 0; n < sizeof(double); n += write(reader, (&res)+n, sizeof(double) - n));

  close(writer);

  return EXIT_SUCCESS;
}
