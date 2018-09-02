# Data Aggregation Server

## Installation

Refer to the [README](../../www/node.js/README.md)
for the installation instructions.

## Data Format

After parsing the data, the server stores a single reading as an object
in the below format.
Additional properties may be added.

        {
            id: "<sensor id>@<device mac>",
            value: <number>,
            time: <ms since epoch>
        }

## Command Line Interface

*This section details features that are not yet fully implemented.*

The server CLI can be enabled by calling `atsb.cliEnabled(true)`
in the server code. The included example has the CLI enabled
by default. Run `help` in the CLI for more information
about the available commands.

## Server-Side Events

The server emits a variety of events that can be caught by using `atsb.on(event, callback)`.
Below, a variety of events emitted by the server are detailed, along with when and where
the events are emitted, and what arguments they pass to the callback.

Event 		| Emitted by  | Arguments | Description 
:-----------|:------------|:----------|:----------------
`Info` 		| all modules | type, str, ...args | See below.
`Warning`	| all modules | type, str, ...args | See below.
`Error`		| all modules | type, str, ...args | See below.

The next few sections detail the various types of `Info`, `Warning` and `Error`
events. It should be noted that the `type` and `str` arguments are omitted in
those sections to reduce redundancy, but they *will* be passed to any callback
listening to any of the three events.

### Info Events

Emitted when the server does something of potential interest, e.g. adds a new device.
The `type` argument is a short name for what happened, and the following `str` gives
a more detailed account, suitable for logging.

Type | Emitted by | Arguments | Description
:----|:-----------|:----------|:------------------
`ClientConnected` | `atsb.js` | socket, user | Emitted when a user connects to the server. `user` is an object describing their credentials and permissions.
`ClientDisconnected` | `atsb.js` | socket, user | Emitted when a user disconnects. Otherwise same as `ClientConnected`.
`DeviceAdded` | `device.js` | device | Emitted when the server registers a new device.
`DeviceUpdated` | `device.js` | device, propertyName, propertyValue | Emitted when the server changes device information - e.g. adds new sensors, or changes the name.
`MessageParsed` | `atsb.js` | msgId, nParsed | Emitted when the server parses a message from a beacon. `msgId` is an 8-character identifier to help tracking messages, and `nParsed` is the number of readings parsed from the message.
`PostReceived` | `atsb.js` | msgId | Emitted when the server receives a POST request
`StreamedData` | `atsb.js` | msgId | Emitted when the server streams the data from a POST request

### Warning Events

Emitted when the server does something that potentially requires attention.
The `type` argument is a short name for what happened, and the following `str` gives
a more detailed account, suitable for logging.

Type | Emitted by | Arguments | Description
:----|:-----------|:----------|:------------------
`CounterValidationWarning` | `parser.js` | msgId, mac, msg | Emitted when a message with an invalid counter is received.
`HMACValidationWarning` | `parser.js` | msgId, mac, msg | Emitted if HMAC validation fails.
`NoParserOutputWarning` | `atsb.js` | msgId, msg, parser | Emitted if a parser produces no output.
`UnknownDeviceWarning` | `parser.js` | msgId, mac, msg | Emitted if data was received from an unknown device.
`UserAuthWarning` | `atsb.js` | socket, user | Emitted when a user lacks the permissions to view the stream.

### Error Events

Emitted when the server does something that almost certainly requires attention.
The `type` argument is a short name for what happened, and the following `str` gives
a more detailed account, suitable for logging.

Type | Emitted by | Arguments | Description
:----|:-----------|:----------|:------------------
`ParserNotFoundError` | `atsb.js` | msgId, msg | Emitted if no parser was found for the message.

## Client-Side Events

If desired, the visualization client may be rewritten. This can be done by
connecting to the appropriate Socket.IO namespace, and by handling and emitting 
the events described below. If the event is not marked as 'Implemented', it is
not yet supported by the server.

Upon connecting to the server, the client receives an object describing its
permissions. Refer to the [Permissions Module](./permissions.md) for more information.
The client can choose to ignore these, but the server will enforce them.

### Events Emitted by Server

