/* test-ffi-vs.cpp
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
#include <stdio.h>
#include <assert.h>
#include <windows.h> 
#include "base-type-exports.h"

bool testFFIAPI(void)
{
	struct tagffiAPI ffiAPI;
	struct tagffiAPI *p_ffiAPI = &ffiAPI;
	// ffiAPI = testStructFFIAPI; // error
	memcpy(p_ffiAPI, &testStructFFIAPI, sizeof(struct tagffiAPI));
	assert(ffiAPI.size == ((sizeof(ffiAPI) - sizeof(ffiAPI.size)) / sizeof(void *)));
	assert(ffiAPI.size + 1 == ((int)(&(ffiAPI.size)) - (int)(&(ffiAPI.voidFunc)) + sizeof(void *)) / sizeof(void *));
	voidFunc();
	printf("testFFIAPI: ok");
	return true;
}

/* main */
int main(int argc, char **argv)
{
	HINSTANCE hinstLib;
	struct tagffiAPI ffiAPI;
	BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

	assert(true == testFFIAPI());
	// Get a handle to the DLL module.

	hinstLib = LoadLibrary(TEXT("ffi-dll.dll"));

	// If the handle is valid, try to get the function address.

	if (hinstLib != NULL) 
	{
		FFIPROC *stp = (FFIPROC *)(&ffiAPI.voidFunc);
		// long * stp = (long *)(&ffiAPI.voidFunc); // if size of long type equel to size of addressing mode
		for (int i = 0; i < ffiAPI.size; i++)
		{
			*(stp + i) = (FFIPROC)GetProcAddress(hinstLib, (char *)(*(stp + i)));
//			*(stp + i) = (long)GetProcAddress(hinstLib, (char *)(*(stp + i)));
		}
		// If the function address is valid, call the function.

		if (NULL != ffiAPI.voidFunc.func)
		{
			char s1 = 12, s2 = 2, s3;
			fRunTimeLinkSuccess = TRUE;
			s2 = (ffiAPI.charFunc.func)(s1, &s3);
			printf("in %c, out: %c", s1, s2);
		}
		// Free the DLL module.

		fFreeResult = FreeLibrary(hinstLib);
	}

	hinstLib = LoadLibrary(TEXT("ffi-dll.dll"));

	// If the handle is valid, try to get the function address.

	if (hinstLib != NULL)
	{
		char s1 = 12, s2 = 2, s3;
		struct tagffiAPIStatic ffiAPI;
		struct tagffiAPIStatic (* lffi)(void);
		lffi = (struct tagffiAPIStatic(*)(void))GetProcAddress(hinstLib, "LoadFFI");

		ffiAPI = lffi();
		ffiAPI.charF.func(s1, &s3);
		
		fFreeResult = FreeLibrary(hinstLib);

	}

	// If unable to call the DLL function, use an alternative.
	if (!fRunTimeLinkSuccess)
		printf("tests: ok\n");

	return 0;

}
