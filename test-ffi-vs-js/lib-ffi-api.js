/* lib-ffi-api.cpp
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */

var ref = require('ref');

var char_ptr = ref.refType('char');
var float_ptr = ref.refType('float');

module.exports = {
	api: {
		"voidFunc": ["void", []],
		"charFunc": ["char", ["char", char_ptr]],
		"floatFunc": ["float", ["float", float_ptr]]
	},
	out: {
		"char": ref.alloc('char'),
		"float": ref.alloc('float')
	}
};