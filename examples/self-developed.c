#include <stdio.h>
#include <syscall.h>

int main(int argc, char **argv)
{
	int pid[3], fd, i;
  char test_name[3][13] = {"child-simple", "write-normal", "wait-twice"};
  char buffer[32];
	
  printf("argc=%d, argv=%s", argc, *(argv+1));
	printf("1. Create Sequnces.\n");
  
  for (i = 0 ; i < 3 ; i++)
  {	
    pid[i] = exec(test_name[i]);
    printf("%s:%d\n",test_name[i], pid[i]);
  }

  for (i = 0 ; i < 3 ; i++)
  {
    printf("wating %s...\n", test_name[i]);
    wait(pid[i]);
  }

  printf("2. Open and read file : test.txt\n");
	
	fd = open("test.txt");
  read(fd, buffer, 32);
  
  printf("test.txt:%s\n", buffer);
  
	if (argc >=2)
  {
	  printf("3. Execute userprogram from argument : %s\n", *(argv + 1));
	  wait(exec(*(argv + 1)));
  }	
	return 0;
}
