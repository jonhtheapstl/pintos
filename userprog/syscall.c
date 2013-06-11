#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <console.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "devices/input.h"

/* This is a skeleton system call handler */

static void syscall_handler (struct intr_frame *);
static void sys_halt (struct intr_frame *);
static void sys_exit (struct intr_frame *);
static void sys_exec (struct intr_frame *);
static void sys_wait (struct intr_frame *);
static void sys_create (struct intr_frame *);
static void sys_remove (struct intr_frame *);
static void sys_open (struct intr_frame *);
static void sys_filesize (struct intr_frame *);
static void sys_read (struct intr_frame *);
static void sys_write (struct intr_frame *);
static void sys_seek (struct intr_frame *);
static void sys_tell (struct intr_frame *);
static void sys_close (struct intr_frame *);
static void sys_mmap (struct intr_frame *);
static void sys_munmap (struct intr_frame *);
static void sys_chdir (struct intr_frame *);
static void sys_mkdir (struct intr_frame *);
static void sys_readdir (struct intr_frame *);
static void sys_isdir (struct intr_frame *);
static void sys_inumber (struct intr_frame *);

static void *is_valid_virtual_address(const void *);

static void (*syscall_table[SYS_LAST])(struct intr_frame *f_) =
{ /* Projects 2 and later. */
  /* 0 : SYS_HALT */      sys_halt,       /* Halt the operating system. */
  /* 1 : SYS_EXIT */      sys_exit,       /* Terminate this process. */
  /* 2 : SYS_EXEC */      sys_exec,   /* Start another process. */
  /* 3 : SYS_WAIT */      sys_wait,       /* Wait for a child process to die. */
  /* 4 : SYS_CREATE */    sys_create,       /* Create a file. */
  /* 5 : SYS_REMOVE */    sys_remove,       /* Delete a file. */
  /* 6 : SYS_OPEN */      sys_open,       /* Open a file. */
  /* 7 : SYS_FILESIZE */  sys_filesize,       /* Obtain a file's size. */
  /* 8 : SYS_READ */      sys_read,       /* Read from a file. */
  /* 9 : SYS_WRITE */     sys_write,       /* Write to a file. */
  /* 10 : SYS_SEEK */     sys_seek,       /* Change position in a file. */
  /* 11 : SYS_TELL */     sys_tell,       /* Report current position in a file. */
  /* 12 : SYS_CLOSE */    sys_close,       /* Close a file. */

  /* Project 3 and optionally project 4. */
  /* 13 : SYS_MMAP */     sys_mmap,       /* Map a file into memory. */
  /* 14: SYS_MUNMAP */    sys_munmap,       /* Remove a memory mapping. */

  /* Project 4 only. */
  /* 15 : SYS_CHDIR */    sys_chdir,       /* Change the current directory. */
  /* 16 : SYS_MKDIR */    sys_mkdir,       /* Create a directory. */
  /* 17 : SYS_READDIR */  sys_readdir,       /* Reads a directory entry. */
  /* 18 : SYS_ISDIR */    sys_isdir,       /* Tests if a fd represents a directory. */
  /* 19 : SYS_INUMBER */  sys_inumber,       /* Returns the inode number for a fd. */
};


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f)
{
  void (*function)() = syscall_table[*(int *)(f->esp)];
  if (*function != NULL) (*function)(f);
}

/* system calls. */
static void sys_halt (struct intr_frame *f_) 
{
  power_off();
}

static void sys_exit(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  int status = *(esp + 1);
  struct thread *t = thread_current();
  printf("%s:exit(%d)\n", t->name, status);
  thread_exit ();
}

static void sys_exec(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  const char *file = is_valid_virtual_address(*(esp + 1));
  
  f_->eax = process_execute(file);
}

static void sys_wait(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  int pid = *(esp + 1);
  
  f_->eax = process_wait(pid);
}

static void sys_create(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  char *file = is_valid_virtual_address(*(esp + 1));
  unsigned int initial_size = *(esp + 2);
  
  if (initial_size < 0) f_->eax = -1;
  else f_->eax = filesys_create(file, initial_size);
}

static void sys_remove(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  char *file = is_valid_virtual_address(*(esp + 1));
  
  f_->eax = filesys_remove(file);
}

static void sys_open(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  char *file = is_valid_virtual_address(*(esp + 1));
  struct file *f = filesys_open(file);
    
  if (f == NULL) f_->eax = -1;
  else 
  {
    f_->eax = file_insert_in_fd(f);
  }
}

static void sys_filesize(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  int fd = *(esp + 1);
  struct file *f = file_search_in_fd(fd);
    
  if (f == NULL) f_->eax = -1;
  else f_->eax = file_length(f);  
}

static void sys_read(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  int fd = *(esp + 1);
  char *buffer = is_valid_virtual_address(*(esp + 2));
  unsigned int size = *(esp + 3);
  struct file *f;
  
  if (fd == STDIN_FILENO) f_->eax = input_getc();
  else if (fd < 3) f_->eax = -1;
  else
  {
    f = file_search_in_fd(fd);
    if (f == NULL) f_->eax = -1;
    else f_->eax = file_read(f, buffer, size);
  }
}

static void sys_write(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  int fd = *(esp + 1);
  char *buffer = is_valid_virtual_address(*(esp + 2));
  unsigned int size = *(esp + 3);
  struct file *f;
  
  if (fd == STDOUT_FILENO) putbuf(buffer, size);
  else if (fd < 3) f_->eax = -1;
  else
  {
    f = file_search_in_fd(fd);
    if (f == NULL) f_->eax = -1;
    else f_->eax = file_write(f, buffer, size);
  }
}

static void sys_seek(struct intr_frame *f_)
{
}

static void sys_tell(struct intr_frame *f_)
{
}

static void sys_close(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  int fd = *(esp + 1);
  struct file *f = file_search_in_fd(fd);
  
  if (f != NULL) 
  {
    file_remove_in_fd(fd);
    file_close(f);
  }
}

static void sys_mmap(struct intr_frame *f_)
{
}

static void sys_munmap(struct intr_frame *f_)
{
}

static void sys_chdir(struct intr_frame *f_)
{
}

static void sys_mkdir(struct intr_frame *f_)
{
}

static void sys_readdir(struct intr_frame *f_)
{
}

static void sys_isdir(struct intr_frame *f_)
{
}

static void sys_inumber(struct intr_frame *f_)
{
}

static void *is_valid_virtual_address(const void *addr)
{
  return (is_user_vaddr(addr) && pagedir_get_page(thread_current()->pagedir, addr)) ? addr : NULL;
}
