/*********************************************************
 * This source code is licensed under the MIT license.	 *
 * See LICENSE in the project root folder for more info. *
 *********************************************************/

/**
 * logging.js
 * 
 * Created on: 2018-08-29
 * Last Modified: 2018-09-02
 * Author(s): Veli-Matti Rantanen
 **/
 
module.exports = (opt)=>{
	function timestamp() {
		return (new Date()).toJSON().slice(11, 19).replace(/[-T]/g, ':');
	}
	
	function prefix(type) {
		return opt.prefix.replace("$TIME", timestamp()).replace("$TYPE", type);
	}
	
	function postfix(type) {
		return opt.postfix.replace("$TIME", timestamp()).replace("$TYPE", type);
	}
	
	return {
		info: (...args)=>{
			if (opt.events && 
				(opt.events.includes("All") 
				|| opt.events.includes("Info")
				|| opt.events.includes(args[0])))
			{
				console.log(prefix(args[0]) + args[1] 
							+ postfix(args[0]));
			}
		},
		warning: (...args)=>{
			if (opt.events && 
				(opt.events.includes("All") 
				|| opt.events.includes("Warning")
				|| opt.events.includes(args[0])))
			{
				console.log(prefix(args[0]) + args[1] 
							+ postfix(args[0]));
			}
		},
		error: (...args)=>{
			if (opt.events && 
				(opt.events.includes("All") 
				|| opt.events.includes("Error")
				|| opt.events.includes(args[0])))
			{
				console.log(prefix(args[0]) + args[1] 
							+ postfix(args[0]));
			}
		}
	};
}
