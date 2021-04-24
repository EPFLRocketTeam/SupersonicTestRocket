function [decodedByte] = decodeErrorByte(errorByte)
% Decodes the error byte into a usable format for CSV


skippedBeat = bitget(errorByte, 8);
drNoTrigger = bitget(errorByte, 7);
missingData = bitget(errorByte, 6);


decodedByte = sprintf('%d, %d, %d', skippedBeat, drNoTrigger, missingData);

end

