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

const crypto = require("crypto");
const fs = require("fs");
 
/**
 * Data is sent in the following format to "{Device MAC}@{Sensor Type}" room:
 * {
 *		id: <Sensor Type>@<Device MAC>,
 *		time: <ms epoch time>,
 *		value: <Sensor Reading Value>
 * }
 **/

/**
 *@brief	A private hashing function, used to generate message ids.
 **/
function hash(str, fmt="hex", algo="sha256") {
	const sha = crypto.createHash(algo);
	sha.update(str);
	return sha.digest(fmt);
};

module.exports = function (opt={}, ...services) {

/************************
 *		OPTIONS			*
 ************************/

	/* Load config. If not defined in runtime options, load default. */
	opt = {config: "./config.json", ...opt};
	/* FIXME: An error will be produced if no config is found. Should generate default. */
	opt = {...JSON.parse(fs.readFileSync(opt.config, "utf8")), ...opt};
	
	/* Parse the storage prefix */
	opt.storage.prefix = opt.storage.prefix.replace("$DIRNAME", __dirname);

/************************
 *		MODULES			*
 ************************/	

	/* Import the device sub-module to extend it */
	var atsb 	= require("./device.js")();
	/* Permissions sub-module */
	atsb.permissions = require("./permissions.js")();
	
/****************************
 *			CACHE			*
 ****************************/
	
	var __cache = {};
	
	/**
	 *@brief	Initialize the cache for the given id.
	 **/
	function initCache(id) {
		__cache[id] = {
			id: id,
			data: []
		};
	}
	
	/**
	 *@brief	Place a message in cache.
	 **/
	function cache(msg) {
		if (!__cache[msg.id]) initCache(msg.id);
		__cache[msg.id].data.unshift(msg);
	}
	
	/**
	 *@brief	A maintenance function to call to keep the amount of data reasonable.
	 **/
	function cleanCaches() {
		for (var c of __cache) {
			if(c.data.length >= 1000) c.data.length = 900;
		}
	}
	
	/**
	 *@brief	Retrieve the cached data.
	 **/
	function getCached(id) {
		if (__cache[id]) return __cache[id].data;
		else return null;
	}
	
/****************************
 *			PARSERS			*
 ****************************/
 
	var parsers = [];
	atsb.parsers = parsers;
	
	/**
	 *@brief	Returns the first parser capable of parsing the given message.
	 **/
	function getParserFor(msg) {
		for (var p of parsers) {
			if (p.canParse(msg)) return p;
		}
		return;
	}
	
	/**
	 *@brief	Detects the type of the received message, then parses it with
	 *			the correct parser.
	 *
	 *@param	msg			The received message
	 *@param	msgId		The message id, for logging. Can be freely omitted,
	 *						but logs are easier to follow if included. 
	 **/
	function parseMessage(msg, msgId=null) {	
		var time = Date.now();
		if (!msg.time) msg.time = time;
		
		var p = getParserFor(msg);
		if (p) {
			var data = p.parse(msg, msgId);
			if (data && data.length) {
				atsb.emit("Info", "MessageParsed", 
						`Parsed ${data.length} readings. (${msgId})`,
						msgId, data.length);
			} else {
				atsb.emit("Warning", "NoParserOutputWarning", 
						`Parsing '${msgId}' resulted in no parser output.`,
						msgId, p);
			}
			return data;
		} else {
			atsb.emit("Error", "ParserNotFoundError", 
						`No parser was found for '${msgId}'.`,
						msgId, msg);
			return;
		}
	}
	
	/**
	 *@brief	Add a new parser to use for parsing received device messages.
	 *@detail	The parser must implement canParse() and parse(),
	 *			where canParse() takes in a message body, and returns truthy
	 *			value if it can parse it, and falsey value if it can't, and
	 *			parse() takes in a message body and optionally a message id for
	 *			logging, and returns an Array of parsed messages, or nothing.
	 **/
	atsb.registerParser = (newParser) => {
		if (!newParser.canParse) {
			throw new Error(`Parser is missing implementation of canParse().`);
		} else if (!newParser.parse) {
			throw new Error(`Parser is missing implementation of parse().`);
		}
		parsers.push(newParser);
	}
	
	/**
	 *@brief	Returns info about the parser. Used for CLI.
	 **/
	atsb.parserInfo = (pid, ...args)=>parsers[pid].info(...args);
	
	/**
	 *@brief	A private function to attach to a Socket.IO service.
	 **/
	var attachIO = (io) => {
		const nsp = io.of(opt.namespace);
		atsb.namespace 	= nsp;
		atsb.nsp 		= nsp;
		
		/* Set up device state change handlers */
		/* When a new device is created, transmit it to clients. */
		atsb.on("DeviceAdded", (device) => 
						nsp.emit("DeviceAdded", device.info));
		/* When a device is updated, transmit it to clients. */
		atsb.on("DeviceUpdated", (device) => 
						nsp.emit("DeviceUpdated", device.info));
		
		/* Set up the connection handler */
		nsp.on("connection", socket=>{
			/* Authenticate the user */
			if(socket.query) {
				// FIXME:
				socket.tokenId = socket.query.authToken.id;
				socket.permissions = atsb.authorize(socket.query.authToken);
			} else {
				socket.tokenId = "N/A";
				socket.permissions = atsb.permissions.default;
			}
			
			// If socket has viewing permissions, send them device list
			// and attach other listeners.
			if (socket.permissions.view) {
				/**
				 *@brief	Used to send the device list to the client.
				 **/
				function sendDeviceList() {
					socket.emit("DeviceList", atsb.deviceInfo());
				}
				
				/**
				 *@brief	Used to send the cache for a sensor to a client.
				 **/
				function flushCache(room) {
					var data = getCached(room);
					if (data) {
						console.log("Flushed cache from " + room);
						socket.emit("Data", ...data);
					}
				}
				
				atsb.emit("Info", "ClientConnected", 
					`A new client has connected with id ${socket.tokenId}`,
					 socket, socket.tokenId);
		
				/* Transmit devices */
				sendDeviceList();
				
				atsb.connections++;
			
				/* Handle client subscribing to a list of rooms */
				socket.on("Subscribe", (...rooms)=>{
					console.log(rooms);
					socket.join(rooms);
					rooms.forEach(flushCache);
				});
				
				/* Handle client unsubscribing a list of rooms */
				socket.on("Unsubscribe", (...rooms)=>socket.leave(rooms));
				
				/* Handle client requesting data */
				if (socket.permissions.loadCache) {
					socket.on("RequestData", (devices, time)=>{
						//TODO: Implement data fetching on-demand
					});
				}
				
				/* Handle client updating the devices */
				if (socket.permissions.editDevices) {
					socket.on("ChangeDevice", (...args)=>{
						//TODO: Implementation
					});
					
					socket.on("AddDevice", (...args)=>{
						//TODO: Implementation
					});
				}
				
				/* Respond to requests for the device list */
				socket.on("ListDevices", ()=>sendInfo());
				
				/* Handle a disconnecting socket */
				socket.on("disconnect", ()=>{
					atsb.emit("Info", "ClientDisconnected", `A client with id ${socket.tokenId} has disconnected.`,
							socket, socket.tokenId);
					atsb.connections--;
				});
			}
			// Socket does not have permission to view the stream. Boot them.
			else
			{
				atsb.emit("Warning", "UserAuthWarning", `Client authentication failed`, socket, socket.query.authToken);
				socket.close();
			}
		});
	};
	
	/**
	 *@brief	A private function for attaching to an express server
	 **/
	var attachExpress		= (app) => {
		/* Create the route for module index */
		app.get(`${opt.urlPrefix}`, function(req, res) {
			res.sendFile(`${__dirname}/static/index.html`);
		});
		
		/* Create the route for other static module files */
		app.get(`${opt.urlPrefix}/:f.:ext`, 
			function(req, res) {
				if (opt.moduleIndex || req.params.f !== "index")
					res.sendFile(`${__dirname}/static/${req.params.f}.${req.params.ext}`);
			});
		
		/* Create the endpoint for receiving data */
		app.post(`${opt.urlEndpoint}`, function(req, res) {
			res.set("Connection", "close");
			//FIXME: NO POST AUTHENTICATION
			/* Compute an id to track the message in logs */
			var msgId = hash(JSON.stringify(req.body)).slice(0,8);
			atsb.emit("Info", "PostReceived", `Received a POST request (${msgId})`, 
							msgId, req.body.length);
			/* Parse the data */
			var data = parseMessage(req.body, msgId);
			
			if (data && data.length) {
				
				data.forEach(msg=>{
					cache(msg);
					atsb.nsp.in(msg.id).emit("Data", msg);
				});
				atsb.emit("Info", "StreamedData", 
					`Streamed the data from '${msgId}'`, msgId);
			}
			
			res.status(200).send();
		});
	};
	
	/**
	 *@brief	Attach ATSB to either Express or Socket.IO servers.
	 **/
	atsb.attach = (...services) => {
		services.forEach(service=>{
			// FIXME: Very crude way to detect the type. Figure out a better way?
			if(service.get && service.post) attachExpress(service);
			else if (service.of) attachIO(service);
			else throw new Error("Attempted to attach to unknown service.");
		});
	};
	
	/**
	 *@brief	Validate the given endpoint and authentication token(s)
	 **/
	 //FIXME: URLs are not validated.
	atsb.urlAuth = (endpoint, body, ...auth) => {
		return true;
	};
	
	/* Expose variables */
	atsb.connections = 0; // Number of connected clients
	
	/* Enable the default logging functionality. */	
	if(opt.logging.enabled) {
		var log = require("./logging.js")(opt.logging);
		atsb.on("Info", log.info);
		atsb.on("Warning", log.warning);
		atsb.on("Error", log.error);
	}
	
	/* Load default parser */
	if (opt.defaultParser) {
		fs.readFile(opt.storage.prefix + opt.storage.sensors, 
				(err, data)=>{
					if(!err) {
						atsb.registerParser(require(opt.defaultParser)
								(atsb, JSON.parse(data)));
					}else{
						throw new Error(err);
					}
				});
	}

	/* Attach services */
	services.forEach(atsb.attach);

	return atsb;
}
