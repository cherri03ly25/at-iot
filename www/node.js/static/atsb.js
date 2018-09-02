/********************************************************
 * This source code is licensed under the MIT license.	*
 * See LICENSE in the project root folder for more info.*
 ********************************************************/

/**
 * atsb.js
 * 
 * Created on: 2018-07-20
 * Last Modified: 2018-09-02
 * Author(s): Veli-Matti Rantanen
 **/

const ATSBDefaultOptions = {
		namespace: "/atsb",
		refreshRate: 60,
		timeScope: 60
}

function initATSB(opt={}, query={}) {
	var opt = {...ATSBDefaultOptions, ...opt};
	
	var devices = {};
	var streams = {};
	var subscriptions = [];
	
	/* Connect to namespace */
	var atsb = io(`${opt.namespace}`, {query: query});
	
	/* Export variables */
	atsb.devices = devices;
	atsb.opt = opt;
	atsb.streams = streams;
	
	/* Class to manage the data streams */
	class Stream {
		constructor(id, name, style) {
			this.id 		= id;		// Sensor identifier
			this.name 		= name;		// Sensor name
			this.style 		= style;	// Graph style
			this.viewers 	= {};		// Viewer ids, and callbacks to remove them.
			this.streaming 	= false;	// Whether receiving data or not
			this.maxIdx 	= 0;		// Index of max value
			this.minIdx 	= 0;		// Index of min value
			this.maxSize	= 600;		// A "soft" limit on the size of the array.
			this.cache 		= [];		// Cached data points
			
			console.log("New stream: " + this.id);
			
			setInterval(()=>this.clean(), 15000);
		}
		
		/**
		 *@brief	Get the minimum value.
		 **/
		get minValue() {
			return this.min().value;
		}
		
		/**
		 *@brief	Get the maximum value.
		 **/
		get maxValue() {
			return this.max().value;
		}
		
		/**
		 *@brief	Get the point with the lowest value
		 **/
		min() {
			if(this.cache.length > this.minIdx)
				return this.cache[this.minIdx];
			else
				return 0;
		}
		
		/**
		 *@brief	Get the point with the highest value
		 **/
		max() {
			if(this.cache.length > this.maxIdx)
				return this.cache[this.maxIdx];
			else
				return 0;
		}
		
		/**
		 *@brief	Sort the cache newest to oldest
		 **/
		sort() {
			this.cache.sort((a,b)=>(b.time-a.time));
		}
		
		/**
		 *@brief	Enter points into the cache
		 **/
		receive(...points) {
			this.cache.unshift(...points);
			this.recalcMinMax();
		}
		
		/**
		 *@brief 	Recalculate the min/max values
		 **/
		recalcMinMax() {
			var max = 0;
			var min = 0;
			for (var i in this.cache) {
				if (this.cache[i].value < this.cache[min].value)
					min = i;
				if (this.cache[i].value > this.cache[max].value)
					max = i;
			}
			this.maxIdx = max;
			this.minIdx = min;
		}
		
		/**
		 *@brief	Reset the stream.
		 *@detail	The stream is stopped, the viewers removed and data discarded.
		 **/
		reset() {
			this.stop();
			this.cache.length = 0;
			for (var unwatch of this.viewers) unwatch();
		}
		
		/**
		 *@brief	Reduce the size of the data array if needed.
		 *@detail	The size of the array is reduced only if data is being
		 *			streamed, and it exceeds maxSize by at least 10%.
		 **/
		clean() {
			if (this.streaming && this.cache.length >= 1.1 * this.maxSize)
			{
				this.cache.length = this.maxSize;
				this.recalcMinMax();
			}
		}
		
		/**
		 *@brief	Stop the stream.
		 **/
		stop() {
			this.streaming = false;
			atsb.emit("Unsubscribe", this.id);
			console.log("Stopping stream for: " + this.id);
		}
		
		/**
		 *@brief	Starts/resumes the stream.
		 **/
		start() {
			this.streaming = true;
			atsb.emit("Subscribe", this.id);
			console.log("Starting stream for: " + this.id);
		}
		
		join(id, callback) {
			this.viewers[id] = callback;
			if (!this.streaming) this.start();
		}
		
		leave(id) {
			delete this.viewers[id];
		}
	}
	
	/**
	 *@brief	Create a device.
	 **/
	function createDevice(dev) {
		devices[dev.mac] = {
			mac: dev.mac,
			name: dev.name,
			sensors: {}
		};
		
		for (var sensor of dev.sensors) {
			var sid = sensor.id+"@"+dev.mac;
			streams[sid] = new Stream(sid, sensor.name, sensor.style);
			devices[dev.mac].sensors[sid] = streams[sid];
		}
	}
	
	/* Handle the server sending a device list */
	atsb.on("DeviceList", list=>{
		console.log("Device list updated.");
		list.forEach(createDevice);
		updateDeviceLists();
	});
	
	/* Handle the server announcing a new device */
	atsb.on("DeviceAdded", devInfo=>{
		createDevice(devInfo);
		console.log("New device added: "+devInfo.mac);
		updateDeviceLists();
	});
	
	/* Handle the server announcing a changed device */
	atsb.on("DeviceUpdated", devInfo=>{
		console.log("Device updated: "+devInfo.mac);
		var dev = devices[devInfo.mac];
		dev.name = devInfo.name; // Update name
		/* Update sensors */
		devInfo.sensors.forEach(s=>{
			if (dev.sensors[s.id]) {
				var sensor = dev.sensors[s.id];
				sensor.name = s.name;
				sensor.style = s.style;
			} else {
				dev.sensors[s.id] = new Stream(s.id, s.name, s.style);
			}
		});
		
		updateDeviceLists();
	});
	 
	/* Handle incoming data */
	atsb.on("Data", (...data)=>{
		var stream = streams[data[0].id];
		if (stream) {
			if (data.length > 1)
				console.log("Received blob of size: " + data.length);
			stream.receive(...data);
		} else {
			console.log("Received data from an unknown stream.");
		}
	});

	/**
	 *@brief	Get a sensor/device by its identifier.
	 **/
	atsb.getByIdentifier = (id) => {
		id = id.split("@");
		var mac = id[1];
		var sid = id[0];
		var dev = devices[mac];
		if (id.length == 2) {
			for (var i in dev.sensors) {
				if(dev.sensors[i].id == sid) return dev.sensors[i];
			}
			return null;
		}
		return dev;
	};

/***********************************************************************
 *                                                                     *
 *                      CANVAS FUNCTIONALITY                           *
 *                                                                     *
 ***********************************************************************/

	var transfer = null;
	
	class Canvas {
		constructor(element) {
			this.element = element;
			if(!this.element.id) this.element.id = randomId();
			
			this.element.classList.add("atsb-canvas");
			this.element.atsb = this;
			
			/* The click/drop handler */
			// TODO: "this" keyword will likely act up here.
			function drop(evt) {
				var stream = streams[transfer];
				
				if (transfer !== null && stream) {
					console.log("Transfer received: " + transfer);
					console.log(stream);
					this.atsb.watch(stream);
				}
				
				transfer = null;
			};
			
			this.element.ondrop = (evt) => {evt.preventDefault(); drop(evt);}
			this.element.onclick = drop;
			this.element.ondragover = (evt) => evt.preventDefault();		
			
			// The watchlist will be populated by the streams that the canvas
			// is following.
			this.watchList = [];
			this.limits = null;
			
			this.padding = 20;
			
			this.grid = {
				horizontal: 3,
				vertical: 7
			};
			
			this.update();
		}
		
		get id() {
			return this.element.id;
		}
		
		get node() {
			return this.element;
		}
		
		get width() {
			return this.element.width;
		}
		
		set width(w) {
			return this.element.width = w;
		}
		
		get height() {
			return this.element.height;
		}
		
		set height(h) {
			return this.element.height = h;
		}
		
		/**
		 *@brief	A getter for the limits of the canvas
		 **/
		get limits() {
			return this.__limits;
		}
		
		/**
		 *@brief	A setter for the limits of the canvas
		 *@detail	Sets the limits to defaults if object is equal to null.
		 **/
		set limits(obj) {
			return (this.__limits = obj!=null ? {...obj} : {
				startTime: null,
				endTime: null,
				duration: 60,
				maxValue: null,
				minValue: null
			});
		}
		
		/**
		 *@brief	Add a stream to the canvas
		 **/
		watch(stream) {
			if (!this.watchList.includes(stream)) {
				this.watchList.push(stream);
				stream.join(this.id, ()=>this.unwatch(stream));
			}
		}
		
		/**
		 *@brief	Remove a stream from the canvas
		 **/
		unwatch(stream) {
			var idx = this.watchList.indexOf(stream);
			if (idx >= 0) {
				this.watchList.splice(idx, 0);
				stream.leave(this.id);
			}
		}
		
		/**
		 *@brief	Returns the width/height ratio of the canvas.
		 **/
		get ratio() {
			return this.width/this.height;
		}
		
		/**
		 *@brief	Calculate the scaling for the canvas
		 **/
		scaling() {
			var min = Infinity;
			var max = -Infinity;
			
			if (!(this.limits.maxValue !== null && this.limits.minValue !== null))
				this.watchList.forEach(stream=>{
					if(stream.minValue < min) min = stream.minValue;
					if(stream.maxValue > max) max = stream.maxValue;
				});
			max = max > 1 ? max : 1;
			min = min < -1 ? min : -1;
			max = this.limits.maxValue !== null ? this.limits.maxValue : max;
			min = this.limits.minValue !== null ? this.limits.minValue : min;	
			
			var diff = max - min;
			
			var end = this.limits.endTime !== null
						? this.limits.endTime 
						: Date.now();
			
			var start = this.limits.startTime !== null
						? this.limits.startTime 
						: end - this.limits.duration * 1000;
			
			var duration = end - start;
			
			var scale = {
				x: this.width/duration,
				y: (this.height-2*this.padding)/diff
			};
			
			return {
				scale: scale,
				scope: {
					start: start,
					duration: duration,
					end: end,
					min: min,
					diff: diff,
					max: max
				}
			};
		}
		
		/**
		 *@brief	Repaints the canvas
		 **/
		repaint() {
			var longer = this.ratio < 1 ? this.width : this.height;
			var largeFont = longer/10+"px Arial";
			var smallFont = longer/15+"px Arial";
			
			var ctx = this.element.getContext("2d");
			
			if (this.watchList.length == 0) {
				/* Clear canvas */
				ctx.clearRect(0, 0, this.width, this.height);
				
				ctx.textAlign = "center";
				ctx.strokeStyle = "#000000";
				
				ctx.font = largeFont;
				ctx.fillText("This canvas is empty", 
						this.width/2, this.height/2 - longer/20);
				ctx.font = smallFont;
				ctx.fillText("Drag and drop something here!",
							this.width/2, this.height/2 + longer/30);
			} else {
				var scaling = this.scaling();
				
				/* Clear canvas */
				ctx.clearRect(0, 0, this.width, this.height);
				
				/* Paint the grid */
				ctx.textAlign = "left";
				ctx.font = smallFont;
				ctx.strokeStyle = "#454545";
				ctx.lineWidth = 1;
				
				/* Paint the horizontal lines */
				for(var y = 1; y <= this.grid.horizontal; y++)
				{
					var ry = y/(this.grid.horizontal + 1);
					
					/* Paint horizontal line */
					ctx.beginPath();
					ctx.moveTo(0, ry*this.height);
					ctx.lineTo(this.width, ry*this.height);
					ctx.stroke();
					
					/* Draw the label */
					var val = scaling.scope.max-ry*scaling.scope.diff;
					val = Math.abs(val)>10 ? Math.round(val) : val.toPrecision(2);
					ctx.fillText(val, 5, ry*this.height-5);
				}
				
				/* Paint the vertical lines */
				for(var x = 1; x <= this.grid.vertical; x++)
				{
					var rx = x/(this.grid.vertical + 1);
					
					/* Paint the line */
					ctx.beginPath();
					ctx.moveTo(rx*this.width, 0);
					ctx.lineTo(rx*this.width, this.height);
					ctx.stroke();
					
					var time = "T-"+Math.round(scaling.scope.duration*(1-rx)/1000)+"s";
					ctx.fillText(time, rx*this.width+5, this.height-5);
				}
				
				/* Paint the graphs */
				ctx.lineWidth = 2;
				this.watchList.forEach(stream=>{					
					// Helper function to calculate the point on canvas based
					// on data point value and time.
					var padding = this.padding;
					function getPoint(pt) {
						return [
							(pt.time - scaling.scope.start) * scaling.scale.x,
							(scaling.scope.max - pt.value) * scaling.scale.y 
							+ padding
						];
					}
					
					//
					ctx.strokeStyle = stream.style;
					ctx.beginPath();
					
					for (var idx in stream.cache) {
						var pt = getPoint(stream.cache[idx]);
						if (idx == 0) {
							ctx.moveTo(...pt);
						} else {
							ctx.lineTo(...pt);
						}
					}
					
					ctx.stroke();
				});
			}
		}
		
		/**
		 *@brief	Update the canvas
		 **/
		update() {
			if (this.width != this.element.offsetWidth)
				this.width = this.element.offsetWidth;
			if (this.height != this.element.offsetHeight)
				this.height = this.element.offsetHeight;
			
			this.repaint();
			setTimeout(()=>this.update(), 1000/opt.refreshRate);
		}
	};

	/**
	 *@brief	Creates and returns a new ATSB-initialized canvas.
	 **/
	atsb.createCanvas = ()=>{
		var c = document.createElement("canvas");
		return new Canvas(c);
	}
	
	/**
	 *@brief	Initializes the given canvas for use with ATSB.
	 **/
	atsb.initCanvas = (canvas)=>{
		return new Canvas(canvas);
	}
	
/***********************************************************************
 *                                                                     *
 *                     OTHER ATSB HTML ELEMENTS                        *
 *                                                                     *
 ***********************************************************************/
	
	var devLists = [];
	
	/**
	 *@brief	Drag (and drop) functionality for sensor and device elements
	 **/
	function drag(event) {
		transfer = this.identifier;
	}
	
	/**
	 *@brief	Click-to-hide functionality for device/list elements.
	 **/
	// FIXME: Unused
	function clickToHide(event) {
		this.open = !this.open;
		this.findAllATSB().forEach(e=>{
			e.style.display = this.open?null:"none";
		});
	}
	
	/**
	 *@brief	Create an ATSB-initialized sensor element.
	 **/
	function newSensorElement(sensor, mac) {
		var el = document.createElement("div");
		el.id = randomId();
		el.classList.add("atsb-sensor");
		el.setAttribute("draggable", true);
		el.identifier = sensor;
		el.sensor = devices[mac].sensors[sensor];
		
		var ind = document.createElement("span");
		ind.id = "indicator@" + el.id;
		ind.classList.add("atsb-style-indicator");
		ind.style.backgroundColor = el.sensor.style;
		el.appendChild(ind);
		
		var lbl = document.createElement("b");
		lbl.id = "label@" + el.id;
		lbl.classList.add("atsb-label");
		lbl.innerHTML = el.sensor.name;
		el.appendChild(lbl);
		
		el.update = () => {
			// TODO: Something?
		}
		
		console.log("New sensor: "+el.identifier);
		
		el.ondragstart = drag;
		el.onclick = (evt) => {
			evt.preventDefault();
			console.log("Clicked "+el.identifier);
			transfer = el.identifier;
		};
		
		return el;
	}
	
	/**
	 *@brief Create an ATSB-initialized device element.
	 **/
	function newDeviceElement(mac) {
		var el = document.createElement("div");
		el.id = randomId();
		el.classList.add("atsb-device");
		//FIXME: Device drag-and-drop.
		//el.setAttribute("draggable", false);
		el.open = true;
		el.device = devices[mac];
		el.identifier = mac;

		var hdr = document.createElement("div");
		hdr.id = "header@"+el.id;
		hdr.classList.add("atsb-device-header");
		el.appendChild(hdr);
		
		/**
		 *@brief	Find all ATSB child elements 
		 **/
		el.findAllATSB = () => {
			var atsbNodes = [];
			
			for (var k in el.childNodes) {
				if (el.childNodes[k].identifier)
					atsbNodes.push(el.childNodes[k]);
			}
			
			return atsbNodes;
		};
		
		/**
		 *@brief	Update the element, making it match with the stored one.
		 *@detail	The update is slightly delayed, to catch multiple updates
		 *			into one.
		 *@detail	This function does not block.
		 **/
		el.update = () => {
			/* Just to be safe, use a lock */
			if (!el.updateLock) {
				el.updateLock = true;
				setTimeout(()=>{
					var sensors = el.findAllATSB();
					// FIXME: Device name doesn't update
					/* Update the name of the device in case it changed */
					hdr.innerHTML = `${el.device.name} (${mac})`;
					/* Grab a list of sensor identifiers */
					var ids = [];
					for (var id in el.device.sensors) ids.push(id);
					var toRemove = [];
					/** 
					 *	Figure out which sensor elements to remove, and for which
					 * 	identifiers new elements need to be created
					 **/
					sensors.forEach(sel=>{
						var idx = ids.indexOf(sel.identifier);
						if(idx >= 0) {
							ids.splice(idx, 1);
						} else {
							toRemove.push(sel);
						}
					});
					/**
					 *	Remove the elements that don't match a sensor.
					 *	Update the elements that do.
					 *	Add new elements for sensors that don't have ones.
					 **/
					toRemove.forEach(el.removeChild);
					sensors.forEach(sel => sel.update());
					ids.forEach(id => el.appendChild(
									newSensorElement(id, el.identifier)
								));
					el.updateLock = false;
				}, 100);
			}
		};

//		FIXME: Device drag-and-drop
//		el.onclick = clickToHide;
//		el.ondragstart = dragElement;
		
		el.update();
		
		return el;
	}
	
	/**
	 *@brief	Creates and returns a new device list element.
	 **/
	function newDeviceList() {
		var list = document.createElement("div");
		list.classList.add("atsb-list");
		
		var hdr = document.createElement("div");
		hdr.classList.add("atsb-list-header");
		hdr.innerHTML = "Connected Devices";
		list.appendChild(hdr);
		list.header = hdr;
		
		var innerList = document.createElement("div");
		innerList.classList.add("atsb-list-inner");
		list.appendChild(innerList);
		
		/**
		 *@brief	Find all ATSB child elements 
		 **/
		list.findAllATSB = () => {
			var atsbNodes = [];
			for (var node of innerList.childNodes) {
				if (node.identifier) atsbNodes.push(node);
			}
			return atsbNodes;
		}
		
		/**
		 *@brief	Update the list and all its children.
		 **/
		list.update = () => {
			/* Use a lock, just in case */
			if(!list.updateLock) {
				list.updateLock = true;
				/* Do not block */
				setTimeout(()=> {
					var ids = Object.keys(devices);
					var toRemove = [];
					var listDevs = list.findAllATSB();

					/* Find the devices to remove and remove existing devices from list */
					listDevs.forEach(de=>{
						var idx = ids.indexOf(de.identifier);
						if(idx >= 0) {
							ids.splice(idx, 1);
						} else {
							toRemove.push(de);
						}
					});
					
					/* Update devices */
					listDevs.forEach(de=>de.update());
					/* Remove devices that were removed */
					toRemove.forEach(innerList.removeChild);
					/* Add devices that were added */
					ids.forEach(id=>innerList.appendChild(newDeviceElement(id)));
					
					/* Sort the devices */
					// FIXME: Not sure this works, at all.
					var nodes = list.findAllATSB();
					var elarr = [];
					for (var node of nodes){
						if(node.nodeType == 1) {
							elarr.push(node);
						}
					}
					
					elarr.sort((a,b)=> a.innerHTML==b.innerHTML? 0 : a.innerHTML>b.innerHTML? 1 : -1);
					elarr.forEach(el=>innerList.appendChild(el));
					
					/* Log */
					console.log(`Device List updated. `
								+ `${toRemove.length} removed, `
								+ `${listDevs.length} updated and `
								+ `${ids.length} added.`);
					/* Unlock */
					list.updateLock = false;
				}, 100);
			}
		};
		
		list.update();
		
		/* Set updating on an interval */
		setInterval(list.update, 15000);
		
		devLists.push(list);
		
		return list;
	};
	
	/**
	 *@brief	Update all device lists.
	 **/
	function updateDeviceLists() {
		if(devLists)
			devLists.forEach(l=>l.update());
		console.log("List updates queued.");
	};
	
/***********************************************************************
 *                                                                     *
 *                       EXPORTED ATSB FUNCTIONS                       *
 *                                                                     *
 ***********************************************************************/
	
	/* Request device synchronization */
	atsb.refreshDevices = () => atsb.emit("ListDevices");
	
	/* Export functions */
	atsb.newDeviceList 			= newDeviceList;
	
/***********************************************************************
 *                                                                     *
 *                    COMPLETE THE INITIALIZATION                      *
 *                                                                     *
 ***********************************************************************/
	
	return atsb;
	
/***********************************************************************
 *                                                                     *
 *                  MISCELLANEOUS UTILITY FUNCTIONS                    *
 *                                                                     *
 ***********************************************************************/
	
	/**
	 *@brief	Generate a random id for an element.
	 *@detail	The identifiers start with 'atsb#' and end with 8 hexadecimal
	 *			characters.
	 **/
	function randomId() {
		var id = "atsb#"+Math.random().toString(16).slice(-8);
		/* Ensure uniqueness. */
		return document.getElementById(id) ? randomId()	: id;
	}
}


