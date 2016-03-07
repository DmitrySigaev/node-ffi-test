/* test-ffi-vs.cpp
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <windows.h> 
#include "base-type-exports.h"

#define EPSILON 0.00001
#define EXPECT(test, x) ((x))?(printf(#x":passed\n"), test&=1):(printf(#x":fault, f: %s, l: %d\n", __FILE__, __LINE__), test&=0)

bool testFFIAPI(struct tagffiAPI *ffiAPIin)
{
	bool t = true;
	struct tagffiAPI ffiAPI;
	struct tagffiAPI *pffiAPI = &ffiAPI;
	char cout;
	float fout;
	double dout;
	memcpy(pffiAPI, ffiAPIin, sizeof(struct tagffiAPI));
	EXPECT(t,ffiAPI.size == (const int)((sizeof(ffiAPI) - sizeof(ffiAPI.size)) / sizeof(void *)));
	EXPECT(t, ffiAPI.size + 1 == (const int)(((int)(&(ffiAPI.size)) - (int)(&(ffiAPI.voidFunc)) + sizeof(void *)) / sizeof(void *)));
	EXPECT(t, '#' == (ffiAPI.charFunc.func)('#', &cout));
	EXPECT(t, '#' == cout);
	EXPECT(t, fabs(12.13 - ffiAPI.floatFunc.func(12.13, &fout)) < EPSILON);
	EXPECT(t, fabs(12.13 - fout) < EPSILON);
	EXPECT(t, fabs(12.13 - ffiAPI.doubleFunc.func(12.13, &dout)) < EPSILON);
	EXPECT(t, fabs(12.13 - dout) < EPSILON);
	EXPECT(t, t == true);
	return true;
}

bool testFFIAPIs(struct tagffiAPIStatic *ffiAPIin)
{
	bool t = true;
	struct tagffiAPIStatic ffiAPI;
	struct tagffiAPIStatic *pffiAPI = &ffiAPI;
	memcpy(pffiAPI, ffiAPIin, sizeof(struct tagffiAPIStatic));
	char cout;
	float fout;
	double dout;

	EXPECT(t, '#' == (ffiAPI.charF.func)('#', &cout));
	EXPECT(t, '#' == cout);
	EXPECT(t, fabs(12.13 - (ffiAPI.floatF.func)(12.13, &fout)) < EPSILON);
	EXPECT(t, fabs(12.13 - fout) < EPSILON);
	EXPECT(t, fabs(12.13 - (ffiAPI.doubleF.func)(12.13, &dout)) < EPSILON);
	EXPECT(t, fabs(12.13 - dout) < EPSILON);
	EXPECT(t, t == true);
	return true;
}

/* main */
int main(int argc, char **argv)
{
	HINSTANCE hinstLib;
	BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

	// Get a handle to the DLL module.

	hinstLib = LoadLibrary(TEXT("ffi-dll.dll"));

	// If the handle is valid, try to get the function address.

	if (hinstLib != NULL) 
	{
		struct tagffiAPI ffiAPI;
		FFIPROC *stp = (FFIPROC *)(&ffiAPI.voidFunc);
		// long * stp = (long *)(&ffiAPI.voidFunc); // if size of long type equel to size of addressing mode
		for (int i = 0; i < ffiAPI.size; i++)
		{
			*(stp + i) = (FFIPROC)GetProcAddress(hinstLib, (char *)(*(stp + i)));
//			*(stp + i) = (long)GetProcAddress(hinstLib, (char *)(*(stp + i)));
		}
		// If the function address is valid, call the function.
		if (NULL != stp)
		{
			fRunTimeLinkSuccess = TRUE;
			testFFIAPI(&ffiAPI);
		}
		// Free the DLL module.

		fFreeResult = FreeLibrary(hinstLib);
	}

	hinstLib = LoadLibrary(TEXT("ffi-dll.dll"));

	// If the handle is valid, try to get the function address.

	if (hinstLib != NULL)
	{
		struct tagffiAPIStatic ffiAPI;
		struct tagffiAPIStatic (* lffi)(void);
		lffi = (struct tagffiAPIStatic(*)(void))GetProcAddress(hinstLib, "LoadFFI");
		testFFIAPIs(&(lffi()));
		fFreeResult = FreeLibrary(hinstLib);

	}

	// If unable to call the DLL function, use an alternative.
	if (!fRunTimeLinkSuccess)
		printf("end of tests\n");

	return 0;

}
