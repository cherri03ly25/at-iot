EESchema Schematic File Version 4
LIBS:BMD340DV-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Rigado BMD340 Testing Board"
Date "2018-08-02"
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L BMD-340-A-R:BMD-340-A-R U1
U 1 1 5B604395
P 5650 2700
F 0 "U1" H 5700 4567 50  0000 C CNN
F 1 "BMD-340-A-R" H 5700 4476 50  0000 C CNN
F 2 "BMD-340-A-R:RIGADO_BMD-340-A-R" H 5650 2700 50  0001 L BNN
F 3 "None" H 5650 2700 50  0001 L BNN
F 4 "10.08 USD" H 5650 2700 50  0001 L BNN "Field4"
F 5 "Rigado" H 5650 2700 50  0001 L BNN "Field5"
F 6 "BMD-340-A-R" H 5650 2700 50  0001 L BNN "Field6"
F 7 "Unavailable" H 5650 2700 50  0001 L BNN "Field7"
F 8 "Bluetooth Low Energy 5.0 Module" H 5650 2700 50  0001 L BNN "Field8"
	1    5650 2700
	1    0    0    -1  
$EndComp
$Comp
L Connector:USB_B_Micro J3
U 1 1 5B6044B3
P 8250 5550
F 0 "J3" H 8305 6017 50  0000 C CNN
F 1 "USB_B_Micro" H 8305 5926 50  0000 C CNN
F 2 "Connector_USB:USB_Micro-B_Molex-105017-0001" H 8400 5500 50  0001 C CNN
F 3 "~" H 8400 5500 50  0001 C CNN
	1    8250 5550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5B604626
P 8950 5550
F 0 "R4" V 8743 5550 50  0000 C CNN
F 1 "27" V 8834 5550 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 8880 5550 50  0001 C CNN
F 3 "~" H 8950 5550 50  0001 C CNN
	1    8950 5550
	0    1    1    0   
$EndComp
$Comp
L Device:R R5
U 1 1 5B604723
P 9400 5650
F 0 "R5" V 9193 5650 50  0000 C CNN
F 1 "27" V 9284 5650 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9330 5650 50  0001 C CNN
F 3 "~" H 9400 5650 50  0001 C CNN
	1    9400 5650
	0    1    1    0   
$EndComp
$Comp
L Device:C C1
U 1 1 5B60482D
P 6900 1500
F 0 "C1" H 6785 1454 50  0000 R CNN
F 1 "4,7uF" H 6785 1545 50  0000 R CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6938 1350 50  0001 C CNN
F 3 "~" H 6900 1500 50  0001 C CNN
	1    6900 1500
	-1   0    0    1   
$EndComp
Wire Wire Line
	6250 4400 6500 4400
Wire Wire Line
	6500 4400 6500 4500
$Comp
L power:GNDREF #PWR0101
U 1 1 5B604A89
P 6500 4500
F 0 "#PWR0101" H 6500 4250 50  0001 C CNN
F 1 "GNDREF" H 6505 4327 50  0000 C CNN
F 2 "" H 6500 4500 50  0001 C CNN
F 3 "" H 6500 4500 50  0001 C CNN
	1    6500 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 5550 8800 5550
Wire Wire Line
	8550 5650 9250 5650
Wire Wire Line
	9100 5550 9200 5550
Wire Wire Line
	9550 5650 9700 5650
Text Label 9700 5650 0    50   ~ 0
D-
Text Label 8700 5550 0    50   ~ 0
+
Text Label 8700 5650 0    50   ~ 0
-
Wire Wire Line
	8150 5950 8150 6100
Wire Wire Line
	8150 6100 8250 6100
Wire Wire Line
	8250 5950 8250 6100
Connection ~ 8250 6100
Wire Wire Line
	8250 6100 8250 6200
$Comp
L power:GNDREF #PWR0102
U 1 1 5B604D12
P 8250 6200
F 0 "#PWR0102" H 8250 5950 50  0001 C CNN
F 1 "GNDREF" H 8255 6027 50  0000 C CNN
F 2 "" H 8250 6200 50  0001 C CNN
F 3 "" H 8250 6200 50  0001 C CNN
	1    8250 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 3900 4900 3900
