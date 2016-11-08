/*
Copyright (C) 2016 Dmitry Sigaev

Contact: Dmitry Sigaev <dima.sigaev@gmail.com>
*/


#ifndef _LAL_ENCODING_H_
#define _LAL_ENCODING_H_

#include <stdint.h>

#define LAL_NONEXISTENT (unsigned char)31

void seq2encodedseq(sequence_t inseq, sequence_t enseq, const char* encode);
void lal_reverse(const char * source, int len, char *dest, const char *reverse_tab);

#endif /* _LAL_ENCODING_H_ */