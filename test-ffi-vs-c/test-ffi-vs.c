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

	memcpy(pffiAPI, ffiAPIin, sizeof(struct tagffiAPI));
	EXPECT(t, '#' == (ffiAPI.charFunc)('#', &cout));
	EXPECT(t, '#' == cout);
	EXPECT(t, fabs(12.13 - ffiAPI.floatFunc(12.13, &fout)) < EPSILON);
	EXPECT(t, fabs(12.13 - fout) < EPSILON);
	EXPECT(t, fabs(12.13 - ffiAPI.doubleFunc(12.13, &dout)) < EPSILON);
	EXPECT(t, fabs(12.13 - dout) < EPSILON);
	EXPECT(t, true == ffiAPI.boolFunc(true, &bout));
	WEXPECT(t, 0x263B == (ffiAPI.wcharFunc)(0x263B, &wout)); // '☻'
	WEXPECT(t, 0x263B == wout);
	EXPECT(t, true == bout);
	EXPECT(t, true == ffiAPI.read_scoring_matrix_func((struct tag_scoring_matrix_t *)&mtx, gaptest_utest, strlen(gaptest_utest)));
	if(read_scoring_matrix_f)
		EXPECT(t, true == read_scoring_matrix_f((struct tag_scoring_matrix_t *)&mtx, gaptest_utest, strlen(gaptest_utest)));
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
		struct tagffiAPI(* lffi)(void);
	    lffi = (struct tagffiAPI(*)(void))GetProcAddress(hinstLib, "LoadFFI");
		ffiAPI = lffi();
		read_scoring_matrix_f = (FFIPROC)GetProcAddress(hinstLib, "read_scoring_matrix");
		testFFIAPI(&ffiAPI);
		fFreeResult = FreeLibrary(hinstLib);
	}

	// If unable to call the DLL function, use an alternative.
	if (!fRunTimeLinkSuccess)
		printf("end of tests\n");

	return 0;
}
