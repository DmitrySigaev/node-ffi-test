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
FFIEXPORT int read_scoring_matrix(struct tag_scoring_matrix_t *mtx, const char *matrixstring, size_t len);

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