Text Label 5000 3900 0    50   ~ 0
RST
$Comp
L Switch:SW_Push SW2
U 1 1 5B605045
P 4700 3900
F 0 "SW2" H 4700 4185 50  0000 C CNN
F 1 "SW_Push" H 4700 4094 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_SPST_B3U-1000P" H 4700 4100 50  0001 C CNN
F 3 "" H 4700 4100 50  0001 C CNN
	1    4700 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 3900 4150 3900
Wire Wire Line
	4150 3900 4150 4050
$Comp
L power:GNDREF #PWR0103
U 1 1 5B605190
P 4150 4050
F 0 "#PWR0103" H 4150 3800 50  0001 C CNN
F 1 "GNDREF" H 4155 3877 50  0000 C CNN
F 2 "" H 4150 4050 50  0001 C CNN
F 3 "" H 4150 4050 50  0001 C CNN
	1    4150 4050
	1    0    0    -1  
$EndComp
Text Label 9200 5550 0    50   ~ 0
D+
Wire Wire Line
	8550 5350 8700 5350
Text Label 8700 5350 0    50   ~ 0
VBUS
Wire Wire Line
	6250 1200 6450 1200
Wire Wire Line
	6250 1300 6450 1300
Wire Wire Line
	6450 1200 6450 1300
Connection ~ 6450 1300
Wire Wire Line
	6250 1100 7300 1100
$Comp
L Device:C C2
U 1 1 5B607435
P 7300 1400
F 0 "C2" H 7415 1446 50  0000 L CNN
F 1 "4.7uF" H 7415 1355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 7338 1250 50  0001 C CNN
F 3 "~" H 7300 1400 50  0001 C CNN
	1    7300 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 1250 7300 1100
Connection ~ 7300 1100
Wire Wire Line
	7300 1100 7600 1100
Wire Wire Line
	7300 1550 7300 1700
$Comp
L power:GNDREF #PWR0105
U 1 1 5B6079B9
P 7300 1700
F 0 "#PWR0105" H 7300 1450 50  0001 C CNN
F 1 "GNDREF" H 7305 1527 50  0000 C CNN
F 2 "" H 7300 1700 50  0001 C CNN
F 3 "" H 7300 1700 50  0001 C CNN
	1    7300 1700
	1    0    0    -1  
$EndComp
Text Label 7600 1100 0    50   ~ 0
VCC
$Comp
L Device:LED D3
U 1 1 5B607BC5
P 5000 6550
F 0 "D3" V 5038 6433 50  0000 R CNN
F 1 "LED" V 4947 6433 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 5000 6550 50  0001 C CNN
F 3 "~" H 5000 6550 50  0001 C CNN
	1    5000 6550
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D2
U 1 1 5B607CAC
P 4650 6550
F 0 "D2" V 4688 6433 50  0000 R CNN
F 1 "LED" V 4597 6433 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 4650 6550 50  0001 C CNN
F 3 "~" H 4650 6550 50  0001 C CNN
	1    4650 6550
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D1
U 1 1 5B607D35
P 4300 6550
F 0 "D1" V 4338 6433 50  0000 R CNN
F 1 "LED" V 4247 6433 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 4300 6550 50  0001 C CNN
F 3 "~" H 4300 6550 50  0001 C CNN
	1    4300 6550
	0    -1   -1   0   
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 5B607DD5
P 3700 6150
F 0 "SW1" V 3654 6298 50  0000 L CNN
F 1 "SW_Push" V 3745 6298 50  0000 L CNN
F 2 "Button_Switch_SMD:SW_SPST_B3U-1000P" H 3700 6350 50  0001 C CNN
F 3 "" H 3700 6350 50  0001 C CNN
	1    3700 6150
	0    1    1    0   
$EndComp
Wire Wire Line
	5150 3100 4900 3100
