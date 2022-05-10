#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" Modules to decode C structure packets into usable data in Python
    Contains a PacketType dataclass in which the existing legal C structures
    are defined and modules which will decode these packets in pandas
    DataFrames that can easily be used within Python.
Created on Mon Jul 19 18:06:57 2021

@author: newbi
"""

import struct
import numpy as np
import pandas as pd
from dataclasses import dataclass, field
from typing import List

@dataclass
class PacketType:
    _registry = {}
    
    packetID: int
    bytesSignature: str
    columnNames: List[str]
    filename: str = "out"
    sensitivities: np.ndarray = np.array([])
    packetLength: int = 0
    
    
    def __post_init__(self):
        # verify that the ID is unique
        assert self.packetID not in self._registry,\
            "Packet ID already declared."
            
        if self.packetLength == 0:
            self.packetLength = struct.calcsize(self.bytesSignature)
        else:
            assert struct.calcsize(self.bytesSignature) == self.packetLength,(
                "bytesSignature inconsistent with packet length.")
        elementsToUnpack = len(self.bytesSignature) - 1
        # set placeholder for the sensitivities if necessary
        if self.sensitivities.size == 0:
            self.sensitivities = np.ones(elementsToUnpack)
        # check that the packet's length is consistent
        assert elementsToUnpack == len(self.columnNames),\
            "Number of columns inconsistent with packet length."
        assert elementsToUnpack == self.sensitivities.size,\
            "Sensitivities inconsistent with packet length."
            
        self._registry[self.packetID] = self # keep track of existing Packets
        
        
# define the different packet types
headerPacket = PacketType(-1, '<BBBBL', ["packetType",
                                         "packetSize",
                                         "sensorID",
                                         "errors",
                                         "timestamp (us)"])
ADIS16470_archivedPacket = PacketType(1, '<hhhhhhhh',
                                      ["gyroX (deg/s)", "gyroY (deg/s)",
                                       "gyroZ (deg/s)", "accX (g)", "accY (g)",
                                       "accZ (g)", "temperature (degC)",
                                       "padding"], "ADIS16470",
                                      np.array([0.1, 0.1, 0.1,
                                                1/800, 1/800, 1/800, 0.1, 1]))
AISx120SX_archivedPacket = PacketType(2, '<hh', ["accX (g)","accY (g)"],
                                      "AISx120SX",
                                      np.array([1/(68*4), 1/(68*4)]))
RSC_pressurePacket = PacketType(3, '<f', ["pressure (PSI)"], "RSC_pressure")
RSC_tempPacket = PacketType(4, '<f', ["temperature (degC)"], "RSC_temp")
MAX_archivedPacket = PacketType(5, '<hh', ["probeTemp (degC)",
                                           "ambientTemp (degC)"],
                                "MAX", np.array([0.25/4, 0.0625/16]))
Altimax_Packet = PacketType(6, '<', [])
ADIS16470_Packet = PacketType(7, '<fffffff', ["gyroX (deg/s)", "gyroY (deg/s)",
                                              "gyroZ (deg/s)", "accX (g)",
                                              "accY (g)", "accZ (g)",
                                              "temperature (degC)"],
                              "ADIS16470")
AISx120SX_Packet = PacketType(8, '<ff', ["accX (g)","accY (g)"], "AISx120SX")
MAX_Packet = PacketType(9, '<ff', ["probeTemp (degC)", "ambientTemp (degC)"],
                        "MAX")

MAX7_Packet = PacketType(10,'<III', ["latitude (deg * 10^-7)", "longitude (deg * 10^-7)", "altitude (mm)"], "MAX7" )

# Packet type 11 is XBee, should not be logged

AD8556_PACket = PacketType(12,'<f',["Load (N)"], "AD8556")

errorNames = ["measLate", "skippedBeat", "drNoTrigger", "checksumError",
              "measInvalid", "placeholder", "placeholder", "placeholder"]


def unpackHeader(buffer):
    """ Takes a C structure packet and unpacks its header it into a tuple
            Only reads a single packet and stops afterwards
    

    Parameters
    ----------
    buffer : Bytes
        Buffer containing the packet to unpack.

    Returns
    -------
    packetType : int
        Numerical value corresponding to the packet type
    packetLength : int
        Size of the packet in bytes
    sensorID : int
        ID of the sensor
    errors : numpy array
        Array containing the boolean flags for errors for the packet
    timestamp: int
        Current timestamp of the packet in microseconds
    unpackedPacket : Tuple
        Tuple containing the unpacked data
    """
    header = struct.unpack(headerPacket.bytesSignature, 
                           buffer[0:headerPacket.packetLength])
    packetType = header[0]
    packetLength = header[1]
    
    
    if (packetType == 0):# or packetLength ==0):
        raise Warning("An empty packet was sent.")
    #elif (packetLength == PacketType._registry[packetType].packetLength
    #    + headerPacket.packetLength):
    else:
        packetLength = PacketType._registry[packetType].packetLength\
            + headerPacket.packetLength
        sensorID = header[2]
        errors = np.unpackbits(np.array([header[3]], dtype=np.uint8))
        timestamp = header[4]
            
        return (packetType, packetLength, sensorID, errors, timestamp)
    #else:
    #    raise ValueError("The packet size didn't match the packet type. "
    #                     "Perhaps an invalid packet was sent.")
        
def unpackPacketData(buffer, packetType):
    """ Takes a C structure packet and unpacks its content into a tuple
            Only reads a single packet and stops afterwards
    

    Parameters
    ----------
    buffer : Bytes
        Buffer containing the packet to unpack.
    packetType : int
        Integer corresponding to the dictionary key for which PacketType it is

    Returns
    -------
    packetType : int
        Numerical value corresponding to the packet type
    packetLength : int
        Size of the packet in bytes
    sensorID : int
        ID of the sensor
    errors : numpy array
        Array containing the boolean flags for errors for the packet
    timestamp: int
        Current timestamp of the packet in microseconds
    unpackedPacket : Tuple
        Tuple containing the unpacked data
    """
    unpackedPacket = struct.unpack(
            PacketType._registry[packetType].bytesSignature,
            buffer[headerPacket.packetLength:
                   headerPacket.packetLength + 
                   PacketType._registry[packetType].packetLength])
            
    return unpackedPacket

def unpackSinglePacket(buffer):
    """ Takes a C structure packet and unpacks it into a tuple
            Only reads a single packet and stops afterwards.
            Does not perform any sensitivity scaling.
    

    Parameters
    ----------
    buffer : Bytes
        Buffer containing the packet to unpack.

    Returns
    -------
    packetType : int
        Numerical value corresponding to the packet type
    packetLength : int
        Size of the packet in bytes
    sensorID : int
        ID of the sensor
    errors : numpy array
        Array containing the boolean flags for errors for the packet
    timestamp: int
        Current timestamp of the packet in microseconds
    unpackedPacket : Tuple
        Tuple containing the unpacked data
    """
    
    packetType, packetLength, sensorID, errors, timestamp = (
        unpackHeader(buffer))
    packetData = unpackPacketData(buffer, packetType)
    return packetType, packetLength, sensorID, errors, timestamp, packetData
    
def unpackMultiplePackets(buffer):
    """ Unpacks multiple C structure packets into a dataframe
        Take in bytes datatype with one or many C structure packets in it
        according to the defined PacketTypes. Processed them and unpacks all
        of them into a dataframe containing their raw data. This can later be
        processed with splitPacketsDataFrame()

    Parameters
    ----------
    buffer : Bytes
        Buffer containing the packet(s) to unpack..

    Returns
    -------
    df : pandas DataFrame
        DataFrame containing the unpacked mixed data.
    errorFlag : boolean
        Whether an error occured or not.

    """
    
    errorFlag = False
    cursor = 0
    dataEntries = []
    
    while (cursor < len(buffer)):
        try:
            packetType, packetLength, sensorID, errors, timestamp =\
                unpackHeader(buffer[cursor: cursor + headerPacket.packetLength])
            unpackedPacket = unpackPacketData(
                buffer[cursor:cursor+packetLength], packetType)
            
            dataEntries.append([packetType,
                                packetLength,
                                sensorID,
                                errors,
                                timestamp,
                                unpackedPacket])
        except Warning:
            print("    An empty packet was encountered. "
                  "The unpacking process is stopping.")
            errorFlag = True
            break
        except ValueError:
            print("    An incorrectly sized packet was encountered. "
                  "The unpacking process is stopping.")
            errorFlag = True
            break
        
        cursor += packetLength
        
    print("    Converting data into a pandas DataFrame.")
    df = pd.DataFrame(dataEntries,
                      columns = headerPacket.columnNames + ["data"])
    print("    Finished converting data.")
    
    return (df, errorFlag)


def splitPacketsDataFrame(df):
    """ Splits a DataFrame with mixed sensor data into individual DataFrames
        Takes a DataFrame containing mixed data from various sensors with
        various IDs and splits them into individual DataFrames.

    Parameters
    ----------
    df : pandas DataFrame
        DataFrame containing the unpacked mixed data.

    Returns
    -------
    dfs : 2D Dictionary containing pandas DataFrames
        The DataFrames are returned in a 2D dictionary.
        The first key corresponds to the packet type.
        The second key corresponds to the sensor ID.
        I.E. dfs[1][2] will return the DataFrame for PacketType 1 and ID 2 

    """
    
    assert np.array_equal(df.columns, headerPacket.columnNames + ["data"]), (
        "    Wrong dataframe type received.")
    
    dfs = {}
    
    for packetType in df["packetType"].unique():
        print(f"    Splitting {packetType=} into its own DataFrame.")
        dfPacketType = df.loc[df["packetType"] == packetType]
        
        dfs[packetType] = {}
        for sensorID in dfPacketType["sensorID"].unique():
            dfSensorID = dfPacketType.loc[dfPacketType["sensorID"] == sensorID]
            
            # unpack the error column into multiple columns
            errors = pd.DataFrame(dfSensorID["errors"].values.tolist(),
                                  dfSensorID.index)
            errors.columns = errorNames
            errors = errors.drop("placeholder", 1, errors='ignore')
            
            # unpack the data column into multiple columns
            data = pd.DataFrame(dfSensorID["data"].values.tolist(),
                                  dfSensorID.index)
            data.columns = PacketType._registry[packetType].columnNames
            data = data * PacketType._registry[packetType].sensitivities
            data = data.drop("padding", 1, errors='ignore')
                        
            dfCleaned = pd.concat([dfSensorID["timestamp (us)"],
                                  errors, data], 1)
            
            dfs[packetType][sensorID] = dfCleaned
            
    return dfs