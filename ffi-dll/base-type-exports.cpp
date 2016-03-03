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
	printf("[ffi-dll] call charFunc %c, in f: %s,l: %d\n", inChar, __FILE__, __LINE__);
	*outChar = inChar;
	return inChar;
}

float floatFunc(float inFloat, float *outFloat)
{
	printf("[ffi-dll] call charFunc %f, in f: %s,l: %d\n", inFloat, __FILE__, __LINE__);
	*outFloat = inFloat;
	return inFloat;
}

struct tagffiAPIStatic LoadFFI(void)
{
	struct tagffiAPIStatic FFI;
	FFI.voidF.func = voidFunc;
	FFI.charF.func = charFunc;
	FFI.floatF.func = floatFunc;
	printf("[ffi-dll] call LoadFFI in f: %s,l: %d\n", __FILE__, __LINE__);
	return FFI;
}