Text Label 4900 3100 2    50   ~ 0
BUTTON1
$Comp
L power:GNDREF #PWR0106
U 1 1 5B6085B6
P 3700 6950
F 0 "#PWR0106" H 3700 6700 50  0001 C CNN
F 1 "GNDREF" H 3705 6777 50  0000 C CNN
F 2 "" H 3700 6950 50  0001 C CNN
F 3 "" H 3700 6950 50  0001 C CNN
	1    3700 6950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 5950 3700 5800
Text Label 3700 5800 0    50   ~ 0
BUTTON1
Wire Wire Line
	4300 5950 4300 5800
Wire Wire Line
	4650 5950 4650 5800
Wire Wire Line
	5000 5950 5000 5800
Text Label 4300 5800 0    50   ~ 0
VCC
Text Label 4650 5800 0    50   ~ 0
VCC
Text Label 5000 5800 0    50   ~ 0
VCC
$Comp
L Device:R R1
U 1 1 5B609F5F
P 4300 6100
F 0 "R1" H 4370 6146 50  0000 L CNN
F 1 "1k" H 4370 6055 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4230 6100 50  0001 C CNN
F 3 "~" H 4300 6100 50  0001 C CNN
	1    4300 6100
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 5B609FEA
P 4650 6100
F 0 "R2" H 4720 6146 50  0000 L CNN
F 1 "1k" H 4720 6055 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4580 6100 50  0001 C CNN
F 3 "~" H 4650 6100 50  0001 C CNN
	1    4650 6100
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 5B60A04C
P 5000 6100
F 0 "R3" H 5070 6146 50  0000 L CNN
F 1 "1k" H 5070 6055 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4930 6100 50  0001 C CNN
F 3 "~" H 5000 6100 50  0001 C CNN
	1    5000 6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 6250 4300 6400
Wire Wire Line
	4650 6250 4650 6400
Wire Wire Line
	5000 6250 5000 6400
Wire Wire Line
	4300 6700 4300 6950
Wire Wire Line
	3700 6350 3700 6950
$Comp
L power:GNDREF #PWR0107
U 1 1 5B60D4A0
P 4300 6950
F 0 "#PWR0107" H 4300 6700 50  0001 C CNN
F 1 "GNDREF" H 4305 6777 50  0000 C CNN
F 2 "" H 4300 6950 50  0001 C CNN
F 3 "" H 4300 6950 50  0001 C CNN
	1    4300 6950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 6700 4650 7150
Wire Wire Line
	5000 6700 5000 7150
Text Label 4650 7150 0    50   ~ 0
LED1
Text Label 5000 7150 0    50   ~ 0
LED2
Wire Wire Line
	2200 6300 2550 6300
Text Label 2550 6300 0    50   ~ 0
SWDIO
Wire Wire Line
	2200 6400 2550 6400
Text Label 2550 6400 0    50   ~ 0
SWCLK
Wire Wire Line
	5150 1500 4900 1500
Wire Wire Line
	5150 1700 4900 1700
Text Label 4900 1500 2    50   ~ 0
SWCLK
Text Label 4900 1700 2    50   ~ 0
SWDIO
$Comp
L Connector_Generic:Conn_01x20 J1
U 1 1 5B617C87
P 1450 2000
F 0 "J1" H 1370 775 50  0000 C CNN
F 1 "Conn_01x20" H 1370 866 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x20_P2.54mm_Vertical" H 1450 2000 50  0001 C CNN
F 3 "~" H 1450 2000 50  0001 C CNN
	1    1450 2000
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x20 J2
U 1 1 5B617DEE
P 1450 4300
F 0 "J2" H 1370 3075 50  0000 C CNN
F 1 "Conn_01x20" H 1370 3166 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x20_P2.54mm_Vertical" H 1450 4300 50  0001 C CNN
F 3 "~" H 1450 4300 50  0001 C CNN
	1    1450 4300
	-1   0    0    1   
$EndComp
Wire Wire Line
	2300 5200 2300 5350
