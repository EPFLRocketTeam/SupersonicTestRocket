# Hermes II Avionics Software : README

Authors: Jushua Cayetano-Emond, Mael Feurgard

Last edited : 30.04.2022

Note : this is still work in progress, and there is no guarantee that this program works correctly at the time

## Hardware

This code is made for a Teensy 3.5 board mounted on a custom PCB with the following components:

* IMU: ADIS16470
* Accelerometer: AISx1120SX
* Pressure sensor: Honeywell RSC
* Thermocouple: Two MAX31855
* Event-based altimeter: Alitmax G4
* GNSS module: uBlox MAX-7C
* Load cell: AD8556
* Radio: XBee SX 868

## Objectives

The avionics have two main objectives:

* Data acquisition
* Position telemetry for recovery

They are combined in a more general "data acquisition, local storage and telemetry".
The code is in charge the log as often as possible data from the sensors to the SD card mounted on the Teensy controller,
and occasionally transmit data from all sensors via radio.

## Code structure

This code is meant to work in the context of a PlatformIO project (addon for VS Code, take care of library maangement and upload to microcontroller).

Some cleanup is to be made, but the main files are in the `src` folder, which is as follow

```txt
src
│   CustomTypes.cpp
│   dataAcquisition.cpp
│   io.cpp
│   logging.cpp
│   main.cpp
│   Packet.cpp
│   XB8XWrapper.cpp
│
└───Sensors
    │   [Sensor's name]Wrapper.cpp
    │   [Sensor's name]Wrapper.hpp
    │   Sensor.cpp
    │   Sensor.hpp
    │
    └───PacketBody
            [Sensor's name]Body.h
```

(This folder has its companion `include` folder, with the corresponding headers)

We provide a brief overview of each file:

* `CutsomTypes` : Contains an `enum` type for the different Packet types
* `dataAcquisition` : Where the bulk of the work is done; contains the main acquisition loop, looking for if some sensors has data to be collected. Also manage radio emission and writing to the SD card the different Packets.
* `io` : Helper functions to manage LEDs
* `logging` : Helper function to simplify interaction with the SD card (and the associated library)
* `main` : Contains `setup` as well as `loop` functions. `setup` is in charge of initializing Sensors and checking then behave as expected.
* `Packet` : A virtual class to harmonize data management from the different Sensors
* `XB8XWrapper` : Interface to have radio transmission compliant with the Packet class as well as the format expected by the ground station
* `Sensors` : Folder for the `Sensor` class (virtual class to streamline sensors management) as well as its implementation for each specific sensor.
* `PacketBody` : Folder containing specifically the structure of each Packet emitted by each Sensor. This folder is linked to the ground station code to allow communications.

## Future development

The first step is to fix this code and ensure it behaves as expected. Possible additions are:

* Add a 'low consumption mode' as the GNSS and radio modules are especially power hungry
* Add a real-time Kalman filter to improve telemetry readings. Could be useful for more complex rockets and software (for instance, deploy aerobrakes)
