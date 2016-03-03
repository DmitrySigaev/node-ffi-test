/* base-type-exports.h
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */

#if !defined(__BASE_TYPE_EXPORTS_H__)
#define __BASE_TYPE_EXPORTS_H__ 

#if defined(FFIDLL_EXPORTS)
	#define FFIEXPORT extern "C" __declspec(dllexport)
#else
	#define FFIEXPORT extern "C" __declspec(dllimport)
#endif

FFIEXPORT void voidFunc( void );
FFIEXPORT char charFunc(char inChar, char *outChar);
FFIEXPORT float floatFunc(float inFloat, float *outFloat);

#define CALLBACKFFI
typedef void (CALLBACKFFI *FFIPROC)(void);

FFIEXPORT struct tagffiAPI
{
	union { FFIPROC func; char *name = "voidFunc"; } voidFunc;
	union { char(*func)(char inChar, char *outChar); char *name = "charFunc"; } charFunc;
	const int size = (sizeof(struct tagffiAPI) - sizeof(const int)) / sizeof(void *);
};

// struct tagffiAPI testStructFFIAPI;

FFIEXPORT struct tagffiAPIStatic
{
	struct { FFIPROC func; char *name = "voidFunc"; } voidF;
	struct { char(*func)(char, char *); char *name = "charFunc"; } charF;
	struct { float(*func)(float, float *); char *name = "charFunc"; } floatF;
};

// struct tagffiAPIStatic testStructFFIAPIStatic;

FFIEXPORT struct tagffiAPIStatic LoadFFI(void);

#endif /* defined(__BASE_TYPE_EXPORTS_H__) */
