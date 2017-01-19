/* test-ffi-vs.c
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <windows.h> 
#include <stdbool.h>
#include <stdint.h>

#include "base-type-exports-c.h"

#define EPSILON 0.00001
#define EXPECT(test, x) ((x))?(printf(#x":passed\n"), test&=1):(printf(#x":fault, f: %s, l: %d\n", __FILE__, __LINE__), test&=0)
#define WEXPECT(test, x) ((x))?(wprintf(L#x":passed\n"), test&=1):(wprintf(L#x":fault, f: %s, l: %d\n", __FILE__, __LINE__), test&=0)

int(*read_scoring_matrix_f)(struct tag_scoring_matrix_t *mtx, const char *matrixstring, size_t len);
int(*read_scoring_matrix_f_js)(ptrdiff_t *mtx, const char *matrixstring, size_t len);

enum MATTYPE_UTEST { VOIDTYPE, DOUBLETYPE, INTTYPE, FLOATTYPE, CHARTYPE };
/**
*  Structure of a matrix[nrows][ncols]
*  @typedef {struct} matrix_t
*  @field data  matrix data
*  @field nrows the number of rows
*  @field ncols the number of columns
*/
typedef struct tag_matrix_utest {
	union {
		double **ddata;
		int64_t **idata;
		int8_t  **cdata;
	};
	size_t nrows;
	size_t ncols;
	enum MATTYPE_UTEST type;
} matrix_utest;

typedef struct tag_sequence_utest {
	size_t ID;
	char * seq;
	size_t len;
}sequence_utest;

typedef struct tag_mat {
	double **ddata;
	size_t nrows;
	size_t ncols;
	int type;
}mat_t;

typedef struct  tag_search_profile_utest {
	double gapOpen;
	double gapExt;
	struct tag_scoring_matrix_api *mtx;
}search_profile_utest;

typedef struct tag_score_matrix {
	struct tag_matrix_api score;
	struct tag_matrix_api directions;
}score_matrix_utest;

score_matrix_utest(*sw_directions_f_js)(struct  search_swag_profile_api const * sp, struct sequence_api const *xseq, struct sequence_api const *yseq);
score_matrix_utest(*sw_genc_m_f_js)(struct  search_swag_profile_api const * sp, struct sequence_api const *xseq, struct sequence_api const *yseq);
mat_t(*matrix_f_js)(const size_t nrows, const size_t ncols, int type);
matrix_utest(*matrix_f_js2)(const size_t nrows, const size_t ncols, int type);
matrix_utest * (*matrix_fd_js)(const size_t nrows, const size_t ncols, int type);
int * (*matrix_fi_js)(const size_t nrows, const size_t ncols, int type);
void(*encode_seq_f_js)(struct sequence_api const *s, struct sequence_api const *d);
void(*fasta_open_f_js)(const char * filename);
void(*fasta_close_f_js)(void);
void(*fasta_read_f_js)(void);
size_t(*fasta_get_sequence_count_f_js)(void);

#define MAX_LINE_LEN_UTEST  1024
#define MAX_DOC_LEN_UTEST   (MAX_LINE_LEN_UTEST*4)

typedef struct tag_scoring_matrix_utest {
	matrix_utest sc_double_matrix;
	matrix_utest sc_int_matrix;
	double scale;
	double scaleback;  /* 1.0/scale */
	double man2mip[3];
	double gapOpen, gapExtend;
	char Doc[MAX_DOC_LEN_UTEST];
} scoring_matrix_utest;

