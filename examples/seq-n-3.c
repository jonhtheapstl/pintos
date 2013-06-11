#include <stdio.h>
#include <string.h>
#include <syscall.h>

int main(void)
{
	int fd, i;
	char buffer[32];
	
	if(create("seq_n_3.txt", 320))
	{
		fd = open("seq_n_3.txt");
		
		write(fd, "Sequence An = n^3, n = 1~100\n", 30);
		
		for (i = 1 ; i <= 100 ; i++)
		{
			snprintf(buffer, 32, "A(%d) = %d\n", i, i * i * i);
			write(fd, buffer, strnlen(buffer, 32));
		}
		
		close(fd);
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}
	
}