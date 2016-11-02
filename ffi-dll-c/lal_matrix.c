/*
Copyright (C) 2016

Contact: Dmitry Sigaev <dima.sigaev@gmail.com>
*/

#include <stdio.h>
#include <float.h>
#include <malloc.h>
#include <string.h>
#include "lal_matrix.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

matrix_t matrix(const size_t nrows, const size_t ncols, enum MATTYPE type)
{
	switch (type) {
	case DOUBLETYPE: {
		double ** score_mat = (double **)malloc(nrows * sizeof(double *));
		if (NULL == score_mat) {
			return (matrix_t) { NULL, 0, 0, VOIDTYPE };
		}

		for (size_t i = 0; i < nrows; i++) {
			score_mat[i] = (double *)malloc(ncols * sizeof(double));
			if (NULL == score_mat[i]) {
				for (size_t j = 0; j < i; j++) {
					free(score_mat[j]);
				}
				free(score_mat);
				return (matrix_t) { NULL, 0, 0, VOIDTYPE };
			}
		}
		return (matrix_t) { (double **)score_mat, nrows, ncols, DOUBLETYPE };
	} break;
	case INTTYPE: {
		int64_t ** score_mat = (int64_t **)malloc(nrows * sizeof(int64_t *));
		if (NULL == score_mat) {
			return (matrix_t) { NULL, 0, 0, VOIDTYPE };
		}

		for (size_t i = 0; i < nrows; i++) {
			score_mat[i] = (int64_t *)malloc(ncols * sizeof(int64_t));
			if (NULL == score_mat[i]) {
				for (size_t j = 0; j < i; j++) {
					free(score_mat[j]);
				}
				free(score_mat);
				return (matrix_t) { NULL, 0, 0, VOIDTYPE };
			}
		}
		return (matrix_t) { (double **)score_mat, nrows, ncols, INTTYPE };
	}
	case CHARTYPE: {
		int8_t ** score_mat = (int8_t **)malloc(nrows * sizeof(int8_t *));
		if (NULL == score_mat) {
			return (matrix_t) { NULL, 0, 0, VOIDTYPE };
		}

		for (size_t i = 0; i < nrows; i++) {
			score_mat[i] = (int8_t *)malloc(ncols * sizeof(int8_t));
			if (NULL == score_mat[i]) {
				for (size_t j = 0; j < i; j++) {
					free(score_mat[j]);
				}
				free(score_mat);
				return (matrix_t) { NULL, 0, 0, VOIDTYPE };
			}
		}
		return (matrix_t) { (double **)score_mat, nrows, ncols, CHARTYPE };
	}
	}
	return (matrix_t) { NULL, 0, 0, VOIDTYPE };
}

int matrix_set(matrix_t *matrix, element_t value)
{
	printf("[ffi-dll-c] call matrix_set, type %d, in f: %s,l: %d\n", value.type, __FILE__, __LINE__);
	if (!matrix) return 0;
	printf("[ffi-dll-c] call matrix_set, mx %d, in f: %s,l: %d\n", !matrix, __FILE__, __LINE__);
	if (!matrix->ddata) return  0;
	printf("[ffi-dll-c] call matrix_set, mx->d %d, address: %x in f: %s,l: %d\n", !matrix->ddata, matrix->ddata, __FILE__, __LINE__);
	if (matrix->type != value.type) return 0;
	printf("[ffi-dll-c] call matrix_set, mx->t %d, in f: %s,l: %d\n", !matrix->type != value.type, __FILE__, __LINE__);

	switch (matrix->type) {
	case DOUBLETYPE: {
		printf("[ffi-dll-c] double *");
		double dd = value.d;
		for (size_t i = 0; i < matrix->nrows; i++) {
			printf("%x ", matrix->ddata[i]);
			if(!value.d)
				memset(matrix->ddata[i], 0, sizeof(double)*matrix->ncols);
			else
				for (size_t j = 0; j < matrix->ncols; j++) {
					matrix->ddata[i][j] = dd;// value.d;
					dd--;
				}
		}
		printf("\n");
		return 1;
	}
	case INTTYPE: {
		printf("[ffi-dll-c] int64 *");
		int64_t ii = value.i;
		for (size_t i = 0; i < matrix->nrows; i++) {
			printf("%x ", matrix->idata[i]);
			if (!value.i)
				memset(matrix->idata[i], 0, sizeof(int64_t)*matrix->ncols);
			else
				for (size_t j = 0; j < matrix->ncols; j++) {
					matrix->idata[i][j] = ii;// value.i;
					ii--;
				}
		}
		printf("\n");
		return 1;
	}
	case CHARTYPE: {
		printf("[ffi-dll-c] char *");
		char cc = value.c;
		for (size_t i = 0; i < matrix->nrows; i++) {
			printf("%x ", matrix->cdata[i]);
			if (!value.c)
			memset(matrix->cdata[i], 0, sizeof(int8_t)*matrix->ncols);
			else
				for (size_t j = 0; j < matrix->ncols; j++) {
					matrix->cdata[i][j] = cc;// value.c;
					cc--;
				}
		}
		printf("\n");
		return 1;
	}
	}
	return 0;
}

