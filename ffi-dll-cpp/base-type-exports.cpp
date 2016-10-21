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

IMPL_EXCEPTION(dnest, ArrayError, "array");

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

int testSerialize(const char *str, byte **buf, int *size)
{
	auto &tmp = getThreadTmpData();
	tmp.string.appendString(str, 0);

	*buf = (byte *)tmp.string.ptr();
	*size = tmp.string.size();
	printf("[ffi-dll] call testSerialize (%s) in f: %s,l: %d\n", str, __FILE__, __LINE__);
	printf("[ffi-dll] (cell 1) address  of address : (%x)\n", buf);
	printf("[ffi-dll] (cell 1) address  of address in char 1: (%d)\n", ((unsigned long)buf) & 255);
	printf("[ffi-dll] (cell 1) address  of address in char 2: (%d)\n", (((unsigned long)buf)>>8) & 255);
	printf("[ffi-dll] (cell 2) aderess of size : (%x)\n", size);
	printf("[ffi-dll] (cell 2) aderess of size in char 1 :(%d)\n", ((unsigned long)size) & 255);
	printf("[ffi-dll] (cell 2) aderess of size in char 2 :(%d)\n", (((unsigned long)size)>>8) & 255);
	printf("[ffi-dll] size (%x)\n", *size);
	printf("[ffi-dll] aderess of string (%x)\n", (byte *)tmp.string.ptr());
	printf("[ffi-dll] aderess of string in char 1: (%x)\n", ((unsigned long)(*buf)) & 255);
	printf("[ffi-dll] aderess of string in char 2: (%x)\n", (((unsigned long)(*buf)) >> 8) & 255);
	printf("[ffi-dll] fist char : (%d)\n", **buf);
	printf("[ffi-dll] fist char : (%c)\n", **buf);
	printf("[ffi-dll] second char : (%d)\n", *((byte *)(*buf) + 1));
	printf("[ffi-dll] second char : (%c)\n", *((byte *)(*buf) + 1));
	return 1;

}

char* testUnserialize(const byte *buf, int size)
{
	dnest::Array<byte> string;
	string.copy(buf, size);
	string.push(0);
	auto &tmp = getThreadTmpData();
	printf("[ffi-dll] call testUnserialize (%s) in f: %s,l: %d\n", string.ptr(), __FILE__, __LINE__);
	return ( char *)buf; /*as I suspect string var is a copy of input buf! but it doesn't matter, because function returns input pointer   */
}

char* tmpUnserialize()
{
	auto &tmp = getThreadTmpData();
	char *buf = (char *)tmp.string.ptr();
	tmp.string.push(0);
	printf("[ffi-dll] call tmpUnserialize (%s) in f: %s,l: %d\n", buf, __FILE__, __LINE__);
	return buf;
}

int intArray(int length, int *inArray)
{
	printf("[ffi-dll] call intArray el:");
	for (int i = 0; i < length; i++)
		printf(" %d,", inArray[i]);
	printf("\n in f: %s,l: %d\n", __FILE__, __LINE__);
	return length;
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
	FFI.testSerialize.func = testSerialize;
	FFI.testUnserialize.func = testUnserialize;
	FFI.tmpUnserialize.func = tmpUnserialize;
	FFI.intArray.func = intArray;
	printf("[ffi-dll] call LoadFFI in f: %s,l: %d\n", __FILE__, __LINE__);
	return FFI;
}
