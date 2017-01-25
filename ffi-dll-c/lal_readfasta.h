/*
Copyright (C) 2017 Dmitry Sigaev

Contact: Dmitry Sigaev <dima.sigaev@gmail.com>
*/

#ifndef _LAL_READFASTA_H_
#define _LAL_READFASTA_H_

int lal_seq_base_create(void);
void lal_seq_base_close(void);
int add_string(const char * string);
int add_fasta(const char * filename);

size_t fasta_get_sequence_count(void);
size_t fasta_get_symbol_count(void);
size_t fasta_get_longest_sequence(void);
sequence_t * fasta_get_longest_seq_struct(void);
sequence_t * fasta_get_seq_struct(size_t seqno);

#endif /* #define _LAL_READFASTA_H_ */
