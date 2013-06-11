#include <list.h>
#include <stdbool.h>
#include <stdint.h>
#include "devices/disk.h"
#include "threads/thread.h"

//extern struct list swap_table;

void swap_swap_disk_init(void);
bool swap_swap_in(void *vaddr);
bool swap_swap_out(struct thread *t, void *vpage);
bool swap_is_in_swap_table(void *vaddr);
int swap_release_swaped_page_of(tid_t tid);
void swap_print_stats(void);

