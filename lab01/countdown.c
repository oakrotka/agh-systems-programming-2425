#include <stdio.h>
#include <unistd.h>

int main(void) {
  for (int i = 10; i > 0; i--) {
    printf("%d\n", i);
    sleep(1);
  }
  printf("0\n");
  return 0;
}

