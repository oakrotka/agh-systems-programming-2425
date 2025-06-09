#include <fcntl.h>
#include <linux/limits.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

#define CLIENT_AMNT 16

static const char qname[] = "sq";

int main(int argc, char *argv[]) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    fprintf(stderr, "Error: couldn't read the current working directory.\n");
    return EXIT_FAILURE;
  }
  
  int cwd_len = strchr(cwd, '\0') - cwd + 1;
  int q_len = cwd_len + sizeof(qname) + 2;
  if (q_len > PATH_MAX) {
    fprintf(stderr, "Error: couldn't create a queue.\n");
    return EXIT_FAILURE;
  }

  char qpath[q_len];
  if (sprintf(qpath, "%s/%s", cwd, qname) < 0) {
    fprintf(stderr, "Malloc error\n");
    return EXIT_FAILURE;
  }

  mqd_t q = mq_open(qpath, O_RDWR);
  if (q < 0) {
    fprintf(stderr, "Error: couldn't create a queue.\n");
    return EXIT_FAILURE;
  }

  struct mq_attr attr;
  if (mq_getattr(q, &attr) < 0) {
    fprintf(stderr, "Error: couldn't read queue attributes.\n");
    return EXIT_FAILURE;
  }
  attr.mq_msgsize = MAX_MSG_LEN;
  if (mq_setattr(q, &attr, NULL) < 0) {
    fprintf(stderr, "Error: couldn't set queue attributes.\n");
    return EXIT_FAILURE;
  }

  char buf[MAX_MSG_LEN];
  for (;;) {
    ssize_t n = mq_receive(q, buf, MAX_MSG_LEN, NULL);
    if (n < 0) {
      fprintf(stderr, "Error: couldn't receive a message from the queue.\n");
      return EXIT_FAILURE;
    }
    

  }

  return EXIT_SUCCESS;
}

