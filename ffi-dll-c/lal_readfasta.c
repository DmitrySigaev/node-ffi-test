/*
Copyright (C) 2017

Contact: Dmitry Sigaev <dima.sigaev@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lal_report.h"

/* currently we limit the size of readed sequences */
#define LAL_MAX_READ_REST_SEQUENCE INT_MAX
#define LAL_FASTA_LINE_ALLOC 4096
#define LAL_SIZE_OF_CHUNK 1048576 // 1MB
/* local variables */
static FILE * fp = NULL; /* fasta file descriptor */
static size_t sequences = LAL_MAX_READ_REST_SEQUENCE; /* calculate how many sequence the fasta file contains.*/

/*
 * The fasta_open function shall open the fasta file whose filename is the string pointed to by filename, and associates a stream with it.
*/
void fasta_open(const char * filename) {
	if (filename) {
		if (NULL == (fp = fopen(filename, "r"))) { /* Open a file for reading*/
			report_error("the file does not exist or cannot be read");
			return;
		}
	}
	else {
		report_error("please, check the string pointed to filename");
		return;
	}
	sequences = 0;
}

void fasta_read(void) {
	switch (sequences) {
	case LAL_MAX_READ_REST_SEQUENCE:
		report_error("please, open a fasta file before using fasta_read function");
		return;
	case 0:
		break;
	default:
		report_error("the fast file alredy readed");

	}
	if (NULL == fp) {
		report_error("please, check the fasta file descriptor");
		return;
	}

	fclose(fp);
	fp = NULL;
}

void fasta_close(void) {
	sequences = LAL_MAX_READ_REST_SEQUENCE;
	fp = NULL;
}