char gaptest_utest[] = { " #gaptest1.table                                          \n \
    A    B    C    D    G    H    K    M    R    S    T    U    V    W    Y    \n \
A  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
B -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
C -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
D -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
G -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
H -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
K -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
M -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
R -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
S -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
T -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0  1.0 -1.0 -1.0 -1.0   \n \
U -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  1.0  2.0 -1.0 -1.0 -1.0   \n \
V -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0   \n \
W -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0   \n \
Y -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0" };

typedef struct tag_scoring_matrix_api sm_t;
bool testFFIAPI(struct tagffiAPI*ffiAPIin)
{
	bool t = true;
	struct tagffiAPI ffiAPI;
	struct tagffiAPI *pffiAPI = &ffiAPI;
	char cout;
	wchar_t wout;
	float fout;
	double dout;
	bool bout;
	scoring_matrix_utest mtx;
	size_t szofenum = sizeof(enum MATTYPE_UTEST);
	matrix_utest mx = matrix_f_js2(10, 10, 1);
	mat_t mmm = matrix_f_js(10, 10, 1);
	matrix_utest *dmx = matrix_fd_js(10, 10, 1);
	int *imx = matrix_fi_js(10, 10, 1);

	fasta_open_f_js("fasta.fasta");
	fasta_read_f_js();
	size_t seqcount = fasta_get_sequence_count_f_js();
	fasta_close_f_js();
	fasta_read_f_js();
	size_t seqcount2 = fasta_get_sequence_count_f_js();



	int status = read_scoring_matrix_f_js(&mtx, gaptest_utest, strlen(gaptest_utest));
	char seq1[] = { "CAACTTCCTGGCGCTATCACTTCTACCATCGTCTGCAGCGT" };
	size_t len1 = strlen(seq1);
	char seq2[] = { "acgatggtagaagtgatagcgccagttgctccacccct" };
	//char seq2[]={ "TCGTACGCTGCAGACGATGGTAGAAGTGATAGCGCCAGTTGCTCCACCCCTCCGTAGGCATTGCCCACGCCGCACTACTATGACCCAACGTAGGAAGTTG" };
	size_t len2 = strlen(seq2);
	sequence_utest inseq1 = { 1, (char *)seq1, len1 };
	sequence_utest inseq2 = { 2, (char *)seq2, len2 };
	sequence_utest enseq1 = { 1, malloc(len1 + 1), len1 };
	sequence_utest enseq2 = { 2, malloc(len2 + 1), len2 };
	encode_seq_f_js(&inseq1, &enseq1);
	encode_seq_f_js(&inseq2, &enseq2);
	search_profile_utest sp = { -1.6, -0.3, (!status) ? (NULL) : (&mtx) };
	score_matrix_utest sd = sw_directions_f_js(&sp, &enseq1, &enseq2);
	score_matrix_utest sdgc = sw_genc_m_f_js(&sp, &enseq1, &enseq2);

	//  element_t score = find_max(&sd.score);
	//	ck_assert_int_eq((int)score.d, 33); /* Max score */ // ok


	memcpy(pffiAPI, ffiAPIin, sizeof(struct tagffiAPI));
	EXPECT(t, '#' == (ffiAPI.charFunc)('#', &cout));
	EXPECT(t, '#' == cout);
	EXPECT(t, fabs(12.13 - ffiAPI.floatFunc((float)12.13, &fout)) < EPSILON);
	EXPECT(t, fabs(12.13 - fout) < EPSILON);
	EXPECT(t, fabs(12.13 - ffiAPI.doubleFunc(12.13, &dout)) < EPSILON);
	EXPECT(t, fabs(12.13 - dout) < EPSILON);
	EXPECT(t, true == ffiAPI.boolFunc(true, &bout));
	WEXPECT(t, 0x263B == (ffiAPI.wcharFunc)(0x263B, &wout)); // '☻'
	WEXPECT(t, 0x263B == wout);
	EXPECT(t, true == bout);
	EXPECT(t, true == ffiAPI.read_scoring_matrix_func((struct tag_scoring_matrix_t *)&mtx, gaptest_utest, strlen(gaptest_utest)));
	if (read_scoring_matrix_f)
		EXPECT(t, true == read_scoring_matrix_f((struct tag_scoring_matrix_t *)&mtx, gaptest_utest, strlen(gaptest_utest)));
	if (read_scoring_matrix_f_js)
		EXPECT(t, true == read_scoring_matrix_f_js((ptrdiff_t *)&mtx, gaptest_utest, strlen(gaptest_utest)));
	EXPECT(t, t == true);
	return true;
}

/* main */
int main(int argc, char **argv)
{
	HINSTANCE hinstLib;
	BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

	// Get a handle to the DLL module.

	hinstLib = LoadLibrary(TEXT("ffi-dll-c.dll"));

	// If the handle is valid, try to get the function address.
	if (hinstLib != NULL)
	{
		struct tagffiAPI ffiAPI;
		struct tagffiAPI(*lffi)(void);
		lffi = (struct tagffiAPI(*)(void))GetProcAddress(hinstLib, "LoadFFI");
		ffiAPI = lffi();
		read_scoring_matrix_f = (FFIPROC)GetProcAddress(hinstLib, "read_scoring_matrix");
		read_scoring_matrix_f_js = (FFIPROC)GetProcAddress(hinstLib, "read_scoring_matrix_js");
		matrix_f_js2 = (FFIPROC)GetProcAddress(hinstLib, "matrix_js");
		matrix_f_js = (FFIPROC)GetProcAddress(hinstLib, "matrix_js");
		matrix_fd_js = (FFIPROC)GetProcAddress(hinstLib, "matrix_js_d");
		matrix_fi_js = (FFIPROC)GetProcAddress(hinstLib, "matrix_js_i");
		encode_seq_f_js = (FFIPROC)GetProcAddress(hinstLib, "encode_seq_js");
		sw_directions_f_js = (FFIPROC)GetProcAddress(hinstLib, "sw_directions_js");
		sw_genc_m_f_js = (FFIPROC)GetProcAddress(hinstLib, "sw_genc_m_js");
		fasta_open_f_js = (FFIPROC)GetProcAddress(hinstLib, "fasta_open_js");
		fasta_close_f_js = (FFIPROC)GetProcAddress(hinstLib, "fasta_close_js");
		fasta_read_f_js = (FFIPROC)GetProcAddress(hinstLib, "fasta_read_js");
		fasta_get_sequence_count_f_js = (FFIPROC)GetProcAddress(hinstLib, "fasta_get_sequence_count_js");
		testFFIAPI(&ffiAPI);
		fFreeResult = FreeLibrary(hinstLib);
	}

	// If unable to call the DLL function, use an alternative.
	if (!fRunTimeLinkSuccess)
		printf("end of tests\n");

	return 0;
}