$Comp
L power:GNDREF #PWR0110
U 1 1 5B619889
P 2300 5350
F 0 "#PWR0110" H 2300 5100 50  0001 C CNN
F 1 "GNDREF" H 2305 5177 50  0000 C CNN
F 2 "" H 2300 5350 50  0001 C CNN
F 3 "" H 2300 5350 50  0001 C CNN
	1    2300 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 1000 2300 1150
Wire Wire Line
	2300 2900 2300 3050
$Comp
L power:GNDREF #PWR0111
U 1 1 5B61B741
P 2300 1150
F 0 "#PWR0111" H 2300 900 50  0001 C CNN
F 1 "GNDREF" H 2305 977 50  0000 C CNN
F 2 "" H 2300 1150 50  0001 C CNN
F 3 "" H 2300 1150 50  0001 C CNN
	1    2300 1150
	1    0    0    -1  
$EndComp
$Comp
L power:GNDREF #PWR0112
U 1 1 5B61B799
P 2300 3050
F 0 "#PWR0112" H 2300 2800 50  0001 C CNN
F 1 "GNDREF" H 2305 2877 50  0000 C CNN
F 2 "" H 2300 3050 50  0001 C CNN
F 3 "" H 2300 3050 50  0001 C CNN
	1    2300 3050
	1    0    0    -1  
$EndComp
$Comp
L Device:Crystal Y1
U 1 1 5B60CA9D
P 4550 2150
F 0 "Y1" H 4550 2418 50  0000 C CNN
F 1 "Crystal" H 4550 2327 50  0000 C CNN
F 2 "Crystal:Crystal_SMD_TXC_9HT11-2Pin_2.0x1.2mm" H 4550 2150 50  0001 C CNN
F 3 "~" H 4550 2150 50  0001 C CNN
	1    4550 2150
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C3
U 1 1 5B611129
P 4050 1950
F 0 "C3" V 3798 1950 50  0000 C CNN
F 1 "9pF" V 3889 1950 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4088 1800 50  0001 C CNN
F 3 "~" H 4050 1950 50  0001 C CNN
	1    4050 1950
	0    1    1    0   
$EndComp
$Comp
L Device:C C4
U 1 1 5B6111B2
P 4050 2350
F 0 "C4" V 3798 2350 50  0000 C CNN
F 1 "9pF" V 3889 2350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4088 2200 50  0001 C CNN
F 3 "~" H 4050 2350 50  0001 C CNN
	1    4050 2350
	0    1    1    0   
$EndComp
Wire Wire Line
	4850 1950 4550 1950
Wire Wire Line
	4850 2350 4550 2350
Wire Wire Line
	4550 2300 4550 2350
Connection ~ 4550 2350
Wire Wire Line
	4550 2350 4200 2350
Wire Wire Line
	4550 2000 4550 1950
Connection ~ 4550 1950
Wire Wire Line
	4550 1950 4200 1950
Wire Wire Line
	3900 1950 3750 1950
Wire Wire Line
	3900 2350 3750 2350
Wire Wire Line
	3750 1950 3750 2150
Wire Wire Line
	3750 2150 3450 2150
Wire Wire Line
	3450 2150 3450 2250
Connection ~ 3750 2150
Wire Wire Line
	3750 2150 3750 2350
$Comp
L power:GNDREF #PWR0104
U 1 1 5B61E5DA
P 3450 2250
F 0 "#PWR0104" H 3450 2000 50  0001 C CNN
F 1 "GNDREF" H 3455 2077 50  0000 C CNN
F 2 "" H 3450 2250 50  0001 C CNN
F 3 "" H 3450 2250 50  0001 C CNN
	1    3450 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 3700 1900 3700
Wire Wire Line
	1650 3800 1900 3800
Wire Wire Line
	1650 3900 1900 3900
Wire Wire Line
	1650 3400 1900 3400
Wire Wire Line
	1650 3500 1900 3500
Wire Wire Line
	1650 3600 1900 3600
Wire Wire Line
	1650 4000 1900 4000
