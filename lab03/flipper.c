#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#define BUF_SIZE 1024

// Reverses a string in place given its starting and ending point
void revstr(char* start, char* end) {
  if (start >= end) return;
  do {
    char temp = *start;
    *start = *end;
    *end = temp;
  } while(++start < --end);
}

int cpyrev(FILE* from, FILE* to) {
  char* buf = malloc(BUF_SIZE * sizeof(char));
  size_t offset = 0, allocated = BUF_SIZE;
  if (buf == NULL) return -1;

  size_t n = fread(buf + offset, sizeof(char), allocated - offset - 1, from);
  while (n > 0) {
    buf[offset + n] = '\0'; // Terminating read string
    char* newline = strchr(buf + offset, '\n'); // Searching for a newline

    // Load more characters in order to get a newline if we haven't already
    if (newline == NULL) {
      // Check if we need to allocate more space
      if (offset + 2 * n >= allocated) {
        allocated *= 2;
        buf = realloc(buf, allocated);
        if (buf == NULL) return -1;
      }

      offset += n;
      n = fread(buf + offset, sizeof(char), allocated - offset - 1, from);
      continue;
    }

    char* last_point = buf;

    // Reverse lines in the buffer
    while (newline != NULL) {
      revstr(last_point, newline - 1);
      last_point = newline + 1;
      newline = strchr(last_point, '\n');
    }

    // Write reversed lines to the new file
    size_t to_write = last_point - buf;
    if (fwrite(buf, sizeof(char), to_write, to) == 0) {
      free(buf);
      return -2;
    }

    // Move unreversed lines at the end of the buffer to the front
    offset += n - to_write;
    if (offset > 0) memmove(buf, last_point, offset * sizeof(char));

    n = fread(buf + offset, sizeof(char), allocated - offset - 1, from);
  }

  // Reverse the rest of the buffer and write it
  if (offset > 0) {
    revstr(buf, buf + offset - 1);
    if (fwrite(buf, sizeof(char), offset, to) == 0) {
      free(buf);
      return -2;
    }
  }
  
  free(buf);
  return 0;
}

// Concatenate the directory name and the filename to a format readable by the filesystem
char* appenddir(const char* dir, const char* file) {
  size_t dirlen  = strlen(dir);
  size_t filelen = strlen(file);

  char* res = malloc((dirlen + filelen + 2) * sizeof(char));
  if (res == NULL) return NULL;

  if (asprintf(&res, "%s/%s", dir, file) == -1) {
    free(res);
    return NULL;
  }

  return res;
}


int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Error: Expected exactly 2 arguments.\n");
    return 1;
  }

  // Attempt to open the source directory
  DIR* fromdir = opendir(argv[1]);
  if (fromdir == NULL) {
    fprintf(stderr, "Error: Directory %s not found.\n", argv[1]);
    return 2;
  }

  // Check if the destination directory exists, if not then attempt to create it
  DIR* todir = opendir(argv[2]);
  if (todir == NULL) {
    fprintf(stderr, "Warning: Directory %s not found, attempting to create it...\n", argv[2]);

    if (mkdir(argv[2], 0777) != 0) {
      fprintf(stderr, "Error: Failed to create directory %s.\n", argv[2]);
      return 3;
    }
  } else {
    closedir(todir);
  }

  // Iterate over files in the source directory
  struct dirent* fromfile = readdir(fromdir);
  while (fromfile != NULL) {
    // Check if the found file is a text file
    char* filename = fromfile->d_name;
    size_t filename_len = strlen(filename);
    if (filename_len > 4 && strcmp(filename + filename_len - 4, ".txt") == 0) {
      // Generate the relative filenames of the files we will be working on
      char* fromname = appenddir(argv[1], filename);
      char* toname   = appenddir(argv[2], filename);

      if (fromname == NULL || toname == NULL) {
        fprintf(stderr, "Malloc error.\n");
        if (fromname != NULL) free(fromname);
        else if (toname != NULL) free(toname);
        return 4;
      }

      // I'm pretty sure this isn't really a file descriptor but I can't think of a better name
      FILE* fromfd = fopen(fromname, "r");
      if (fromfd == NULL) {
        fprintf(stderr, "Error: Failed to open file %s.\n", fromname);
        return 5;
      }

      FILE* tofd = fopen(toname, "w");
      if (tofd == NULL) {
        fprintf(stderr, "Error: Failed to open file %s.\n", toname);
        return 6;
      }

      int status = cpyrev(fromfd, tofd);
      if (status == -1) {
        fprintf(stderr, "Malloc error.\n");
        return 4;
      } else if (status == -2) {
        fprintf(stderr, "Error: Failed to write to file %s.\n", toname);
        return 7;
      }

      fclose(fromfd);
      fclose(tofd);
      free(fromname);
      free(toname);
    }

    fromfile = readdir(fromdir);
  }

  closedir(fromdir);
  return 0;
}

