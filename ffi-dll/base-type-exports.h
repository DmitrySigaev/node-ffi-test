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

#define CALLBACKFFI
typedef void (CALLBACKFFI *FFIPROC)(void);

FFIEXPORT struct tagffiAPI
{
	union { FFIPROC func; char *name = "voidFunc"; } voidFunc;
	union { char(*func)(char inChar, char *outChar); char *name = "charFunc"; } charFunc;
	const int size = (sizeof(struct tagffiAPI) - sizeof(const int)) / sizeof(void *);
};

struct tagffiAPI testStructFFIAPI;

FFIEXPORT struct tagffiAPIStatic
{
	struct { FFIPROC func = voidFunc; char *name = "voidFunc"; } voidF;
	struct { char(*func)(char inChar, char *outChar) = charFunc; char *name = "charFunc"; } charF;
};

struct tagffiAPIStatic testStructFFIAPIStatic;

FFIEXPORT struct tagffiAPIStatic LoadFFI(void);

#endif /* defined(__BASE_TYPE_EXPORTS_H__) */
