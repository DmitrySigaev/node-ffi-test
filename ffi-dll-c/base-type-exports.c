/* base-type-exports.c
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include "base-type-exports-c.h"
#include "lal_scoring_matrix.h"
#include "lal_typedefs.h"
#include "sw.h"
#include "gc_sw.h"
#include "lal_tables.h"
#include "lal_encoding.h"
#include "lal_readfasta.h"

void voidFunc(void)
{
	printf("[ffi-dll-c] call voidFunc in f: %s,l: %d\n", __FILE__, __LINE__);
}

char charFunc(char inChar, char *outChar)
{
	printf("[ffi-dll-c] call charFunc %c, in f: %s,l: %d\n", inChar, __FILE__, __LINE__);
	*outChar = inChar;
	return inChar;
}

float floatFunc(float inFloat, float *outFloat)
{
	printf("[ffi-dll-c] call floatFunc %f, in f: %s,l: %d\n", inFloat, __FILE__, __LINE__);
	*outFloat = inFloat;
	return inFloat;
}

double doubleFunc(double inDouble, double *outDouble)
{
	printf("[ffi-dll-c] call doubleFunc %f, in f: %s,l: %d\n", inDouble, __FILE__, __LINE__);
	*outDouble = inDouble;
	return inDouble;
}

bool boolFunc(bool inBool, bool *outBool)
{
	printf("[ffi-dll-c] call boolFunc %d, in f: %s,l: %d\n", inBool, __FILE__, __LINE__);
	*outBool = inBool;
	return inBool;
}

wchar_t wcharFunc(wchar_t inWchar, wchar_t *outWchar)
{
	wprintf(L"[ffi-dll-c] call wcharFunc %C, in f: %s, l: %d\n", inWchar, __FILE__, __LINE__);
	*outWchar = inWchar;
	return inWchar;
}

int read_scoring_matrix_js(struct tag_scoring_matrix_api * mtx, const char *matrixstring, size_t len)
{
	return read_scoring_matrix((scoring_matrix_t *)mtx, matrixstring, len);
}


struct tag_matrix_api matrix_js(const size_t nrows, const size_t ncols, int type)
{
	struct tag_matrix_api *mat;
	matrix_t mx = matrix(nrows, ncols, type);
	mat = (struct tag_matrix_api *)&mx;
	printf("[ffi-dll-c] call matrix_js:type %d, address %x,  in f: %s,l: %d\n", type, mx.ddata, __FILE__, __LINE__);
	return *mat;
}

struct tag_matrix_api * matrix_js_d(const size_t nrows, const size_t ncols, int type)
{
	struct tag_matrix_api *mat = malloc(sizeof(matrix_t));
	matrix_t mx = matrix(nrows, ncols, type);
	memcpy(mat, &mx, sizeof(matrix_t));
	return mat;
}

int * matrix_js_i(const size_t nrows, const size_t ncols, int type)
{
	int *mat = malloc(sizeof(matrix_t));
	matrix_t mx = matrix(nrows, ncols, type);
	memcpy(mat, &mx, sizeof(matrix_t));
	return mat;
}

int matrix_set_int(struct tag_matrix_api *matrix, const int value)
{
	printf("[ffi-dll-c] call matrix_set_int:val %d, in f: %s,l: %d\n", value, __FILE__, __LINE__);
	element_t el = { 0, INTTYPE };
	el.i = value;
	return matrix_set((matrix_t *)matrix, el);
}

int matrix_set_double(struct tag_matrix_api *matrix, const double value)
{
	element_t el = { 0, DOUBLETYPE };
	el.d = value;
	return matrix_set((matrix_t *)matrix, el);
}

int matrix_set_char(struct tag_matrix_api *matrix, const char value)
{
	element_t el = { 0, CHARTYPE };
	el.c = value;
	return matrix_set((matrix_t *)matrix, el);
}

struct score_matrix_api sw_directions_js(struct  search_swag_profile_api const * sp, struct sequence_api const *xseq, struct sequence_api const *yseq)
{
	printf("[ffi-dll-c] call sw_directions in f: %s,l: %d\n", __FILE__, __LINE__);
	printf("[ffi-dll-c] call sw_directions:val %s, in f: %s,l: %d\n", xseq->seq, __FILE__, __LINE__);
	printf("[ffi-dll-c] call sw_directions:val %s, in f: %s,l: %d\n", yseq->seq, __FILE__, __LINE__);
	struct score_matrix_api *sm;
	score_matrix_t smt = sw_directions((const search_swag_profile_t *)sp, (const sequence_t *)xseq, (const sequence_t *)yseq);
	sm = (struct score_matrix_api *)&smt;
	return *sm;
}

void encode_seq_js(struct sequence_api const *s, struct sequence_api const *d) {
	seq2encodedseq(*(sequence_t *)s, *(sequence_t *)d, lal_encode31);
}

double sw_genc_js(struct  search_swag_profile_api const * sp, struct sequence_api const *xseq, struct sequence_api const *yseq) {
	return sw_gencore((const search_swag_profile_t *)sp, (const sequence_t *)xseq, (const sequence_t *)yseq);
}

double sw_genc_js_sigaev(struct  search_swag_profile_api const * sp, struct sequence_api const *xseq, struct sequence_api const *yseq) {
	return sw_genc_sigaev((const search_swag_profile_t *)sp, (const sequence_t *)xseq, (const sequence_t *)yseq);
}

struct score_matrix_api sw_genc_m_js(struct  search_swag_profile_api const * sp, struct sequence_api const *xseq, struct sequence_api const *yseq)
{
	printf("[ffi-dll-c] call sw_genc_m_js in f: %s,l: %d\n", __FILE__, __LINE__);
	printf("[ffi-dll-c] call sw_genc_m_js:val %s, in f: %s,l: %d\n", xseq->seq, __FILE__, __LINE__);
	printf("[ffi-dll-c] call sw_genc_m_js:val %s, in f: %s,l: %d\n", yseq->seq, __FILE__, __LINE__);
	struct score_matrix_api *sm;
	score_matrix_t smt = sw_genc_directions((const search_swag_profile_t *)sp, (const sequence_t *)xseq, (const sequence_t *)yseq);
	sm = (struct score_matrix_api *)&smt;
	return *sm;
}

void fasta_open_js(const char * filename)
{
	fasta_open(filename);
}

void fasta_close_js(void)
{
	fasta_close();
}

void fasta_read_js(void)
{
	fasta_read();
}

size_t fasta_get_sequence_count_js(void) {
	return fasta_get_sequence_count();
}

size_t fasta_get_symbol_count_js(void) {
	return fasta_get_symbol_count();
}

size_t fasta_get_longest_sequence_js(void) {
	return fasta_get_longest_sequence();
}

struct sequence_api const * fasta_get_longest_seq_struct_js(void) {
	return (struct sequence_api const *)fasta_get_longest_seq_struct();
}

struct tagffiAPI LoadFFI(void)
{
	struct tagffiAPI FFI;
	FFI.voidFunc = voidFunc;
	FFI.charFunc = charFunc;
	FFI.floatFunc = floatFunc;
	FFI.doubleFunc = doubleFunc;
	FFI.boolFunc = boolFunc;
	FFI.wcharFunc = wcharFunc;
	FFI.read_scoring_matrix_func = read_scoring_matrix;
	printf("[ffi-dll] call LoadFFI in f: %s,l: %d\n", __FILE__, __LINE__);
	return FFI;
}
