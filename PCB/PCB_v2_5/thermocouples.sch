EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 5
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 6450 6750 3    50   Input ~ 0
CS_TC1
Text HLabel 6350 6750 3    50   Input ~ 0
CS_TC2
Text HLabel 6250 6750 3    50   Input ~ 0
CS_TC3
Text HLabel 6150 6750 3    50   Input ~ 0
CS_TC4
Text HLabel 6050 6750 3    50   Input ~ 0
SCK0
Text HLabel 5950 6750 3    50   Input ~ 0
MISO0
Connection ~ 4100 850 
$Comp
L power:+3.3V #PWR?
U 1 1 6139C19E
P 4100 850
AR Path="/6139C19E" Ref="#PWR?"  Part="1" 
AR Path="/61381DB9/6139C19E" Ref="#PWR09"  Part="1" 
F 0 "#PWR09" H 4100 700 50  0001 C CNN
F 1 "+3.3V" H 4115 1023 50  0000 C CNN
F 2 "" H 4100 850 50  0001 C CNN
F 3 "" H 4100 850 50  0001 C CNN
	1    4100 850 
	1    0    0    -1  
$EndComp
Connection ~ 4600 3500
$Comp
L power:GND #PWR?
U 1 1 6139C18E
P 4600 3500
AR Path="/6139C18E" Ref="#PWR?"  Part="1" 
AR Path="/61381DB9/6139C18E" Ref="#PWR014"  Part="1" 
F 0 "#PWR014" H 4600 3250 50  0001 C CNN
F 1 "GND" H 4605 3327 50  0000 C CNN
F 2 "" H 4600 3500 50  0001 C CNN
F 3 "" H 4600 3500 50  0001 C CNN
	1    4600 3500
	1    0    0    -1  
$EndComp
Connection ~ 4600 1900
$Comp
L pspice:C C?
U 1 1 6139C169
P 4350 1900
AR Path="/6139C169" Ref="C?"  Part="1" 
AR Path="/61381DB9/6139C169" Ref="C1"  Part="1" 
F 0 "C1" V 4500 1750 50  0000 L CNN
F 1 "0.1uF" V 4600 1750 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 4350 1900 50  0001 C CNN
F 3 "~" H 4350 1900 50  0001 C CNN
	1    4350 1900
	0    1    1    0   
$EndComp
Connection ~ 5000 1350
Connection ~ 5000 2950
Wire Wire Line
	5000 3000 5000 2950
Wire Wire Line
	4600 3250 4600 3500
Wire Wire Line
	4100 3300 4100 3500
Connection ~ 4100 3300
Wire Wire Line
	5000 3300 4100 3300
Wire Wire Line
	4100 2450 4100 3300
$Comp
L Device:R R?
U 1 1 6139C13A
P 5000 3150
AR Path="/6139C13A" Ref="R?"  Part="1" 
AR Path="/61381DB9/6139C13A" Ref="R8"  Part="1" 
F 0 "R8" H 5070 3196 50  0000 L CNN
F 1 "10kΩ" H 5070 3105 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 4930 3150 50  0001 C CNN
F 3 "~" H 5000 3150 50  0001 C CNN
	1    5000 3150
	1    0    0    -1  
$EndComp
Connection ~ 4100 2450
$Comp
L pspice:C C?
U 1 1 6139C133
P 4350 3500
AR Path="/6139C133" Ref="C?"  Part="1" 
AR Path="/61381DB9/6139C133" Ref="C2"  Part="1" 
F 0 "C2" V 4550 3350 50  0000 L CNN
F 1 "0.1uF" V 4650 3350 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 4350 3500 50  0001 C CNN
F 3 "~" H 4350 3500 50  0001 C CNN
	1    4350 3500
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR?
U 1 1 6139C12D
P 4100 2450
AR Path="/6139C12D" Ref="#PWR?"  Part="1" 
AR Path="/61381DB9/6139C12D" Ref="#PWR010"  Part="1" 
F 0 "#PWR010" H 4100 2300 50  0001 C CNN
F 1 "+3.3V" H 4115 2623 50  0000 C CNN
F 2 "" H 4100 2450 50  0001 C CNN
F 3 "" H 4100 2450 50  0001 C CNN
	1    4100 2450
	1    0    0    -1  
