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
  int foundFileDescriptor;

  while (1) {
    Pthread_mutex_lock(&bufferLock);

    // Wait for buffer to not be empty
    while (bufferSpotsUsed == 0) {
      Pthread_cond_wait(&requestAccepted, &bufferLock);
    }

    // Find fd and start work
    foundFileDescriptor = 0;
    for (int i = 0; i < maxBuffer; i++) {
      if (bufferArray[i].fileDescriptor != 0) {
        foundFileDescriptor = bufferArray[i].fileDescriptor;
        bufferArray[i].fileDescriptor = 0;
        break;
      }
    }
    // Prevent requests to fd 0 on subsequent loops
    // (requestHandle is outside of locking)
    if (foundFileDescriptor == 0) {
      Pthread_mutex_unlock(&bufferLock);
      continue;
    }
    
    // Tell main that buffer was cleared
    bufferSpotsUsed--;
    Pthread_cond_signal(&requestProcessed);
    Pthread_mutex_unlock(&bufferLock);
    
    // Do work
    requestHandle(foundFileDescriptor);
    Close(foundFileDescriptor);
  }
}

int main(int argc, char *argv[]) {
  int listenfd, connfd, port, clientlen;
  struct sockaddr_in clientaddr;

  getargs(&port, argc, argv);

  struct threadBuffer *bufferArray;
  bufferArray =
      (struct threadBuffer *)Malloc(maxBuffer * sizeof(struct threadBuffer));
  for (int i = 0; i < maxBuffer; i++) {
    bufferArray[i].fileDescriptor = 0;
  }

  bufferSpotsUsed = 0;

  pthread_t pool[maxThreads];
  for (int i = 0; i < maxThreads; i++) {
    Pthread_create(&pool[i], NULL, workerPool, bufferArray);
  }

  listenfd = Open_listenfd(port);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);

    // Main thread waits until buffer has empty spot
    Pthread_mutex_lock(&bufferLock);
    while (bufferSpotsUsed == maxBuffer) {
      Pthread_cond_wait(&requestProcessed, &bufferLock);
    }

    // Add new request to first open spot
    for (int i = 0; i < maxBuffer; i++) {
      if (bufferArray[i].fileDescriptor == 0) {
        bufferArray[i].fileDescriptor = connfd;
        break;
      }
    }

    // Tell pool threads that buffer has accepted request
    bufferSpotsUsed++;
    Pthread_cond_signal(&requestAccepted);
    Pthread_mutex_unlock(&bufferLock);
  }
}