/*
Copyright (C) 2016

Contact: Dmitry Sigaev <dima.sigaev@gmail.com>
*/

#ifndef _LAL_SCORING_MATRIX_H_
#define _LAL_SCORING_MATRIX_H_

#include <stdint.h>
#include "lal_matrix.h"

#define SCORE_MATRIX_DIM 32
#define MAX_LINE_LEN  1024
#define MAX_DOC_LEN   (MAX_LINE_LEN*4)

extern int8_t* score_matrix_8;
extern int16_t * score_matrix_16;
extern int64_t * score_matrix_64;
extern double * score_matrix_d;


typedef struct tag_scoring_matrix {
	matrix_t sc_double_matrix;
	matrix_t sc_int_matrix;
	double scale;
	double scaleback;  /* 1.0/scale */
	double man2mip[3];
	double gapOpen, gapExtend;
	char Doc[MAX_DOC_LEN];
} scoring_matrix_t;


int read_scoring_matrix(scoring_matrix_t *mtx, const char *matrixstring, size_t len);
void free_scoring_matrix(scoring_matrix_t *scmat);
void update_gap_scoring_matrix(scoring_matrix_t *mtx, double gapOpen, double gapExtend);
void gencore_mat_init_constant_scoring(scoring_matrix_t *scmat);


#endif /* _LAL_SCORING_MATRIX_ */