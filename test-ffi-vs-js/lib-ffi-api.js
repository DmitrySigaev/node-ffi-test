/* lib-ffi-api.cpp
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */

var ref = require('ref');
var StructType = require('ref-struct');
var ArrayType = require('ref-array');
var UnionType = require('ref-union');
var IntArray = ArrayType('int');
var DoubleArray = ArrayType('double');
var byte_ptr = ref.refType('byte');
var char_ptr = ref.refType('char');
var int_ptr = ref.refType('int');
var float_ptr = ref.refType('float');
var double_ptr = ref.refType('double');
var bool_ptr = ref.refType('bool');
var size_t_ptr = ref.refType('size_t');

var wchar = ('win32' == process.platform) ? ('uint16') : ('uint32');
var wchar_ptr = ref.refType(wchar);

var byte_p_ptr = ref.refType(byte_ptr);
var double_p_ptr = ref.refType(double_ptr);
var int_p_ptr = ref.refType(int_ptr);

var void_ptr = ref.refType('void');
var void_p_ptr = ref.refType(void_ptr);
var doubleArrayThree = ArrayType('double', 3);
var DocArray = ArrayType('char', 1024 * 4);

var ut = UnionType({
    d: double_p_ptr,
    i: int_p_ptr,
    c: byte_p_ptr
});

var xyz = StructType({
    x: 'float',
    y: 'float',
    z: 'float'
});

var xyz_ptr = ref.refType(xyz);

var matrix_utest = StructType({
    data: ut,
    nrows: 'size_t',
    ncols: 'size_t',
    type: 'int32'
});

var matrix_utest_ptr = ref.refType(matrix_utest);

var scoring_matrix_utest = StructType({
    sc_double_matrix: matrix_utest,
    sc_int_matrix: matrix_utest,
    scale: 'double',
    scaleback: 'double',
    man2mip: doubleArrayThree,
    gapOpen: 'double',
    gapExtend: 'double',
    Doc: DocArray
});


var scoring_matrix_utest_ptr = ref.refType(scoring_matrix_utest);
var matrix_utest_ptr = ref.refType(matrix_utest);


var search_swag_profile_utest = StructType({
    gapOpen: 'double',
    gapExt: 'double',
    mtx: scoring_matrix_utest_ptr
});

var search_swag_profile_utest_ptr = ref.refType(search_swag_profile_utest);

var sequence_utest = StructType({
    ID: 'size_t',
    seq: 'string',
    len: 'size_t'
});

var sequence_utest_ptr = ref.refType(sequence_utest);

var score_matrix_utest = StructType({
    score: matrix_utest,
    directions: matrix_utest
});


module.exports = {
    api_cpp: {
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
        "read_scoring_matrix_js": ["int", [scoring_matrix_utest_ptr, "string", "size_t"]]
    },
    api_c: {
        "voidFunc": ["void", []],
        "charFunc": ["char", ["char", char_ptr]],
        "floatFunc": ["float", ["float", float_ptr]],
        "doubleFunc": ["double", ["double", double_ptr]],
        "boolFunc": ["bool", ["bool", bool_ptr]],
        "wcharFunc": [wchar, [wchar, wchar_ptr]],
        "matrix_js_i": [int_ptr, ["size_t", "size_t", "int32"]],
        "matrix_js_d": [matrix_utest_ptr, ["size_t", "size_t", "int32"]],
        "matrix_js": [matrix_utest, ["size_t", "size_t", "int32"]],
        "matrix_set_int": ["int", [matrix_utest_ptr, "int"]],
        "matrix_set_double": ["int", [matrix_utest_ptr, "double"]],
        "matrix_set_char": ["int", [matrix_utest_ptr, "char"]],
        "read_scoring_matrix_js": ["int", [scoring_matrix_utest_ptr, "string", "size_t"]],
        "sw_directions_js": [score_matrix_utest, [search_swag_profile_utest_ptr, sequence_utest_ptr, sequence_utest_ptr]],
        "encode_seq_js": ["void", [sequence_utest_ptr, sequence_utest_ptr]],
        "sw_genc_js": ["double", [search_swag_profile_utest_ptr, sequence_utest_ptr, sequence_utest_ptr]],
        "sw_genc_js_sigaev": ["double", [search_swag_profile_utest_ptr, sequence_utest_ptr, sequence_utest_ptr]],
        "sw_genc_m_js": [score_matrix_utest, [search_swag_profile_utest_ptr, sequence_utest_ptr, sequence_utest_ptr]],
        "lal_seq_base_create_js": ["int", []],
        "lal_seq_base_close_js": ["void", []], 
        "lal_add_fasta_js": ["int", ["string"]], 
        "lal_add_string_js": ["int", ["string"]],
        "fasta_get_sequence_count_js": ["size_t", []],
        "fasta_get_symbol_count_js": ["size_t", []],
        "fasta_get_longest_sequence_js": ["size_t", []],
        "fasta_get_longest_seq_struct_js": [sequence_utest_ptr, []],
        "fasta_get_seq_struct_js": [sequence_utest_ptr, ["size_t"]]
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
        "DoubleArray": DoubleArray,
        "substitution_matrix": scoring_matrix_utest,
        "sequence": sequence_utest,
        "score_matrix": score_matrix_utest,
        "matrix": matrix_utest,
        "search_profile": search_swag_profile_utest
    }
};