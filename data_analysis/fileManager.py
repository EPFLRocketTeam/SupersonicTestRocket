# -*- coding: utf-8 -*-
""" Contains modules to manage the binary log files.
    These functions do everything from reading the files to saving the
    converted data.
Created on Mon Jul 19 23:21:28 2021

@author: newbi
"""

import os

# user-defined modules
from Packet import PacketType, unpackMultiplePackets, splitPacketsDataFrame

extension = ".dat"
rawLogDirName = "data/rawLog"
decodedDataDirName = "data/decodedData"
decodedDataErrorDirName = "data/decodedDataError"
analyzedDataDirName = "data/analyzedData"

def createDirs(rawLogDirName_ = rawLogDirName,
               decodedDataDirName_ = decodedDataDirName,
               decodedDataErrorDirName_ = decodedDataErrorDirName,
               analyzedDataDirName_ = analyzedDataDirName):
    """ Makes sure the required directories exist.
    

    Parameters
    ----------
    rawLogDirName_ : str, optional
        Directory path for raw data logs. The default is rawLogDirName.
    decodedDataDirName_ : str, optional
        Directory path for data decoded from logs.
        The default is decodedDataDirName.
    decodedDataErrorDirName_ : str, optional
        Directory path for logs processed which had errors.
        The default is decodedDataErrorDirName.
    analyzedDataDirName_ : str, optional
        Directory path for analyzed data. The default is analyzedDataDirName.

    Returns
    -------
    None.

    """
    
    os.makedirs(rawLogDirName_, exist_ok=True)
    os.makedirs(decodedDataDirName_, exist_ok=True)
    os.makedirs(decodedDataErrorDirName_, exist_ok=True)
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
        The extension of the files to consider. The default is extension.

    Returns
    -------
    fileName : str
        Name of the file, without the directory in which it is contained.
    file : BufferedReader
        Handle to the file itself.
    fileContent : bytes
        Binary contents of the file.

    """
    
    createDirs(rawLogDirName_)
    
    for fileName in os.listdir(rawLogDirName_):
        if fileName.endswith(extension):
            print(f"Loading {fileName}")
            file = open(os.path.join(rawLogDirName_, fileName), "rb")
            fileContent = file.read()
            yield(fileName, file, fileContent)
            file.close()

def processData(rawLogDirName_ = rawLogDirName,
                extension_ = extension):
    """ Processes the data from the logs into a pandas dataframe
    

    Parameters
    ----------
    rawLogDirName_ : str, optional
        Directory path for raw data logs. The default is rawLogDirName.
        The default is decodedDataErrorDirName.
    extension_ : str, optional
        The extension of the files to consider. The default is extension.

    Returns
    -------
    fileName : str
        Name of the file, without the directory in which it is contained.
    file : BufferedReader
        Handle to the file itself.
    splitDataFrame : 2D Dictionary containing pandas DataFrames
        Data in the file returned as a 2D dictionary of DataFrames.
    errorFlag : boolean
        Whether an error occured or not.
        
    """
    
    createDirs(rawLogDirName_)
    
    for fileName, file, fileContent in loadBinaryFile(rawLogDirName_,
                                                      extension_):
        print(f"Processing {fileName}")
        (df, errorFlag) = unpackMultiplePackets(fileContent)
        splitDataFrame = splitPacketsDataFrame(df)
        yield(fileName, file, splitDataFrame, errorFlag)
        
def processAndSaveData(rawLogDirName_ = rawLogDirName,
                       decodedDataDirName_ = decodedDataDirName,
                       decodedDataErrorDirName_ = decodedDataErrorDirName,
                       extension_ = extension):
    """ Processes binary files in a directory and saves CSVs accordingly
    

    Parameters
    ----------
    rawLogDirName_ : str, optional
        Directory path for raw data logs. The default is rawLogDirName.
    decodedDataDirName_ : str, optional
        Directory path for data decoded from logs.
        The default is decodedDataDirName.
    decodedDataErrorDirName_ : str, optional
        Directory path for logs processed which had errors.
        The default is decodedDataErrorDirName.
    extension_ : str, optional
        The extension of the files to consider. The default is extension.

    Returns
    -------
    None.

    """
    
    createDirs(rawLogDirName,
               decodedDataDirName,
               decodedDataErrorDirName,
               analyzedDataDirName)
    
    for fileName, file, dfs, errorFlag in processData(rawLogDirName_,
                                                      extension_):
        # figure out where to save the files
        if errorFlag:
            outDirName = os.path.join(decodedDataErrorDirName_,
                                   os.path.splitext(fileName)[0])
        else:
            outDirName = os.path.join(decodedDataDirName_,
                                   os.path.splitext(fileName)[0])
        # go through each sensor
        for packetType in dfs.keys():
            for sensorID in dfs[packetType].keys():
                print(f"    Saving {packetType=}, {sensorID=} in a CSV.")
                outFileName = (f"{PacketType._registry[packetType].filename}_"
                               f"{sensorID}.csv")
                fullFileName = os.path.join(outDirName, outFileName)
    
                os.makedirs(outDirName, exist_ok=True)
                dfs[packetType][sensorID].to_csv(fullFileName, index = False)
                
        file.close()
        os.rename(os.path.join(rawLogDirName_, fileName),
                  os.path.join(outDirName, fileName))
        
        
if __name__ == "__main__":
    processAndSaveData()