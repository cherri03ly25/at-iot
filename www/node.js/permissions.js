/********************************************************
 * This source code is licensed under the MIT license.	*
 * See LICENSE in the project root folder for more info.*
 ********************************************************/

/**
 * atsb.js
 * 
 * Created on: 2018-08-29
 * Last Modified: 2018-08-29
 * Author(s): Veli-Matti Rantanen
 **/

module.exports = function(userList={}) {
	var users = userList;

	var permissions = {
		/* A list of the permissions */
		list: ["view", "rename", "loadCache", "editDeviceKeys", 
			"editPermissions", "createUrlAuth", "removeUrlAuth"],
	
		/* The default permissions */
		default: {
			view: true,
			rename: false,
			loadCache: true,
			editDeviceKeys: false,
			editPermissions: false,
			createUrlAuth: false,
			removeUrlAuth: false
		},
		
		all: {
			view: true,
			rename: true,
			loadCache: true,
			editDeviceKeys: true,
			editPermissions: true,
			createUrlAuth: true,
			removeUrlAuth: true
		},
		
		none: {
			view: false,
			rename: false,
			loadCache: false,
			editDeviceKeys: false,
			editPermissions: false,
			createUrlAuth: false,
			removeUrlAuth: false
		}
	};
	
	permissions.users = users;

	/**
	 *@brief	Attempt user authentication
	 **/
	permissions.authUser = (id, secret) => {
		if(users[id] && hash(secret + users[id].salt) 
					== users[id].secret)
			return users[id].permissions;
		else
			return permissions.default;
	};
	
	/**
	 *@brief	Attempt adding a new user
	 *@detail	The user's permissions can be set here, or omitted for
	 *			the defaults.
	 *
	 *@param	id		The user id
	 *@param	pw		The plaintext password
	 *@param	salt	The salt for the user password
	 *@param	perm	The permissions to enable for the user
	 *
	 *@return	true/false depending on if the user was added.
	 **/
	permissions.addUser = (id, pw, salt, ...perm) => {
		if (!users[id]) {
			var permissions = {...permissions.default};
			perm.forEach(p=>permissions[p]=true);
			users[id] = {
				id: id,
				permissions: permissions,
				secret: hash(pw + salt),
				salt: salt
			};
			return true;
		} else {
			return false;
		}
	};
	
	/**
	 *@brief	Remove a user
	 **/
	permissions.removeUser = (id) => delete users[id];
	
	/**
	 *@brief	Grant a user new permissions
	 **/
	permissions.grant = (id, ...perm) => 
			perm.forEach(p=>users[id].permissions[p] = true);
	
	/**
	 *@brief	Revoke user permissions
	 **/
	permissions.revoke = (id, ...perm) => 
			perm.forEach(p=>users[id].permissions[p] = false);

	return permissions;
}
