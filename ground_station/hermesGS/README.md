# Hermes GS

-----------------------------------------------------------------
This project is based on the [ERT 2020 Bella Lui GS project](https://github.com/MorgesHAB/ERT2020GS)

Note: this is still work in progress

-----------------------------------------------------------------
## Project purpose 
Since we are aiming for the rocket (Hermes II) to go at supersonic speeds and its size is relatively small,
it is mandatory that we have a telemetry link to receive GPS data on the ground in order to be able to successfully recover the rocket.

The Hermes ground station therefore has the objective of receiving and logging telemetry data coming from the rocket.


-----------------------------------------------------------------
## Repository organization tree
```
hermesGS
│   README.md
│   CmakeLists.txt              main cmake file calling cmake subfiles
│   autoBuild.sh                bash script to compile all this software   
└───Telecom
│   │   CMakeLists.txt          Telecom compilation config
│   └───Worker
│   │   │   Worker.h/cpp        Program main routine
│   │
│   └───Serializer
│   │   │   Packet.h/cpp        Serialize basic data types into a uint8_t buffer
│   │
│   └───DataHandler
│   │   │   DatagramTypes.h     DatagramID enum  
│   │   │   DataHandler.h/cpp   Manage Datagram at Tx & Rx
│   │   │   Datagram.h/cpp      Class that contains Data and a packet
│   │
│   └───DataStructures
│   │   │   Data.h              Abstact superclass that all DataStructures must inherit
│   │   │   Template.h/cpp      Example to follow when creating new Data structures
│   │   │                       // Then Data are sorted by substem
│   │   └───Basic               Basic generic class for simple data transmission
│   │   └───FrameInfo           Packet metadata required (XbeeOptions, Header, CRC)
│   │   └───File                Manage the transmission of a file
│   │   └───Avionics            Subsystem Data definition
│   │   └───GSE                 ""
│   │   └───Payload             ""
│   │   └───Propulsion          ""
│   │   └───Hermes              ""
│   │
│   └───RFmodem
│       │   RFmodem.h/cpp       Superclass for RF communication
│       │   Xbee.h/cpp          Manage serial communication with a packet
│       │   LoRa.h/cpp          Same but adapted for this RF modem
│       └───lib                 libraries for xbee and LoRa modem
│
└───RF-UI Interface             Allows the exchange of data between DataHandler & UI
│   │   connector.h/cpp         class containing an array<atomic<uint64_t>
│   │   ProtocolDefine.h        Define all indexes with an enum
│
└───UI
│   │   Guiwindow.h/cpp         backend of the GUI main window
│   │   SecondWindow.h/cpp      backend of the 2nd window
│   │   ui_form.h               file auto generated by Qt (GUI front end)
│   └───ui_file                 Qt Designer file from which ui_form.h can be generated
│   └───assets                  All the images, icons, etc for the GUI
│
└───Logger
│   │   Logger.h/cpp            Manage file logging
│   │   Loggable.h/cpp          Superclass of all loggable entity eg: Datagram
│   │   ...
│
└───src                         all available executables
│   │   hermesGS.cpp            Main program
│   │   ERT2020GS.cpp           Bella Lui 2020 Main program with GUI
│   │   PacketXTest.cpp         Test the Tx or Rx of a specified Datagram
│   │   AVsimulator.cpp         Simulate AV computer by sending multiple AV Datagram
│   │   TestDebug.cpp           Test only the xbee transmission without this software
│   
└───doc
    │   XbeeGS2020config.xpro   XCTU configuration file for xbee
    │   Doxyfile                Allows to generate Doxygen documention of this software
    └───html                    Result of doxygen documentation
    └───archive                 Old code
```
-----------------------------------------------------------------
## Ground Station system diagram
###  GST Hardware diagram (ERT2020GS)
<img src="doc/img/2020_GS_GST_Hardware_System_Diagram.svg">

###  GST Software diagram (ERT2020GS)
<img src="doc/img/2020_GS_GST_Software_System_Diagram.svg">

-----------------------------------------------------------------

## Hardware

The ground stations runs directly on a computer running a unix based OS (Windows is also possible through WSL).

The XBee board is composed of an XBee SX868 RF transceiver and a Silicon Labs CP2104 USB-to-UART bridge
to provide a USB serial interface.

-----------------------------------------------------------------
## Prerequisites

In order to have a correct building, you will need to install the following software

Install cmake to compile the code
```console
sudo apt-get install cmake
```
Install git to be able to clone this git repository
```console
sudo apt-get install git
```


<img src="doc/img/Qt.png" width=120 align="right" >

-----------------------------------------------------------------
## Configure the xbee
For a correct communication, if the xbee leaves the factory, you will have to flash
the same configurations profile as the other xbee modules using XCTU software.  
You can find this configuration profile in [doc/XbeeGS2020config.xpro](doc/XbeeGS2020config.xpro)  

-----------------------------------------------------------------
## Building the software

First clone the GitHub repository in a folder using:
```console
git clone https://github.com/EPFLRocketTeam/SupersonicTestRocket.git
```
Move to the root folder of the project and then to the GS directory
```console
cd SupersonicTestRocket/ground_station/hermesGS/
```
Then run the bash to compile and build the executable files:
```console
sudo bash autoBuild.sh
```
If there are some errors :
* Open the main [CmakeLists.txt](CMakeLists.txt) and select your build configuration.
Maybe your Qt installation, or other get some errors.
```console
set(USE_GUI ON) # write ON or OFF 
set(USE_XBEE ON)
set(RUNNING_ON_RPI OFF) # OFF if you're working on your laptop
set(USE_SOUND OFF)
```
* First test that the cmake works correctly
```console
cd build
cmake ..
```
* Then use make and specify your target :
```console
make clean hermesGS
```
-----------------------------------------------------------------
## Running the software
Once the executable is successfully built, you can run it using :
```console
./hermesGS [serial port]                # RX
./hermesGS [DatagramID] [serial port]   # TX (to send a test packet)
```

You will need to run the program as sudo if you want to log received packets


-----------------------------------------------------------------
### Authors
* Michael Ha - Implementation of the ground station for Hermes II

### Acknowledgements (Original authors)
* [Cem Keske](https://ch.linkedin.com/in/cem-keske-565363164) Mission UI & Logger
* [Stéphanie Lebrun](https://ch.linkedin.com/in/st%C3%A9phanie-lebrun-491695192) implementation of some Data structures
* [Lionel Isoz](https://github.com/MorgesHAB) RF telecommunication - xbee network - DataHandler

-----------------------------------------------------------------
### Useful links
Documentations
* [XBee API mode](https://www.digi.com/resources/documentation/Digidocs/90001942-13/concepts/c_api_frame_structure.htm?tocpath=XBee%20API%20mode%7C_____2)
* [XBee Zigbee Mesh Kit](https://www.digi.com/resources/documentation/digidocs/pdfs/90001942-13.pdf)
* [XBee SX 868](https://www.digi.com/resources/documentation/digidocs/pdfs/90001538.pdf)

[<img src="doc/img/ERTbig.png" width=800>](https://epflrocketteam.ch/fr/)


-----------------------------------------------------------------
# Appendix

## Datagrams description
Here is the communication protocol interface file that we share with other subsystems.  
Zoom in for better visibility (svg).  

<img src="doc/img/2020_GS_Communication%20Protocol%20Interface.svg">

-----------------------------------------------------------------
## Tutorial 1 : creating a new Datagram

- [X] First, create a new DatagramID in [DatagramTypes.h](Telecom/DataHandler/DatagramTypes.h) 
and modify the getDatagramIDName() function.  
- [ ] Then go in the [constructor of DataHandler](Telecom/DataHandler/DataHandler.cpp) and create 
your new Datagram referred to as `dataHandler[MY_DATAGRAMID]`  by adding it all the 
Data you want.  
NB : the "add" method calling order defines the data order in your packet. 
```cpp
dataHandler[DatagramID]->add(new MyData);
```
For example, if my DatagramID is "AVIONIC_TEST"
```cpp
dataHandler[AVIONIC_TEST]->add(new BasicData<float>(DataType::TEMPERATURE_SENSOR));
dataHandler[AVIONIC_TEST]->add(new BasicData<float>(DataType::AV_ALTITUDE));
dataHandler[AVIONIC_TEST]->add(new BasicData<uint8_t>(DataType::AV_ORDER));
dataHandler[AVIONIC_TEST]->add(new String("Hello from space"));
dataHandler[AVIONIC_TEST]->add(new GPS);
//... and so on
```
You can add as much data as you want as long as the datagram (packet) size does not exceed 256 bytes.  

If your Datagram is quiet basic, that means it only need to stock the received data in
the RF-GUI connector or send a data located in that connector, you can use the generic 
"BasicData" class and only specify the connector index define in 
[ProtocolDefine.h](RF-UI-Interface/ProtocolDefine.h). Thus you will be able to process 
and print your data in the GUI program.

If your data is more complex, you have to create a new class that inherits of 
[Data](Telecom/DataStructures/Data.h) by following this [Template.h](Telecom/DataStructures/Template.h).

```cpp
// Pattern class
#include <Data.h>

class MyData : public Data {         // must inherite of the super class Data
public:
    void write(Packet& packet) override;
    void parse(Packet& packet) override;
    void print() const override;

    bool updateTx(std::shared_ptr<Connector> connector) override;
    bool updateRx(std::shared_ptr<Connector> connector) override;

private:
    float nbr;
    int x;
    char id;
    // ... Your class attributes
};
```
Then choose the data you want to transmit for example nbr & x  
So you just need to implement the write & parse function by adding   
Warning: the order is important.
```cpp
void MyData::write(Packet &packet) {
    packet.write(nbr);  // Use low level function to write bit to bit the RFpacket
    packet.write(x);
}
// Now in same same data order :
void MyData::parse(Packet &packet) {
    packet.parse(nbr);  // Use low level function to parse bit to bit the RFpacket
    packet.parse(x);
}
```

Finally, here is an example diagram of how to implement a new datagram
<img src="doc/img/2020_GS_Datagram_Diagram.svg">
