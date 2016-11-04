/* test-ffi.js
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */
var path = require('path');
var ffi = require('ffi');
var ref = require('ref');
var ArrayType = require('ref-array');
var local = path.join.bind(path, __dirname);

var lib_api = require(local('lib-ffi-api'));

var test_ffi = function (options) {
    options = options || {};
    var mode = options.mode || 'Debug';
    var libpath = local('../out/' + process.platform + '/' + process.arch + '/' + mode + '/ffi-dll-cpp');
    var libpath_c = local('../out/' + process.platform + '/' + process.arch + '/' + mode + '/ffi-dll-c');
    this.libpath = options.libpath || libpath_c;
    this.logger = options.logger || console;
    this._lib = ffi.Library(this.libpath, lib_api.api_c);
    this._out = lib_api.out;
    this._type = lib_api.type;
};


test_ffi.prototype.voidFunc = function () {
    this._lib.voidFunc();
};

test_ffi.prototype.charFunc = function (char, array) {
    char = char.charCodeAt(0) & 255;
    var ret = String.fromCharCode(this._lib.charFunc(char, this._out.achar));
    array.push(String.fromCharCode(this._out.achar.deref()));
    return ret;
};

test_ffi.prototype.floatFunc = function (float, array) {
    var ret = this._lib.floatFunc(float, this._out.afloat);
    array.push(this._out.afloat.deref());
    return ret;
};

test_ffi.prototype.doubleFunc = function (double, array) {
    var ret = this._lib.doubleFunc(double, this._out.adouble);
    array.push(this._out.adouble.deref());
    return ret;
};

test_ffi.prototype.boolFunc = function (bool, array) {
    var ret = this._lib.boolFunc(bool, this._out.abool);
    array.push(this._out.abool.deref());
    return ret;
};

test_ffi.prototype.wcharFunc = function (wchar, array) {
    wchar = wchar.charCodeAt(0);
    var ret = String.fromCharCode(this._lib.wcharFunc(wchar, this._out.awchar));
    array.push(String.fromCharCode(this._out.awchar.deref()));
    return ret;
};

test_ffi.prototype.tmpXYZ = function (x, y, z) {
    var out = [];
    if (this._lib.tmpXYZ) {
        var ref_test = this._lib.tmpXYZ(x, y, z);
        var testv = ref_test.deref();
        out.push(testv.x);
        out.push(testv.y);
        out.push(testv.z);
    }
    return out;
};

test_ffi.prototype.serialize = function (string) {
    var res = [];
    if (this._lib.testSerialize) {
        var size = this._out.aint;
        var pointer = this._out.apbyte;
        var status = this._lib.testSerialize(string, pointer, size);
        var buf = ref.readPointer(pointer, 0, size.deref());
        for (i = 0; i < size.deref(); i++) {
            res.push(buf[i]);
        }
    }
    return res;
};

test_ffi.prototype.testIntArray = function (array) {
    if (this._lib.intArray)
        return this._lib.intArray(array.length, array);
    else
        return [];
};

test_ffi.prototype.unserialize = function (array) {
    if (this._lib.intArray) {
        var buf = new Buffer(array);
        var pointer = ref.alloc(this._type.byte_ptr, buf);
        var res = this._lib.testUnserialize(pointer.deref(), buf.length);
    } else {
        var res = [];
    }
    return res;
};

test_ffi.prototype.read_scoring_matrix = function (string) {
    if (this._lib.read_scoring_matrix_js) {
        var sm = new this._type.scoring_matrix;
        var pointer = this._out.scoring_matrix_alloc;
        var res = this._lib.read_scoring_matrix_js(pointer, string, string.length);
        return res;
    } else {
        return 0;
    }
};

test_ffi.prototype.tmp_unserialize = function () {
    if (this._lib.tmpUnserialize) {
        var res = this._lib.tmpUnserialize();
        return res;
    } else {
        return 0;
    }
};

