/********************************************************
 * This source code is licensed under the MIT license.	*
 * See LICENSE in the project root folder for more info.*
 ********************************************************/

/**
 * storage.js
 * 
 * Created on: 2018-08-23
 * Last Modified: 2018-08-29
 * Author(s): Veli-Matti Rantanen
 **/

const crypto = require("crypto");
const fs = require("fs");
const zlib = require("zlib");

module.exports = function(opt={}) {
	var storage = {};

	function encrypt(data, key, iv="") {
		const cipher = crypto.createCipheriv(
					opt.encryption.algorithm, key, iv);
		var cipherText = cipher.update(data);
		return cipherText + cipher.final();
	}
	
	function decrypt(data, key, iv="") {
		const decipher = crypto.createDecipheriv(
					opt.encryption.algorithm, key, iv);
		var plainText = decipher.update(data);
		return plainText + decipher.final();
	}
	
	storage.load(file, cb=null, ecb=null) {
		if (!cb) cb=()=>{};
		if (!ecb) ecb=()=>{};
		
		fs.readFile(opt.prefix + file, (err, data)=>{
			if (err) ecb(err);
			else {
				if (opt.encryption.enabled)
					data = decrypt(data, opt.encryption.key);
				
				if (opt.compression)
					zlib.unzip(data, (err, data)=>{
						if(err) ecb(err);
						else cb(data);
					});
				else 
					cb(data);
			}
		});
	}
	
	storage.loadSync(file) {
		var fbuf = fs.readFileSync(opt.prefix + file);
		if (opt.encryption.enabled)
			fbuf = decrypt(fbuf, opt.encryption.key);
		if (opt.compression)
			fbuf = zlib.unzipSync(fbuf);
		return fbuf;
	}
	
	storage.store(file, data, cb, ecb=null) {
	
	}
	
	return storage;
}
