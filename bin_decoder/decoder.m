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

% sensor sensitivities
IMU_gyroSensitivity = 1/10; % [deg/LSB]
IMU_accelSensitivity = 1/800; % [g/LSB]
IMU_tempSensitivity = 0.1; % [degC/LSB]
AIS_sensitivity = 1 / (68 * 4); % [g/LSB]
MAX_probeSensitivity = 0.25 / 4; % [degC/LSB]
MAX_ambientSensitivity = 0.0625 / 16; % [degC/LSB]

% headers
error_header = 'measSkippedBeat, skippedBeat, drNoTrigger, checksumError';
IMU_header = 'timestep (us), gyroX (deg/s), gyroY (deg/s), gyroZ (deg/s), accelX (g), accelY (g), accelZ (g), temp (degC)';
RSC_pressureHeader = 'timestep (us), pressure (psi)';
RSC_temperatureHeader = 'timestep (us), temperature (degC)';
AIS_header = 'timestep (us), accelX (g), accelY (g)';
MAX_header = 'timestep (us), probe temperature (degC), ambient temperature (degC)';

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
  
  % create the output files and write the headers
  IMUfile = fopen(fullfile(outputDir,'/IMU.csv'),'w');
  fprintf(IMUfile, '%s, %s\n', error_header, IMU_header);
  AISfile = fopen(fullfile(outputDir,'/AIS.csv'),'w');
  fprintf(AISfile, '%s, %s\n', error_header, AIS_header);
  
  
  % open the RSC pressure files
  for i = 1:2
    RSC_pressureFile(i) = fopen(fullfile(outputDir, ...
        sprintf('/RSC_pressure%i.csv',i)),'w');
    fprintf(RSC_pressureFile(i), '%s, %s\n', error_header, ...
        RSC_pressureHeader);
  end
  
  % open the RSC temperature files
  for i = 1:2
    RSC_temperatureFile(i) = fopen(fullfile(outputDir, ...
        sprintf('/RSC_temp%i.csv',i)),'w');
    fprintf(RSC_temperatureFile(i), '%s, %s\n', error_header, ...
        RSC_temperatureHeader);
  end
  
  % open the Tempfiles
  for i = 1:4
    MAX_file(i) = fopen(fullfile(outputDir, ...
        sprintf('/Temp%i.csv',i)),'w');
    fprintf(MAX_file(i), '%s, %s\n', error_header, MAX_header);
  end
  
  % write the headers to the output files
  
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
        sensorId = fread(inFile, 1, 'uint8');
        errorMessage = decodeErrorByte(fread(inFile, 1, 'uint8'));
        timestep = fread(inFile, 1, 'uint32');
        gyroX = fread(inFile, 1, 'int16') * IMU_gyroSensitivity; 
        gyroY = fread(inFile, 1, 'int16') * IMU_gyroSensitivity;
        gyroZ = fread(inFile, 1, 'int16') * IMU_gyroSensitivity;
        accelX = fread(inFile, 1, 'int16') * IMU_accelSensitivity;
        accelY = fread(inFile, 1, 'int16') * IMU_accelSensitivity;
        accelZ = fread(inFile, 1, 'int16') * IMU_accelSensitivity;
        temp = fread(inFile, 1, 'int16', 2) * IMU_tempSensitivity;
        fprintf(IMUfile, '%s, %d, %i, %i, %i, %i, %i, %i, %i\n', ...
            errorMessage, timestep, gyroX, gyroY, gyroZ, accelX, ...
            accelY, accelZ, temp);
    elseif (packetType == 2 && packetLength == 12) % AIS packet
        sensorId = fread(inFile, 1, 'uint8');
        errorMessage = decodeErrorByte(fread(inFile, 1, 'uint8'));
        timestep = fread(inFile, 1, 'uint32');
        accelX = fread(inFile, 1, 'uint16') * AIS_sensitivity;
        accelY = fread(inFile, 1, 'uint16') * AIS_sensitivity;
        fprintf(AISfile, '%s, %d, %i, %i\n', errorMessage, timestep, ...
            accelX, accelY);
    elseif (packetType == 3 && packetLength == 12) % RSC presure packet
        sensorId = fread(inFile, 1, 'uint8');
        errorMessage = decodeErrorByte(fread(inFile, 1, 'uint8'));
        timestep = fread(inFile, 1, 'uint32');
        pressure = fread(inFile, 1, 'float32');
        fprintf(RSC_pressureFile(sensorId + 1), '%s, %d, %i\n', ...
            errorMessage, timestep, pressure);
    elseif (packetType == 4 && packetLength == 12) % RSC temp packet
        sensorId = fread(inFile, 1, 'uint8');
        errorMessage = decodeErrorByte(fread(inFile, 1, 'uint8'));
        timestep = fread(inFile, 1, 'uint32');
        temperature = fread(inFile, 1, 'float32');
        fprintf(RSC_temperatureFile(sensorId + 1), '%s, %d, %i\n', ...
            errorMessage, timestep, temperature);
    elseif (packetType == 5 && packetLength == 12) % MAX packet
        sensorId = fread(inFile, 1, 'uint8');
        errorMessage = decodeErrorByte(fread(inFile, 1, 'uint8'));
        timestep = fread(inFile, 1, 'uint32');
        rawProbeT = fread(inFile, 1, 'int16') * MAX_probeSensitivity;
        rawAmbientT = fread(inFile, 1, 'int16') * MAX_ambientSensitivity;
        fprintf(MAX_file(sensorId + 1), '%s, %d, %i, %i\n', ...
            errorMessage, timestep, rawProbeT, rawAmbientT);
    else
        fprintf('ERROR. Could not determine packet type. Stopping early.\n');
        fprintf('This was likely caused by logging stopped prematurely (i.e power loss or SD card disconnected).\n');
        fclose('all');
        movefile(fullFileName, fullfile(errorDir, fileName));
        break;
    end
    
    
  end
end