Wire Wire Line
	1650 4100 1900 4100
Wire Wire Line
	1650 4200 1900 4200
Wire Wire Line
	1650 4300 1900 4300
Wire Wire Line
	1650 4400 1900 4400
Wire Wire Line
	1650 4500 1900 4500
Wire Wire Line
	1650 4600 1900 4600
Wire Wire Line
	1650 4700 1900 4700
Wire Wire Line
	1650 4800 1900 4800
Wire Wire Line
	1650 4900 1900 4900
Wire Wire Line
	1650 5000 1900 5000
Wire Wire Line
	1650 5100 1900 5100
Wire Wire Line
	1650 2600 1900 2600
Wire Wire Line
	1650 2700 1900 2700
Wire Wire Line
	1650 2800 1900 2800
Wire Wire Line
	1650 2300 1900 2300
Wire Wire Line
	1650 2400 1900 2400
Wire Wire Line
	1650 2500 1900 2500
Wire Wire Line
	1650 2000 1900 2000
Wire Wire Line
	1650 2100 1900 2100
Wire Wire Line
	1650 2200 1900 2200
Wire Wire Line
	1650 1700 1900 1700
Wire Wire Line
	1650 1800 1900 1800
Wire Wire Line
	1650 1900 1900 1900
Wire Wire Line
	1650 1400 1900 1400
Wire Wire Line
	1650 1500 1900 1500
Wire Wire Line
	1650 1600 1900 1600
Wire Wire Line
	1650 1100 1900 1100
Wire Wire Line
	1650 1200 1900 1200
Wire Wire Line
	1650 1300 1900 1300
Wire Wire Line
	6250 3600 6500 3600
Wire Wire Line
	6250 3700 6500 3700
Wire Wire Line
	6250 3800 6500 3800
Wire Wire Line
	6250 3300 6500 3300
Wire Wire Line
	6250 3400 6500 3400
Wire Wire Line
	6250 3500 6500 3500
Wire Wire Line
	6250 3200 6500 3200
Wire Wire Line
	4900 2300 5150 2300
Text Label 6500 3800 0    50   ~ 0
P0.25
Text Label 6500 3700 0    50   ~ 0
P0.26
Text Label 6500 3600 0    50   ~ 0
P0.27
Text Label 6500 3500 0    50   ~ 0
P0.28
Text Label 6500 3400 0    50   ~ 0
P0.29
Text Label 6500 3300 0    50   ~ 0
P0.30
Text Label 6500 3200 0    50   ~ 0
P0.31
Text Label 4900 2300 0    50   ~ 0
P0.02
Wire Wire Line
	4850 2100 4850 1950
Wire Wire Line
	4850 2200 4850 2350
Wire Wire Line
	4850 2200 5150 2200
Wire Wire Line
	4850 2100 5150 2100
Wire Wire Line
	6250 2100 6500 2100
Wire Wire Line
	6250 2200 6500 2200
Wire Wire Line
	6250 2300 6500 2300
Wire Wire Line
	6250 2000 6500 2000
Wire Wire Line
	6250 2400 6500 2400
Wire Wire Line
	6900 1650 6900 1700
$Comp
L power:GNDREF #PWR0109
U 1 1 5B699B0D
P 6900 1700
F 0 "#PWR0109" H 6900 1450 50  0001 C CNN
F 1 "GNDREF" H 6905 1527 50  0000 C CNN
F 2 "" H 6900 1700 50  0001 C CNN
F 3 "" H 6900 1700 50  0001 C CNN
	1    6900 1700
	1    0    0    -1  
$EndComp
Text Label 6500 2000 0    50   ~ 0
P1.05
Wire Wire Line
	6900 1350 6900 1300
