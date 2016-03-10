/* base-type-exports.c
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
#include <stdio.h>
#include <stdbool.h>
#include "base-type-exports-c.h"

void voidFunc( void )
{
	printf("[ffi-dll-c] call voidFunc in f: %s,l: %d\n", __FILE__, __LINE__);
}

char charFunc(char inChar, char *outChar)
{
	printf("[ffi-dll-c] call charFunc %c, in f: %s,l: %d\n", inChar, __FILE__, __LINE__);
	*outChar = inChar;
	return inChar;
}

float floatFunc(float inFloat, float *outFloat)
{
	printf("[ffi-dll-c] call floatFunc %f, in f: %s,l: %d\n", inFloat, __FILE__, __LINE__);
	*outFloat = inFloat;
	return inFloat;
}

double doubleFunc(double inDouble, double *outDouble)
{
	printf("[ffi-dll-c] call doubleFunc %f, in f: %s,l: %d\n", inDouble, __FILE__, __LINE__);
	*outDouble = inDouble;
	return inDouble;
}

bool boolFunc(bool inBool, bool *outBool)
{
	printf("[ffi-dll-c] call boolFunc %d, in f: %s,l: %d\n", inBool, __FILE__, __LINE__);
	*outBool = inBool;
	return inBool;
}

struct tagffiAPI LoadFFI(void)
{
	struct tagffiAPI FFI;
	FFI.voidFunc = voidFunc;
	FFI.charFunc = charFunc;
	FFI.floatFunc = floatFunc;
	FFI.doubleFunc = doubleFunc;
	FFI.boolFunc = boolFunc;
	printf("[ffi-dll] call LoadFFI in f: %s,l: %d\n", __FILE__, __LINE__);
	return FFI;
}
