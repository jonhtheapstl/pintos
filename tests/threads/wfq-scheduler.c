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
#include "threads/interrupt.h"
#include <stdlib.h>
#endif

unsigned int start_test;
unsigned int stop_test;
static void accumulator (void);
static void analyze_result(void);

void test_wfq_scheduler (void) 
{
  int priority, i, n_threads;
  enum intr_level old_level;
  
  /* This test does not work with the MLFQS. */
  ASSERT (!thread_mlfqs);
  
  n_threads = atoi((char *)argument);
  
  msg ("Creating %d threads.", n_threads);
  old_level = intr_disable();

  /* Start threads. */
  for (i = 0; i < n_threads; i++)
  {
    char name[16];
    
    priority = random_ulong() % 64;
    total_weight = total_weight + p_to_w(PRI_MIN) / p_to_w(priority);
    snprintf (name, sizeof name, "thread %d", i);
         
    thread_create (name, priority, accumulator, NULL);
  }
  msg("Start.");
#ifdef DEBUG
  trace_scheduler = 1;
#endif
  start_test = cpu_clock();
  intr_set_level(old_level);
  /* Wait long enough for all the threads to finish. */
  timer_sleep (n_threads * 100);
  stop_test = cpu_clock();
  msg("Done.");
#ifdef DEBUG
  old_level = intr_disable();
  trace_scheduler = 0;
  
  /* Analyze Result. */
  analyze_result();
  
  intr_set_level(old_level);
#endif  
}

/* thread function. Accumulates 1 as it is executed. 
   I changed "Make.config" for this function.
   if the compile option "-O" is set, compiler unroll
   loop, so we can't get a expected result. */
static void accumulator (void) 
{
  while(1);
}

static void analyze_result(void)
{
  int64_t expected = 0, test_duration = stop_test - start_test;
  int64_t tot_inv_w = 0, delta, error = 0;
  struct list_elem *e;
  struct thread *t;
  
  struct list *ready_list = get_ready_list();
  
  for (e = list_begin(ready_list) ; e != list_end(ready_list) ; e = list_next(e))
  {
    t = list_entry(e, struct thread, elem);
    tot_inv_w = tot_inv_w + p_to_w(PRI_MIN) / p_to_w(t->priority);
  }
  printf("                                                                \n");
  printf("                                                                \n");
  printf("                                                                \n");
  printf("                                                                \n");
  printf("                                                                \n");
  printf("                                                                \n");
  printf("                                                                \n");
  printf("                                                                \n");
  
  printf("tid,priority,vruntime,runtime,expected,error,gps,ste_max,ste_min\n");
  for (e = list_begin(ready_list) ; e != list_end(ready_list) ; e = list_next(e))
  {
    t = list_entry(e, struct thread, elem);
    expected = test_duration * ( p_to_w(PRI_MIN) / p_to_w(t->priority) )
               / tot_inv_w;
    delta = (expected - t->actual_runtime);
    error = error + delta;
    printf("%d,%d,%d,%d,%lld,%lld,%d,%d,%d\n",
           t->tid,t->priority,t->vruntime,t->actual_runtime, expected, delta,
           t->gps_time, t->ste_max, t->ste_min);
  }
  printf("\n");
  
  printf("Number of threads : %s\n", (char *)argument);
  printf("Test Duration     : %lld (%d to %d)\n", test_duration, start_test, stop_test);
  printf("Total Error       : %lld\n", error);
  printf("schedule (total/count/Min/Max) : %d / %d / %d / %d\n",
         o_sched.total, o_sched.count, o_sched.min, o_sched.max);
  printf("Readylist(total/count/Min/Max) : %d / %d / %d / %d\n", 
         o_ready.total, o_ready.count, o_ready.min, o_ready.max);
  printf("Waitlist (total/count/Min/Max) : %d / %d / %d / %d\n", 
         o_sleep.total, o_sleep.count, o_sleep.min, o_sleep.max);
  
}
  
