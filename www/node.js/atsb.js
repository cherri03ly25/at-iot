/********************************************************
 * This source code is licensed under the MIT license.	*
 * See LICENSE in the project root folder for more info.*
 ********************************************************/

/**
 * atsb.js
 * 
 * Created on: 2018-07-20
 * Last Modified: 2018-08-17
 * Author(s): Veli-Matti Rantanen
 **/

const crypto = require("crypto");
const fs = require("fs");

/**
 *	General Description
 *===============================
 * 1) Data is received on an endpoint 
 *		(by default: "/atsb/endpoint/webhook/:devId")
 * 2) The module determines whether it wants to collect that data.
 * 3) The data is parsed, and buffered
 * 4) The buffer is sent to the clients that are subscribed to the device
 */
 
/**
 * Data is sent in the following format to "{Device MAC}@{Sensor Type}" room,
 * as well as to rooms "all" and "{Device MAC}":
 * {
 *		device: <Device MAC>,
 *		sensor: <Sensor Type>,
 *		time: <ms epoch time>,
 *		value: <Sensor Reading Value>
 * }
 **/
 
/**
 * Data is received at the http/https endpoint in the following format:
 * [{
 *		device: <Device MAC>,
 *		time: <ms epoch time>,
 *		sensors: [{
 *			type: <Sensor Type>,
 *			value: <Sensor Reading Value>
 * 		}],
 *		auth: <Authentication field>
 * }]
 **/

function hash(str, fmt="hex", algo="sha256") {
	const sha = crypto.createHash(algo);
	sha.update(str);
	return sha.digest(fmt);
};

var deviceKeys = {};
var urlAuthList = {"MyNameIsCarlAndILikePinkFluffyUnicorns":""};

