function [measurements] = generateNominalIMU(time_e, heading_e, ...
    angular_rate_e, accel_e, time_b, IMU_angle)
%GENERATENOMINALIMU Generates nominal IMU measurements in the body frame
%   Given nominal values for the vehicle in the Earth frame, calculate the
%   corresponding IMU measurements in the body frame at the right frequency
%
%INPUTS
%   time_e (n x 1) : time vector of the nominal values in Earth frame
%   heading_e (n x 1): heading vector in Earth frame
%   angular_rate_e (n x 1): angular rate in Earth frame
%   accel_e (n x 2): acceleration vector in Earth frame
%   time_b (m x 1): time vector of IMU measurements in body frame
%   IMU_angle: angle of the IMU w.r.t body frame
%
%OUTPUTS
%   measurements: structure containing the following:
%       - time (m x 1): time vector of the measurements
%       - angular_rate (m x 1): angular rate vector
%       - acceleration (m x 2): acceleration vector

measurements.time = time_b;

% interpolate values to obtain the measurements
heading = interp1(time_e, heading_e, time_b);
measurements.angular_rate = interp1(time_e, angular_rate_e, time_b);
measurements.acceleration = interp1(time_e, accel_e, time_b);

% rotate the accelerations
measurements.acceleration = rotate2D(measurements.acceleration, ...
    heading + IMU_angle);

end

