function [hat_error, meas_error, std_hat, ...
    std_meas, innovation, quality] = ...
    statistics(x_true, x_tilda, x_hat, z_unfiltered, H, P, meas_idx)
%STATISTICS Generates the relevant statistics
%   Generates statistics on the standard deviation, innovation and
%   predicted quality on the original signal and filtered signal
%
%INPUTS
%   x_true (N x n_samples): the true values of the state
%   x_tilda (N x n_samples): the predicted values of the state
%   x_hat (N x n_samples): the filtered values of the state
%   z_unfiltered (M x n_samples): the unfiltered values of the measurements
%   H (M x N): the design matrix
%   P (N x N x n_samples): the covariance matrix of the state
%   meas_idx (1 x n_samples): boolean array of elements in x_true
%       that have corresponding measurements
%
%OUTPUTS
%   hat_error (N x n_samples): absolute error of filtered state
%   meas_error (M x n_samples): absolute error of the measurements
%   std_hat (N x 1): standard deviation of the filtered state
%   std_meas (M x 1): standard deviation of the measurements
%   innovation (M x n_samples): innovation sequence of the Kalman filter
%   quality (N x n_samples): predicted quality of the state

% get the errors on the state and measurements
hat_error = abs(x_true - x_hat);
meas_error = abs(H * x_true(:,meas_idx) - z_unfiltered(:,meas_idx));

% get the standard deviations on the state and measurements
std_hat = std(hat_error,[],2);
std_meas = std(meas_error,[],2);

% get innovation
innovation = z_unfiltered(:,meas_idx) - H * x_tilda(:,meas_idx);

% get the quality
quality = zeros(size(x_true));
for i = 1:length(x_true)
    quality(:,i) = diag(P(:,:,i));
end

end