module.exports = function (opt={}, ...services) {
	opt = {config: "./config.json", ...opt};
	opt = {...JSON.parse(fs.readFileSync(opt.config, "utf8")), ...opt};
	var __cache = {};
	
	opt.storage.prefix = opt.storage.prefix.replace("$DIRNAME", __dirname);
	
	function initCache(id) {
		__cache[id] = {
			id: id,
			data: []
		};
	}
	
	function cache(msg) {
		if (!__cache[msg.id]) initCache(msg.id);
		__cache[msg.id].data.unshift({time:msg.time, value:msg.value});
	}
	
	function cleanCaches() {
		for (var c of __cache) {
			if(c.data.length >= 1000) c.data.length = 900;
		}
	}
	
	function getCached(id) {
		if (__cache[id]) return __cache[id].data;
		else return null;
	}
	
	/* Import the device sub-module to extend it */
	var atsb 	= require("./device.js")();
	/* Permissions sub-module */
	atsb.permissions = require("./permissions.js")();
	
	atsb.connections = 0;
	
	/* Set up device state change handlers */
	/* When a new device is created, transmit it to clients. */
	atsb.on("DeviceAdded", (device) => 
					atsb.nsp.emit("DeviceAdded", device.info));
	/* When a device is updated, transmit it to clients. */
	atsb.on("DeviceUpdated", (device) => 
					atsb.nsp.emit("DeviceUpdated", device.info));
	
	/* Parser functionality */
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
	function parse(msg, msgId=null) {	
		var time = Date.now();
		if (!msg.time) msg.time = time;
		
		var p = getParserFor(msg);
		if (p) {
			var data = p.parse(msg, msgId);
			if (data.length) {
				atsb.emit("Info", "MessageParsed", msgId, data.length, 
							`Parsed ${data.length} readings. (${msgId})`);
			} else {
				atsb.emit("Warning", "NoParserOutputWarning", msgId,
							`Parsing '${msgId}' resulted in no parser output.`);
			}
			return data;
		} else {
			atsb.emit("Error", "ParserNotFoundError", msg, msgId,
						`No parser was found for message. (${msgId})`);
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
	atsb.addParser = (newParser) => {
		if (!newParser.canParse) {
			throw new Error(`Parser is missing implementation of canParse().`);
		} else if (!newParser.parse) {
			throw new Error(`Parser is missing implementation of parse().`);
		}
		parsers.push(newParser);
	}
	
	atsb.registeredSensors = ()=>parsers[0].registered();
	
	/* Create the module's Socket.IO namespace */
	var attachIO = (io) => {
		const nsp = io.of(opt.namespace);
		atsb.namespace 	= nsp;
		atsb.nsp 		= nsp;
		
		nsp.on("connection", socket=>{
			if(socket.query) {
				socket.tokenId = socket.query.authToken.id;
				socket.permissions = atsb.authorize(socket.query.authToken);
			} else {
				socket.tokenId = "N/A";
				socket.permissions = atsb.permissions.default;
			}
			
			// If socket has viewing permissions, send them device list
			// and attach other listeners.
			if (socket.permissions.view) {
				function sendInfo() {
					socket.emit("DeviceList", atsb.deviceInfo());
				}
				
				function flushCache(room) {
					var data = getCached(room);
					if (data) socket.emit("CachedData", room, data);
				}
				
				atsb.emit("Info", "ClientConnected", socket, socket.tokenId,
					`A new client has connected with id ${socket.tokenId}`);
				sendInfo();
				
				atsb.connections++;
			
				/* Handle client subscribing to a list of rooms */
				socket.on("Subscribe", rooms=>{
					rooms.forEach(flushCache);
					socket.join(rooms)
				});
				
				/* Handle client unsubscribing a list of rooms */
				socket.on("Unsubscribe", rooms=>socket.leave(rooms));
				
				/* Handle client requesting data */
				if (socket.permissions.loadCache) {
					socket.on("DataRequest", (devices, time)=>{
						//TODO: Implement data fetching on-demand
					});
				}
				
				socket.on("ListDevices", ()=>sendInfo());
				
				/* Code to run on a disconnecting socket */
				socket.on("disconnect", ()=>{
					atsb.emit("Info", "ClientDisconnect", socket, socket.tokenId,
						`A client with id ${socket.tokenId} has disconnected.`);
					atsb.connections--;
				});
			}
			// Socket does not have permission to view the stream. Boot them.
			else
			{
				atsb.emit("Warning", "UserAuthWarning", socket.query.authToken,
					`Client authentication failed`);
				socket.close();
			}
		});
	};
	
	/**
	 *@brief	A function for attaching to an express server
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
			/* Check the endpoint authentication */
			/* Not used right now, as the gateway has not been patched. */
			/*if (!atsb.urlAuth(req.params.endpoint, req.params.auth)) {
				atsb.emit("error", atsb.UrlAuthError, 
					`Received unauthorized POST request.`, req);
				res.status(403).send();
			} else {*/
				var msgHash = hash(JSON.stringify(req.body)).slice(0,8);
				atsb.emit("Info", "PostReceived", req.body.length, msgHash,
								`Received a POST request with `
								+ `${req.body.length} messages (${msgHash})`);
				var data = atsb.parse(req.body, msgHash);
				
				if (data) {
					data.forEach(msg=>{
						cache(msg);
						atsb.nsp.in(msg.id).emit("Data", msg);
					});
					atsb.emit("Info", "StreamedData", 
						`Streamed the data from '${msgHash}'`);
				}
			/*}*/
		});
	};
	
	/**
	 *@brief	Attach ATSB to either Express or Socket.IO servers.
	 **/
	atsb.attach = (...services) => {
		services.forEach(service=>{
			if(service.get && service.post) attachExpress(service);
			else if (service.of) attachIO(service);
			else throw new Error("Attempted to attach to unknown service.");
		});
	};
	
	atsb.urlAuth = (endpoint, auth) => {
		return urlAuthList.hasOwnProperty(endpoint);
	};
	
	/* Enable the default logging functionality. */	
	if(opt.logging.enabled) {
		var log = require("./logging.js")(opt.logging);
		atsb.on("Info", log.info);
		atsb.on("Warning", log.warning);
		atsb.on("Error", log.error);
	}
	
	/* Add default parser */
	if (opt.defaultParser) {
		fs.readFile(opt.storage.prefix + opt.storage.sensors, 
				(err, data)=>{
					if(!err) {
						atsb.addParser(require(opt.defaultParser)
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
