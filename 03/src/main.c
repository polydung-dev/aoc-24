#include <assert.h>
#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(arr) sizeof((arr)) / sizeof((arr)[0])

char* read_file(const char* path);
void parse_data(const char* data, int* output);
char* copy_match(regmatch_t* pmatch, const char* s, size_t idx);

int main(void) {
	char* buf = NULL;
	int output = 0;

	buf = read_file("./data/test.txt");
	parse_data(buf, &output);
	free(buf);

	printf("Sum of products: %i\n", output);

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
	fclose(fp);

	printf("bytes read == %lu\n", bytes_read);

	/* fread might not read all bytes at once, fix later if needed*/
	assert(byte_count == bytes_read);
	if (byte_count != bytes_read) {
		fprintf(stderr, "incomplete read\n");
		exit(1);
	}

	buf[byte_count] = '\0';

	return buf;
}

void parse_data(const char* data, int* output) {
	static const char* const re = "mul\\(([0-9]{1,3}),([0-9]{1,3})\\)";
	regex_t regex;
	regmatch_t pmatch[3]; /* whole match + two groups */
	const char* substr = NULL;
	size_t i = 0;

	if (regcomp(&regex, re, REG_EXTENDED)) {
		fprintf(stderr, "failed to compile regex\n");
		exit(1);
	}

	substr = data;
	printf("string = %s\n", data);
	printf("matches: \n");
	for (i = 0; ; ++i) {
		char* a = NULL;
		char* b = NULL;
		if (regexec(&regex, substr, ARRAY_SIZE(pmatch), pmatch, 0)) {
			break;
		}

		printf("#%lu: ", i);
		a = copy_match(pmatch, substr, 1);
		b = copy_match(pmatch, substr, 2);
		printf("a == %3s, b == %3s\n", a, b);

		*output += atoi(a) * atoi(b);

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
