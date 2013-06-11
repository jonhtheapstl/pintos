/*
 * vm.h
 *
 *  Created on: Jun 2, 2013
 *      Author: jb
 */
#ifndef VM_FRAME_H
#define VM_FRAME_H
#include "kernel/list.h"
#include "kernel/hash.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include <stdio.h>

struct list pagedir_list;
struct lock pagedir_list_lock;

void frame_init (void);
void add_pagedir(struct thread*);
void remove_pagedir(struct thread*);
struct pagedir_list_entry* search_pagedir(struct thread *);

struct page_id
{
  struct thread* owner;
  uint32_t *pagedir_ptr;
};

struct pagedir_list_entry
{
	struct list_elem elem;
	struct page_id pagedir;
	struct lock modify_lock;
};
#endif