$EndComp
$Comp
L Sensor_Temperature:MAX31855KASA U?
U 1 1 6139C127
P 4600 2850
AR Path="/6139C127" Ref="U?"  Part="1" 
AR Path="/61381DB9/6139C127" Ref="U6"  Part="1" 
F 0 "U6" H 4600 3431 50  0000 C CNN
F 1 "MAX31855KASA" H 4600 3340 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5600 2500 50  0001 C CIN
F 3 "http://datasheets.maximintegrated.com/en/ds/MAX31855.pdf" H 4600 2850 50  0001 C CNN
	1    4600 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 1400 5000 1350
$Comp
L power:GND #PWR?
U 1 1 6139C120
P 4600 1900
AR Path="/6139C120" Ref="#PWR?"  Part="1" 
AR Path="/61381DB9/6139C120" Ref="#PWR013"  Part="1" 
F 0 "#PWR013" H 4600 1650 50  0001 C CNN
F 1 "GND" H 4605 1727 50  0000 C CNN
F 2 "" H 4600 1900 50  0001 C CNN
F 3 "" H 4600 1900 50  0001 C CNN
	1    4600 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 1650 4600 1900
Wire Wire Line
	4100 1700 4100 1900
Connection ~ 4100 1700
Wire Wire Line
	5000 1700 4100 1700
Wire Wire Line
	4100 850  4100 1700
$Comp
L Device:R R?
U 1 1 6139C10F
P 5000 1550
AR Path="/6139C10F" Ref="R?"  Part="1" 
AR Path="/61381DB9/6139C10F" Ref="R7"  Part="1" 
F 0 "R7" H 5070 1596 50  0000 L CNN
F 1 "10kΩ" H 5070 1505 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 4930 1550 50  0001 C CNN
F 3 "~" H 5000 1550 50  0001 C CNN
	1    5000 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 850  4600 850 
$Comp
L Sensor_Temperature:MAX31855KASA U?
U 1 1 6139C108
P 4600 1250
AR Path="/6139C108" Ref="U?"  Part="1" 
AR Path="/61381DB9/6139C108" Ref="U5"  Part="1" 
F 0 "U5" H 4600 1831 50  0000 C CNN
F 1 "MAX31855KASA" H 4600 1740 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5600 900 50  0001 C CIN
F 3 "http://datasheets.maximintegrated.com/en/ds/MAX31855.pdf" H 4600 1250 50  0001 C CNN
	1    4600 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 2450 4600 2450
Connection ~ 4100 4250
$Comp
L power:+3.3V #PWR?
U 1 1 6076D2CA
P 4100 4250
AR Path="/6076D2CA" Ref="#PWR?"  Part="1" 
AR Path="/61381DB9/6076D2CA" Ref="#PWR011"  Part="1" 
F 0 "#PWR011" H 4100 4100 50  0001 C CNN
F 1 "+3.3V" H 4115 4423 50  0000 C CNN
F 2 "" H 4100 4250 50  0001 C CNN
F 3 "" H 4100 4250 50  0001 C CNN
	1    4100 4250
	1    0    0    -1  
$EndComp
Connection ~ 4600 6900
$Comp
L power:GND #PWR?
U 1 1 6076D2D1
P 4600 6900
AR Path="/6076D2D1" Ref="#PWR?"  Part="1" 
AR Path="/61381DB9/6076D2D1" Ref="#PWR016"  Part="1" 
F 0 "#PWR016" H 4600 6650 50  0001 C CNN
F 1 "GND" H 4605 6727 50  0000 C CNN
F 2 "" H 4600 6900 50  0001 C CNN
F 3 "" H 4600 6900 50  0001 C CNN
	1    4600 6900
	1    0    0    -1  
