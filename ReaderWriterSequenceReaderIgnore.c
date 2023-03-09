

#include "generalStuff.h"

int rwvariable = 0;
sem_t semUpdatingQueue;

void setDefaultSemaphores() {
  sem_init(&semUpdatingQueue, 0, 1);
  return;
}

struct thirdReaderWriter {
  sem_t access;
  struct thirdReaderWriter *next;
  int isReader;
};

struct thirdReaderWriter *head = NULL;

struct thirdReaderWriter *joinQueue(struct thirdReaderWriter *newNode) {
  struct thirdReaderWriter *temp = head;
  printf("???joining %x\n", newNode);
  printf("???head is %x\n", head);
  newNode->next = NULL;
  if (head == NULL) {
    head = newNode;
    return NULL;
  }
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = newNode;
  return temp;
}

void leaveQueue() {
  struct thirdReaderWriter *temp = head;
  head = head->next;
  printf("###deQueing %x\n", temp);
  printf("###new head is %x\n", head);
}

void *reader() {
  sleep(getRandom());
  printf("\tReader is waiting\n");
  sem_wait(&semUpdatingQueue);
  printf("\tReader is 2\n");
  struct thirdReaderWriter *newNode =
      (struct thirdReaderWriter *)malloc(sizeof(struct thirdReaderWriter));
  printf("\tReader is 3\n");
  sem_init(&newNode->access, 0, 1);
  printf("\tReader is 4\n");
  newNode->isReader = 1;
  struct thirdReaderWriter *last = joinQueue(newNode);
  printf("\tReader is 5\n");
  // printf("last is %d\n", last->isReader);
  // printf("newNode is %d\n", newNode->isReader);
  // printf("head is %d\n", head->isReader);
  // printf("next is %x\n", newNode->next);
  // printf("self is %x\n", last->next);
  // printf("last is %x\n", last);
  sem_wait(&newNode->access);
  sem_post(&semUpdatingQueue);
  printf("\tReader is in queue\n");

  if (last != NULL) { // queing after last writer
    if (last->isReader == 0) {
      printf("\tReader is waiting for last writer\n");
      sem_wait(&last->access);
      sem_post(&last->access);
      // free(last);
    }
  }
  printf("\tReader Read %d\n", rwvariable);

  if (last != NULL) { // waiting dequeing after last if reader
    if (last->isReader == 1) {
      printf("\tReader is waiting for last reader to leave\n");
      sem_wait(&last->access);
      sem_post(&last->access);
    }
  }
  free(last);

  sem_wait(&semUpdatingQueue);
  sem_post(&newNode->access);
  leaveQueue(newNode);
  sem_post(&semUpdatingQueue);
  printf("\tReader is done\n");
}

void *writer() {
  sleep(getRandom());
  printf("Writer is waiting\n");
  sem_wait(&semUpdatingQueue);
  struct thirdReaderWriter *newNode =
      (struct thirdReaderWriter *)malloc(sizeof(struct thirdReaderWriter));
  sem_init(&newNode->access, 0, 1);
  newNode->isReader = 0;
  struct thirdReaderWriter *last = joinQueue(newNode);
  sem_wait(&newNode->access); // hold access to self
  sem_post(&semUpdatingQueue);

  if (last != NULL) {
    sem_wait(&last->access); // wait for last to finish
    sem_post(&last->access);
    free(last);
  }

  rwvariable = rwvariable + 2;
  printf("Writer wrote %d\n", rwvariable);
  sem_wait(&semUpdatingQueue);
  sem_post(&newNode->access);
  leaveQueue(newNode);
  sem_post(&semUpdatingQueue);
  printf("Writer is done\n");
}

void *countNodes() {
  return NULL;
  struct thirdReaderWriter *temp = head;
  while (1) {
    sleep(1);
    int count = 0;
    while (temp != NULL) {
      count++;
      temp = temp->next;
    }
    printf("Count is %d\n", count);
  }
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
  pthread_t t;
  resultCode = pthread_create(&t, NULL, countNodes, NULL);
  assert(!resultCode);
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
  resultCode = pthread_join(t, NULL);
  printf("done\n");
  return 0;
}