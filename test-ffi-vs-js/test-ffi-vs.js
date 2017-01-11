/* test-ffi-vs.js
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
var path = require('path');
var local = path.join.bind(path, __dirname);
fs = require('fs');

var test = require(local('test-ffi'));
test.voidFunc();
var out = [];
var char = test.charFunc('\u263B', out);
var float = test.floatFunc(12.13, out);
var double = test.doubleFunc(12.13, out);
var floatxyz = test.tmpXYZ(0.01, 2.02, 4.004);
var arr = test.serialize("Hello world!");
var str = test.unserialize(arr);
var str2 = test.tmp_unserialize();


var str2 = test.matrix_js();
/*
var str3 = test.matrix_js_d();
var str4 = test.matrix_js_i();
*/
var out2 = [];

var gaptest_utest=  " #gaptest1.table                                          \n \
    A    B    C    D    G    H    K    M    R    S    T    U    V    W    Y    \n \
A  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
B -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
C -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
D -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
G -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
H -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
K -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
M -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
R -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
S -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0 -1.0 -1.0 -1.0   \n \
T -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0  1.0 -1.0 -1.0 -1.0   \n \
U -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  1.0  2.0 -1.0 -1.0 -1.0   \n \
V -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0 -1.0   \n \
W -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0 -1.0   \n \
Y -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0 -1.0  2.0";

var substitution = test.read_substitution_matrix(gaptest_utest);
var profile = test.set_profile(-1, 0, substitution);
//var seq1 = "CTTCCTACGTTGGGTCATAGTAGTGCGGCGTGGGCAATGCCTACGGAGGGGTGGAGCAACTGGCGCTATCACTTCTACCATCGTCTGCAGCGTACGA";
//var seq2 = "tcgtacgctgcagacgatggtagaagtgatagcgccagttgctccacccctccgtaggcattgcccacgccgcactactatgacccaacgtaggaagttg";
var seq2 = "CAACTTCCTGGCGCTATCACTTCTACCATCGTCTGCAGCGT";
var seq1 = "acgatggtagaagtgatagcgccagttgctccacccct";
var res = test.sw_genc_m(profile, seq1, seq2);

//var res = test.sw_directions(profile, seq1, seq2);

//var score = Math.max.apply(null, res[0]);
var score = res[0][0][0];
for (var i = 0; i < res[0].length; i++) {
	elementArray = res[0][i];
	if (score < Math.max.apply(Math, elementArray)) {
		score = Math.max.apply(Math, elementArray);
	}
}


var full_object = { score: score, scorematrix: res[0], directions: res[1], seq1: seq2, seq2: seq1 };
var score_table = JSON.stringify(full_object);
fs.writeFile('score_table_gc.txt', score_table, function (err) {
    if (err)
        return console.log(err);
    console.log('score table from sw_genc > score_table_gc.txt');
});

//var res = test.sw_genc_m(profile, seq1, seq2);

var res = test.sw_directions(profile, seq1, seq2);

//var score = Math.max.apply(null, res[0]);
var score = res[0][0][0];
for (var i = 0; i < res[0].length; i++) {
	elementArray = res[0][i];
	if (score < Math.max.apply(Math, elementArray)) {
		score = Math.max.apply(Math, elementArray);
	}
}

var full_object = { score: score, scorematrix: res[0], directions: res[1], seq1: seq2, seq2: seq1 };
var score_table = JSON.stringify(full_object);
fs.writeFile('score_table_swd.txt', score_table, function (err) {
	if (err)
		return console.log(err);
	console.log('score table from sw_directions > score_table_swd.txt');
});

var res = test.sw_genc_sigaev(profile, seq1, seq2);

test.EXPECT("';' == test.charFunc('\u263B', out)", out2);
test.EXPECT("';' == out.pop()", out2);
test.EXPECT("'@' == test.charFunc('\u263B', out)", out2);
test.EXPECT("'@' == out.pop()", out2);
test.EXPECT("Math.abs(12.13 - test.floatFunc(12.13, out)) < 0.00001", out2);
test.EXPECT("Math.abs(12.13 - out.pop()) < 0.00001", out2);
test.EXPECT("Math.abs(12.13 - test.doubleFunc(12.13, out)) < 0.00001", out2);
test.EXPECT("Math.abs(12.13 - out.pop()) < 0.00001", out2);
test.EXPECT("true == test.boolFunc(true, out)", out2);
test.EXPECT("true == out.pop()", out2);
test.EXPECT("'\u263B' == test.wcharFunc('\u263B', out)", out2);
test.EXPECT("'\u263B' == out.pop()", out2);
test.EXPECT("'\u263B' == test.wcharFunc('\u263B', out)", out2);
test.EXPECT("72 == test.serialize('Hello world!')[0]", out2);
test.EXPECT("4 == test.testIntArray([1,2,65000,7])", out2);
test.EXPECT("4 == test.testIntArray([1,2,65000,7])", out2);

console.log(out);
