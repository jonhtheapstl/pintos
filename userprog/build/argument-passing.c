#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define LOADER_ARGS_LEN 128

int main(int argc, char **argv)
{
	char buffer[LOADER_ARGS_LEN], temp;
	int length, i, j, upper, quote, isInQuotes = 0;
	char alphabet[60] = {	'a', 'b', 'c', 'd', 'e', 'f', 'g', ' ',
												'h', 'i', 'j', 'k', 'l', 'm', 'n', '"',
												'o', 'p', 'q', 'r', 's', 't', 'u', ' ',
												'v', 'w', 'x', 'y', 'z', ' ',
												'A', 'B', 'C', 'D', 'E', 'F', 'G', '"',
												'H', 'I', 'J', 'K', 'L', 'M', 'N', ' ',
												'O', 'P', 'Q', 'R', 'S', 'T', 'U', ' ',
												'V', 'W', 'X', 'Y', 'Z', '"'};
												
	
	memset(buffer, 0, LOADER_ARGS_LEN);
	srand(time(NULL));

	if (argc < 2)
	{
		length = LOADER_ARGS_LEN - 5;
	}
	else if (atoi(*(argv + 1)) < 9)
	{
		length = rand() % (LOADER_ARGS_LEN - 5) + 1;
	}
	else
	{
		length = atoi(*(argv + 1));
	}

  buffer[0] = 'a';
  buffer[1] = 'r';
  buffer[2] = 'g';
  buffer[3] = '-';
  buffer[4] = 'p';
	buffer[5] = 'a';
	buffer[6] = 's';
	buffer[7] = 's';
	buffer[8] = ' ';

	for (i = 9 ; i < length - 1 ; i++)
	{
    temp = alphabet[rand()%60];

    if (temp == '"')
    {
      if (isInQuotes)
      {
        buffer[i] = '"';
        i++;
        buffer[i] = ' ';
        isInQuotes = 0;
      }
      else
      {
        buffer[i] = ' ';
        i++;
        buffer[i] = '"';
        isInQuotes = 1;
      }
    }
    else
    {
  		buffer[i] = temp;
    }
	}

  if(isInQuotes) buffer[i] = '"';
	
	execl("/home/jonhtheapstl/Project/Pintos/utils/pintos", "pintos", "run", buffer, NULL);  

	return 0;
}
