#ifndef RKMATCHER_H
#define RKMATCHER_H

typedef struct {
	int line;
	int n_column;
	int *column;
} Results;

int crawling_dir(const char *direct, const char *word, int all_search, int rec);
char *create_new_path(const char *external_dir, const char *interior_dir);
int input_str(const char *file, const char *word);
int search(const char *word, const char *text, int line, int all_search, const char *file);
Results *create_res(int n);
void free_res(Results *res);
unsigned int hash_f(const char *word);
void output_search(Results *res, const char *text, int n_word, const char *file);

#endif
