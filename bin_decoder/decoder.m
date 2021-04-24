% decoder.m Decodes binary files from the supersonic rocket into CSV
%
%   Author: Joshua Cayetano-Emond
%   Creation date: 2021-04-23
%
%
clear all; clc; close all;

%% Variables ==============================================================
% file directories
inDir = './inFiles';
outDir = './outFiles';
processedDir = './processedFiles';
errorDir = './errorFiles';

% headers
error_header = 'skippedBeat, drNoTrigger, missingData';
IMU_header = 'timestep (us), gyroX, gyroY, gyroZ, accelX, accelY, accelZ, temp';
RSC_header = 'timestep (us), gyroX, gyroY, gyroZ, accelX, accelY, accelZ, temp';
AIS_header = 'timestep (us), accelX, accelY';
temp_header = 'timestep (us), gyroX, gyroY, gyroZ, accelX, accelY, accelZ, temp';

%% Code ===================================================================

inFiles = dir(fullfile(inDir,'*.dat')); %gets all wav files in struct
for k = 1:length(inFiles)
  fileName = inFiles(k).name;
  fileNameNoExtension = fileName(1:end-4);
  fullFileName = fullfile(inDir, fileName);

  fprintf(1, 'Now reading %s\n', fileName);
  
  % create the output directory
  outputDir = fullfile(outDir, fileNameNoExtension);
  mkdir(outputDir);
  % create the output files
  IMUfile = fopen(fullfile(outputDir,'/IMU.csv'),'w');
  RSCfile = fopen(fullfile(outputDir,'/RSC.csv'),'w');
  AISfile = fopen(fullfile(outputDir,'/AIS.csv'),'w');
  Tempfile = fopen(fullfile(outputDir,'/Temp.csv'),'w');
  
  % write the headers to the output files
  fprintf(IMUfile, '%s, %s\n', error_header, IMU_header);
  fprintf(RSCfile, '%s, %s\n', error_header, RSC_header);
  fprintf(AISfile, '%s, %s\n', error_header, AIS_header);
  fprintf(Tempfile, '%s, %s\n', error_header, temp_header);
  
  % open the input file
  inFile = fopen(fullFileName,'r');
  
  while true
    packetType = fread(inFile, 1, 'uint8');
    packetLength = fread(inFile, 1, 'uint8');
    
    
    if (~isscalar(packetType) || ~isscalar(packetLength))
        fprintf('Reached EOF.\n');
        fclose('all');
        movefile(fullFileName, fullfile(processedDir, fileName));
        break;
    elseif (packetType == 1 && packetLength == 24) % IMU packet
    elseif (packetType == 2 && packetLength == 12) % AIS packet
        errorMessage = decodeErrorByte(fread(inFile, 1, 'uint8',1));
        timestep = fread(inFile, 1, 'uint32');
        accelX = fread(inFile, 1, 'uint16');
        accelY = fread(inFile, 1, 'uint16');
        fprintf(AISfile, '%s, %d, %i, %i\n', errorMessage, timestep, ...
            accelX, accelY);
    elseif (packetType == 3 && packetLength == 16) % RSC presure packet
    elseif (packetType == 4 && packetLength == 16) % RSC temp packet
    elseif (packetType == 5 && packetLength == 40) % temp packet
    else
        fprintf('ERROR. Could not determine packet type. Stopping early.\n');
        fclose('all');
        movefile(fullFileName, fullfile(errorDir, fileName));
        break;
    end
    
    
  end
end