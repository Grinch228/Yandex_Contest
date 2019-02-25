#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include "rkmatcher.h"

#define CSI "\x1B\x5B"

char colors[][5] = {
"0;30", /* Black */ "1;30", /* Dark Gray */
"0;31", /* Red */ "1;31", /* Bold Red */
"0;32", /* Green */ "1;32", /* Bold Green */
"0;33", /* Yellow */ "1;33", /* Bold Yellow */
"0;34", /* Blue */ "1;34", /* Bold Blue */
"0;35", /* Purple */ "1;35", /* Bold Purple */
"0;36", /* Cyan */ "1;36" /*Bold Cyan */ };
int colors_sz = sizeof(colors) / sizeof(colors[0]);

int crawling_dir(const char *direct, const char *word, int all_search, int rec)
{
	struct dirent *entry;
	char *new_path;
	DIR *dir = opendir(direct);
	if (!dir) {
		return -1;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_name[0] == '.') {
			continue;
		}
		if (entry->d_type & DT_DIR && rec == 1) {
			new_path = create_new_path(direct, entry->d_name);
			if (new_path == NULL) {
				continue;
			}
			all_search = crawling_dir(new_path, word, all_search, rec);
			free(new_path);
		} else {
			new_path = create_new_path(direct, entry->d_name);
			if (new_path == NULL) {
				continue;
			}
			all_search += input_str(new_path, word);
			free(new_path);
		}
	}

	closedir(dir);
	return all_search;
}

char *create_new_path(const char *external_dir, const char *interior_dir)
{
	int ex_dir = strlen(external_dir);
	int int_dir = strlen(interior_dir);
	int new_path = ex_dir + int_dir;
	char *new_dir = malloc(sizeof(char) * new_path * 2);
	if (new_dir == NULL) {
		printf("Error allocating memory for new path\n");
		printf("Path 1: %s\n", external_dir);
		printf("Path 2: %s\n", interior_dir);
		return NULL;
	}

	strcpy(new_dir, external_dir);
	new_dir[ex_dir] = '/';
	strcpy(&new_dir[ex_dir + 1], interior_dir);

	return new_dir;
}

int input_str(const char *file, const char *word)
{
	FILE *in = fopen(file, "r");
	int line = 0, all_search = 0;
	if (!in) {
		perror(file);
		return 0;
	}

	fseek(in, 0, SEEK_END);
	long int len = ftell(in);
	rewind(in);

	char *text = malloc(sizeof(char) * len);
	while (fgets(text, len, in)) {
		line++;
		all_search += search(word, text, line, 0, file);
	}

	fclose(in);
	free(text);
	return all_search;
}

int search(const char *word, const char *text, int line, int all_search, const char *file)
{
	int n_text = strlen(text), buf_hash = 0;
	int n_word = strlen(word), h_word;
	char cpy[n_text], buf[1];
	Results *res = create_res(n_text / n_word);
	if (res == NULL) {
		return 0;
	}

	res->line = line;
	strcpy(cpy, text);
	h_word = hash_f(word);

	for (int i = 0; i < n_text - n_word; i++) {
		
		buf[0] = cpy[i + n_word];
		cpy[i + n_word] = '\0';
		buf_hash = hash_f(&cpy[i]);

		if (buf_hash == h_word) {
			if (strcmp(word, &cpy[i]) == 0) {
				res->column[res->n_column] = i;
				res->n_column++;
				all_search++;
			}
		}
		cpy[i + n_word] = buf[0];
	}
	if (res->n_column != 0) {
		output_search(res, text, n_word, file);
	}
	free_res(res);
	return all_search;
}

Results *create_res(int n)
{
	Results *res = malloc(sizeof(Results));
	if (res == NULL) {
		printf("Error allocating memory for structure\n");
		return NULL;
	}
	res->n_column = 0;
	res->column = malloc(sizeof(int) * n);
	if (res->column == NULL) {
		printf("Error allocating memory for the structure element\n");
		free(res);
		return NULL;
	}
	return res;
}

void free_res(Results *res)
{
	if (res != NULL) {
		free(res->column);
		free(res);
	}
}

unsigned int hash_f(const char *word)
{
	unsigned int h = 0;
	int n = strlen(word), m = n , d = 7;
	for (int i = 0; i < n; i++, m--) {
		if (word[i] == '\n') {
			continue;
		}
		h += word[i] * pow(d, m);
	}
	return h;
}

void output_search(Results *res, const char *text, int n_word, const char *file)
{
	int z = 0;
	printf("\nPath to file: %s\n", file);
	printf("Line in file: %d\n", res->line);
	printf("Column in file: ");

	for (int i = 0; i < res->n_column; i++) {
		printf("|%d| ", res->column[i] + 1);
	}
	printf("\n");
	printf("String: ");

	for (int i = 0; i < res->n_column; i++) {
		for (int j = z; j < res->column[i]; j++) {
			if (i == 0 && text[j] == '\t') {
				continue;
			}
			printf("%c", text[j]);
		}
		printf("%s%sm", CSI, colors[2]);
		for (int j = res->column[i]; j < res->column[i] + n_word; j++) {
			printf("%c", text[j]);
		}
		printf("%s0m", CSI);
		z = res->column[i] + n_word;
	}
	printf("%s\n", &text[z]);
}