Event | Implemented | Arguments | Description
:----|:----------|:----------|:------------------
`Data`  | x | data | Data. Either a single data point, or an `Array`. The data matches the [Data Format](#data-format)
`DeviceAdded`| x | devInfo | Emitted when the server registers a new device. `devInfo` is an object describing the device.
`DeviceList` | x | devInfoList | Emitted upon list request or when connecting to the server. `devInfoList` contains all device info objects.
`DeviceUpdated` | x | devInfo | Emitted when the server updates a device. `devInfo` is the updated device description.

### Events Emitted by Client

Event | Implemented | Arguments | Description
:----|:----------|:----------|:------------------
`ListDevices` | x | Used to request the list of devices.
`Subscribe` | x | rooms | `rooms` should be an `Array` of sensor identifiers. This event indicates to the server that the client wants the associated data.
`Unsubscribe` | x | rooms | This event can be used to undo `Subscribe`.
`RequestData` | | requests | Used to request data from a certain interval and sensor from the server. `requests` is an `Array` of objects: `{id:<sensor id>, start:<time1>, end:<time2>}`
`ChangeDevice`| | mac, property, value | Used to update a property of a device, e.g. rename it or edit its key.
`AddDevice` | | mac, key, name | Used to create a new device.

## Customizing the Server

There are two primary methods to customizing the server to suit
your needs: editing the JSON configs or by overriding functions or modules.

### config.json

*Some of the features described in this section are not fully described.*

Options available are:

Option | Values | Default | Description
:------|:-------|:-----------------
`urlPrefix` | String | "/atsb" | Sets the prefix to all related URLs, including the endpoints and static files.
`urlEndpoint` | String | "/endpoint/webhook" | Sets the endpoint. To use URL authentication, include `:auth` in the URL.
`namespace` | String | "atsb" | The name of the Socket.IO namespace.
`defaultParser` | String, `null` | "./parser.js" | The name of the parser to load on initialization. Set to `null` for none.
`logging` | | | Various options for logging
`logging.enabled` | boolean | `true` | Enable or disable logging
`logging.events` | Array | `["Warning", "Error"]` | A list of all the events to log. "All" for all events, "Info", "Warning" or "Error" for all events of that category.
`logging.prefix` | String | "\[ATSB\] <$TIME> $TYPE: " | A prefix to prepend to all logged events. '$TIME' is replaced with current time (hh:mm:ss) and '$TYPE' with the event type.
`logging.postfix` | String | "" | A postfix to append to logged events. Works like `logging.prefix`.
`storage` | | | Various options for the default storage module
`storage.prefix` | String | "$DIRNAME/var/atsb" | A prefix to append to all file paths (except config). '$DIRNAME' is replaced with the `__dirname` variable.
`storage.data` | String | "/data/$YYYY/$MM/$DD/$hh$mm" | The file(s) to store collected data in. The variables ($YYYY, $MM, etc.) are replaced with the appropriate time from the data timestamp(s).
`storage.exitSave` | boolean | `true` | Whether the server should save changed configurations on exit. If disabled, calling `atsb.save()` does the same.
`storage.devices` | String | "devices.json" | The file to store device configurations in.
`storage.sensors` | String | "sensors.json" | The file to store sensor configurations in for the default parser.
`storage.users` | String | "users.json" | The file to store users, their permissions and login credentials in.
`storage.compression` | boolean | `true` | Whether files should be compressed.
`storage.encryption` | | | Various options for encrypting files
`storage.encryption.enabled` | boolean | true | Whether to encrypt/decrypt files.
`storage.encryption.keyMethod` | String | "stdin" | How to retrieve the key. "stdin" queries the key from `stdin` on startup, "config" requires the key to be defined in the config file (very insecure!), "file" retrieves the key from a file (equally insecure).
`storage.encryption.key` | String, `null` | `null` | If `storage.encryption.keyMethod` is set to "config", this option sets the key. It should be a hex-encoded string.
`storage.encryption.algorithm` | String | "aes-256" | The encryption algorithm for the module. Must be supported by the `crypto` module.

### Storage

The API for the storage module is not yet done, so this section is left empty

### Custom Parsers

The server software supports addition of new parsers for user-defined protocols.
This is done by creating a new object that has the public member functions 
`canParse()` and `parse()` defined as below. If desired, the parser may also
implement the `info()` function.

If desired, the default parser can be disabled by setting `defaultParser` in
`config.json` to `null`, or replaced by setting it to refer to a new file
(not recommended).

#### canParse(msg)

* The function must take a single argument, the message.
* It must determine whether the message is a valid input for `parse()`
* It must return `true` if the message is valid input, and `false` otherwise

#### parse(msg, msgId)

* The function is passed two arguments: the message, and the message identifier.
* It must parse the message, and return an `Array` containing data objects. 
* The parsed data objects *must* follow the [Data Format](#data-format).
* The function should return an empty set (`[]`) if the message contained no readings.
* The function should return `null` if the parser failed due to invalid message.
* The function *may* call `update()` on the associated `Device`(s)
* The function *may* call `log()` on the associated `Device`(s).
The default parser logs the following statistics: 
`AllMessages`, `ValidMessages`, `SkippedMessages`, `InvalidCTRs`, `InvalidHMACs`
* The `msgId` parameter can safely be ignored
* The implementation can safely assume that `canParse()` returned
true for the message.

#### info(...args)

*This section details features that are not made use of in the current version*

* The function is called when the user uses the `parser info <parser>` command.
* Any arguments after `<parser>` are passed to the function.
* The function should return either an `Array` of `string`s or a `string`.
* The return value is printed in console.
* If the returned value was an `Array`, `join("\n")` is called on it first.
