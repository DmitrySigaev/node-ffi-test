/* base-type-exports.cpp
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
#include <stdio.h>
#include "base-type-exports.h"

void voidFunc( void )
{
	printf("[ffi-dll] call voidFunc in f: %s,l: %d\n", __FILE__, __LINE__);
}

char charFunc(char inChar, char *outChar)
{
	printf("[ffi-dll] call charFunc in f: %s,l: %d\n", __FILE__, __LINE__);
	*outChar = inChar;
	return inChar;
}

struct tagffiAPIStatic LoadFFI(void)
{
	struct tagffiAPIStatic FFI;
	printf("[ffi-dll] call LoadFFI in f: %s,l: %d\n", __FILE__, __LINE__);
	return FFI;
}