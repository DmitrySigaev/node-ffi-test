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
#include "base-type-exports-c.h"

#define EPSILON 0.00001
#define EXPECT(test, x) ((x))?(printf(#x":passed\n"), test&=1):(printf(#x":fault, f: %s, l: %d\n", __FILE__, __LINE__), test&=0)

bool testFFIAPI(struct tagffiAPI*ffiAPIin)
{
	bool t = true;
	struct tagffiAPI ffiAPI;
	struct tagffiAPI *pffiAPI = &ffiAPI;
	char cout;
	float fout;
	double dout;
	bool bout;
	memcpy(pffiAPI, ffiAPIin, sizeof(struct tagffiAPI));
	EXPECT(t, '#' == (ffiAPI.charFunc)('#', &cout));
	EXPECT(t, '#' == cout);
	EXPECT(t, fabs(12.13 - ffiAPI.floatFunc(12.13, &fout)) < EPSILON);
	EXPECT(t, fabs(12.13 - fout) < EPSILON);
	EXPECT(t, fabs(12.13 - ffiAPI.doubleFunc(12.13, &dout)) < EPSILON);
	EXPECT(t, fabs(12.13 - dout) < EPSILON);
	EXPECT(t, true == ffiAPI.boolFunc(true, &bout));
	EXPECT(t, true == bout);
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
		testFFIAPI(&ffiAPI);
		fFreeResult = FreeLibrary(hinstLib);
	}

	// If unable to call the DLL function, use an alternative.
	if (!fRunTimeLinkSuccess)
		printf("end of tests\n");

	return 0;
}
