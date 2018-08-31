# Web Interface

## Supported Browsers

Browser | Version
-------:|:------
Mozilla Firefox | 60.0+
Google Chrome | 67.0+
Safari | 11.0+
Microsoft Edge | N/A

## Installation Instructions

Due to the short amount of time available for this project, server installation is only documented for linux machines with the Aptitude package manager installed.

### Steps

1. Install node.js, and optionally npm and pm2.
2. Install socket.io and express. (This can be done by running `npm install` in `./node.js` if npm was installed)
3. Tweak `node.js/config.json` to your liking.
4. Run server with `node node.js/index.js` or `pm2 start node.js/index.js`
5. Navigate to [http://localhost:3000/atsb] if using the default settings.


### Dependencies

Dependency | Required | Recommended | Version | Install Command
----------:|:--------:|:-------:|:----------------
`node.js` 	| x | x | v8.10.0 	| `sudo apt install nodejs`
`npm` 		|   | x | 3.5.2 	| `sudo apt install npm`
`pm2` 		|   | x | 2.10.4 	| `sudo npm install -g pm2`
`socket.io` 	| x | x | 2.1.1 	| `npm install socket.io`
`express`	| x | x | 		| `npm install express`
