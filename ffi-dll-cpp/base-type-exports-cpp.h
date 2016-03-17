/* base-type-exports-cpp.h
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

#ifndef __byte_typedef__
#define __byte_typedef__
typedef unsigned char byte;
#endif

FFIEXPORT void voidFunc( void );
FFIEXPORT char charFunc(char inChar, char *outChar);
FFIEXPORT float floatFunc(float inFloat, float *outFloat);
FFIEXPORT double doubleFunc(double inDouble, double *outDouble);
FFIEXPORT bool boolFunc(bool inBool, bool *outBool);
FFIEXPORT wchar_t wcharFunc(wchar_t inWchar, wchar_t *outWchar);
FFIEXPORT float * tmpXYZ(float x, float y, float z);
FFIEXPORT int testSerialize(const char *str, byte **buf, int *size);
FFIEXPORT char* testUnserialize(const byte *buf, int size);
FFIEXPORT char* tmpUnserialize(void);


#define CALLBACKFFI
typedef void (CALLBACKFFI *FFIPROC)(void);

FFIEXPORT struct tagffiAPI
{
	union { FFIPROC func; char *name = "voidFunc"; } voidFunc;
	union { char(*func)(char, char *); char *name = "charFunc"; } charFunc;
	union { float(*func)(float, float *); char *name = "floatFunc"; } floatFunc;
	union { double(*func)(double, double *); char *name = "doubleFunc"; } doubleFunc;
	union { bool(*func)(bool, bool *); char *name = "boolFunc"; } boolFunc;
	union { wchar_t(*func)(wchar_t, wchar_t *); char *name = "wcharFunc"; } wcharFunc;
	const int size = (sizeof(struct tagffiAPI) - sizeof(const int)) / sizeof(void *);
};

FFIEXPORT struct tagffiAPIStatic
{
	struct { FFIPROC func; char *name = "voidFunc"; } voidF;
	struct { char(*func)(char, char *); char *name = "charFunc"; } charF;
	struct { float(*func)(float, float *); char *name = "floatFunc"; } floatF;
	struct { double(*func)(double, double *); char *name = "doubleFunc"; } doubleF;
	struct { bool(*func)(bool, bool *); char *name = "boolFunc"; } boolF;
	struct { wchar_t(*func)(wchar_t, wchar_t *); char *name = "wcharFunc"; } wcharF;
	struct { float * (*func)(float x, float y, float z); char *name = "tmpXYZ"; } tmpXYZ;
	struct { int(*func)(const char *str, byte **buf, int *size); char *name = "testSerialize"; } testSerialize;
	struct { char* (*func)(const byte *buf, int size); char *name = "testUnserialize"; } testUnserialize;
	struct { char* (*func)(void); char *name = "tmpUnserialize"; } tmpUnserialize;
};

FFIEXPORT struct tagffiAPIStatic LoadFFI(void);

#endif /* defined(__BASE_TYPE_EXPORTS_H__) */
