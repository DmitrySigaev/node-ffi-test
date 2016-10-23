/*
Copyright (C) 2016

Contact: Dmitry Sigaev <dima.sigaev@gmail.com>
*/

#ifndef _LAL_TABLES_H_
#define _LAL_TABLES_H_

extern const unsigned char lal_encode31[];

/*
* Contains the numerical representation of the complementary nucleotide symbol.
*/

extern const char cns[31];
extern const char lal_revers31_s[256];
extern const char lal_decode31_s[31];
extern char identity_nuc[];
extern char blosum100[];
extern char gaptest1[];

#endif /* _LAL_TABLES_H_ */
