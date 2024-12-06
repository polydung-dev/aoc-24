#include <assert.h>
#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(arr) sizeof((arr)) / sizeof((arr)[0])

enum NodeType {
	NODE_NULL = 0,
	NODE_MUL,
	NODE_DO,
	NODE_DONT
};

struct Node {
	enum NodeType node_type;
	int a;
	int b;
	struct Node* next;
};

char* read_file(const char* path);
void parse_data(const char* data, struct Node** output);
char* copy_match(regmatch_t* pmatch, const char* s, size_t idx);

void ll_free(struct Node* node) {
	struct Node* tmp = NULL;

	/* iterate through list, freeing on the way */
	while (node != NULL) {
		tmp = node->next;
		free(node);
		node = tmp;
	}
}

int main(void) {
	char* buf = NULL;
	struct Node* output = NULL;
	struct Node* node = NULL;
	int sum = 0;
	int controlled_sum = 0;
	int do_sum = 1;

	buf = read_file("./data/test.txt");
	parse_data(buf, &output);
	free(buf);

	node = output;
	while (node != NULL) {
		switch (node->node_type) {
			case NODE_MUL:
				if (do_sum) {
					controlled_sum += (node->a * node->b);
				}
				sum += (node->a * node->b);
				break;

			case NODE_DO:
				do_sum = 1;
				break;

			case NODE_DONT:
				do_sum = 0;
				break;

			case NODE_NULL:
			default:
				break;

		}
		node = node->next;
	}

	printf("Sum of products: %i\n", sum);
	printf("Sum of products (controlled): %i\n", controlled_sum);

	ll_free(output);
	return 0;
}

char* read_file(const char* path) {
	size_t byte_count = 0;
	size_t bytes_read = 0;
	char* buf = NULL;
	FILE* fp = NULL;

	fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(
			stderr, "Could not open file `%s`: %s\n",
			path, strerror(errno)
		);
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	byte_count = ftell(fp);
	rewind(fp);

	printf("num bytes == %lu\n", byte_count);

	buf = malloc(byte_count + 1);
	if (buf == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}

	bytes_read = fread(buf, 1, byte_count, fp);
	buf[byte_count] = '\0';

	printf("bytes read == %lu\n", bytes_read);

	/* fread doesn't set errno? */
	assert(byte_count == bytes_read);
	if (byte_count != bytes_read && !feof(fp)) {
		fprintf(stderr, "error reading file %i\n", ferror(fp));
		fclose(fp);
		exit(1);
	}

	fclose(fp);
	return buf;
}

void parse_data(const char* data, struct Node** output) {
	static const char* const re = \
		"mul\\(([0-9]{1,3}),([0-9]{1,3})\\)|do\\(\\)|don't\\(\\)";

	regex_t regex;
	regmatch_t pmatch[3]; /* whole match + two groups */
	const char* substr = NULL;
	size_t i = 0;
	struct Node* node = calloc(1, sizeof(struct Node));
	*output = node;

	if (regcomp(&regex, re, REG_EXTENDED)) {
		fprintf(stderr, "failed to compile regex\n");
		exit(1);
	}

	substr = data;
	printf("string = %s\n", data);
	printf("matches: \n");
	for (i = 0; ; ++i) {
		char* m = NULL;
		char* a = NULL;
		char* b = NULL;
		if (regexec(&regex, substr, ARRAY_SIZE(pmatch), pmatch, 0)) {
			break;
		}

		printf("command %lu: ", i + 1);
		m = copy_match(pmatch, substr, 0);
		printf("%s\n", m);

		if (strncmp(m, "mul(", 4) == 0) {
			node->node_type = NODE_MUL;
			a = copy_match(pmatch, substr, 1);
			b = copy_match(pmatch, substr, 2);
			node->a = atoi(a);
			node->b = atoi(b);
			node->next = calloc(1, sizeof(struct Node));
			node = node->next;
		} else if (strncmp(m, "do()", 4) == 0) {
			node->node_type = NODE_DO;
			node->next = calloc(1, sizeof(struct Node));
			node = node->next;
		} else if (strncmp(m, "don't()", 7) == 0) {
			node->node_type = NODE_DONT;
			node->next = calloc(1, sizeof(struct Node));
			node = node->next;
		}

		free(m);
		free(a);
		free(b);

		substr += pmatch[0].rm_eo;
	}

	regfree(&regex);
}

char* copy_match(regmatch_t* pmatch, const char* s, size_t idx) {
	size_t len = pmatch[idx].rm_eo - pmatch[idx].rm_so;
	char* mstr = malloc(len + 1);

	memcpy(mstr, s + pmatch[idx].rm_so, len);
	mstr[len] = '\0';

	return mstr;
}
