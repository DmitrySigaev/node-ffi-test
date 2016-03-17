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

var out2 = [];
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

console.log(out);