$EndComp
Connection ~ 4600 5300
$Comp
L pspice:C C?
U 1 1 6076D2D8
P 4350 5300
AR Path="/6076D2D8" Ref="C?"  Part="1" 
AR Path="/61381DB9/6076D2D8" Ref="C3"  Part="1" 
F 0 "C3" V 4500 5100 50  0000 L CNN
F 1 "0.1uF" V 4600 5100 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 4350 5300 50  0001 C CNN
F 3 "~" H 4350 5300 50  0001 C CNN
	1    4350 5300
	0    1    1    0   
$EndComp
Connection ~ 5000 4750
Connection ~ 5000 6350
Wire Wire Line
	5000 6400 5000 6350
Wire Wire Line
	4600 6650 4600 6900
Wire Wire Line
	4100 6700 4100 6900
Connection ~ 4100 6700
Wire Wire Line
	5000 6700 4100 6700
Wire Wire Line
	4100 5850 4100 6700
$Comp
L Device:R R?
U 1 1 6076D2EE
P 5000 6550
AR Path="/6076D2EE" Ref="R?"  Part="1" 
AR Path="/61381DB9/6076D2EE" Ref="R10"  Part="1" 
F 0 "R10" H 5070 6596 50  0000 L CNN
F 1 "10kΩ" H 5070 6505 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 4930 6550 50  0001 C CNN
F 3 "~" H 5000 6550 50  0001 C CNN
	1    5000 6550
	1    0    0    -1  
$EndComp
Connection ~ 4100 5850
$Comp
L pspice:C C?
U 1 1 6076D2F5
P 4350 6900
AR Path="/6076D2F5" Ref="C?"  Part="1" 
AR Path="/61381DB9/6076D2F5" Ref="C4"  Part="1" 
F 0 "C4" V 4550 6700 50  0000 L CNN
F 1 "0.1uF" V 4650 6700 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm" H 4350 6900 50  0001 C CNN
F 3 "~" H 4350 6900 50  0001 C CNN
	1    4350 6900
	0    1    1    0   
$EndComp
$Comp
L power:+3.3V #PWR?
U 1 1 6076D2FB
P 4100 5850
AR Path="/6076D2FB" Ref="#PWR?"  Part="1" 
AR Path="/61381DB9/6076D2FB" Ref="#PWR012"  Part="1" 
F 0 "#PWR012" H 4100 5700 50  0001 C CNN
F 1 "+3.3V" H 4115 6023 50  0000 C CNN
F 2 "" H 4100 5850 50  0001 C CNN
F 3 "" H 4100 5850 50  0001 C CNN
	1    4100 5850
	1    0    0    -1  
$EndComp
$Comp
L Sensor_Temperature:MAX31855KASA U?
U 1 1 6076D301
P 4600 6250
AR Path="/6076D301" Ref="U?"  Part="1" 
AR Path="/61381DB9/6076D301" Ref="U8"  Part="1" 
F 0 "U8" H 4600 6831 50  0000 C CNN
F 1 "MAX31855KASA" H 4600 6740 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5600 5900 50  0001 C CIN
F 3 "http://datasheets.maximintegrated.com/en/ds/MAX31855.pdf" H 4600 6250 50  0001 C CNN
	1    4600 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 4800 5000 4750
$Comp
L power:GND #PWR?
U 1 1 6076D308
P 4600 5300
AR Path="/6076D308" Ref="#PWR?"  Part="1" 
AR Path="/61381DB9/6076D308" Ref="#PWR015"  Part="1" 
F 0 "#PWR015" H 4600 5050 50  0001 C CNN
F 1 "GND" H 4605 5127 50  0000 C CNN
F 2 "" H 4600 5300 50  0001 C CNN
F 3 "" H 4600 5300 50  0001 C CNN
	1    4600 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 5050 4600 5300
