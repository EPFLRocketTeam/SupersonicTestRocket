function [error_meas] = addINSerror(nom_meas, ...
    gBias, gWN, gGMsigma, gGMbeta, aBias, aWN)
%ADDINSERROR Adds the INS error to nominal measurements
%   Adds the INS error to nominal measurements on the gyro & accelerometers
%   Gyro errors:
%       - random bias
%       - random walk
%   Accelerometer errors (assumed to be identical):
%       - random bias
%       - random walk
%INPUTS
%   nom_meas: structure containing the following:
%       - time (m x 1): time vector of the measurements
%       - angular_rate (m x 1): nominal angular rate vector
%       - acceleration (m x 2): nominal acceleration vector
%   gBias: the gyro bias in [rad/s]
%   gWN: the gyro white noise amplitude in [rad/(s / sample)]
%   aBias: the accelerometer bias in [m/s^2]
%   aWN: the accelerometers white noise in [m/(s^2 / sample)]
%
%OUPUTS
%   error_meas: structure containing the following:
%       - time (m x 1): time vector of the measurements
%       - angular_rate (m x 1): angular rate vector with error
%       - acceleration (m x 2): acceleration vector with error

% gyro measurements with noise
error_meas.time = nom_meas.time;
error_meas.angular_rate = nom_meas.angular_rate + gBias ...
    + gWN * randn(size(nom_meas.time)) ...
    + gauss_markov(nom_meas.time, gGMsigma, gGMbeta, 0);
error_meas.acceleration = nom_meas.acceleration + aBias ...
    + aWN .* randn([size(nom_meas.time,1),2]);

end

