/*********************************************************
 * This source code is licensed under the MIT license.	 *
 * See LICENSE in the project root folder for more info. *
 *********************************************************/

/**
 * parser.js
 * 
 * Created on: 2018-08-17
 * Last Modified: 2018-08-29
 * Author(s): Veli-Matti Rantanen
 **/
 
const crypto = require("crypto");

var defaultSensor = {
	size: 16, signed: false,
	name: "Unnamed Sensor",
	precision: 1, color: "black"
};

var sensorTypes = {
	"default": [defaultSensor, defaultSensor]
}

module.exports = function(atsb, loadedSensorTypes={}) {
	/* Helper functions */
	function registerType(type, arr) {
		if ((typeof type != "string" || arr.constructor.name != "Array")) 
			return;
		
		if (!sensorTypes[type])
		{
			var length = 0;
			sensorTypes[type] = [];

			arr.forEach(e=>{
				e = {
					...defaultSensor,
					...e
				};
				sensorTypes[type].push(e);
				length += e.size;
			});
			
			if (length != 32) {
				delete sensorTypes[type];
				throw new Error("Wrong total size (" + length 
								+ " bits) for sensor type '"+type+"'."
								+ " Has to be 32 bits.");
			}
		} else {
			throw new Error("Re-definition of sensor type '"+type+"'.");
		}
	}
	
	/**
	 *@brief	Decrypt data.
	 *
	 *@param	data		The data in hex encoding.
	 *@param	key			The key, either as buffer or string.
	 *
	 *@returns	Decrypted data in hex encoding.
	 **/
	function decrypt(data, key) {
		const decipher = crypto.createDecipheriv("aes-256-ecb", key, "");
		decipher.setAutoPadding(false);
		return decipher.update(data, "hex").toString("hex");
	}
	
	/**
	 *@brief	Compute HMAC for data authentication.
	 *
	 *@param	data		The data to compute HMAC for, in hex encoding.
	 *@param	key			The key to use
	 *
	 *@returns	The HMAC in hex encoding.
	 **/
	function hmac(data, key) {
		const hmac = crypto.createHmac("sha-256", key);
		hmac.update(data, "hex");
		return hmac.digest("hex");
	}
	
	/* For each key-value pair in loadedSensorTypes... */
	Object.entries(loadedSensorTypes).forEach(kvp=>registerType(...kvp));
	
	/* Parser code */
	var parser = {};
	
	parser.registered = () => {
		var out = "";
		for (var k in sensorTypes) {
			out += `['${k}']: ${sensorTypes[k].map(f=>f.name).toString()}\n`
		}
		return out;
	}
	
	/**
	 *@brief	A function for the selector to call to determine whether
	 *			this parser can parse the given message.
	 **/
	parser.canParse = (msg) => msg.mac !== undefined 
						&& msg.data !== undefined
						&& msg.data.length == 54;
	
	/**
	 *@brief	The function that the selector calls to parse message contents.
	 **/
	parser.parse = (msg, msgId) => {
		function parseField(field) {
			var type = field.slice(8,12);
			var info = sensorTypes[type];
			if (!info) info = sensorTypes["default"];
			
			var offset = 0;
			var parsed = [];
			for (var i in info)
			{
				var slc = field.slice(offset, offset+info[i].size/4);
				offset += info[i].size/4;
				var val = parseInt(slc, 16);
				
				if (info[i].signed && slc.match(/^[89a-fA-F]/)) 
						val -= (0x1<<info[i].size);
				
				parsed.push({
					sensor: {
						id: type + i,
						name: info[i].name,
						style: info[i].style
					},
					value: val*info[i].precision
				});
			};
			return parsed;
		}
		
		/* Check that the device exists */
		var dev = atsb.getDevice(msg.mac);
		if (!dev) {
			atsb.emit("Warning", "UnknownDeviceWarning", msgId, msg.mac,
						`Data was received from unknown device '${msg.mac}'.`
						+ ` (${msgId})`);
			return;
		}
		var key = dev.key;
		
		/* Authenticate the message */
		var cpld = msg.data.slice(0,32);
		
		if (hmac(cpld, key).slice(0,22) != msg.data.slice(32, msg.data.length)) 
		{
			/* Authentication failed. Log it. */
			atsb.emit("Warning", "HMACValidationWarning", msgId,
				`Received data had invalid HMAC. (${msgId})`);
			dev.log("InvalidHMAC");
			return;
		} else {
			/* Authentication succeeded, decrypt message. */
			var pld		= parser.decrypt(cpld, key);
			/* Check counter */
			if (!dev.isNextCounter(parseInt(pld.slice(0, 8), 16)))
			{
				/* Counter was invalid/duplicate. */
				atsb.emit("Warning", "CounterValidationWarning", msgId,
					`Received data had invalid counter. (${msgId})`);
				return;
			} else {
				/* Counter was valid. Authentication complete. */						
				var fields 	= [];
				
				fields.push(...parseField(pld.slice(8,20)),
							...parseField(pld.slice(20,32)));
				
				var sensors = [];
				var data = [];

				fields.forEach(fld=>{
					sensors.push(fld.sensor);
					data.push({
						id: `${fld.sensor.id}@${msg.mac}`,
						value: fld.value,
						time: msg.time
					});
				});
				
				/* Have the device check if it needs to update */
				dev.update(...sensors);
				return data;
			}
		}
	}
	
	return parser;
};
