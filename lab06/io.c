#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  double l, r, h;

  // I assume the input to be at most 4095 bytes long
  char buf[4096] = {0};
  int n = 0, read_status = 1;
  while (n < 4095 && read_status > 0) {
    read_status = read(0, buf + n, 4095 - n);
    n += read_status;
  }
  l = atof(buf);

  char* next = strchr(buf, ' ');
  if (*next == '\0') {
    fprintf(stderr, "Error: expected 3 arguments to be passed through stdin.\n");
    return EXIT_FAILURE;
  }
  r = atof(++next);

  next = strchr(next, ' ');
  if (*next == '\0') {
    fprintf(stderr, "Error: expected 3 arguments to be passed through stdin.\n");
    return EXIT_FAILURE;
  }
  h = atof(++next);

  double args[3] = {l, r, h};
  int writer = open("./input", O_WRONLY);
  if (writer < 0) {
    fprintf(stderr, "Error opening the pipe to pass arguments.\n");
    return EXIT_FAILURE;
  }

  // cursed
  for (int n = 0; n < sizeof(args); n += write(writer, args + n, sizeof(args) - n));

  close(writer);

  int reader = open("./output", O_RDONLY);
  if (reader < 0) {
    fprintf(stderr, "Error opening the pipe to read the result.\n");
    return EXIT_FAILURE;
  }

  double res;
  for (int n = 0; n < sizeof(double); n += read(reader, (&res)+n, sizeof(double) - n));

  close(reader);

  printf("The calculated value is %f\n", res);

  return EXIT_SUCCESS;
}
