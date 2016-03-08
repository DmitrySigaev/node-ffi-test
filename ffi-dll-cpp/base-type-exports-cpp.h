/* base-type-exports.h
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */

#if !defined(__BASE_TYPE_EXPORTS_CPP_H__)
#define __BASE_TYPE_EXPORTS_CPP_H__ 

#if defined(FFIDLL_EXPORTS)
	#define FFIEXPORT extern "C" __declspec(dllexport)
#else
	#define FFIEXPORT extern "C" __declspec(dllimport)
#endif

FFIEXPORT void voidFunc( void );
FFIEXPORT char charFunc(char inChar, char *outChar);
FFIEXPORT float floatFunc(float inFloat, float *outFloat);
FFIEXPORT double doubleFunc(double inDouble, double *outDouble);

#define CALLBACKFFI
typedef void (CALLBACKFFI *FFIPROC)(void);

FFIEXPORT struct tagffiAPI
{
	union { FFIPROC func; char *name = "voidFunc"; } voidFunc;
	union { char(*func)(char, char *); char *name = "charFunc"; } charFunc;
	union { float(*func)(float, float *); char *name = "floatFunc"; } floatFunc;
	union { double(*func)(double, double *); char *name = "doubleFunc"; } doubleFunc;
	const int size = (sizeof(struct tagffiAPI) - sizeof(const int)) / sizeof(void *);
};

FFIEXPORT struct tagffiAPIStatic
{
	struct { FFIPROC func; char *name = "voidFunc"; } voidF;
	struct { char(*func)(char, char *); char *name = "charFunc"; } charF;
	struct { float(*func)(float, float *); char *name = "floatFunc"; } floatF;
	struct { double(*func)(double, double *); char *name = "doubleFunc"; } doubleF;
};

FFIEXPORT struct tagffiAPIStatic LoadFFI(void);

#endif /* defined(__BASE_TYPE_EXPORTS_H__) */
