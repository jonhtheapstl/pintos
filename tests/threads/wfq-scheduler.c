/* Creates N threads, each of which sleeps a different, fixed
   duration, M times.  Records the wake-up order and verifies
   that it is valid. */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "devices/timer.h"
#include <random.h>
#ifdef DEBUG
#include <stdlib.h>
#endif

struct test
{
  int *output_pos;
  int exec_start;
};

int key = 0;
unsigned int start_test;
unsigned int stop_test;
static void accumulator (void *t_);

void test_wfq_scheduler (void) 
{
  int *output, priority, i, n_threads;
  struct test *t;
  
  /* This test does not work with the MLFQS. */
  ASSERT (!thread_mlfqs);
  
  n_threads = atoi((char *)argument);
  
  msg ("Creating %d threads.", n_threads);

  /* Allocate memory. */
  output = malloc (sizeof *output * n_threads);
  t->exec_start = timer_ticks() + n_threads * 30;
  
  if (output == NULL) PANIC ("couldn't allocate memory for test");

  /* Start threads. */
  for (i = 0; i < n_threads; i++)
  {
    char name[16];
    
    *(output + i) = 0;
    
    priority = random_ulong() % 64;
    snprintf (name, sizeof name, "thread %d", i);
    msg ("thread %d : %d", i, priority);
    t->output_pos = (output + i);
         
    thread_create (name, priority, accumulator, t);
  }
  msg("Start.");
#ifdef DEBUG
  trace_scheduler = 1;
#endif
  start_test = cpu_clock();
  key = 1;
  /* Wait long enough for all the threads to finish. */
  timer_sleep (n_threads * 100);
  key = 0;
  stop_test = cpu_clock();
  msg("Done.");
#ifdef DEBUG
  trace_scheduler = 0;
#endif  
  free (output);
}

/* thread function. Accumulates 1 as it is executed. 
   I changed "Make.config" for this function.
   if the compile option "-O" is set, compiler unroll
   loop, so we can't get a expected result. */
static void accumulator (void *t_) 
{
  struct test *t = t_;
  timer_sleep(t->exec_start - timer_ticks());
  while(1) *(t->output_pos) = *(t->output_pos) + key;
}
