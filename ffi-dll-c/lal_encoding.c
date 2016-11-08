/*
Copyright (C) 2016

Contact: Dmitry Sigaev <dima.sigaev@gmail.com>
*/
#include <stdio.h>
#include "lal_typedefs.h"
#include "lal_report.h"
#include "lal_encoding.h"

/**
* Returns a new created sequence, with the mapped values of the input sequence.
*
* @param inseq   the input sequence that should be coded
* @param enseq   the new coded sequence
* @param encode  the encode table
*/
void seq2encodedseq(sequence_t inseq, sequence_t enseq, const char* encode) {
	size_t illicit_symbol = 0;
	for (size_t i = 0; i < inseq.len; i++) {
		char cm;
		if ((cm = encode[(size_t)inseq.seq[i]]) >= 0) {
			enseq.seq[i] = cm;
		}
		else {
			enseq.seq[i] = LAL_NONEXISTENT;  /*if encode table has -1 value*/
			illicit_symbol++;
		}
	}
	if (illicit_symbol > 0) {
		report_warning("illicit symbols found and set to LAL_NONEXISTENT");
	}
	enseq.seq[enseq.len] = LAL_NONEXISTENT;
}

void lal_reverse(const char * source, int len, char *dest, const char *reverse_tab)
{
	char *ptmp;
	if (len <= 0) return;
	ptmp = dest + (len - 1);
	while (len--)
		*ptmp-- = reverse_tab[*source++];
}
