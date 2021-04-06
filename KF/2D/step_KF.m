function [new_x_hat, new_P] = step_KF(phi, Q, H, old_x_hat, old_P, Z, R)
%KF Perform a step of a Kalman filter iteration
%   Perform a single step of a Kalman filter iteration
%   on the measurements using the necessary matrices
%
%INPUTS
%   phi (N x N): state transition matrix
%   Q (N x N): noise shaping matrix
%   H (M x N): design matrix
%   old_x_hat (N x 1): old state
%   old_P (N x N): old covariance matrix
%   Z (M x 1): measurements matrix
%   R (M x M): measurements noise matrix
%
%OUPUTS
%   new_x_hat (N x 1): updated state calculated by KF
%   new_P (N x N): covariance matrix calculated by KF

x_tilda = phi * old_x_hat;
P_tilda = phi * old_P * phi' + Q;

% check if there any measurements for the timestep
if all(isnan(Z),'all')
    % if there aren't, use the prediction
    new_x_hat = x_tilda;
    new_P = P_tilda;
else
    % if there are, perform KF update
    K = P_tilda * H'  * inv(H * P_tilda * H' + R);
    new_x_hat = x_tilda + K * (Z - H * x_tilda);
    new_P = (eye(size(old_P)) - K*H) * P_tilda;
end

end

