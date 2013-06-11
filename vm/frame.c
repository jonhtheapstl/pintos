/*
 * vm.c
 *
 *  Created on: Jun 2, 2013
 *      Author: jb
 */
#include <stdio.h>
#include "kernel/list.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "frame.h"

/*
 * Page directory list initialization.
 * Call from main()->ram_init() in init.c.
 */
void
frame_init (void)
{
	list_init(&pagedir_list);
	lock_init(&pagedir_list_lock);
	printf("Page Directory List initializing...\n");
	return;
}

/*
 * Add a page directory to the list.
 * Marks pointer of owning thread and its page directory ptr.
 */
void
add_pagedir(struct thread *t)
{
	struct pagedir_list_entry *ple = (struct pagedir_list_entry*)malloc(sizeof(struct pagedir_list_entry));
	if (ple == NULL)
		return;

	ple->pagedir.owner = t;
	ple->pagedir.pagedir_ptr = t->pagedir;

	list_push_back(&pagedir_list, &ple->elem);

	return;
}

/*
 * Remove page directory entry from list.
 */
void
remove_pagedir(struct thread *t)
{
	struct pagedir_list_entry *ple = search_pagedir(t);
	free(ple);

	return;
}

/*
 * Find entry of page directory list and return its pointer.
 * If it not exist, return NULL.
 */
struct pagedir_list_entry*
search_pagedir(struct thread *t)
{
	struct pagedir_list_entry *ple;
	struct list_elem *e;

	for (e = list_begin(&pagedir_list); e != list_end(&pagedir_list); e = list_next(e))
	{
		ple = list_entry(e, struct pagedir_list_entry, elem);
		if (ple == t->pagedir){
			return ple;
		}
	}
	return NULL;
}
