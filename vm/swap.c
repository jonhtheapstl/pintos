#include <stdio.h>
#include <string.h>
#include <bitmap.h>
#include "vm/swap.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/pte.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"

#define SECTORS_FOR_A_PAGE PGSIZE / DISK_SECTOR_SIZE

struct swap_page
{
  struct list_elem elem;
  tid_t tid;
  uint32_t page_number;
  bool writable;
  disk_sector_t sector;
};

struct list swap_table;
static struct disk *swap_disk;
static disk_sector_t n_sectors;
static struct bitmap *swap_pool;

static uint32_t swapped_in, swapped_out, released;

void swap_swap_disk_init(void)
{  
  list_init(&swap_table);
  swap_disk = disk_get(1, 1);
  n_sectors = disk_size(swap_disk);
  printf("Size of Swap Disk : %d\n", (int)n_sectors);
  
  swapped_in = 0;
  swapped_out = 0;
  released = 0;
  
  bitmap_set_all(swap_pool = bitmap_create(n_sectors), true);
}

bool swap_swap_in(void *vaddr)
{
  struct list_elem *e;
  struct swap_page *p;
  struct thread *t = thread_current();
  char disk_buffer[DISK_SECTOR_SIZE], *page_swaped_in;
  int i;
  
  for (e = list_begin(&swap_table) ; e != list_end(&swap_table) ; e = list_next(e))
  {
    p = list_entry(e, struct swap_page, elem);
    if (p->page_number == ((uintptr_t)vaddr & PTE_ADDR))
    {
      page_swaped_in = palloc_get_page(PAL_USER);
      
      for (i = 0 ; i < SECTORS_FOR_A_PAGE ; i++)
      {
        disk_read(swap_disk, p->sector + i, disk_buffer);
        memcpy(page_swaped_in + i * DISK_SECTOR_SIZE, disk_buffer,
               DISK_SECTOR_SIZE);
      } 
      bitmap_set_multiple(swap_pool, p->sector, SECTORS_FOR_A_PAGE, true);
      pagedir_set_page(t->pagedir, p->page_number, page_swaped_in, p->writable);
      list_remove(e);
      free(p);
      
      swapped_in++;
      return true;
    }
  }
  return false;
}

bool swap_swap_out(struct thread *t, void *vpage)
{
  int i, sector = 0;
  static char disk_buffer[DISK_SECTOR_SIZE];
  struct swap_page *p = calloc (1, sizeof *p);
  char *kpage;
  
  if (p != NULL)
  {
    
    p->tid = t->tid;
    p->page_number = (uintptr_t)vpage;
    p->writable = pagedir_is_writable(t->pagedir, vpage);
    kpage = pagedir_get_page(t->pagedir, vpage);
    
    sector = bitmap_scan(swap_pool, 0, SECTORS_FOR_A_PAGE, true);
    for (i = 0 ; i < (PGSIZE / DISK_SECTOR_SIZE) ; i++)
    {
      memcpy(disk_buffer, kpage + i * DISK_SECTOR_SIZE, DISK_SECTOR_SIZE);
      
      disk_write(swap_disk, sector + i, disk_buffer);
    }
    p->sector = sector;
    bitmap_set_multiple(swap_pool, sector, SECTORS_FOR_A_PAGE, false);
    pagedir_clear_page(t->pagedir, vpage);
    palloc_free_page(kpage);
    list_push_back(&swap_table, &p->elem);
    
    swapped_out++;
    return true;
  }
  else
  {
    return false;
  }  
}

int swap_release_swaped_page_of(tid_t t)
{
  struct swap_page *p;
  struct list_elem *e = list_begin(&swap_table);
  int count = 0;
  
  while(e != list_end(&swap_table))
  {
    p = list_entry(e, struct swap_page, elem);
    
    if (p->tid == t)
    {
      bitmap_reset(swap_pool, p->sector);
      e = list_remove(e);
      free(p);
    }
    count++;
  }
  
  released += count;
  return count;
}

/* if designated address is in swap table, return disk sector number.
   if not, return 0.  */
bool swap_is_in_swap_table(void *vaddr)
{
  uint32_t page_number = ((uintptr_t)vaddr & PTE_ADDR);
  struct swap_page *p;
  struct list_elem *e;
  
  for (e = list_begin(&swap_table) ; e != list_end(&swap_table) ; e = list_next(e))
  {
    p = list_entry(e, struct swap_page, elem);
    if (p->page_number == page_number)
    {
      return true;
    }
  }
  return false;
}

void swap_print_stats(void)
{
  printf("swapped in : %u\n", swapped_in);
  printf("swapped out : %u\n", swapped_out);
  printf("released : %u\n", released);
}
