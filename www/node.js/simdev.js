/*********************************************************
 * This source code is licensed under the MIT license.	 *
 * See LICENSE in the project root folder for more info. *
 *********************************************************/

/**
 * sim_beacon.js
 * 
 * Created on: 2018-09-02
 * Last Modified: 2018-09-02
 * Author(s): Veli-Matti Rantanen
 **/
 
const crypto = require("crypto");
const request = require("request");

/**
 *@brief	Generate random hex byte.
 **/
function rbyte() {
	return Math.random().toString(16).slice(-2);
}

var destination = process.argv[2];
var interval = parseInt(process.argv[3], 10);
interval = interval ? interval : 500;
var values = [0, 0, 0, 0];
var min = -100;
var max = 100;
var mac = `ff:ff:ff:${rbyte()}:${rbyte()}:${rbyte()}`;
var key = Buffer.from(crypto.randomBytes(32));
var ctr = 0;
var diff = max-min;

/**
 *@brief	Return a random value floating in the range (min, max).
 **/
function nextValue(n) {
	return (min-values[n]) * Math.random() 
			- (max-values[n]) * Math.random();
}

function parseHex(num) {
	num = Math.round(num);
	num = num < 0 ? num+0x10000 : num;
	num = "000"+num.toString(16);
	return num.slice(-4);
}

/**
 *@brief	POST data to the remote server.
 **/
var post = (data) => request.post(
				destination,
				{json: data},
				(err, res, body) => {
					if (!err) {
					} else {
						console.log(err);
					}
			});
			
/**
 *@brief	Pack the data into the correct format.
 **/
function pack(...data) {
	var fields = [("00000000"+ctr.toString(16)).slice(-8)];
	
	data.forEach(rdg=>fields.push(parseHex(rdg)));
	
	/* Insert the type fields */
	fields.splice(3, 0, "fffe");
	fields.push("ffff");
	
	var packed = fields.join("");
	
	const cipher = crypto.createCipheriv("aes-256-ecb", key, "");
	cipher.setAutoPadding(false);
	packed = cipher.update(packed, "hex", "hex");
	
	packed += crypto.createHmac("sha256", key).update(packed).digest("hex");
	
	return packed.toString("hex").slice(0, 54);
}

/* Start pinging the server at the given intervals. */
setInterval(()=>{
	var data = [];
	for (var i in values) {
		values[i] = nextValue(i);
		data.push(values[i]);
	}
	ctr++;
	data = pack(...data);
	post({mac: mac, data: data});
}, interval);

setInterval(()=>{
		console.log("Current values: [" + 
			values.map(v=>(parseHex(v) + " (" + Math.round(v) + ")")).join(", ") + "]")
	}, 15000);
			
console.log("Simulating a broadcasting device.");
console.log("Command to add the device:\n\tnew device "+mac+" "+key.toString("hex")+" \"Spoofed Device\"");
