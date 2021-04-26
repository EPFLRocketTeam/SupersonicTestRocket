function [decodedByte] = decodeErrorByte(errorByte)
% Decodes the error byte into a usable format for CSV


measSkippedBeat = bitget(errorByte, 8);
acqSkippedBeat = bitget(errorByte, 7);
drNoTrigger = bitget(errorByte, 6);
checksumError = bitget(errorByte, 5);


decodedByte = sprintf('%d, %d, %d, %d', measSkippedBeat, ...
    acqSkippedBeat, drNoTrigger, checksumError);

end

