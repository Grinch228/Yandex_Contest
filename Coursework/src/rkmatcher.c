#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rkmatcher.h"

int main(int argc, char *argv[])
{
	char *word, *dir;
	int result, rec = 0;
	if (argc == 3) {
		word = argv[1];
		dir = argv[2];
	} else if (argc == 4 && strcmp("-r", argv[1]) == 0) {
		rec = 1;
		word = argv[2];
		dir = argv[3];
	} else {
		printf("Invalid number of arguments\n");
		return 0;
	}
	printf("Search '%s' in directory %s\n", word, dir);

	result = crawling_dir(dir, word, 0, rec);
	if (result == -1) {
		perror(dir);
	} else {
		printf("\nTotal words found in derictory %s: %d\n", dir, result);
	}	
	return 0;
}


