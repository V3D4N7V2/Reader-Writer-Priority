

#include "generalStuff.h"

sem_t reading;
sem_t writing;
sem_t updateReaderCountSemaphore, updateWritersCountSemaphore, z;
int activeReaders = 0;
int activeWriters = 0;
int rwvariable = 0;

void setDefaultSemaphores() {
  sem_init(&reading, 0, 1);
  sem_init(&writing, 0, 1);
  sem_init(&updateReaderCountSemaphore, 0, 1);
  sem_init(&updateWritersCountSemaphore, 0, 1);
  sem_init(&z, 0, 1);
}

void *writer() {

  sleep(getRandom());
  sem_wait(&updateWritersCountSemaphore);
  activeWriters++;
  if (activeWriters == 1)
    sem_wait(&reading);
  sem_post(&updateWritersCountSemaphore);
  sem_wait(&writing);

  int tmp = readfromfile();

  rwvariable = rwvariable + 2;
  tmp = rwvariable;
  writetofile(tmp);
  printf("New Value is : %d\n", rwvariable);
  sem_post(&writing);
  sem_wait(&updateWritersCountSemaphore);
  activeWriters--;
  if (activeWriters == 0)
    sem_post(&reading);
  sem_post(&updateWritersCountSemaphore);
  return 0;
}

void *reader() {

  sleep(getRandom());
  sem_wait(&z);

  sem_wait(&reading);
  sem_wait(&updateReaderCountSemaphore);
  activeReaders++;
  if (activeReaders == 1)
    sem_wait(&writing);
  sem_post(&updateReaderCountSemaphore);
  sem_post(&reading);
  sem_post(&z);
  printf("Read value : %d\n", rwvariable);
  int tmp = readfromfile();
  printf("Read value from file : %d\n", rwvariable);
  sem_wait(&updateReaderCountSemaphore);
  activeReaders--;
  if (activeReaders == 0)
    sem_post(&writing);
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