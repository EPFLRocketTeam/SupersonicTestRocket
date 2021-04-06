function [zMeas] = addGPSerror(zNom, meas_idx, GPS_uncertainty)
%ADDGPSERROR Adds the GPS error to nominal measurements
%   Adds the GPS error to nominal measurements according to the position
%   uncertainty. Assumes both directions have the same uncertainty.
%   
%INPUTS
%   zNom (2 x n): nominal measurements of the position
%   meas_idx (1 x n) boolean: indices that should get a GPS measurement
%       indices that are not true will receive a measurement of NaN
%   GPS_uncertainty: uncertainty in the GPS's position
%
%OUPUTS
%   zMeas (2 x n): GPS measurements with noise

% get necessary sizes
total_size = size(meas_idx,2);
GPS_size = sum(meas_idx);

% create an empty position vector
zMeas = NaN(2,total_size);

% generate the measurements
GPS_error = GPS_uncertainty * randn(2,GPS_size);
zMeas(:,meas_idx) = zNom(:,meas_idx) + GPS_error;

end

