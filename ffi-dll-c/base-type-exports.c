/* base-type-exports.cpp
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
#include <stdio.h>
#include "base-type-exports-c.h"

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
	printf("[ffi-dll] call floatFunc %f, in f: %s,l: %d\n", inFloat, __FILE__, __LINE__);
	*outFloat = inFloat;
	return inFloat;
}

double doubleFunc(double inDouble, double *outDouble)
{
	printf("[ffi-dll] call doubleFunc %f, in f: %s,l: %d\n", inDouble, __FILE__, __LINE__);
	*outDouble = inDouble;
	return inDouble;
}

struct tagffiAPI LoadFFI(void)
{
	struct tagffiAPI FFI;
	FFI.voidFunc = voidFunc;
	FFI.charFunc = charFunc;
	FFI.floatFunc = floatFunc;
	FFI.doubleFunc = doubleFunc;
	printf("[ffi-dll] call LoadFFI in f: %s,l: %d\n", __FILE__, __LINE__);
	return FFI;
}