int matrix_set_value(matrix_t *matrix, size_t y, size_t x, element_t value)
{
	if (!matrix) return 0;
	if (!matrix->ddata) return  0;
	if (matrix->type != value.type) return 0;

	switch (matrix->type) {
	case DOUBLETYPE: {
		matrix->ddata[y][x] = value.d;
		return 1;
	}
	case INTTYPE: {
		matrix->idata[y][x] = value.i;
		return 1;
	}
	case CHARTYPE: {
		matrix->cdata[y][x] = value.c;
		return 1;
	}
	}
	return 0;
}

int matrix_or_bitwise(matrix_t *matrix, size_t y, size_t x, element_t value)
{
	if (!matrix) return 0;
	if (!matrix->ddata) return  0;
	if (matrix->type != value.type) return 0;

	switch (matrix->type) {
	case DOUBLETYPE: {
		return 0;
	}
	case INTTYPE: {
		matrix->idata[y][x] |= value.i;
		return 1;
	}
	case CHARTYPE: {
		matrix->cdata[y][x] |=  value.c;
		return 1;
	}
	}
	return 0;
}

element_t find_max(const matrix_t *matrix)
{
	if (!matrix) return (element_t) { 0, VOIDTYPE };
	if (!matrix->ddata) return  (element_t) { 0, VOIDTYPE };

	switch (matrix->type) {
	case DOUBLETYPE: {
		double score = matrix->ddata[0][0];
		for (size_t i = 0; i < matrix->nrows; i++) {
			for (size_t j = 0; j < matrix->ncols; j++) {
				if (score < matrix->ddata[i][j]) {
					score = matrix->ddata[i][j];
				}
			}
		}
		return (element_t) { (double)score, DOUBLETYPE };
	}
	case INTTYPE: {
		int64_t score = matrix->idata[0][0];
		for (size_t i = 0; i < matrix->nrows; i++) {
			for (size_t j = 0; j < matrix->ncols; j++) {
				if (score < matrix->idata[i][j]) {
					score = matrix->idata[i][j];
				}
			}
		}
		element_t ret = { 0.0, INTTYPE };
		ret.i = score;
		return ret;
	}
	case CHARTYPE: {
		int8_t score = matrix->cdata[0][0];
		for (size_t i = 0; i < matrix->nrows; i++) {
			for (size_t j = 0; j < matrix->ncols; j++) {
				if (score < matrix->cdata[i][j]) {
					score = matrix->cdata[i][j];
				}
			}
		}
		element_t ret = { 0.0, CHARTYPE };
		ret.c = score;
		return ret;
	}
	}
	return (element_t) { 0, VOIDTYPE };
}

void print_matrix(const matrix_t *matrix)
{
	if (!matrix) return;
	if (!matrix->ddata) return;
	switch (matrix->type) {
	case DOUBLETYPE: {
		double score = matrix->ddata[0][0];
		for (size_t i = 0; i < matrix->nrows; i++) {
			for (size_t j = 0; j < matrix->ncols; j++) {
				printf(" %2.2f", matrix->ddata[i][j]);
			}
			printf(" \n");
		}
	}break;
	case INTTYPE: {
		int64_t score = matrix->idata[0][0];
		for (size_t i = 0; i < matrix->nrows; i++) {
			for (size_t j = 0; j < matrix->ncols; j++) {
				printf(" %lld", matrix->idata[i][j]);
			}
			printf(" \n");
		}
	}
	case CHARTYPE: {
		int8_t score = matrix->cdata[0][0];
		for (size_t i = 0; i < matrix->nrows; i++) {
			for (size_t j = 0; j < matrix->ncols; j++) {
				printf(" %c", matrix->cdata[i][j]);
			}
			printf(" \n");
		}
	}
	}
	printf(" \n");
}

void free_matrix(matrix_t *matrix)
{
	if (!matrix)
		return;
	if (!matrix->ddata)
		return;
	for (size_t i = 0; i < matrix->nrows; i++) {
		free(matrix->ddata[i]);
	}
	free(matrix->ddata);
	matrix->ddata = NULL;
	matrix->nrows = 0;
	matrix->nrows = 0;
}

