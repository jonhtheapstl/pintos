#include <stdio.h>
#include <string.h>
#include <syscall.h>

int main(void)
{
	int fd, i, status;
	char buffer[] = "Happy Birthday To You!";
	
	printf("I'm child.\n");
	printf("Now I'll try to write executing file.\n");
	fd = open("print_child");
	status = write(fd, buffer, strnlen(buffer, 64))
	
	if(status == 0) printf("Write is denied because i tried writing executing file.\n");
	else if(status = -1) printf("Write is denied because i tried writing none existing file.\n");
	else printf("Write is success!!\n");	
	
	return EXIT_SUCCESS;	
}