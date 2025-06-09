#include "collatz.h"

int collatz_conjecture(int input) {
  if (input % 2) {
    return 3 * input + 1;
  } else {
    return input / 2;
  }
}

int test_collatz_convergence(int input, int max_iter, int *steps) {
  for (int i = 0; i < max_iter; i++) {
    input = collatz_conjecture(input);
    steps[i] = input;
    if (input == 1)
      return i + 1;
  }
  return 0;
}
