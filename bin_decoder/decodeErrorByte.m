function [decodedByte] = decodeErrorByte(errorByte)
% Decodes the error byte into a usable format for CSV


skippedBeat = bitget(errorByte, 8);
drNoTrigger = bitget(errorByte, 7);
missingData = bitget(errorByte, 6);
checksumError = bitget(errorByte, 5);


decodedByte = sprintf('%d, %d, %d, %d', skippedBeat, ...
    drNoTrigger, missingData, checksumError);

end

