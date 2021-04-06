function [z, v, a, alpha, omega, f] = generateNominal(r0, psi0, omega0, ang_acc0, t)
%GENERATENOMINAL Generates nominal positions and measurements
%   Generates nominal positions, velocities and accelerations as well as
%   gyro and accelerometer readings for a vehicle rotating around
%   the origin given initial conditions and a timestep
%
%INPUTS
%   r0: initial radius of the circle
%   psi0: initial angular position of the vehicle. 0 = on North axis, 
%       90 = on East axis
%   omega0: initial angular velocity of the vehicle
%   ang_acc0: initial angular acceleration of the vehicle.
%       here it is assumed constant
%   t (1 x n): time vector
%
%OUTPUTS
%   z (2 x n): cartesian coordinates of the vehicle
%   v (2 x n): cartesian velocity of the vehicle
%   a (2 x n): cartesian acceleration of the vehicle
%   alpha (1 x n): azimuth of the vehicle.
%       0 = pointing North, 90 = point East
%   omega (1 x n): angular velocity in vehicle frame
%   f (2 x n): force in vehicle frame


% get the values in polar coordinates
ang_acc = repmat(ang_acc0,size(t));
omega = omega0 + t .* ang_acc;
psi = psi0 + t .* omega;
alpha = psi + pi/2;

% accelerometer readings
f = [r0 * ang_acc; r0 * omega.^2];

% bring everything to cartesian coordinates
z = r0 * [cos(psi);sin(psi)];
v = r0 * omega .* [cos(psi+pi/2);sin(psi+pi/2)];
a = f(1,:) .* [cos(psi+pi);sin(psi+pi)] ...
    + f(2,:) .* [cos(psi+pi);sin(psi+pi)];
end