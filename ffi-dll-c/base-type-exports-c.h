/* base-type-exports.h
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

#define CALLBACKFFI
typedef void (CALLBACKFFI *FFIPROC)(void);

struct tagffiAPI
{
	FFIPROC voidFunc;
	char(*charFunc)(char, char *);
	float(*floatFunc)(float, float *);
	double (*doubleFunc)(double, double *);
};
FFIEXPORT struct tagffiAPI LoadFFI(void);

#endif /* defined(__BASE_TYPE_EXPORTS_H__) */