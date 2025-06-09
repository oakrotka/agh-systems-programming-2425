#include <stdio.h>
#include <stdlib.h>

#ifndef DYNAMIC
#include "collatz.h"
#else
#include <dlfcn.h>
#endif

int main(int argc, char *argv[]) {
  #ifdef DYNAMIC
  void *handle = dlopen("libcollatz.so", RTLD_LAZY);
  if (!handle) {
    fprintf(stderr, "Error opening library libcollatz.so\n");
    return 2;
  }

  int (*test_collatz_convergence)(int,int,int*);
  test_collatz_convergence = (int (*)(int,int,int*)) dlsym(handle, "test_collatz_convergence");

  if (dlerror() != NULL) {
    fprintf(stderr, "Error loading function test_collatz_convergence\n");
    return 3;
  }
  #endif

  if (argc != 3) {
    fprintf(stderr, "Expected exactly 2 arguments!\n");
    return 1;
  }

  int start = atoi(argv[1]);
  int iters = atoi(argv[2]);
  printf("start %d iters %d\n", start, iters);

  int* res = malloc(sizeof(int[iters]));

  int n = test_collatz_convergence(start, iters, res);

  if (start == 1 || n) {
    printf("%d ", start);
    for (int i = 0; i < n; i++) {
      printf("%d ", res[i]);
    }
    printf("\n");
  } else {
    printf("Niepowodzenie :(\n");
  }

  free(res);

  #ifdef DYNAMIC
  dlclose(handle);
  #endif

  return 0;
}

