/* Demonstrate that the stack can grow.
   This must succeed. */

#include <stdio.h>
#include <string.h>
#include "tests/arc4.h"
#include "tests/cksum.h"
#include "tests/lib.h"
#include "tests/main.h"

struct myArray
{
  int n;
  char elem[2000];
};

void callByValueTest(struct myArray);

void
test_main(void)
{
  struct myArray source;
  
  source.n = 800;
  snprintf(source.elem, 2000, "Now I'm %d.\n", source.n);
  printf("Before : %s\n", source.elem);
  callByValueTest(source);
  printf("After : %s\n", source.elem);
  
  
}

void callByValueTest(struct myArray src)
{
  if (src.n == 0)
  {
    msg("Now I'm Finished.\n");
  }
  else
  {
    src.n--;
    snprintf(src.elem, 500, "%d", src.n);
    callByValueTest(src);
  }
}
