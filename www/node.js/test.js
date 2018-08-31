module.exports = function(atsb) {
	function rb() {
		return Math.random().toString(16).slice(-2);
	}

	function randomMac() {
		return `${rb()}:${rb()}:${rb()}:${rb()}:${rb()}:${rb()}`;
	}
	
	function normalVariate(avg, sdev) {
		return (Math.random()-Math.random())*sdev + avg;
	}
	
	function spoofSensor() {
		var id = rb()+rb();
		return {
			id: id,
			name: "Spoofed Sensor #"+id,
			style: "#"+rb()+"33"+rb()
		}
	}
	
	return {
		dummyDevice: () => {
			var sList = [spoofSensor(), spoofSensor(), spoofSensor(), spoofSensor()];
			var sVal = new Array(sList.length).fill(0);
			var si = 0;
			var mac = randomMac();
			var dev = atsb.getDevice(mac);
			dev.update(...sList);

			setInterval(()=>{
				if(si >= sList.length) si = 0;
				sVal[si] += normalVariate(0, 0.1);

				var data = {
					id: sList[si].id + "@" + mac,
					time: Date.now(),
					value: sVal[si++]
				}

				atsb.emit("DataParsed", data);
			}, 200);
			
			return dev;
		}
		
	}
}
