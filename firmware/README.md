After cloning the git the submodules need to be updated using the command: 
'git submodule update --init'
to get the necessary sensor APIs.
### Packet Format
| Field | Size (in bytes) | Fixed Value | Description |
|:-----:|:----:|:-----------:|:-----------:|
| **Fixed Header** |
| Length | 1 | 0x1E |
| Type | 1 | 0xFF |
| Company ID | 2 | 0xFFFF | Bluetooth company ID |
| **Payload** |
| CTR | 4 | | Incrementing counter to improve security
| DATA1 | 4 | | The data
| TYP1 | 2 | | The data type
| DATA2 | 4 | | The data
| TYP2 | 2 | | The data type
| HASH | 11 | | A hash of the encrypted data
												
* modified to have max 27 bytes of user payload
* modified sdk_config.h and Makefile to run ble-twi-aes
* created encrypt.h and encrypt.c for data encryption
* created drivers for AES,I2C,BME280,APDS9960,sensors.h etc.
* refactoring
											
### Update:
* run beacon at low power mode
* added radio_notification SWI interrupt (need removing nrf_error.h in  sdk/components/drivers_nrf/nrf_soc_nosd)
* modified payload
* successfully uploaded codes to the custom board
* extract and give the path to sdk in Makefile
* replace Makefile.posix in sdk/components/toolchain/gcc with one in config/
* Jlink debugging at different terminals:

$ JLinkExe -if swd -device nrf52 -AutoConnect 1 -speed 4000

$ JLinkRTTClient

### Instructions of setting up development environment:

