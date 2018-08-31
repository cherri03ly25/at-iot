/********************************************************
 * This source code is licensed under the MIT license.	*
 * See LICENSE in the project root folder for more info.*
 ********************************************************/

/**
 * atsb.js
 * 
 * Created on: 2018-07-20
 * Last Modified: 2018-08-23
 * Author(s): Veli-Matti Rantanen
 **/

const ATSBDefaultOptions = {
		namespace: "/atsb",
		paintInterval: 50,
		timeScope: 60,
		query: {}
	};

function initATSB(opt={}) {	
	var devices = {};
	var opt = {...ATSBDefaultOptions, ...opt};
	var cache = {};
	
	var subscriptions = [];
	var atsb = io(`${opt.namespace}`, {query: {...opt.query}});
	
	/* Export variables */
	atsb.devices = devices;
	atsb.opt = opt;
	
	/* Handle the server sending a device list */
	atsb.on("DeviceList", list=>{
			console.log("Device list updated.");
			console.log(list);
			list.forEach(dev=>devices[dev.mac] = {...dev});
			updateDeviceLists();
		});
	
	/* Handle the server announcing a new device */
	atsb.on("DeviceAdded", devInfo=>{
			devices[devInfo.mac] = {...devInfo};
			console.log("New device added");
			updateDeviceLists();
		});
	
	/* Handle the server announcing a changed device */
	atsb.on("DeviceUpdated", devInfo=>{
		console.log("Device updated");
		var dev = devices[devInfo.mac];
		dev.name = devInfo.name;
		var ids = dev.sensors.map(s=>s.id);
		/* Append the sensors that are not already in the list */
		dev.sensors.push(...devInfo.sensors.filter(s=>!ids.includes(s.id)));
		updateDeviceLists();
	});
	
	/**
	 *@brief 	Handle server sending data
	 *
	 *@detail	Data that has same counter as prior data is discarded
	 *@detail	The counter is incremented server-side when sending new data
	 *@detail 	This prevents the client from getting confused when multiples 
	 *			of the same packet arrive from different streams. 
	 *
	 *@warning	If subscriptions are not properly kept track of, the client may
	 *			receive multiples of the same data.
	 **/
	function recalcMinMax(che) {
		var max = 0;
		var min = 0;
		for (var i in che.data) {
			if (che.data.length > 400 && i > che.data.length-100) break;
			if (che.data[i].value < che.data[min].value)
				min = i;
			if (che.data[i].value > che.data[max].value)
				max = i;
		}
		che.maxIdx = max;
		che.minIdx = min;
	}
	 
	function putInCache(msg) {
		if (!cache[msg.id]) {
			console.log("Initialized cache for "+msg.id);
			cache[msg.id] = {
				sensor: atsb.getByIdentifier(msg.id),
				maxIdx: 0,
				minIdx: 0,
				data: [{time:msg.time, value:msg.value}]
			};
			cache[msg.id].max = ()=>{
					if(!cache[msg.id].data[cache[msg.id].maxIdx]) {
						recalcMinMax(cache[msg.id]);
					}
					return cache[msg.id].data[cache[msg.id].maxIdx].value;
				}
			cache[msg.id].min = ()=>{
					if(!cache[msg.id].data[cache[msg.id].minIdx]) {
						recalcMinMax(cache[msg.id]);
					}
					return cache[msg.id].data[cache[msg.id].minIdx].value;
				}
		}else{
			var che = cache[msg.id];

			che.maxIdx = msg.value >= che.max() ? 0 : che.maxIdx + 1;
			che.minIdx = msg.value <= che.min() ? 0 : che.minIdx + 1;
			
			che.data.unshift({time:msg.time, value:msg.value});
			/* Should work without this... */
			//cache[data.id].data.sort((a,b)=>(b.time-a.time));
		}
	}
	 
	atsb.on("Data", putInCache);
	atsb.on("CachedData", (id, data)=>
			data.reverse().forEach(msg=>putInCache({...msg, id:id})));
	
	/**
	 *@brief 	Clean the caches by trimming them to a reasonable size.
	 **/
	function cleanCaches() {
		for (var k in cache) {
			if (cache[k].data.length >= 700) {
				recalcMinMax(cache[k]);
				cache[k].data.length = 600;
			}
		}
	}
	/* Set it on a 15s interval */
	setInterval(cleanCaches, 15000);

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
	
	var canvasList = [];
	
	/**
	 *@brief	Handle a drag-and-drop event on the canvas.
	 **/
	function canvasDrop(event) {
		event.preventDefault();

		var identifier = event.dataTransfer.getData("identifier");
		/** 
		 *	Add the identifier to the devices/sensors that the canvas tracks.
		 *	If something was *added* then the canvas is forcibly redrawn,
		 *	even if the data is static.
		 **/
		if (identifier && this.track(identifier)) {
			this.update();
		}
	}
	
	function clickToDrop(event) {
		if (clickTransfer && this.track(clickTransfer)) {
			this.update();
		}
	}

	/**
	 *@brief	Creates and returns a new ATSB-initialized canvas.
	 **/
	function newCanvas() {
		var c = document.createElement("canvas");
		c.id = randomId();
		
		initCanvas(c);
		
		return c;
	}
	
	/**
	 *@brief	Initializes the given canvas for use with ATSB.
	 **/
	function initCanvas(canvas) {
		canvasList.push(canvas);
		canvas.classList.add("atsb-canvas");
		
		canvas.limits	= {
			time:		60000
		};
		
		canvas.tracked = [];
		
		/**
		 *@brief	Adds an identifier to be tracked.
		 *@detail	If sensor identifiers are present when a device identifier is
		 *			added, they are all merged together.
		 *@detail	Guaranteed no duplicates
		 *
		 *@return	Whether it was added.
		 **/
		canvas.track = (identifier) => {	
			if (canvas.tracked.includes(identifier) || 
					canvas.tracked.filter(id=>identifier.endsWith(id)).length)
				return false;
			
			atsb.subscribe(identifier);
			canvas.tracked = canvas.tracked.filter(id=>!id.endsWith(identifier));
			canvas.tracked.push(identifier);
			
			if (canvas.isStatic)
				canvas.isStatic = false;
			
			console.log("Canvas " + canvas.id + " is now tracking " + identifier);
			
			return true;
		};
		
		/**
		 *@brief	Resets the canvas to a clean state.
		 **/
		canvas.reset = () => {
			atsb.unsubscribe(canvas.tracked);
			
			canvas.tracked.length = 0;
		
			var big = canvas.height/10;
			var sml = canvas.height/15;
			
			var ctx = canvas.getContext("2d");
			ctx.clearRect(0, 0, canvas.width, canvas.height);
			
			ctx.textAlign = "center";
			
			ctx.font = big+"px Arial";
			ctx.fillText("This canvas is empty", canvas.width/2, 
							canvas.height/2-big/2);
			
			ctx.font = sml+"px Arial";
			ctx.fillText("Drag and drop something here!", 
								canvas.width/2, canvas.height/2+sml/2);
		}
		
		/**
		 *@brief	Paints the canvas, clearing any previous data.
		 **/
		canvas.update = () => {
			if (canvas.updateLock)
				return;
				
			canvas.updateLock = true;
			/* If data is not static but there's nothing on graph, reset. */
			if (canvas.tracked.length == 0) {
				canvas.reset();
			/* Otherwise, paint on it. */
			} else {
				/* Get context and clear the canvas */
				var ctx = canvas.getContext("2d");
				
				/* Get tracked caches */
				var caches = Object.entries(cache).filter(e=>
										canvas.tracked.includes(e[0])
								).map(e=>e[1]);
				/* Get min/max */
				var max = -Infinity;
				var min = Infinity;
				for (var k in caches) {
					if (caches[k].max() > max)
						max = caches[k].max();
					if (caches[k].min() < min)
						min = caches[k].min();
				}
				
				max = max > 1 ? max : 1;
				min = min < -1 ? min : -1;
				
				/* Calculate various parameters for drawing */
				/* Actual min/max are overridden by user-set ones. */
				var scope = {
					maxValue: max,
					minValue: min,
					...canvas.limits
				};
				
				//scope.time *= 1000;
				scope.end = scope.end?scope.end:Date.now();
				scope.start = scope.start?scope.start:scope.end-scope.time;
				scope.time = scope.end-scope.start;
				scope.range = scope.maxValue-scope.minValue;
				
				var scale = {
					x: canvas.width/scope.time,
					y: canvas.height*0.8/scope.range
				};
				
				var cidx = 0;
				
				//console.log(caches);
				
				ctx.clearRect(0, 0, canvas.width, canvas.height);
				
				/* Paint grid */
				ctx.textAlign = "left";
				ctx.font = canvas.height/15+"px Arial";
				ctx.strokeStyle = "#454545";
				ctx.lineWidth = 1;
				var x, y;
				for(var i = 1; (y = i*canvas.lineSpacing.horizontal) < 1; i++)
				{
					ctx.beginPath();
					ctx.moveTo(0, y*canvas.height);
					ctx.lineTo(canvas.width, y*canvas.height);
					ctx.stroke();
					ctx.fillText((scope.maxValue-y*scope.range).toPrecision(2), 
								5, y*canvas.height-5);
				}
				
				for(var i = 1; (x = i*canvas.lineSpacing.vertical) < 1; i++)
				{
					ctx.beginPath();
					ctx.moveTo(x*canvas.width, 0);
					ctx.lineTo(x*canvas.width, canvas.height);
					ctx.stroke();
					ctx.fillText("T-"+Math.round(scope.time*(1-x)/1000)+"s", 
						x*canvas.width+5, canvas.height-5);
				}
				
				ctx.lineWidth = 2;
				for(var che of caches) {
					var set = che.data;
					var idx = 0;
					
					function getPoint(pt) {
						return [
							(pt.time - scope.start) * scale.x,
							(scope.maxValue - pt.value) * scale.y 
							+ canvas.height * 0.1
						];
					}
					
					ctx.strokeStyle = che.sensor.style;
					ctx.beginPath();
					ctx.moveTo(...getPoint(set[idx]));
					idx++;
					
					for(; idx < set.length; idx++) {
						ctx.lineTo(...getPoint(set[idx]));
					}
					
					ctx.stroke();
				}
			}
			canvas.updateLock = false;
		}
		
		canvas.reset();
		
		canvas.ondragover = (ev)=>ev.preventDefault();
		canvas.ondrop = canvasDrop;
		canvas.onclick = clickToDrop;
		
		/* Start automatic updates */
		setInterval(canvas.update, opt.paintInterval);
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
	function dragElement(event) {
//		event.preventDefault();
		console.log("Dragged " + this.identifier);
		event.dataTransfer.setData("identifier", this.identifier);
	}
	
	/**
	 *@brief	Click-to-hide functionality for device/list elements.
	 **/
	function clickToHide(event) {
		this.open = !this.open;
		this.findAllATSB().forEach(e=>{
			e.style.display = this.open?null:"none";
		});
	}
	
	var clickTransfer = null;
	function clickToDrag(event) {
		clickTransfer = this.identifier;
		console.log("Transfering: " + clickTransfer);
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
		el.sensor = devices[mac].sensors[
						devices[mac].sensors.map(s=>s.id+"@"+mac).indexOf(sensor)
					];
		
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
			
		}
		
		console.log("New sensor: "+el.identifier);
		
		el.ondragstart = dragElement;
		el.onclick = clickToDrag;
		
		return el;
	}
	
	/**
	 *@brief Create an ATSB-initialized device element.
	 **/
	function newDeviceElement(mac) {
		var el = document.createElement("div");
		el.id = randomId();
		el.classList.add("atsb-device");
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
			if (!el.updateLock) {
				el.updateLock = true;
				setTimeout(()=>{
					var sensors = el.findAllATSB();
					/* Update the name of the device in case it changed */
					hdr.innerHTML = `${el.device.name} (${mac})`;
					/* Grab a list of sensor identifiers from the global list */
					var ids = el.device.sensors.map(s=>s.id+"@"+mac);
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
		
		/*var lbl = document.createElement("h3");
		lbl.innerHTML = "Connected Devices";
		lbl.classList.add("atsb-list-label");
		hdr.appendChild(lbl);*/
		
		var inner = document.createElement("div");
		inner.classList.add("atsb-list-inner");
		list.appendChild(inner);
		
		/**
		 *@brief	Find all ATSB child elements 
		 **/
		list.findAllATSB = () => {
			var atsbNodes = [];
			for (var k in inner.childNodes) {
				if (inner.childNodes[k].identifier)
					atsbNodes.push(inner.childNodes[k]);
					
			}
			return atsbNodes;
		}
		
		list.update = () => {
			if(!list.updateLock) {
				list.updateLock = true;
				setTimeout(()=> {
					var ids = Object.keys(devices);
					var toRemove = [];
					var listDevs = list.findAllATSB();

					listDevs.forEach(de=>{
						var idx = ids.indexOf(de.identifier);
						if(idx >= 0) {
							ids.splice(idx, 1);
						} else {
							toRemove.push(de);
						}
					});
					
					toRemove.forEach(inner.removeChild);
					listDevs.forEach(de=>de.update());
					ids.forEach(id=>inner.appendChild(newDeviceElement(id)));
					
					var els = inner.childNodes;
					var elarr = [];
					for (var i in els){
						if(els[i].nodeType == 1) {
							elarr.push(els[i]);
						}
					}
					
					elarr.sort((a,b)=> a.innerHTML==b.innerHTML? 0 : a.innerHTML>b.innerHTML? 1 : -1);
					elarr.forEach(inner.appendChild);
					
					console.log(`Device List updated. `
								+ `${toRemove.length} removed, `
								+ `${listDevs.length} updated and `
								+ `${ids.length} added.`);
					list.updateLock = false;
				}, 100);
			}
		};
		
		list.update();
		
		setInterval(list.update, 15000);
		
		devLists.push(list);
		
		return list;
	};
	
	function updateDeviceLists() {
		devLists.forEach(l=>l.update());
		console.log("List updates queued.");
	};
	
/***********************************************************************
 *                                                                     *
 *                       EXPORTED ATSB FUNCTIONS                       *
 *                                                                     *
 ***********************************************************************/

	/* Functions to subscribe to different streams */
	atsb.subscribe 	= (...rooms) => {
		rooms = rooms.filter(r=>!subscriptions.includes(r));
		if (rooms.length) {
			subscriptions.push(...rooms);
			atsb.emit("Subscribe", rooms);
		}
	}
	atsb.unsubscribe = (...rooms) => {
		rooms = rooms.filter(r=>subscriptions.includes(r));
		if (rooms.length) {
			rooms.forEach(r=>subscriptions.splice(subscriptions.indexOf(r)));
			atsb.emit("Unsubscribe", rooms);
		}
	}
	/* Request device synchronization */
	atsb.refreshDevices = () => atsb.emit("ListDevices");
	atsb.initCanvas 			= initCanvas;
	atsb.newCanvas 				= newCanvas;
	atsb.newDeviceList 			= newDeviceList;
	
	return atsb;
	
/***********************************************************************
 *                                                                     *
 *                  MISCELLANEOUS UTILITY FUNCTIONS                    *
 *                                                                     *
 ***********************************************************************/
	
	/* Element creation functions */
	function randomId() {
		var id = "atsb#"+Math.random().toString(16).slice(-8);
		/* Ensure uniqueness. */
		return document.getElementById(id) ? randomId()	: id;
	}
}


