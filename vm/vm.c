#include <list.h>
#include <stdio.h>
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "vm/vm.h"
#include "vm/frame.h"
#include "threads/palloc.h"
#include "threads/pte.h"

void *
select_victim_page(struct thread **t)
{
  struct list_elem *e;
  struct pagedir_list_entry *ple;
  uint32_t *pd;
  uint32_t *pde;
  int turn = 0;
  
SEARCH_VICTIM:  
  
  for (e = list_begin(&pagedir_list) ; e != list_end(&pagedir_list) ; e = list_next(e))
  {
    pd = (ple = list_entry(e, struct pagedir_list_entry, elem)) -> pagedir.pagedir_ptr;
    *t = ple->pagedir.owner;
    
    for (pde = pd ; pde < pd + pd_no (PHYS_BASE) ; pde++)
    {
      if (*pde & PTE_P)
      {
        uint32_t *pt = pde_get_pt(*pde);
        uint32_t *pte;
        
        for (pte = pt ; pte < pt + PGSIZE / sizeof *pte; pte++)
        {
          if (*pte & PTE_P)
          {
            if (*pte & PTE_A)
            {
              *pte &= ~(uint32_t) PTE_A;
            }
            else
            {
              return (void *)(((pde - pd) << 22) + ((pte - pt) << 12));
            }
          }
        }
      }
    }  
  }
  
  if (turn == 0)
  {
    turn++;
    goto SEARCH_VICTIM;
  }
    
  return NULL;
}
