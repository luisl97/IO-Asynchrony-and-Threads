//
// This is the solution to CPSC 213 Assignment 9.
// Do not distribute this code or any portion of it to anyone in any way.
// Do not remove this comment.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/errno.h>
#include <assert.h>
#include "queue.h"
#include "disk.h"
#include "uthread.h"

queue_t      pending_read_queue;
unsigned int sum = 0;

void interrupt_service_routine () {
  void* val;
  queue_dequeue (pending_read_queue, &val, NULL, NULL);
  uthread_t* t = (uthread_t*) val;
  uthread_unblock(*t);
//  printf("ISR FINSIHED\n");
}

void* read_block (void* blocknov) {
//  printf("PASSED 1\n");

  intptr_t blockno = (intptr_t) blocknov;

  //printf("%ld\n", blockno);
  int result;
  uthread_t t = uthread_self();
  queue_enqueue(pending_read_queue, &t, NULL, NULL);
  disk_schedule_read (&result, blockno);
  uthread_block();
  //printf("%d\n", result);
  sum += result;
  return NULL;
}

int main (int argc, char** argv) {

  // Command Line Arguments
  static char* usage = "usage: tRead num_blocks";
  int num_blocks;
  char *endptr;
  if (argc == 2)
    num_blocks = strtol (argv [1], &endptr, 10);
  if (argc != 2 || *endptr != 0) {
    printf ("argument error - %s \n", usage);
    return EXIT_FAILURE;
  }

  // Initialize
  uthread_init (1);
  disk_start (interrupt_service_routine);
  pending_read_queue = queue_create();

  // Sum Blocks
  uthread_t t[num_blocks];
  for (int blockno = 0; blockno < num_blocks; blockno++) {
  //  printf("%d\n", blockno);

    t[blockno] = uthread_create(read_block,(void*)(intptr_t) blockno);
  }
  for (int blockno = 0; blockno < num_blocks; blockno++) {
    uthread_join(t[blockno], NULL);
  }

  printf ("%d\n", sum);
  // TODO
}
