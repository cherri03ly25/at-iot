/********************************************************
 * This source code is licensed under the MIT license.	*
 * See LICENSE in the project root folder for more info.*
 ********************************************************/

/**
 * atsb_events.js
 * 
 * Created on: 2018-08-15
 * Last Modified: 2018-08-17
 * Author(s): Veli-Matti Rantanen
 **/

module.exports = function () {
	var listeners = {};

	return {
		/**
		 *@brief	Register listener for an event (or several)
		 *
		 *@param	evt 		A space-separated list of events to trigger
		 *						the callback on.
		 *@param	callback	A callback function.
		 **/
		on: (evt, callback)=>{
			evt.split(" ").forEach(e=>{
				if (listeners[e] === undefined) listeners[e] = [];
				listeners[e].push(callback);
			});
		},

		/**
		 *@brief	Call all of the listeners associated with events
		 *
		 *@param	evt			A space-separated list of events to trigger
		 *@param	args		A list of arguments to pass to the callbacks
		 **/
		emit: (evt, ...args)=>{
			evt.split(" ").forEach(e=>{
				if (listeners[e]) 
					listeners[e].forEach(l=>setTimeout(l, 0, ...args));
			});
		}
	}
}
