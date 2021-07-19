# -*- coding: utf-8 -*-
"""
Created on Mon Jul 19 23:21:28 2021

@author: newbi
"""

import os

import pandas as pd

# user-defined modules
from Packet import PacketType, unpackPackets, splitPacketsDataFrame

extension = ".dat"
rawLogDirName = "data/rawLog"
decodedDataDirName = "data/decodedData"
processedLogDirName = "data/processedLog"
errorLogDirName = "data/errorLog"
analyzedDataDirName = "data/analyzedData"

def createDirs(rawLogDirName_ = rawLogDirName,
               decodedDataDirName_ = decodedDataDirName,
               processedLogDirName_ = processedLogDirName,
               errorLogDirName_ = errorLogDirName,
               analyzedDataDirName_ = analyzedDataDirName):
    """ Makes sure the necessary directories exist
    

    Returns
    -------
    None.

    """
    os.makedirs(rawLogDirName_, exist_ok=True)
    os.makedirs(decodedDataDirName_, exist_ok=True)
    os.makedirs(processedLogDirName_, exist_ok=True)
    os.makedirs(errorLogDirName_, exist_ok=True)
    os.makedirs(analyzedDataDirName_, exist_ok=True)


def loadBinaryFile(rawLogDirName_ = rawLogDirName, extension_ = extension):
    """ Generator that returns binary content of files in dirname
        Goes through the files with the right ending and yields their binary
        content.
    

    Parameters
    ----------
    rawLogDirName_ : str, optional
        The directory to load files from. The default is rawLogDirName.
    extension_ : str, optional
        The ending of the files to consider. The default is extension.

    Returns
    -------
    None.

    """
    
    createDirs()
    
    for fileName in os.listdir(rawLogDirName_):
        if fileName.endswith(extension):
            file = open(os.path.join(rawLogDirName_, fileName), "rb")
            yield(fileName, file.read())
            file.close()

def processData(rawLogDirName_ = rawLogDirName,
                processedLogDirName_ = processedLogDirName,
                errorLogDirName_ = errorLogDirName,
                extension_ = extension):
    
    for fileName, file in loadBinaryFile(rawLogDirName_, extension_):
        print(f"Processing {fileName}")
        df = unpackPackets(file)
        yield(fileName, splitPacketsDataFrame(df))
        
def saveData(rawLogDirName_ = rawLogDirName,
             decodedDataDirName_ = decodedDataDirName,
             processedLogDirName_ = processedLogDirName,
             errorLogDirName_ = errorLogDirName,
             extension_ = extension):
    
    for fileName, dfs in processData(rawLogDirName_, processedLogDirName_,
                                     errorLogDirName_, extension_):
        for packetType in dfs.keys():
            for sensorID in dfs[packetType].keys():
                outFileName = (f"{PacketType._registry[packetType].filename}_"
                               f"{sensorID}.csv")
                dirName = os.path.join(decodedDataDirName_,
                                       os.path.splitext(fileName)[0])
                fullFileName = os.path.join(dirName, outFileName)
    
                os.makedirs(dirName, exist_ok=True)
                dfs[packetType][sensorID].to_csv(fullFileName)