/* base-type-exports-c.h
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */

#if !defined(__BASE_TYPE_EXPORTS_C_H__)
#define __BASE_TYPE_EXPORTS_C_H__ 

#if defined(FFIDLL_EXPORTS)
	#define FFIEXPORT extern __declspec(dllexport)
#else
	#define FFIEXPORT extern __declspec(dllimport)
#endif

FFIEXPORT void voidFunc( void );
FFIEXPORT char charFunc(char inChar, char *outChar);
FFIEXPORT float floatFunc(float inFloat, float *outFloat);
FFIEXPORT double doubleFunc(double inDouble, double *outDouble);
FFIEXPORT bool boolFunc(bool inBool, bool *outBool);
FFIEXPORT wchar_t wcharFunc(wchar_t inWchar, wchar_t *outWchar);



struct tag_matrix_api {
	double **ddata;
	size_t nrows;
	size_t ncols;
	int type;
};

FFIEXPORT struct tag_matrix_api matrix_js(const size_t nrows, const size_t ncols, int type);
FFIEXPORT struct tag_matrix_api *matrix_js_d(const size_t nrows, const size_t ncols, int type);
FFIEXPORT int *matrix_js_i(const size_t nrows, const size_t ncols, int type);
FFIEXPORT int matrix_set_int(struct tag_matrix_api *matrix, const int value);
FFIEXPORT int matrix_set_double(struct tag_matrix_api *matrix, const double value);
FFIEXPORT int matrix_set_char(struct tag_matrix_api *matrix, const char value);

#define MAX_LINE_LEN_API  1024
#define MAX_DOC_LEN_API   (MAX_LINE_LEN_API*4)

struct tag_scoring_matrix_api {
	struct tag_matrix_api sc_double_matrix;
	struct tag_matrix_api sc_int_matrix;
	double scale;
	double scaleback;  /* 1.0/scale */
	double man2mip[3];
	double gapOpen, gapExtend;
	char Doc[MAX_DOC_LEN_API];
};

FFIEXPORT int read_scoring_matrix_js(struct tag_scoring_matrix_api * mtx, const char *matrixstring, size_t len);

#define CALLBACKFFI
typedef void (CALLBACKFFI *FFIPROC)(void);

struct tagffiAPI
{
	FFIPROC voidFunc;
	char(*charFunc)(char, char *);
	float(*floatFunc)(float, float *);
	double (*doubleFunc)(double, double *);
	bool(*boolFunc)(bool, bool *);
	wchar_t(*wcharFunc)(wchar_t, wchar_t *);
	int (* read_scoring_matrix_func)(struct tag_scoring_matrix_t *mtx, const char *matrixstring, size_t len);
};
FFIEXPORT struct tagffiAPI LoadFFI(void);

#endif /* defined(__BASE_TYPE_EXPORTS_H__) */
