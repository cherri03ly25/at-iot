# BLE Sensor Board and Web Interface

This project defines a sensor board that broadcasts information to an IoT gateway, which then pushes the data to Azure IoT Hub. The data is then transferred to a node.js webserver that then broadcasts it to browser clients.

## Contents

* KiCAD schematics and PCB design for an nRF52840-powered sensor board
* source code for the nRF52840, required for it to send data to an IoT gateway utilizing BLE beacons
* source code for a node.js webserver capable of receiving data from Azure IoT Hub and broadcasting it to the users of a website
* the documentation for all of the above, instructions

## Installation Instructions

All parts of this project (webserver, firmware and KiCAD schematics) come with installation instructions. They can be found in their respective directories, in the INSTALL file.

## Licensing

See LICENSE.
