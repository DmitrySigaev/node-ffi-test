/* test-ffi-vs.js
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
var path = require('path');
var local = path.join.bind(path, __dirname);

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
var str2 = test.matrix_js_d();
var str2 = test.matrix_js_i();

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

var ret = test.read_scoring_matrix(gaptest_utest);

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