Wire Wire Line
	4100 5100 4100 5300
Connection ~ 4100 5100
Wire Wire Line
	5000 5100 4100 5100
Wire Wire Line
	4100 4250 4100 5100
$Comp
L Device:R R?
U 1 1 6076D317
P 5000 4950
AR Path="/6076D317" Ref="R?"  Part="1" 
AR Path="/61381DB9/6076D317" Ref="R9"  Part="1" 
F 0 "R9" H 5070 4996 50  0000 L CNN
F 1 "10kΩ" H 5070 4905 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 4930 4950 50  0001 C CNN
F 3 "~" H 5000 4950 50  0001 C CNN
	1    5000 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 4250 4600 4250
$Comp
L Sensor_Temperature:MAX31855KASA U?
U 1 1 6076D31E
P 4600 4650
AR Path="/6076D31E" Ref="U?"  Part="1" 
AR Path="/61381DB9/6076D31E" Ref="U7"  Part="1" 
F 0 "U7" H 4600 5231 50  0000 C CNN
F 1 "MAX31855KASA" H 4600 5140 50  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 5600 4300 50  0001 C CIN
F 3 "http://datasheets.maximintegrated.com/en/ds/MAX31855.pdf" H 4600 4650 50  0001 C CNN
	1    4600 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 5850 4600 5850
Wire Wire Line
	5000 1150 5950 1150
Wire Wire Line
	5000 2750 5950 2750
Connection ~ 5950 2750
Wire Wire Line
	5950 2750 5950 1150
Wire Wire Line
	5000 4550 5950 4550
Connection ~ 5950 4550
Wire Wire Line
	5950 4550 5950 2750
Wire Wire Line
	6050 6750 6050 6050
Wire Wire Line
	5000 1050 6050 1050
Wire Wire Line
	5000 2650 6050 2650
Connection ~ 6050 2650
Wire Wire Line
	6050 2650 6050 1050
Wire Wire Line
	5000 4450 6050 4450
Connection ~ 6050 4450
Wire Wire Line
	6050 4450 6050 2650
Wire Wire Line
	5000 6050 6050 6050
Connection ~ 6050 6050
Wire Wire Line
	6050 6050 6050 4450
Wire Wire Line
	5000 6350 6150 6350
Wire Wire Line
	5000 4750 6250 4750
Wire Wire Line
	6350 2950 6350 6750
Wire Wire Line
	5000 2950 6350 2950
Wire Wire Line
	6450 6750 6450 1350
Wire Wire Line
	5000 1350 6450 1350
Wire Wire Line
	5950 6150 5950 6750
Wire Wire Line
	5950 4550 5950 6150
Connection ~ 5950 6150
Wire Wire Line
	5000 6150 5950 6150
Wire Wire Line
	3150 6350 4200 6350
Wire Wire Line
	3350 4750 4200 4750
Wire Wire Line
	4200 1150 3150 1150
Wire Wire Line
	3350 2750 4200 2750
$Comp
L Connector:Screw_Terminal_01x08 J9
U 1 1 6077487E
P 2850 3850
F 0 "J9" H 2768 3225 50  0000 C CNN
F 1 "Thermocouples" H 2768 3316 50  0000 C CNN
F 2 "TerminalBlock_Phoenix:TerminalBlock_Phoenix_MPT-0,5-8-2.54_1x08_P2.54mm_Horizontal" H 2850 3850 50  0001 C CNN
F 3 "~" H 2850 3850 50  0001 C CNN
	1    2850 3850
	-1   0    0    1   
$EndComp
Wire Wire Line
	3050 3450 3250 3450
Wire Wire Line
	3050 3550 3150 3550
Wire Wire Line
	3050 3750 3350 3750
Wire Wire Line
	3350 2750 3350 2950
