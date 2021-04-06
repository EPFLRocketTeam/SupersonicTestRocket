function [new_state] = step_INS_ODE(old_state, measurements, dt, IMU_angle)
%STEP_INS_ODE Perform a step of the INS ODE
%   Performs one step of the strapdown navigation using the current and
%   last steps measurements as well as the known old position.
%   Uses a 2nd-order integration schema
%
%INPUTS:
%   old_state (1 x 5): old state of the vehicle
%       contains the attitude, position (x&z) and velocity (x&z)
%   measurements (2 x 3): gyro and accelerometer measurements from the
%       current and last timesteps
%   dt : the time difference between the two measurements
%
%OUTPUTS:
%   new_state (5 x 1): updated state of the vehicle

% separate the variables into their components
old_alpha = old_state(1);
old_p = old_state(2:3);
old_v = old_state(4:5);
old_gyro = measurements(1,1);
old_accel = measurements(1,2:3);
new_gyro = measurements(2,1);
new_accel = measurements(2,2:3);

% get the old rotation matrix
old_R = [[cos(old_alpha + IMU_angle), -sin(old_alpha + IMU_angle)]; ...
         [sin(old_alpha + IMU_angle), cos(old_alpha + IMU_angle)]];

% calculate the new attitude
new_alpha = old_alpha + 1/2 * (new_gyro + old_gyro) * dt;
% get the new rotation matrix
new_R = [[cos(new_alpha + IMU_angle), -sin(new_alpha + IMU_angle)]; ...
         [sin(new_alpha + IMU_angle), cos(new_alpha + IMU_angle)]];
new_v = old_v + 1/2 * (old_accel * old_R + new_accel * new_R) * dt;
new_p = old_p + 1/2 * (old_v + new_v) * dt;

new_state = [new_alpha, new_p, new_v];

end

