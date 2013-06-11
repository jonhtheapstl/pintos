/***************************************************
** arg-pass.c																			**
** This program verifies argument passing.        **
***************************************************/



#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	int i;
	
	printf("Address    Name         Data\n");
	printf("0x%8x argc         %d\n", &argc, argc);
	printf("0x%8x argv         0x%8x\n", &argv, argv);
	
	for (i = 0 ; i <= argc ; i++)
	{
		printf("0x%8x argv[%d]      0x%8x\n", (argv +  i), i, *(argv + i));
	}
	
	for (i = 0 ; i < argc ; i++)
	{
		printf("0x%8x argv[%d][...] '%s'\n", *(argv + i), i, *(argv + i));
	}
		
	return 0;
}