Text Label 6500 2100 0    50   ~ 0
P1.06
Text Label 6500 2200 0    50   ~ 0
P1.07
Text Label 6500 2300 0    50   ~ 0
P1.08
Text Label 6500 2400 0    50   ~ 0
P1.09
Text Label 1900 3400 0    50   ~ 0
P0.25
Text Label 1900 3500 0    50   ~ 0
P0.26
Text Label 1900 3600 0    50   ~ 0
P0.27
Text Label 1900 3700 0    50   ~ 0
P0.28
Text Label 1900 3800 0    50   ~ 0
P0.29
Text Label 1900 3900 0    50   ~ 0
P0.30
Text Label 1900 4000 0    50   ~ 0
P0.31
Wire Wire Line
	2300 3300 2300 3400
Wire Wire Line
	1650 3300 2300 3300
$Comp
L power:GNDREF #PWR0113
U 1 1 5B6AF547
P 2300 3400
F 0 "#PWR0113" H 2300 3150 50  0001 C CNN
F 1 "GNDREF" H 2305 3227 50  0000 C CNN
F 2 "" H 2300 3400 50  0001 C CNN
F 3 "" H 2300 3400 50  0001 C CNN
	1    2300 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 1000 2300 1000
Wire Wire Line
	1650 2900 2300 2900
Wire Wire Line
	1650 5200 2300 5200
Wire Wire Line
	4900 2500 5150 2500
Wire Wire Line
	4900 2600 5150 2600
Wire Wire Line
	4900 2700 5150 2700
Wire Wire Line
	4900 2400 5150 2400
Text Label 1900 4200 0    50   ~ 0
P1.05
Text Label 1900 4300 0    50   ~ 0
P1.06
Text Label 1900 4400 0    50   ~ 0
P1.07
Text Label 1900 4500 0    50   ~ 0
P1.08
Text Label 1900 4600 0    50   ~ 0
P1.09
Text Label 1900 4100 0    50   ~ 0
P0.02
Wire Wire Line
	6250 2500 6500 2500
Text Label 6500 2500 0    50   ~ 0
P1.10
Text Label 4900 2400 0    50   ~ 0
P0.03
Text Label 4900 2500 0    50   ~ 0
P0.04
Text Label 4900 2600 0    50   ~ 0
P0.05
Text Label 4900 2700 0    50   ~ 0
P0.06
Text Label 1900 4700 0    50   ~ 0
P0.03
Text Label 1900 4800 0    50   ~ 0
P0.04
Text Label 1900 4900 0    50   ~ 0
P0.05
Wire Wire Line
	6250 1700 6500 1700
Wire Wire Line
	6250 1800 6500 1800
Wire Wire Line
	6250 1500 6500 1500
Wire Wire Line
	6250 1600 6500 1600
Wire Wire Line
	4900 3600 5150 3600
Wire Wire Line
	4900 3400 5150 3400
Wire Wire Line
	4900 3500 5150 3500
Wire Wire Line
	4900 4000 5150 4000
Wire Wire Line
	4900 4100 5150 4100
Wire Wire Line
	4900 3700 5150 3700
Wire Wire Line
	4900 3800 5150 3800
Wire Wire Line
	6250 4100 6500 4100
Wire Wire Line
	4900 4200 5150 4200
Wire Wire Line
	6250 3900 6500 3900
Wire Wire Line
	6250 4000 6500 4000
Text Label 7050 1300 0    50   ~ 0
VBUS
Connection ~ 6900 1300
Wire Wire Line
	6900 1300 7050 1300
Wire Wire Line
	6450 1300 6900 1300
