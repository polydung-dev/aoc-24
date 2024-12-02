#include "da.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DA_int {
	int* data;
	size_t size;
	size_t capacity;
};

struct DA_da_int {
	struct DA_int** data;
	size_t size;
	size_t capacity;
};

enum ReportStatus {
	REPORT_OK = 0,
	REPORT_NO_CHANGE,
	REPORT_DELTA_TOO_BIG,
	REPORT_BAD_DELTA
};

char* report_strerr(enum ReportStatus status);

void load_from_file(const char* path, struct DA_da_int* reports);
int check_report(struct DA_int* report, int* err_idx);

int main(void) {
	size_t i = 0;
	int status;
	unsigned long safe_sum = 0;
	struct DA_da_int* reports = NULL;

	da_create(reports);

	load_from_file("./data/test.txt", reports);

	printf("Number of reports to check: %lu\n", reports->size);

	safe_sum = 0;
	for (i = 0; i < reports->size; ++i) {
		int err_idx = 0;

		printf("--------------------\n");
		printf("Checking report %4lu\n", i);

		printf("Report : ");
		da_print((reports->data[i]), "%3i");

		status = check_report(reports->data[i], &err_idx);
		if (status == REPORT_OK) {
			++safe_sum;
			printf("[ pass ]\n");
		} else {
			printf("%*c\n", 13 + err_idx + (err_idx * 4), '^');
			printf("[ fail ] %s\n", report_strerr(status));
		}
		printf("\n");
	}

	printf("Number of safe reports: %lu\n", safe_sum);


	/* destroy each report */
	for (i = 0; i < reports->size; ++i) {
		da_destroy(reports->data[i]);
	}
	da_destroy(reports);

	return 0;
}

char* report_strerr(enum ReportStatus status) {
	switch (status) {
		case REPORT_OK:            return "report ok";
		case REPORT_NO_CHANGE:     return "no change";
		case REPORT_DELTA_TOO_BIG: return "delta too big";
		case REPORT_BAD_DELTA:     return "bad delta";
		default:                   return "???";
	}
}

void load_from_file(const char* path, struct DA_da_int* reports) {
	size_t buf_sz = 256;
	char* buf = NULL;
	FILE* fp = NULL;

	buf = malloc(buf_sz);
	if (buf == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}

	fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(
			stderr, "Could not open file `%s`: %s\n",
			path, strerror(errno)
		);
		exit(1);
	}

	/* split report string into int array */
	while (fgets(buf, buf_sz, fp) != NULL) {
		struct DA_int* report = NULL;
		char* s = NULL;

		da_create(report);
		da_reserve(report, 4);

		s = strtok(buf, " ");
		while (s != NULL) {
			int n = atoi(s);
			da_append(report, n);
			s = strtok(NULL, " ");
		}
		da_append(reports, report);
	}

	fclose(fp);
	free(buf);
}

int check_report(struct DA_int* report, int* err_idx) {
	int status = REPORT_OK;
	size_t i = 0;
	int direction = 0;
	struct DA_int* deltas = NULL;
	*err_idx = 0;

	da_create(deltas);

	for (i = 0; i < report->size - 1; ++i) {
		int d = report->data[i] - report->data[i + 1];
		da_append(deltas, d);
	}

	direction = (deltas->data[0] > 0) ? 1 : -1;

	for (i = 0; i < deltas->size; ++i) {
		int d = (deltas->data[i] > 0) ? 1 : -1;

		if (deltas->data[i] == 0) {
			status = REPORT_NO_CHANGE;
			goto abort;
		}
		if (abs(deltas->data[i]) > 3) {
			status = REPORT_DELTA_TOO_BIG;
			goto abort;
		}
		if (d != direction) {
			status = REPORT_BAD_DELTA;
			goto abort;
		}
	}

exit:
	da_destroy(deltas);
	return status;
abort:
	*err_idx = i + 1;
	goto exit;
}
