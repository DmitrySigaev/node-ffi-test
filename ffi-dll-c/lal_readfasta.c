/*
Copyright (C) 2017

Contact: Dmitry Sigaev <dima.sigaev@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lal_report.h"
#include "lal_typedefs.h"

/* currently we limit the size of readed sequences */
#define LAL_MAX_READ_REST_SEQUENCE INT_MAX
#define LAL_FASTA_LINE_ALLOC 4096
#define LAL_SIZE_OF_CHUNK 1048576 // 1MB
/* local variables */
static FILE * fp = NULL; /* fasta file descriptor */
static size_t sequences = LAL_MAX_READ_REST_SEQUENCE; /* calculate how many sequence the fasta file contains.*/
static char * seqdata = NULL;  /* continuous flow of seqences*/
static sequence_t  *seqindex = NULL; /*array of sequence_t that corespond to number of sequences*/
static size_t symbol_residues = 0;  /* the whole set of symbols*/
static size_t longest = 0; /*contains the length of the longest sequence.*/
static size_t longest_index = 0; /*contains the index of the longest sequence.*/



static inline void fasta_data_realloc(size_t *current, size_t new_size) {
	while (new_size >= *current) {
		*current += LAL_SIZE_OF_CHUNK;
		seqdata = realloc(seqdata, *current);
	}
}

static int fasta_read_header(char line[LAL_FASTA_LINE_ALLOC], size_t * dataalloc, size_t *datalen) {
	/* read header */
	if (line[0] != '>') {
		report_error("improper fasta file format");
		return 1;
	}

	size_t headerlen = strlen(line);

	// fgets stops at \n
	if (line[headerlen - 1] == '\n') {
		line[headerlen - 1] = 0;
		headerlen--;
	}
	headerlen--; // without the leading '>'
	return 0;
}


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

static int fasta_create_index(void) {
	seqindex = malloc(sequences * sizeof(sequence_t));
	if (!seqindex) {
		return -1;
	}
	sequence_t * seq_iterator = seqindex;

	char * data_iterator = seqdata;
	for (size_t i = 0; i < sequences; i++) {
		seq_iterator->ID = i;
		seq_iterator->seq = data_iterator;
		seq_iterator->len = strlen(data_iterator);
		data_iterator += seq_iterator->len + 1;
		seq_iterator++;
	}

	return 0;
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

	/* allocate space */
	size_t data_alloc = LAL_SIZE_OF_CHUNK;
	seqdata = malloc(data_alloc); /* todo: stress testing: Customers may use the software on computers that have significantly fewer computational resources */
	/* Stress testing tries to break the system under test by overwhelming its resources or by taking resources away from it (in which case it is sometimes called negative testing). The main purpose of this process is to make sure that the system fails and recovers gracefully—a quality known as recoverability. */
	size_t datalen = 0;

	longest = 0;
	sequences = 0;
	symbol_residues = 0;
	char line[LAL_FASTA_LINE_ALLOC];

	line[0] = 0;
	if (NULL == fgets(line, LAL_FASTA_LINE_ALLOC, fp)) {
		/* If a read error occurs, the error indicator (ferror) is set and a null pointer is also returned (but the contents pointed by str may have changed).*/
		report_error("some read error occur");
		return;
	}

	/* currently we limit the size of readed sequences */
	while (line[0] && (symbol_residues < LAL_MAX_READ_REST_SEQUENCE)) {
		if (fasta_read_header(line, &data_alloc, &datalen)) {
			return;
		}

		/* get next line */
		line[0] = 0;
		if (NULL == fgets(line, LAL_FASTA_LINE_ALLOC, fp)) {
			/* If a read error occurs, the error indicator (ferror) is set and a null pointer is also returned (but the contents pointed by str may have changed).*/
			report_error("some read error occur");
			break;
		}
		/* read sequence */
		size_t seqbegin = datalen;
		// reads until the next sequence header is found
		while (line[0] && (line[0] != '>')) {
			char c;
			char * p = line;
			while ((c = *p++)) {
				// there should check for illegal characters
				if (c != '\n') {
					fasta_data_realloc(&data_alloc, datalen);
					*(seqdata + datalen) = c;
					datalen++;
				}
			}

			/* get next line */
			line[0] = 0;
			if (NULL == fgets(line, LAL_FASTA_LINE_ALLOC, fp)) {
				/* If a read error occurs, the error indicator (ferror) is set and a null pointer is also returned (but the contents pointed by str may have changed).*/
				break;
			}
		}

		fasta_data_realloc(&data_alloc, datalen);

		size_t length = datalen - seqbegin;

		symbol_residues += length;
		*(seqdata + datalen) = '\0'; /* end of line \0 */
		datalen++;

		sequences++;
		if (length > longest) {
			longest = length;
			longest_index = sequences;
		}
	}

	/* does indices */
	fasta_create_index();

	fclose(fp);
	fp = NULL;
}

void fasta_close(void) {
	if (seqdata)
		free(seqdata);
	seqdata = NULL;
	if (seqindex)
		free(seqindex);
	seqindex = NULL;
	sequences = LAL_MAX_READ_REST_SEQUENCE;
	fp = NULL;
}

/* calculate how many sequences the fasta file contains.*/
size_t fasta_get_sequence_count(void) {
	switch (sequences) {
	case LAL_MAX_READ_REST_SEQUENCE:
		report_error("please, open a fasta file before using fasta_read function");
	case 0:
		report_error("please, call fasta_read function before");
		return 0;
		break;
	}
	return sequences; /* calculate how many sequence the fasta file contains.*/
}

/* calculate how many symbols at the whole set of sequences the fasta file contains.*/
size_t fasta_get_symbol_count(void) {
	switch (sequences) {
	case LAL_MAX_READ_REST_SEQUENCE:
		report_error("please, open a fasta file before using fasta_read function");
	case 0:
		report_error("please, call fasta_read function before");
		return 0;
		break;
	}
	return symbol_residues; /* calculate how many symbols at the whole set of sequences the fasta file contains.*/
}

/* returns the length of the longest sequence.*/
size_t fasta_get_longest_sequence(void) {
	switch (sequences) {
	case LAL_MAX_READ_REST_SEQUENCE:
		report_error("please, open a fasta file before using fasta_read function");
	case 0:
		report_error("please, call fasta_read function before");
		return 0;
		break;
	}
	return longest; /* returns the length of the longest sequence.*/
}

/* returns the longest sequence structure .*/
sequence_t * fasta_get_longest_seq_struct(void) {
	switch (sequences) {
	case LAL_MAX_READ_REST_SEQUENCE:
		report_error("please, open a fasta file before using fasta_read function");
	case 0:
		report_error("please, call fasta_read function before");
		return NULL;
		break;
	}
	return seqindex + longest_index; /* returns the longest sequence structure .*/
}

