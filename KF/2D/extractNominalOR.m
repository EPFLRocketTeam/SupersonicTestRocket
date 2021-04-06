function [state] = extractNominalOR(fileName)
%EXTRACTNOMINALOR Gets the nominal state from OpenRocket
%   Takes raw data from an OpenRocket file and gets the 
%   nominal state 
%
%INPUTS
%   fileName: path to the file to extract measurements from
%
%OUTPUTS
%   state (structure): state of the vehicle in the Earth frame, contains:
%       - time (n x 1): time vector from the data
%       - heading (n x 1): heading of the vehicle off the vertical [rad]
%       - angular_rate (n x 1): angular velocity. positive is away from
%       - position (n x 2): x and z components of the position [m]
%       - velocity (n x 2): x and z components of the velocity [m/s]
%       - acceleration (n x 2): x and z components of acceleration [m/s^2]
%       - pressure (n x 1): static pressure [Pa]


data = readmatrix(fileName, 'CommentStyle','#'); % load the data

% set the columns in which data is located. change this if the exported
% file format changes
timeCol = 1; % time
headingCol = 47; % Vertical orientation (zenith)
angularRateCol = 18; % pitch rate
xCol = 7; % position east of launch
zCol = 2; % altitude
vxCol = 11; % lateral velocity
vzCol = 3; % vertical velocity
axCol = 12; % lateral acceleration
azCol = 4; % vertical acceleration
pressureCol = 51; % air pressure

% clip the data after the recovery has deployed
data = data(~isnan(data(:,angularRateCol)),:);

% assign the raw data to the state variable
state.time = data(:,timeCol);
state.heading = pi/2 - deg2rad(data(:,headingCol));
state.angular_rate = 2 * pi * data(:,angularRateCol);
state.position = data(:,[xCol, zCol]);
state.velocity = data(:,[vxCol, vzCol]);
state.acceleration = data(:,[axCol, azCol]);
state.pressure = data(:,pressureCol);

% to fix the signs in the pitch rate
state.angular_rate(2:end) = abs(state.angular_rate(2:end)) ...
    .* sign(state.heading(2:end) - state.heading(1:end-1));

% to add the sign to the lateral acceleration
state.acceleration(2:end,1) = abs(state.acceleration(2:end,1)) ...
    .* sign(state.velocity(2:end,1) - state.velocity(1:end-1,1));
