/********************************************************
 * This source code is licensed under the MIT license.	*
 * See LICENSE in the project root folder for more info.*
 ********************************************************/

/**
 * device.js
 * 
 * Created on: 2018-07-20
 * Last Modified: 2018-08-13
 * Author(s): Veli-Matti Rantanen
 **/
 
const SensorDefaultOptions = {
	label: "N/A",
	maxSamples: 600,	// Maximum n.o. samples
	maxTime: 60,		// Maximum time period, in seconds
}

/**
 *@brief	SensorData is a subclass of Array
 *@detail	The points are automatically sorted from newest to oldest.
 *@detail	The number of points and time period can be restricted.
 *
 **/
class SensorData extends Array{
	constructor(mac, type, ...pts, opt={}) {
		super(...pts);
		this.options 		= {...SensorDefaultOptions, ...opt};
		this.label 			= this.options.label;
		this.buffer 		= 0;
		this.maxSamples		= this.options.maxSamples;
		this.maxTime		= this.options.maxTime;
	}
	
	constrain() {
		if (this.length > this.maxSamples)
			this.length = this.maxSamples;
		else {
			// Find the index out of bounds
			var OOBI = this.getIndex(this.newest-this.maxTime);
		 	if (OOBI>=0)
		 		this.length = OOBI;
		}	
			
	}
	
	add(...pts) {
		this = SensorData(...pts, ...this, this.options);
		this.sort((a,b) => (b.time < a.time)-(a.time < b.time));
	}
	
	get newest() {
		return this[0];
	}
	
	get oldest() {
		return this[this.length];
	}
	
	getIndex(t) {
		for(var i = 0; i < this.length && this[i].time < t && return i; i++);
		return -1;
	}
	
	/**
	 *@brief	Return the number of 
	 *
	 **/
	getPointsIn(start, end) {
		var si = 0;
		var ei = 0;
		for (; ei < this.length && this[ei].time > end; ei++);
		for (si=ei; si < this.length && this[si].time > start; si++);
		if (si == ei)
			return SensorData(this.mac, this.type, this.options);
		return this.slice(ei, si);
	}
};

class SensorElement extends HTMLElement {
	constructor () {
		super();
	}
};

class Device {
	constructor(mac, name) {
		this.mac = mac;
		this.name = name;
		this.sensors = [];
	}
};

if ( module !== undefined ) {
	module.exports = {
		Device: Device,
		Sensor: Sensor,
	};
}
