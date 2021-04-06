function [F, G, Q, phi] = computeKalmanMatrices(alpha, f, dt, W, ...
    betaG, betaA)
%COMPUTEKALMANMATRICES Find the necessary matrices for the KF
%   Finds the matrices F, G, Q and Phi for Kalman Filter calculations
%   given the current attitude, force measurements and noise matrix.
%
%INPUTS
%   alpha: Current heading
%   f (2 x 1): Accelerometer measurements
%   dt: time step
%   W (D x D): Noise matrix
%   betaG: inverse of the correlation time for the gyro
%   betaA: inverse of the correlation time for the accelerometers
%       (assumed to have identical correlation times)
%
%OUTPUTS
%   F (N x N): state matrix
%   G (N x D): noise shaping matrix
%   Q (N x N): System noise matrix
%   phi (N x N): State transition matrix

% get the force in the mapping (inertial) frame

fm = [[cos(alpha), -sin(alpha)];[sin(alpha),cos(alpha)]] * f;

% form the F matrix. Note that here it is specific for this scenario and
% not a general case. it should be redefined for a different scenario
F = [[0,    0,      0,      0,      0,      1,      1,      0,      0];
     [-fm(2),0,      0,      0,      0,      0,  0,cos(alpha), -sin(alpha)];
     [fm(1), 0,      0,      0,      0,      0,  0,sin(alpha),  cos(alpha)];
     [0,    1,      0,      0,      0,      0,      0,      0,      0];
     [0,    0,      1,      0,      0,      0,      0,      0,      0];
     [0,    0,      0,      0,      0,      0,      0,      0,      0];
     [0,    0,      0,      0,      0,      0,      -betaG, 0,      0];
     [0,    0,      0,      0,      0,      0,      0,      -betaA, 0];
     [0,    0,      0,      0,      0,      0,      0,      0,      -betaA]];
% form the G matrix. Also specific for this scenario
G = [[1,    0,      0,      0,      0,      0];
     [0, cos(alpha),-sin(alpha), 0, 0,      0];
     [0, sin(alpha),cos(alpha),  0, 0,      0];
     [0,    0,      0,      0,      0,      0];
     [0,    0,      0,      0,      0,      0];
     [0,    0,      0,      0,      0,      0];
     [0,    0,      0,      1,      0,      0];
     [0,    0,      0,      0,      1,      0];
     [0,    0,      0,      0,      0,      1]];

dim = size(F,1);

% form the auxiliary matrices
A = [[-F, G*W*G'];[zeros(size(F)), F']] * dt;
B = expm(A);

% get phi and Q
B12 = B(1:dim,dim+1:2*dim);
B22 = B(dim+1:2*dim,dim+1:2*dim);
phi = B22';
Q = phi * B12;


end

