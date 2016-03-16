/* base-type-exports.cpp
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
#include <stdio.h>
#include "base-type-exports-cpp.h"
#include "array.h"
#include "tmp-thread-obj.h"

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

bool boolFunc(bool inBool, bool *outBool)
{
	printf("[ffi-dll] call boolFunc %d, in f: %s,l: %d\n", inBool, __FILE__, __LINE__);
	*outBool = inBool;
	return inBool;
}

wchar_t wcharFunc(wchar_t inWchar, wchar_t *outWchar)
{
	wprintf(L"[ffi-dll] call wcharFunc %C, in f: %s, l: %d\n", inWchar, __FILE__, __LINE__);
	*outWchar = inWchar;
	return inWchar;
}

struct TmpData
{
	dnest::Array<char> string;
	float xyz[3];
};

static dnest::TemporaryThreadObjManager<TmpData> _temporary_obj_manager;
TmpData& getThreadTmpData(void)
{
	return _temporary_obj_manager.getObject();
}

float * tmpXYZ(float x, float y, float z)
{
	auto &tmp = getThreadTmpData();
	tmp.xyz[0] = x;
	tmp.xyz[1] = y;
	tmp.xyz[2] = z;
	printf("[ffi-dll] call tmpXYZ (%f,%f,%f) in f: %s,l: %d\n", x, y, z, __FILE__, __LINE__);
	return tmp.xyz;
}

struct tagffiAPIStatic LoadFFI(void)
{
	struct tagffiAPIStatic FFI;
	FFI.voidF.func = voidFunc;
	FFI.charF.func = charFunc;
	FFI.floatF.func = floatFunc;
	FFI.doubleF.func = doubleFunc;
	FFI.boolF.func = boolFunc;
	FFI.wcharF.func = wcharFunc;
	FFI.tmpXYZ.func = tmpXYZ;
	printf("[ffi-dll] call LoadFFI in f: %s,l: %d\n", __FILE__, __LINE__);
	return FFI;
}
