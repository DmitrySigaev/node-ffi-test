/* lib-ffi-api.cpp
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */

var ref = require('ref');
var StructType = require('ref-struct');
var ArrayType = require('ref-array');
var IntArray = ArrayType('int');
var byte_ptr = ref.refType('byte');
var char_ptr = ref.refType('char');
var int_ptr = ref.refType('int');
var float_ptr = ref.refType('float');
var double_ptr = ref.refType('double');
var bool_ptr = ref.refType('bool');
var wchar = ('win32' == process.platform)?('uint16'):('uint32');
var wchar_ptr = ref.refType(wchar);

var byte_p_ptr = ref.refType(byte_ptr);

var void_ptr = ref.refType('void');
var void_p_ptr = ref.refType(void_ptr);
var doubleArrayThree = ArrayType('double', 3);
var DocArray = ArrayType('char', 1024 * 4);


var xyz = StructType({
	x: 'float',
	y: 'float',
	z: 'float'
});

var xyz_ptr = ref.refType(xyz);

var matrix_utest = StructType({
    data: void_p_ptr,
    nrows: 'size_t',
    ncols: 'size_t',
    type: 'int32'
});



var scoring_matrix_utest = StructType({
    sc_double_matrix: matrix_utest,
    sc_int_matrix: matrix_utest,
    scale: 'double',
    scaleback: 'double',
    man2mip: doubleArrayThree,
    gapOpen: 'double',
    gapExtend: 'double',
    Doc:DocArray
});


var scoring_matrix_utest_ptr = ref.refType(scoring_matrix_utest);

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
		"tmpUnserialize": ["string", []],
		"intArray": ["int", ["int", IntArray]],
        "read_scoring_matrix_js": ["int", [scoring_matrix_utest_ptr, "string", int_ptr]] 
	},
	out: {
		"achar": ref.alloc('char'),
		"afloat": ref.alloc('float'),
		"adouble": ref.alloc('double'),
		"abool": ref.alloc('bool'),
		"awchar": ref.alloc(wchar),
		"aint": ref.alloc('int'),
		"apint": ref.alloc(int_ptr),
		"apbyte": ref.alloc(byte_ptr),
		"read": ref.readPointer,
        "alloc": ref.alloc,
        "scoring_matrix_alloc": ref.alloc(scoring_matrix_utest_ptr)
	},
	type: {
		"wchar": wchar,
		"xyz": xyz, 
		"byte_ptr": byte_ptr,
        "IntArray": IntArray,
        "scoring_matrix": scoring_matrix_utest
	}
};