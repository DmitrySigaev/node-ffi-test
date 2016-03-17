/* lib-ffi-api.cpp
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */

var ref = require('ref');
var ref_struct = require('ref-struct');

var byte_ptr = ref.refType('byte');
var char_ptr = ref.refType('char');
var int_ptr = ref.refType('int');
var float_ptr = ref.refType('float');
var double_ptr = ref.refType('double');
var bool_ptr = ref.refType('bool');
var wchar = ('win32' == process.platform)?('uint16'):('uint32');
var wchar_ptr = ref.refType(wchar);

var byte_p_ptr = ref.refType(byte_ptr);

var xyz = ref_struct({
	x: 'float',
	y: 'float',
	z: 'float'
});

var xyz_ptr = ref.refType(xyz);

module.exports = {
	api: {
		"voidFunc": ["void", []],
		"charFunc": ["char", ["char", char_ptr]],
		"floatFunc": ["float", ["float", float_ptr]],
		"doubleFunc": ["double", ["double", double_ptr]],
		"boolFunc": ["bool", ["bool", bool_ptr]],
		"wcharFunc": [wchar, [wchar, wchar_ptr]],
		"tmpXYZ": [xyz_ptr, ["float", "float", "float"]], 
		"testSerialize": ["int", ["string", byte_p_ptr, int_ptr]], 
		"testUnserialize": ["string", [byte_ptr, "int"]], 
		"tmpUnserialize": ["string", []]
	},
	out: {
		"achar": ref.alloc('char'),
		"afloat": ref.alloc('float'),
		"adouble": ref.alloc('double'),
		"abool": ref.alloc('bool'),
		"awchar": ref.alloc(wchar),
		"aint": ref.alloc('int'),
		"apint": ref.alloc(int_ptr),
		"apbyte": ref.alloc(byte_ptr)
	},
	type: {
		"wchar": wchar,
		"xyz": xyz, 
		"byte_ptr": byte_ptr
	}
};