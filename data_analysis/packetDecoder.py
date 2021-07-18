#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Created on Sun Jul 18 18:33:18 2021

@author: newbi
"""

import struct
import numpy as np
import pandas as pd

# Define variables
headerFormat = '<BBBBL'
packetFormats = ['',            # packet 0 - no packet, indicates EOF
                 '<hhhhhhhh',   # packet 1 - ADIS16470 archived packet
                 '<hh',         # packet 2 - AISx120SX archived packet
                 '<f',          # packet 3 - RSC pressure packet
                 '<f',          # packet 4 - RSC temperature packet
                 '<hh',         # packet 5 - MAX archived packets
                 '',            # packet 6 - Altimax packet
                 '<fffffff',    # packet 7 - ADIS16470 packet
                 '<ff',         # packet 8 - AISx120SX packet
                 '<ff']         # packet 9 - MAX packet

# 
  
def unpackPacket(packetBuffer):
    """ UNPACKPACKET Takes a C structure packet and unpacks it into a tuple
            Only reads a single packet and stops afterwards
    

    Parameters
    ----------
    packetBuffer : Bytes
        Buffer containing the packet to unpack.

    Returns
    -------
    packetType : int
        Numerical value corresponding to the packet type
    packetSize : int
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
    header = struct.unpack(headerFormat, 
                           packetBuffer[0:struct.calcsize(headerFormat)])
    packetType = header[0]
    packetSize = header[1]
    
    if (packetType == 0 and packetSize ==0):
        raise Warning("An empty packet was sent.")
    elif (packetSize == struct.calcsize(packetFormats[packetType])
        + struct.calcsize(headerFormat)):
        sensorID = header[2]
        errors = np.unpackbits(np.array([header[3]], dtype=np.uint8))
        timestamp = header[4]
        unpackedPacket = struct.unpack(
            packetFormats[packetType],
            packetBuffer[struct.calcsize(headerFormat):
                         struct.calcsize(headerFormat)+
                         struct.calcsize(packetFormats[packetType])])
            
        return (packetType, packetSize, sensorID,
                errors, timestamp, unpackedPacket)
    else:
        raise ValueError("The packet size didn't match the packet type. "
                         "Perhaps an invalid packet was sent.")
        
def unpackHeader(packetBuffer):
    """ UNPACKPACKET Takes a C structure packet and unpacks the header
            Only reads a single packet and stops afterwards
    
    
    Parameters
    ----------
    packetBuffer : Bytes
        Buffer containing the packet to unpack.
    
    Returns
    -------
    packetType : int
        Numerical value corresponding to the packet type
    packetSize : int
        Size of the packet in bytes
    sensorID : int
        ID of the sensor
    errors : numpy array
        Array containing the boolean flags for errors for the packet
    timestamp: int
        Current timestamp of the packet in microseconds
    
    """
    header = struct.unpack(headerFormat, 
                           packetBuffer[0:struct.calcsize(headerFormat)])
    packetType = header[0]
    packetSize = header[1]
    
    if (packetType == 0 and packetSize ==0):
        raise Warning("An empty packet was sent.")
    elif (packetSize == struct.calcsize(packetFormats[packetType])
        + struct.calcsize(headerFormat)):
        sensorID = header[2]
        errors = np.unpackbits(np.array([header[3]], dtype=np.uint8))
        timestamp = header[4]
            
        return packetType, packetSize, sensorID, errors, timestamp
    else:
        raise ValueError("The packet size didn't match the packet type. "
                         "Perhaps an invalid packet was sent.")
        
def unpackPacketContent(packetBuffer, packetType):
    unpackedPacket = struct.unpack(
        packetFormats[packetType],
        packetBuffer[struct.calcsize(headerFormat):
                     struct.calcsize(headerFormat)+
                     struct.calcsize(packetFormats[packetType])])
        
    return unpackedPacket
    

def unpackPackets(packetBuffer):
    cursor = 0
    
    # IMU_df = pd.DataFrame(columns=["timestamp",
    #                                "errors",
    #                                "gyroX",
    #                                "gyroY",
    #                                "gyroZ",
    #                                "accX",
    #                                "accY",
    #                                "accZ",
    #                                "temp"])
    # AIS_df = pd.DataFrame(columns=["timestamp",
    #                                "errors",
    #                                "accX",
    #                                "accY"])
    # RSC_pressure_df = pd.DataFrame(columns=["timestamp",
    #                                         "errors",
    #                                         "pressure"])
    # RSC_temp_df = pd.DataFrame(columns=["timestamp",
    #                                     "errors",
    #                                     "temp"])
    # MAX_df = pd.DataFrame(columns=["timestamp",
    #                                "errors",
    #                                "probeTemp",
    #                                "ambientTemp"])
    
    # dfs = {1:IMU_df,
    #        2:AIS_df,
    #        3:RSC_pressure_df,
    #        4:RSC_temp_df,
    #        5:MAX_df}
    
    rowEntries = []
    
    while (cursor < len(packetBuffer)):
        packetType, packetSize, sensorID, errors, timestamp =\
            unpackHeader(packetBuffer[cursor:
                                      cursor+struct.calcsize(headerFormat)])
        unpackedPacket = pd.Series(unpackPacketContent(
            packetBuffer[cursor:cursor+packetSize], packetType))
        
        # rowEntries.append([packetType,
        #                  sensorID,
        #                  errors,
        #                  timestamp,
        #                  unpackedPacket])
        
        cursor += packetSize
        
    df = pd.DataFrame(rowEntries,columns=["packetType",
                                          "sensorID",
                                          "errors",
                                          "timestamp",
                                          "data"])
    # IMU_packets = a1[a1['packetType'] == 1]
    # new_IMU = pd.concat([IMU_packets.loc[:,IMU_packets.columns !='data'], IMU_packets['data'].apply(pd.Series)], axis = 1).rename(columns = {0: 'gyroX', 1: 'gyroY', 2: 'gyroZ',3:'accX',4:'accY',5:'accZ',6:'temp',7:'padding'})
    return df
    