#ifndef VM_VM_H
#define VM_VM_H
#endif
#include <stdio.h>
#include "threads/thread.h"

void *select_victim_page(struct thread **t);
