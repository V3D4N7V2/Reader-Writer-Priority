#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#define totalThreads 20

float getRandom() {
  float x = (float)rand() / (float)(RAND_MAX / 2);

  return x;
}

int getReaderThreadsCount() { return totalThreads - 1; }

int readfromfile() {
  int num;
  FILE *fptr;
  if ((fptr = fopen("file.txt", "r+")) == NULL) {
    printf("Error! opening file");
    exit(1);
  }
  fscanf(fptr, "%d", &num);

  fclose(fptr);
  return num;
}

int writetofile(int toFile) {
  int num = toFile;
  FILE *fptr;
  fptr = fopen("file.txt", "w+");
  if (fptr == NULL) {
    printf("Error!");
    exit(1);
  }
  fprintf(fptr, "%d", num);
  fclose(fptr);
  return 0;
}