test_ffi.prototype.matrix_js = function () {
    if (this._lib.matrix_js) {
        console.log('data endianness: ' + ref.endianness);
        var res = this._lib.matrix_js(5, 3, 4);
        var out = this._lib.matrix_set_int(res.ref(), -1);
        var out2 = this._lib.matrix_set_double(res.ref(), -1.1);
        var out3 = this._lib.matrix_set_char(res.ref(), -1);
        switch (res.type) {
            case 1:
                console.log('data address: ' + res.data.d.address().toString(16));
                var obj1 = ref.get(res.data.d, 0, ref.coerceType('double *'));
                console.log('data address: obj1 ' + obj1.address().toString(16));
                var obj2 = ref.get(res.data.d, 8, ref.coerceType('double *'));
                console.log('data address: obj2 ' + obj2.address().toString(16));


                var doubleArrayR = ArrayType(ref.refType('double'), res.ncols);
                var obj1 = ref.get(res.data.d, 0, ref.refType(doubleArrayR));
                var f64View = new Float64Array(obj1.buffer);

                var obj1 = ref.get(res.data.d, 8, ref.refType(doubleArrayR));
                var f64View2 = new Float64Array(obj1.buffer);


                var f64View2 = new Float64Array(obj2.buffer);
                var doubleArrayRR = ArrayType(ref.refType(doubleArrayR), res.ncols);
                //res.data.type = ref.refType(doubleArrayRR);
                var obj = ref.get(res.data, 0, ref.refType(doubleArrayRR));
                var f264View = new Float64Array(obj.buffer);
                var mat_pointer = ref.readObject(obj, 0);
                break;
            case 2:
                console.log('data address: ' + res.data.i.address().toString(16));
                var obj1 = ref.get(res.data.i, 0, ref.coerceType('int64 *'));
                console.log('data address: obj1 ' + obj1.address().toString(16));
                var obj2 = ref.get(res.data.i, 8, ref.coerceType('int64 *'));
                console.log('data address: obj2 ' + obj2.address().toString(16));


                var int64ArrayR = ArrayType(ref.refType('int64'), res.ncols);
                var obj1 = ref.get(res.data.i, 0, ref.refType(int64ArrayR));
                var i64View = new Int32Array(obj1.buffer);

                var obj1 = ref.get(res.data.i, 8, ref.refType(int64ArrayR));
                var i64View2 = new Int32Array(obj1.buffer);
                break;
            case 4:
                console.log('data address: ' + res.data.c.address().toString(16));
                var obj1 = ref.get(res.data.c, 0, ref.coerceType('char *'));
                console.log('data address: obj1 ' + obj1.address().toString(16));
                var obj2 = ref.get(res.data.c, 8, ref.coerceType('char *'));
                console.log('data address: obj2 ' + obj2.address().toString(16));


                var charArrayR = ArrayType(ref.refType('char'), res.ncols);
                var obj1 = ref.get(res.data.c, 0, ref.refType(charArrayR));
                var i8View = new Int8Array(obj1.buffer, 0, res.ncols); // 3 is right length of output i8view 

                var obj1 = ref.get(res.data.c, 8, ref.refType(charArrayR));
                var i8View2 = new Int8Array(obj1.buffer); //wrong length of output i8view
                break;

        }
        var mat_pointer = ref.readPointer(res.data, 0, res.nrows);
        for (i = 0; i < res.nrows; i++) {
            //		mat_pointer[i].type = 
            var buf = ref.readPointer(mat_pointer[i].deref(), 0, res.nrows);
        }
        ref.readObject(mat_pointer[0])
        var tncols = res.ncols;
        var tnrows = res.nrows;
        ref.get()
        return res;
    } else {
        return 0;
    }
};

test_ffi.prototype.matrix_js_i = function () {
    if (this._lib.matrix_js) {
        var res = this._lib.matrix_js_i(10, 10, 1);
        return res;
    } else {
        return 0;
    }
};

test_ffi.prototype.matrix_js_d = function () {
    if (this._lib.matrix_js) {
        var res = this._lib.matrix_js_d(10, 10, 1);
        return res;
    } else {
        return 0;
    }
};

test_ffi.prototype.EXPECT = function (x, out) {
    var out = out || [];
    var code = new Function('test', 'out', "return (" + x + ");");
    var status = (code(this, out)) ? 'passed' : 'fault';
    this.logger.log(x + ':' + status);
};
module.exports = new test_ffi();