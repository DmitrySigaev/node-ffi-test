/* test-ffi.js
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
var path = require('path');
var ffi = require('ffi');
var local = path.join.bind(path, __dirname);

var lib_api = require(local('lib-ffi-api'));

var test_ffi = function (options) {
	options = options || {};
	var mode = options.mode || 'Release';
	var libpath = local('../out/' + process.platform + '/' + process.arch + '/' + mode + '/ffi-dll-c');
	this.libpath = options.libpath || libpath;
	this.logger = options.logger || console;
	this._lib = ffi.Library(libpath, lib_api.api);
	this._out = lib_api.out;
};


test_ffi.prototype.voidFunc = function () {
	this._lib.voidFunc();
};

test_ffi.prototype.charFunc = function (char, array) {
	char = char.charCodeAt(0)&255;
	var ret = String.fromCharCode(this._lib.charFunc(char, this._out.char));
	array.push(String.fromCharCode(this._out.char.deref()));
	return ret;
};

test_ffi.prototype.floatFunc = function (float, array) {
	var ret = this._lib.floatFunc(float, this._out.float);
	array.push(this._out.float.deref());
	return ret;
};

test_ffi.prototype.doubleFunc = function (double, array) {
	var ret = this._lib.doubleFunc(double, this._out.double);
	array.push(this._out.double.deref());
	return ret;
};

test_ffi.prototype.EXPECT = function (x, out) {
	var out = out || [];
	var code = new Function('test', 'out', "return (" + x + ");");
	var status = (code(this, out))?'passed': 'fault';
	this.logger.log(x + ':' + status);
}
module.exports = new test_ffi();