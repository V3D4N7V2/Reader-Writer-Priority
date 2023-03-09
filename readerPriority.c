#include "generalStuff.h"

sem_t reading;
sem_t writing;
sem_t updateReaderCountSemaphore;
int activeReaders = 0;
int activeWriters = 0;
int rwvariable = 0;

void setDefaultSemaphores() {
  sem_init(&reading, 0, 1);
  sem_init(&writing, 0, 1);
  sem_init(&updateReaderCountSemaphore, 0, 1);
}

void *writer() {
  sleep(getRandom());
  printf("\tWriter Waiting\n");
  sem_wait(&writing);
  printf("\tWriter Entered\n");
  int tmp = readfromfile();

  rwvariable = rwvariable + 2;
  tmp = rwvariable;
  writetofile(tmp);
  printf("\tNew Value is : %d\n", rwvariable);
  sem_post(&writing);
  printf("\tWriter Done\n");
  return 0;
}

void *reader() {
  sleep(getRandom());
  printf("Reader Started\n");
  sem_wait(&updateReaderCountSemaphore);
  activeReaders++;
  if (activeReaders == 1)
    sem_wait(&writing);
  printf("Reader Entered, Active Readers : %d\n", activeReaders);
  sem_post(&updateReaderCountSemaphore);
  printf("Read value : %d\n", rwvariable);
  int tmp = readfromfile();
  printf("Read value from file : %d\n", rwvariable);
  sem_wait(&updateReaderCountSemaphore);
  activeReaders--;
  if (activeReaders == 0)
    sem_post(&writing);
  printf("%d Readers Left\n", activeReaders);
  printf("Reader Exiting\n");
  sem_post(&updateReaderCountSemaphore);
  return 0;
}

int main() {
  rwvariable = rand() % 100;
  int i = 0;
  pthread_t threads[totalThreads];
  int threadArgs[totalThreads];
  int resultCode;
  setDefaultSemaphores();

  int readerThreadsCount = 7;
  printf("%d\n", readerThreadsCount);
  for (i = 0; i < totalThreads; i++) {

    if (!(i % 3 == 0)) {
      resultCode = pthread_create(&threads[i], NULL, reader, &threadArgs[i]);
    } else {
      resultCode = pthread_create(&threads[i], NULL, writer, &threadArgs[i]);
    }
    assert(!resultCode);
  }
  printf("created Threads, now waiting for exit\n");

  for (i = 0; i < totalThreads; i++) {
    resultCode = pthread_join(threads[i], NULL);
    assert(!resultCode);
  }
  printf("done\n");
  return 0;
}