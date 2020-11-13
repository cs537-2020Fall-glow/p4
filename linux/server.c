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
  long unsigned int threadId;
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

void* workerPool(void *arg) {
  struct threadBuffer *bufferArray = (struct threadBuffer*) arg;
  printf("worker: tid %ld started\n", pthread_self());
  // for (int i = 0; i < maxBuffer; i++)
  // {
  //   printf("buffer[%d] tid %ld fd %d\n", i, bufferArray[i].threadId, bufferArray[i].fileDescriptor);
  // }
  
  
  while (1) {
    pthread_mutex_lock(&bufferLock);
    
    // wait for buffer to accept a request
    while (bufferSpotsUsed == 0) {
      pthread_cond_wait(&requestAccepted, &bufferLock);
    }
    
    // find fd and start work
    int foundFileDescriptor;
    for (int i = 0; i < maxBuffer; i++) {
      if (bufferArray[i].threadId == 0 &&
          bufferArray[i].fileDescriptor != 0) {
        bufferArray[i].threadId = pthread_self();
        foundFileDescriptor = bufferArray[i].fileDescriptor;
        printf("worker: buffer[%d] tid %ld fd %d\n", i, bufferArray[i].threadId, foundFileDescriptor);
        break;
      }
    }
    pthread_mutex_unlock(&bufferLock);
    requestHandle(foundFileDescriptor);
    Close(foundFileDescriptor);
    
    // worker finished, free the buffer
    pthread_mutex_lock(&bufferLock);
    for (int i = 0; i < maxBuffer; i++) {
      if (bufferArray[i].fileDescriptor == foundFileDescriptor) {
        bufferArray[i].threadId = 0;
        foundFileDescriptor = 0;
        break;
      }
    }
    bufferSpotsUsed--;
    pthread_cond_signal(&requestProcessed);
    pthread_mutex_unlock(&bufferLock);
  }
}

int main(int argc, char *argv[]) {
  int listenfd, connfd, port, clientlen;
  struct sockaddr_in clientaddr;

  getargs(&port, argc, argv);

  // pthread_cond_t bufferQueue;
  pthread_t pool[maxThreads];

  // TODO: remove?
  // pthread_mutexattr_t mutexAttribute;
  // pthread_mutexattr_init(&mutexAttribute);
  // pthread_mutex_init(&bufferLock, NULL);
  // pthread_cond_init(&)

  //
  // CS537: Create some threads...
  //

  struct threadBuffer *bufferArray =
      malloc(maxBuffer * sizeof(struct threadBuffer));
  //memset(bufferArray, 0, maxBuffer * sizeof(struct threadBuffer));
  
  for (int i = 0; i < maxBuffer; i++)
  {
    bufferArray[i].threadId = 0;
    bufferArray[i].fileDescriptor = 0;
  }
  
  for (int i = 0; i < maxBuffer; i++)
  {
    printf("buffer[%d] tid %ld fd %d\n", i, bufferArray[i].threadId, bufferArray[i].fileDescriptor);
  }
  
  
  if (bufferArray == NULL) {
    fprintf(stderr, "Could not allocate space for buffer\n");
    exit(1);
  }

  for (int i = 0; i < maxThreads; i++) {
    pthread_create(&pool[i], NULL, workerPool, &bufferArray);
  }

  listenfd = Open_listenfd(port);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);

    //
    // CS537: In general, don't handle the request in the main thread.
    // Save the relevant info in a buffer and have one of the worker threads
    // do the work.
    //

    // Main thread waits until buffer has empty spot
    pthread_mutex_lock(&bufferLock);
    while (bufferSpotsUsed == maxBuffer) {
      pthread_cond_wait(&requestProcessed, &bufferLock);  // TODO: wrapper
    }

    // Add new request to first open spot
    for (int i = 0; i < maxBuffer; i++) {
      if (bufferArray[i].threadId == 0) {
        bufferArray[i].fileDescriptor = connfd;
        printf("main: buffer[%d] fd %d\n", i, bufferArray[i].fileDescriptor);
        break;
      }
    }

    // Tell pool threads that buffer has accepted request
    bufferSpotsUsed++;
    pthread_cond_signal(&requestAccepted);
    pthread_mutex_unlock(&bufferLock);
  }
}