Wire Wire Line
	3350 3850 3050 3850
Wire Wire Line
	3050 4050 3150 4050
Wire Wire Line
	3150 4050 3150 6150
Wire Wire Line
	3050 4150 3250 4150
Wire Wire Line
	6250 4750 6250 6750
Wire Wire Line
	6150 6350 6150 6750
$Comp
L pspice:C C?
U 1 1 6081BA52
P 3400 1350
AR Path="/6081BA52" Ref="C?"  Part="1" 
AR Path="/61381DB9/6081BA52" Ref="C12"  Part="1" 
F 0 "C12" V 3200 1300 50  0000 L CNN
F 1 "10nF" V 3100 1250 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm" H 3400 1350 50  0001 C CNN
F 3 "~" H 3400 1350 50  0001 C CNN
	1    3400 1350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3650 1350 4200 1350
$Comp
L pspice:C C?
U 1 1 60825D52
P 3600 2950
AR Path="/60825D52" Ref="C?"  Part="1" 
AR Path="/61381DB9/60825D52" Ref="C14"  Part="1" 
F 0 "C14" V 3400 2900 50  0000 L CNN
F 1 "10nF" V 3300 2850 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm" H 3600 2950 50  0001 C CNN
F 3 "~" H 3600 2950 50  0001 C CNN
	1    3600 2950
	0    -1   -1   0   
$EndComp
Connection ~ 3150 1350
Wire Wire Line
	3150 1350 3150 1150
Connection ~ 3650 1350
Wire Wire Line
	3150 1350 3150 3550
Wire Wire Line
	3250 1750 3650 1750
Wire Wire Line
	3250 1750 3250 3450
Wire Wire Line
	3650 1350 3650 1750
Connection ~ 3350 2950
Connection ~ 3850 2950
Wire Wire Line
	3850 2950 4200 2950
Wire Wire Line
	3350 2950 3350 3750
$Comp
L pspice:C C?
U 1 1 608318F8
P 3600 4550
AR Path="/608318F8" Ref="C?"  Part="1" 
AR Path="/61381DB9/608318F8" Ref="C15"  Part="1" 
F 0 "C15" V 3400 4500 50  0000 L CNN
F 1 "10nF" V 3300 4450 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm" H 3600 4550 50  0001 C CNN
F 3 "~" H 3600 4550 50  0001 C CNN
	1    3600 4550
	0    1    1    0   
$EndComp
Connection ~ 3350 4550
Wire Wire Line
	3350 4550 3350 4750
Connection ~ 3850 4550
Wire Wire Line
	3850 4550 4200 4550
Wire Wire Line
	3850 2950 3850 3650
Wire Wire Line
	3050 3650 3850 3650
Wire Wire Line
	3850 3950 3850 4550
Wire Wire Line
	3350 3850 3350 4550
Wire Wire Line
	3050 3950 3850 3950
Connection ~ 3150 6150
Wire Wire Line
	3150 6150 3150 6350
Wire Wire Line
	3650 6150 3750 6150
Wire Wire Line
	3250 5700 3750 5700
Wire Wire Line
	3750 5700 3750 6150
Wire Wire Line
	3250 5700 3250 4150
Connection ~ 3750 6150
Wire Wire Line
	3750 6150 4200 6150
$Comp
L pspice:C C?
U 1 1 6083F5DB
P 3400 6150
AR Path="/6083F5DB" Ref="C?"  Part="1" 
AR Path="/61381DB9/6083F5DB" Ref="C13"  Part="1" 
F 0 "C13" V 3200 6100 50  0000 L CNN
F 1 "10nF" V 3100 6050 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.0mm_W1.6mm_P2.50mm" H 3400 6150 50  0001 C CNN
F 3 "~" H 3400 6150 50  0001 C CNN
	1    3400 6150
	0    1    1    0   
$EndComp
$EndSCHEMATC
