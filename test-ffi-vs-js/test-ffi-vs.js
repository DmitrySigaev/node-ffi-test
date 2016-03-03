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

console.log(out);
