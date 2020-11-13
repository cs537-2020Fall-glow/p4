#include "types.h"
#include "user.h"

// default number of customer if there is no arg
#define NUM_CUSTOMERS_DEFAULT 800
// default number of seats
#define NUM_SEATS_DEFAULT 8
// group size is always same as number of seats
#define GROUP_SIZE_DEFAULT NUM_SEATS_DEFAULT

// real value (can be set from commandline)
int num_seats = NUM_SEATS_DEFAULT;
int group_size = GROUP_SIZE_DEFAULT;
int num_customers = NUM_CUSTOMERS_DEFAULT;
// number of available seats
int num_avail_seats = NUM_SEATS_DEFAULT;

// a counter of total number of threads that has left waiting room
// if num_total_left_seats % num_seats == 0, then the current thread
// is the last one of its group that leaves the waiting room
int num_total_left_waiting = 0;
lock_t waiting_seats_mutex;

// condition variables
cond_t waiting_room;
cond_t outside;

struct thread_meta {
	pthread_t tid;
	int cid; // customer id
};

void* customer_main(void* arg) {
	struct thread_meta my_meta = *(struct thread_meta*)arg;
	int cid = my_meta.cid;
  //printf("Customer [%d] arrived\n", cid);

  lock_acquire(&waiting_seats_mutex);
  // wait outside
  while (num_avail_seats == 0) {
    //printf("Customer [%d]-> outside\n", cid);
    cond_wait(&outside, &waiting_seats_mutex);
  }

  // take a seat in waiting room
  --num_avail_seats; 
  //printf("Customer [%d] -> waiting room\n", cid);

  // wait for waiting room to fill
  while (num_avail_seats % num_seats != 0) {
    //printf("Customer [%d] -> waiting for waiting room to fill\n", cid);
    cond_wait(&waiting_room, &waiting_seats_mutex);
  }
  // tell the next person to leave the waiting room
  cond_signal(&waiting_room);

  // get a table
  ++num_total_left_waiting;
  int gid = (num_total_left_waiting - 1) / num_seats; // group id
  printf("Customer [%d:%d] -> done waiting\n", gid, cid);
  if (num_total_left_waiting % num_seats == 0) {
    printf("=== Group [%d] all done waiting ===\n", gid);
    num_avail_seats = num_seats;
    
    // tell num_seats customers outside to take a seat in the waiting room
    for (int i = 0; i < num_seats; i++) {
      cond_signal(&outside);
    }
  }
  lock_release(&waiting_seats_mutex);

	return NULL;
}

int main(int argc, char** argv) {
	if (argc == 3) {
		num_seats = atoi(argv[1]);
		num_customers = atoi(argv[2]);
		num_avail_seats = num_seats;
		if (num_seats == 0 || num_customers == 0) {
			fprintf(stderr, "Invalid parameter\n");
			exit(1);
		}
	} else if (argc == 1) {
		fprintf(stderr, "Using default parameter\n");
	} else {
		fprintf(stderr, "Usage: cust_group [num_seats] [num_customers]\n");
		exit(1);
	}
  
  lock_init(&waiting_seats_mutex);
  cond_init(&waiting_room);
  cond_init(&outside);

	int next_cid = 1;

	struct thread_meta* all_meta = malloc(num_customers * sizeof(struct thread_meta));
	memset(all_meta, 0, num_customers * sizeof(struct thread_meta));

	for (int i = 0; i < num_customers; ++i) {
		all_meta[i].cid = next_cid;
		++next_cid;
		thread_create(customer_main, &all_meta[i]);
	}

	for (int i = 0; i < num_customers; ++i)
	    thread_join();

	//pthread_mutex_destroy(&waiting_seats_mutex);

	return 0;
}