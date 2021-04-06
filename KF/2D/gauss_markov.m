function [GM] = gauss_markov(t, sigma, beta, init)
%GAUSS_MARKOV Generates a 1st-order Gauss-Markov signal of size
%   Generates a 1st-order Gauss-Markov signal signal.
%   The signal is assumed to be correlated along rows
%   Each row represents an independent signal
%
%   INPUTS:
%   t: the time vector of the gauss-markov signal
%   sigma: the standard deviation the gauss-markov signal should have.
%          can be a double or an array
%   beta: the inverse of the correlation time
%   init: the initial value of the GM process
%
%   OUPUTS:
%   GM: the Gauss-markov signal(s)

% get the dimensions of the time vector
dim = size(t);
% get the time difference between steps
dt = t - circshift(t,[1,0]);
% first element is wrong, as it is compared with the last element
% it is however unused. set to zero just in case
dt(1) = 0;

sigma = sqrt(1 - exp(-2*beta * dt)) * sigma; % scale sigma
WN = sigma .* randn(dim); % Generate the white noise data
GM = zeros(dim); % initialize the array
GM(1) = init;

% generate the GM signal
for i = 2:dim(1)
    GM(i) = exp(-beta*dt(i))*GM(i-1) + WN(i);
end
    
end

