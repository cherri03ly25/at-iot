/********************************************************
 * This source code is licensed under the MIT license.	*
 * See LICENSE in the project root folder for more info.*
 ********************************************************/

/**
 * device.js
 * 
 * Created on: 2018-08-03
 * Last Modified: 2018-08-29
 * Author(s): Veli-Matti Rantanen
 **/

module.exports = function() {
	/* Import the events module to extend it */
	var dev = require("./atsb_events.js")();

	/* Contains the devices, indexed by their MACs */
	dev.devices = {};
	/* Contains the devices, in order of addition */
	dev.deviceList = [];
	
	/**
	 *@brief	Find a device by its mac address.
	 **/
	dev.getDevice = (mac) => {
		return dev.devices[mac];
	}
		
	/**
	 *@brief	Get a list of all the rooms
	 *@detail	The elements are in the following format: 
	 *	 			'<sensor type><field index>@<device mac>'
	 **/
	dev.rooms = () => {
		var rooms = [];
		dev.deviceList.forEach(d=>rooms.push(...d.rooms));
		return rooms;
	}
	
	/**
	 *@brief	Get a list of all device info objects
	 **/
	dev.deviceInfo = () => {
		var info = [];
		dev.deviceList.forEach(d=>info.push(d.info));
		return info;
	}
	
	/**
	 *@brief	Get a printable list of the devices
	 **/
	dev.printableDeviceInfo = () => {
		return dev.deviceList.map(d=>{
				if (d.mac == d.name) return	d.mac;
				else return d.name + "(" + d.mac + ")";
			}).toString().split(",").join("\n");
	}
	
	/**
	 *@brief 	Clears the "recent" statistics of each device.
	 **/
	dev.resetRecentStats = () => {
		dev.deviceList.forEach(d=>d.resetRecentStats());
	}
	
	class Device {
		constructor(mac, key, name=null, sensors=null, ctr=null, stat=null) {
			this.mac = mac;
			this.key = typeof key == "string" ? Buffer.from(key, "hex") : key;
			this.name = name ? name : mac;
			this.sensors = sensors ? sensors : [];
			this.ctr = ctr!==null ? ctr : -1;
			this.stat = stat ? stat : {};
			dev.devices[mac] = this;
			dev.deviceList.push(this);
			dev.emit("DeviceAdded", this);
		}
		
		/**
		 *@brief	Returns an array from which the device can be reconstructed.
		 **/
		get storage() {
			return [this.mac, this.name, this.sensors, this.ctr, this.stat];
		}
		
		/**
		 *@brief	Returns a minimalistic set of information about the device,
		 *			intended for describing it to the clients.
		 **/
		get info() {
			return {
				mac: this.mac,
				name: this.name,
				sensors: this.sensors
			}
		}
		
		/**
		 *@brief	Returns a list of the rooms associated with the device.
		 **/
		get rooms() {
			var rooms = this.sensors.map(s=>s.id+"@"+this.mac);
			rooms.unshift(this.mac);
			return rooms;
		}
		
		/**
		 *@brief	Record any desired statistic about the device.
		 *@detail	By default, this is used to record various information
		 *			about messages received, but virtually anything could
		 *			be recorded.
		 **/
		log(type, amount=1) {		
			if (!this.stat[type]) {
				this.stat[type] = {
					total: amount,
					recent: amount
				}
			}else{
				this.stat[type].total+=amount;
				this.stat[type].recent+=amount;
			}
		}
		
		/**
		 *@brief	Validate and update the device counter.
		 *@detail	Records counter skip and invalid counters.
		 **/
		isNextCounter(ctr) {
			if (this.ctr < ctr) {
				var skip = ctr - this.ctr - 1;
				this.log("CTRSkip", skip);
				this.ctr = ctr;
				return true;
			} else {
				this.log("InvalidCTR");
				return false;
			}
		}
		
		/**
		 *@brief	Given a list of sensors, check if the device already has them
		 *			registered.
		 *
		 *@param	sensors		Sensor objects, with the following structure:
		 *						{
		 *							id: "",
		 *							name: "",
		 *							style: ""
		 *						}						
		 **/
		update(...sensors) {
			var ids = this.sensors.map(s=>s.id);
			var newSensors = sensors.filter(s=>!ids.includes(s.id));
			if(newSensors.length > 0) {
				this.sensors.push(...newSensors);
				dev.emit("DeviceUpdated", this);
			}
		}
		
		/**
		 *@brief	Clear all of the device's recent stats.
	 	 **/
		resetRecentStats() {
			for (var stat of this.stat) stat.recent = 0;
		}
		
		/**
		 *@brief	Resets the device's recent and total stats.
		 **/
		resetStats() {
			for (var stat of this.stat) {
				stat.recent = 0;
				stat.total = 0;
			}
		}
		
		/**
		 *@brief	Reset the device's counter.
		 **/
		resetCounter() {
			this.ctr = 0;
		}
	}
	
	dev.Device = Device;

	return dev;
};