Text Label 6500 1500 0    50   ~ 0
P1.00
Text Label 6500 1600 0    50   ~ 0
P1.01
Text Label 6500 1700 0    50   ~ 0
P1.02
Text Label 6500 1800 0    50   ~ 0
P1.03
Text Label 4900 3400 0    50   ~ 0
P0.13
Text Label 4900 3500 0    50   ~ 0
P0.14
Text Label 4900 3600 0    50   ~ 0
P0.15
Text Label 4900 3700 0    50   ~ 0
P0.16
Text Label 4900 3800 0    50   ~ 0
P0.17
Text Label 4900 4000 0    50   ~ 0
P0.19
Text Label 4900 4100 0    50   ~ 0
P0.20
Text Label 4900 4200 0    50   ~ 0
P0.21
Text Label 6500 4100 0    50   ~ 0
P0.22
Text Label 6500 4000 0    50   ~ 0
P0.23
Text Label 6500 3900 0    50   ~ 0
P0.24
Text Label 1900 1100 0    50   ~ 0
SWDIO
Text Label 1900 1200 0    50   ~ 0
SWCLK
Text Label 1900 1300 0    50   ~ 0
P0.24
Text Label 1900 1400 0    50   ~ 0
P0.23
Text Label 1900 1500 0    50   ~ 0
P0.22
Text Label 1900 1600 0    50   ~ 0
RST
Text Label 1900 1700 0    50   ~ 0
P0.20
Text Label 1900 1800 0    50   ~ 0
P0.19
Text Label 1900 1900 0    50   ~ 0
P0.21
Text Label 1900 2000 0    50   ~ 0
P0.17
Text Label 1900 2100 0    50   ~ 0
P0.16
Text Label 1900 2200 0    50   ~ 0
P0.15
Text Label 1900 2300 0    50   ~ 0
P0.14
Text Label 1900 2400 0    50   ~ 0
P0.13
Text Label 1900 2500 0    50   ~ 0
P1.03
Text Label 1900 2600 0    50   ~ 0
P1.02
Text Label 1900 2700 0    50   ~ 0
P1.01
Text Label 1900 2800 0    50   ~ 0
P1.00
Wire Wire Line
	5150 1900 4900 1900
Wire Wire Line
	5150 1800 4900 1800
Text Label 4900 1800 0    50   ~ 0
D+
Text Label 4900 1900 0    50   ~ 0
D-
Wire Wire Line
	6250 3000 6550 3000
Text Label 6550 3000 0    50   ~ 0
LED1
Wire Wire Line
	5150 2800 4900 2800
Text Label 4900 2800 0    50   ~ 0
LED2
NoConn ~ 5150 3300
NoConn ~ 5150 3200
NoConn ~ 5150 2900
NoConn ~ 5150 3000
NoConn ~ 6250 2600
NoConn ~ 6250 2700
NoConn ~ 6250 2800
NoConn ~ 6250 2900
NoConn ~ 6250 1900
NoConn ~ 8550 5750
Text Label 1900 5000 0    50   ~ 0
VCC
Text Label 1900 5100 0    50   ~ 0
VBUS
$Comp
L Connector_Generic:Conn_02x05_Odd_Even J4
U 1 1 5B871242
P 1900 6500
F 0 "J4" H 1950 6917 50  0000 C CNN
F 1 "Conn_02x05_Odd_Even" H 1950 6826 50  0000 C CNN
F 2 "Connector_PinHeader_1.27mm:PinHeader_2x05_P1.27mm_Vertical_SMD" H 1900 6500 50  0001 C CNN
F 3 "~" H 1900 6500 50  0001 C CNN
	1    1900 6500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 6300 1450 6300
Wire Wire Line
	1700 6400 1400 6400
Wire Wire Line
	1400 6400 1400 6500
Wire Wire Line
	1700 6500 1400 6500
Connection ~ 1400 6500
Wire Wire Line
	1400 6500 1400 6700
Wire Wire Line
	1700 6700 1400 6700
Connection ~ 1400 6700
Wire Wire Line
	1400 6700 1400 6950
$Comp
L power:GNDREF #PWR0108
U 1 1 5B891162
P 1400 6950
F 0 "#PWR0108" H 1400 6700 50  0001 C CNN
F 1 "GNDREF" H 1405 6777 50  0000 C CNN
F 2 "" H 1400 6950 50  0001 C CNN
F 3 "" H 1400 6950 50  0001 C CNN
	1    1400 6950
	1    0    0    -1  
$EndComp
Text Label 1450 6300 2    50   ~ 0
VCC
NoConn ~ 2200 6600
NoConn ~ 1700 6600
NoConn ~ 2200 6500
NoConn ~ 2200 6700
$EndSCHEMATC
