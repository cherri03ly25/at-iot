EESchema Schematic File Version 4
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Sensor:BME280 U1
U 1 1 5B4789AB
P 5600 3000
F 0 "U1" H 5170 3046 50  0000 R CNN
F 1 "BME280" H 5170 2955 50  0000 R CNN
F 2 "Package_LGA:Bosch_LGA-8_2.5x2.5mm_P0.65mm_ClockwisePinNumbering" H 5600 2800 50  0001 C CNN
F 3 "https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf" H 5600 2800 50  0001 C CNN
	1    5600 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 3600 5500 3850
Wire Wire Line
	5700 3600 5700 3850
$Comp
L power:GND #PWR0101
U 1 1 5B478A53
P 5500 3850
F 0 "#PWR0101" H 5500 3600 50  0001 C CNN
F 1 "GND" H 5505 3677 50  0000 C CNN
F 2 "" H 5500 3850 50  0001 C CNN
F 3 "" H 5500 3850 50  0001 C CNN
	1    5500 3850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5B478A79
P 5700 3850
F 0 "#PWR0102" H 5700 3600 50  0001 C CNN
F 1 "GND" H 5705 3677 50  0000 C CNN
F 2 "" H 5700 3850 50  0001 C CNN
F 3 "" H 5700 3850 50  0001 C CNN
	1    5700 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 2400 5700 2200
Wire Wire Line
	5500 1950 5500 2400
Wire Wire Line
	5500 1950 5900 1950
Wire Wire Line
	5700 2200 5900 2200
Wire Wire Line
	5700 1550 5700 2200
Connection ~ 5700 2200
Wire Wire Line
	5500 1950 5500 1550
Connection ~ 5500 1950
$Comp
L Device:C C1
U 1 1 5B478C4F
P 6050 1950
F 0 "C1" V 5798 1950 50  0000 C CNN
F 1 "100nF" V 5889 1950 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6088 1800 50  0001 C CNN
F 3 "~" H 6050 1950 50  0001 C CNN
	1    6050 1950
	0    1    1    0   
$EndComp
Wire Wire Line
	6200 1950 6400 1950
Wire Wire Line
	6200 2200 6400 2200
Text Label 5500 1550 1    50   ~ 0
VDDIO
Text Label 5700 1550 1    50   ~ 0
VDD
$Comp
L power:GND #PWR0103
U 1 1 5B478DE0
P 6400 1950
F 0 "#PWR0103" H 6400 1700 50  0001 C CNN
F 1 "GND" V 6405 1822 50  0000 R CNN
F 2 "" H 6400 1950 50  0001 C CNN
F 3 "" H 6400 1950 50  0001 C CNN
	1    6400 1950
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5B478E14
P 6400 2200
F 0 "#PWR0104" H 6400 1950 50  0001 C CNN
F 1 "GND" V 6405 2072 50  0000 R CNN
F 2 "" H 6400 2200 50  0001 C CNN
F 3 "" H 6400 2200 50  0001 C CNN
	1    6400 2200
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C2
U 1 1 5B478EC1
P 6050 2200
F 0 "C2" V 5798 2200 50  0000 C CNN
F 1 "100nF" V 5889 2200 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6088 2050 50  0001 C CNN
F 3 "~" H 6050 2200 50  0001 C CNN
	1    6050 2200
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x07 J1
U 1 1 5B479421
P 9250 3800
F 0 "J1" H 9330 3842 50  0000 L CNN
F 1 "Conn_01x07" H 9330 3751 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x07_P2.54mm_Vertical" H 9250 3800 50  0001 C CNN
F 3 "~" H 9250 3800 50  0001 C CNN
	1    9250 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 3500 8750 3500
Wire Wire Line
	9050 3600 8750 3600
Wire Wire Line
	9050 3700 8750 3700
Wire Wire Line
	9050 3900 8750 3900
Wire Wire Line
	9050 4000 8750 4000
Wire Wire Line
	9050 4100 8750 4100
Wire Wire Line
	9050 3800 8750 3800
Text Label 8750 3500 2    50   ~ 0
VDD
Text Label 8750 3600 2    50   ~ 0
VDDIO
$Comp
L power:GND #PWR0105
U 1 1 5B47A2A4
P 8750 3700
F 0 "#PWR0105" H 8750 3450 50  0001 C CNN
F 1 "GND" V 8755 3572 50  0000 R CNN
F 2 "" H 8750 3700 50  0001 C CNN
F 3 "" H 8750 3700 50  0001 C CNN
	1    8750 3700
	0    1    1    0   
$EndComp
Text Label 8750 3800 2    50   ~ 0
SDO
Text Label 8750 3900 2    50   ~ 0
SDI
Text Label 8750 4000 2    50   ~ 0
SCK
Text Label 8750 4100 2    50   ~ 0
CSB
Wire Wire Line
	6200 2700 6550 2700
Wire Wire Line
	6200 2900 6550 2900
Wire Wire Line
	6200 3100 6550 3100
Wire Wire Line
	6200 3300 6550 3300
Text Label 6550 2700 2    50   ~ 0
SDO
Text Label 6550 2900 2    50   ~ 0
SCK
Text Label 6550 3100 2    50   ~ 0
SDI
Text Label 6550 3300 2    50   ~ 0
CSB
$EndSCHEMATC
