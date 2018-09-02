/********************************************************
 * This source code is licensed under the MIT license.	*
 * See LICENSE in the project root folder for more info.*
 ********************************************************/

/**
 * index.js
 *
 * Created on: 2018-07-03
 * Last Modified: 2018-08-29
 * Author(s): Veli-Matti Rantanen
 */

const express = require("express");
const app = express();
const http = require("http");
const https = require("https");
const fs = require("fs");
const io = require("socket.io")();
const atsb = require("./atsb.js")();

var https_options = {
	enabled: false,
	enforced: false
}

if (https_options.enabled) {
	https_options.key = 
			fs.readFileSync("/etc/letsencrypt/live/www.example.com/privkey.pem"),
	https_options.cert = 
			fs.readFileSync("/etc/letsencrypt/live/www.example.com/fullchain.pem")
};

const http_server = http.Server(app);

if (https_options.enabled) {
	const https_server = https.Server(https_options, app);
}

const public_dir = "/var/www/public";

app.use(express.json());
app.use(express.urlencoded({extended: true}));

if (https_options.enforced) {
	app.use(function (req, res, next) {
		if (req.protocol == "https")
			next();
		else
			res.redirect(301, `https://${req.hostname}${req.originalUrl}`);
	});
}

app.use(express.static(public_dir, {dotfiles:"allow"}));

app.get("/", function (req, res) {
	var file_path = public_dir + req.url;
	if (file_path.endsWith("/"))
		file_path += "index.html";
	res.sendFile(file_path);
});

app.get("/", function(req, res) {
	res.sendFile("/var/www/public/index.html");
});

if (!https_options.enabled || !https_options.enforced)
	io.attach(http_server);
if (https_options.enabled)
	io.attach(https_server);

atsb.attach(app, io);

http_server.listen(3000, function() {
	console.log("listening on *:3000");
});

if (https_options.enabled)
	https_server.listen(3443, function() {
		console.log("listening on *:3443");
	});
	
var stdin = process.openStdin();

stdin.addListener("data", (data)=>{
	var cmd = data.toString().trim();
	cmd = cmd.split(" ");
	switch(cmd[0]) {
		case "sensors": 
			console.log(atsb.registeredSensors()); 
			break;
		case "devices":
			console.log(atsb.printableDeviceInfo()); 
			break;
		case "connections":
			console.log(atsb.connections);
			break;
		case "stats":
			if (cmd.length != 2) {
				console.log("Invalid syntax.");
				console.log("Usage: stats <mac>");
				break;
			}
			var dev = atsb.getDevice(cmd[1]);
			if (dev) {
				console.log("Statistics for device '"+dev.name+"':");
				for (var stat in dev.stat) {
					console.log(`${stat} (Recent): ${dev.stat[stat].recent}`);
					console.log(`${stat} (Total):  ${dev.stat[stat].total}`);
				}
			} else {
				console.log("Could not find device with MAC '"+cmd[1]+"'.");
			}
			break;
		case "new":
			if (cmd.length < 4 
					|| ["device","user","sensor"].indexOf(cmd[1]) == -1) 
			{
				if (cmd.length != 1) console.log("Invalid syntax.");
				
				console.log("Usage:");
				console.log("\tnew device <mac> <key> [name]");
				console.log("\tnew sensor <type> ...fields");
				console.log("\t\tfield: -f <size> [-s] [-n name] [-p precision] [-c color]");
				console.log("\tnew user <id> <key> ...permissions");
			}
			else
			{
				switch(cmd[1]) {
					default: console.log("Not yet implemented"); break;
					case "device": 
							new atsb.Device(cmd[2], cmd[3], cmd.slice(4, cmd.length-1).join(" ")); 
							console.log("Device created.");
							break;
				}
			}
			break;
		default:
			console.log("Unknown command: '"+cmd+"'.");
		case "help":
			console.log("The following commands may be used:");
			console.log("\tconnections | Print the number of current WebSocket connections");
			console.log("\tdevices     | List names and MACs of all devices");
			console.log("\tnew <...>   | Add new device, sensor or user. See 'new'.");
			console.log("\tsensors     | List the registered sensors for the first parser");
			console.log("\tstats <mac> | List the per-device statistics");
			console.log("\tconnections | Print number of active web socket connections");
	}
});
