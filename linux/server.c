#include <pthread.h>
#include "cs537.h"
#include "request.h"

//
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

struct threadBuffer {
  int fileDescriptor;
};

int bufferSpotsUsed, maxThreads, maxBuffer;
pthread_cond_t requestAccepted = PTHREAD_COND_INITIALIZER;
pthread_cond_t requestProcessed = PTHREAD_COND_INITIALIZER;
pthread_mutex_t bufferLock = PTHREAD_MUTEX_INITIALIZER;

// debugging time
struct timespec startTime;

time_t getSeconds() {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  
  return (ts.tv_sec - startTime.tv_sec);
}

// CS537: Parse the new arguments too
void getargs(int *port, int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <port> <threads> <buffer>\n", argv[0]);
    exit(1);
  }

  *port = atoi(argv[1]);
  if (*port <= 0) {
    fprintf(stderr, "Port must be a positive integer!\n");
    exit(1);
  }

  maxThreads = atoi(argv[2]);
  if (maxThreads <= 0) {
    fprintf(stderr, "Threads must be positive integer!\n");
    exit(1);
  }

  maxBuffer = atoi(argv[3]);
  if (maxBuffer <= 0) {
    fprintf(stderr, "Buffers must be positive integer!\n");
    exit(1);
  }
}

void *workerPool(void *arg) {
  struct threadBuffer *bufferArray = (struct threadBuffer *)arg;
  // for (int i = 0; i < maxBuffer; i++)
  // {
  //   printf("buffer[%d] tid %ld fd %d\n", i, bufferArray[i].threadId,
  //   bufferArray[i].fileDescriptor);
  // }
  // long unsigned int threadId = pthread_self();
  // printf("worker %ld %ld: started\n", threadId, getSeconds());
  int foundFileDescriptor;
  
  while (1) {
    Pthread_mutex_lock(&bufferLock);

    // wait for buffer to not be empty
    while (bufferSpotsUsed == 0) {
      // printf("worker %ld %ld: waiting\n", threadId, getSeconds());
      Pthread_cond_wait(&requestAccepted, &bufferLock);
    }

    // find fd and start work
    foundFileDescriptor = 0;
    for (int i = 0; i < maxBuffer; i++) {
      if (/*bufferArray[i].threadId == 0 && */bufferArray[i].fileDescriptor != 0) {
        // bufferArray[i].threadId = threadId;
        foundFileDescriptor = bufferArray[i].fileDescriptor;
        bufferArray[i].fileDescriptor = 0;
        // printf("worker %ld %ld: buffer[%d] fd %d\n", threadId, getSeconds(), i, foundFileDescriptor);
        break;
      }
    }
    // printf("worker %ld %ld: signal main that buffer is empty\n", threadId, getSeconds());
    bufferSpotsUsed--;
    Pthread_cond_signal(&requestProcessed);
    Pthread_mutex_unlock(&bufferLock);

    if (foundFileDescriptor == 0) {
      continue;
    }
    // printf("worker %ld %ld: run requestHandler fd %d\n", threadId, getSeconds(),
          //  foundFileDescriptor);
    requestHandle(foundFileDescriptor);
    Close(foundFileDescriptor);
  }
}

int main(int argc, char *argv[]) {
  int listenfd, connfd, port, clientlen;
  struct sockaddr_in clientaddr;
  
  int numRequests = 0; // debug
  timespec_get(&startTime, TIME_UTC); // debug set start time

  getargs(&port, argc, argv);

  struct threadBuffer *bufferArray;
  bufferArray =
      (struct threadBuffer *)malloc(maxBuffer * sizeof(struct threadBuffer));

  if (bufferArray == NULL) {
    fprintf(stderr, "Could not allocate space for buffer\n");
    exit(1);
  }

  for (int i = 0; i < maxBuffer; i++) {
    // bufferArray[i].threadId = 0;
    bufferArray[i].fileDescriptor = 0;
  }

  bufferSpotsUsed = 0;

  // for (int i = 0; i < maxBuffer; i++)
  // {
  //   printf("main: buffer[%d] tid %ld fd %d\n", i, bufferArray[i].threadId,
  //   bufferArray[i].fileDescriptor);
  // }

  pthread_t pool[maxThreads];
  for (int i = 0; i < maxThreads; i++) {
    Pthread_create(&pool[i], NULL, workerPool, bufferArray);
  }

  listenfd = Open_listenfd(port);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);
    // printf("main %ld: connfd %d accepted\n", getSeconds(), connfd);

    // Main thread waits until buffer has empty spot
    Pthread_mutex_lock(&bufferLock);
    while (bufferSpotsUsed == maxBuffer) {
      // printf("main %ld: waiting for buffer to empty\n", getSeconds());
      Pthread_cond_wait(&requestProcessed, &bufferLock);  // TODO: wrapper
    }

    // Add new request to first open spot
    for (int i = 0; i < maxBuffer; i++) {
      if (bufferArray[i].fileDescriptor == 0) {
        numRequests++;
        // printf("main %ld: numRequests %d\n", getSeconds(), numRequests);
        bufferArray[i].fileDescriptor = connfd;
        // printf("main %ld: buffer[%d] fd %d\n", getSeconds(), i, bufferArray[i].fileDescriptor);
        break;
      }
    }

    // Tell pool threads that buffer has accepted request
    // printf("main %ld: signal worker connfd %d\n", getSeconds(), connfd);
    bufferSpotsUsed++;
    Pthread_cond_signal(&requestAccepted);
    Pthread_mutex_unlock(&bufferLock);
  }
}