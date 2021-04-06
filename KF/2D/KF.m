% KF Script for post-process Kalman filter on simulated 2D data
%% HEADER
% KF Script
% Author: Joshua Cayetano-Emond
% newbie856@gmail.com
% Date: 2021-02-13


%% General settings =======================================================
clear all; close all; clc;

seed = 42; % seed to assure repeatability
rng(seed); 

set(groot,'DefaultAxesFontSize',14)
set(groot,'DefaultLineLineWidth',1.5)
set(groot,'defaultfigureposition',[400 100 1000 400])


%% Variables ==============================================================
% parameters to change for different simulations
error_loop = true; % whether to close the error loop or not
plot_title = 'Basic';

%% INS values
% provided noise values
gyro_bias     = -400;                               % [deg/h]
gyro_WN =       0.10;                               % [deg/sqrt(h)]
gyro_GM_sigma = 0.01;                               % [deg/(s / sqrt(Hz))]
gyro_GM_tau =   30;                                 % [s]
gyro_GM_init =  0;                                  % [deg/s]
acc_WN =        50;                                 % [ug/sqrt(Hz)]
acc_GM_sigma =  200;                                % [ug/sqrt(Hz)]
acc_GM_tau =    60;                                 % [s]
acc_GM1_init = -100;                                % [ug]
acc_GM2_init = 200;                                 % [ug]

% convert to desired units
gyro_bias = deg2rad(gyro_bias) / 3600;              % [rad/s]
gyro_WN = (deg2rad(gyro_WN) / 60)*sqrt(f);          % [rad/(s / sample)]
gyro_GM_sigma = deg2rad(gyro_GM_sigma);             % [rad/(s / sqrt(Hz))]
acc_WN = acc_WN / 1e6 * g * sqrt(f);                % [m/(s^2 / sample)]
acc_GM_sigma = acc_GM_sigma / 1e6 * g;              % [m/(s^2 / sqrt(Hz))]
acc_GM1_init = acc_GM1_init / 1e6 * g;              % [ug]
acc_GM2_init = acc_GM2_init / 1e6 * g;              % [ug]

% initialization error
alpha0_error = deg2rad(0);                          % [rad]
vx0_error = 0;                                      % [m/s]
vz0_error = 0;                                      % [m/s]

%% GPS values
fGPS = 0.5;                                         % [Hz]
nGPS = round(revNum* T * fGPS) + 1; % GPS timesteps   [-]
tGPS = [0:nGPS-1] / fGPS; % GPS time vector           [s]

if tunnel
    % remove GPS readings if there is a tunnel
    tunnel_start = 90;                              % [deg]
    tunnel_end = 135;                              	% [deg]
    tunnel_indices = mod(tGPS,T) >= tunnel_start/360 * T ...
        & mod(tGPS,T) <= tunnel_end/360 * T;
    tGPS = tGPS(~tunnel_indices); % reassign GPS time vector
end

meas_idx = ismember(t,tGPS); % indices in KF dataset with measurements
sigma_GPS = 1;                                      % [m]

%% Kalman values
% provided initial uncertainties
da0_unc = deg2rad(2);                               % [rad]
dvn0_unc = 5;                                       % [m/s]
dve0_unc = 5;                                       % [m/s]
dpn0_unc = 10;                                      % [m]
dpe0_unc = 10;                                      % [m]
gyro_bias_KF0_unc = deg2rad(0.05);                  % [rad/s]
gyro_GM_KF0_unc = deg2rad(0.01);                    % [rad/s]
acc_GM1_KF0_unc = 300;                              % [ug]
acc_GM2_KF0_unc = 300;                              % [ug]

% convert to desired units
acc_GM1_KF0_unc = acc_GM1_KF0_unc / 1e6 * g;        % [m/s^2]
acc_GM2_KF0_unc = acc_GM2_KF0_unc / 1e6 * g;        % [m/s^2]

% design matrix
H = [[0,    0,      0,      1,      0,      0,      0,      0,      0];
     [0,    0,      0,      0,      1,      0,      0,      0,      0]];
% noise matrix
W = diag([gyro_WN^2, acc_WN^2, acc_WN^2, 2*gyro_GM_sigma^2 / gyro_GM_tau,...
    2*acc_GM_sigma^2 / acc_GM_tau, 2*acc_GM_sigma^2 / acc_GM_tau]);


%% SIM BOX ================================================================
% get nominal positions
[z_true, v_true, a_true, alpha_true, omega, f] = ...
    generateNominal(r0, psi0, w0, ang_acc0, t);
X_TRUE = [alpha_true; v_true; z_true];

% simulate real measurements
[omegaMeas,fMeas] = addINSerror(omega, f, t, ...
    gyro_bias, gyro_WN, gyro_GM_sigma, 1/gyro_GM_tau, gyro_GM_init, ...
    acc_WN, acc_GM_sigma, 1/acc_GM_tau, [acc_GM1_init;acc_GM2_init]);

% add GPS noise
z_GPS = addGPSerror(z_true, meas_idx, sigma_GPS);
R = diag([sigma_GPS, sigma_GPS]);


%% INITIALIZE =============================================================
nT = length(time);
%% INS
% initialize matrices
X_INS_tilda = zeros(nT,5);
X_INS_hat = zeros(nT,5);

% initial conditions
X_INS_tilda(1,:) = state(1,1:5) + [alpha0_error, 0, 0, vx0_error, vz0_error];
X_INS_hat(1,:) = X_INS_tilda(1,:);

%% Kalman
% initial states
pk = zeros(9,9,nT);
pk(:,:,1) = diag([da0_unc, dvn0_unc, dve0_unc, dpn0_unc, dpe0_unc, ...
    gyro_bias_KF0_unc, gyro_GM_KF0_unc, acc_GM1_KF0_unc, acc_GM2_KF0_unc]); 

% error vector
error = zeros(3,nT);

dx_hat1 = zeros(5,nT);

%%
fileName = './data/i600_simulation.csv';
[time, state, measurements] = extractNominalOR(fileName);
for i = 2:nT
    dt = time(i)-time(i-1);
    meas = [state(i-1:i,1),measurements(i-1:i,2:3)];
    X_INS_tilda(i,:) = step_INS_ODE(X_INS_tilda(i-1,:), meas, dt);
end

% trajectory
figure
set(gcf,'Position',[100 100 650 650])
axis equal
%axis([-550 550 -550 550])
hold all
plot(state(:,2),state(:,3)) % nominal trajectory
plot(X_INS_tilda(:,2),X_INS_tilda(:,3))
plot(X_INS_hat(5,:),X_INS_hat(4,:))
title(['Trajectory of the rocket: '])
xlabel('East-position of the vehicle, x_2^l [m]')
ylabel('North-position of the vehicle, x_1^l [m]')
legend('True','INS no error','KF')



%% SOLVER LOOP ============================================================
for i = 2:nT
    dt = t(i)-t(i-1);
    
    % get the measurements vector, updated with the error
    % if error loop is not being done, then a correction of zero is done
    meas = [omegaMeas(i-1:i);fMeas(:,i-1:i)] + [zeros(3,1),error(:,i-1)];
    
    % get the INS estimation
    X_INS_tilda(:,i) = step_INS_ODE(X_INS_hat(:,i-1), meas, dt);
    
    % compute the Kalman matrices
    [F, G, Q, phi] = computeKalmanMatrices(X_INS_tilda(1,i), fMeas(:,i),...
        dt, W, 1/(gyro_GM_tau * GM_factor), 1/(acc_GM_tau * GM_factor));
    
    % get the current GPS measurement (will be NaN if no measurement)
    Z = z_GPS(:,i) - X_INS_tilda(4:5,i);
        
    % perform Kalman filtering
    [dx_hat, pk(:,:,i)] = step_KF(phi, Q, H, zeros(9,1), ...
        pk(:,:,i-1), Z, R);
    
    % separate the error in two components
    dx_hat1(:,i) = dx_hat(1:5);
    dx_hat2 = dx_hat(6:9);
    
    % update error if doing a closed loop
    if error_loop
        error(:,i) = error(:,i - 1) ...
            + [dx_hat2(1) + dx_hat2(2);dx_hat2(3);dx_hat2(4)];
    end
    
    % get the new position after the KF
    X_INS_hat(:,i) = X_INS_tilda(:,i) + dx_hat1(:,i);
end


%% STATISTICAL ANALYSIS ===================================================
H_cut = H(:,1:5); % cut H matrix to remove error
p_cut = pk(1:5,1:5,:); % cut p matrix to remove error

% get the raw statistics
[hat_error, meas_error, std_hat, std_meas, innovation, quality] = ...
    statistics(X_TRUE, X_INS_tilda, X_INS_hat, ...
    z_GPS, H_cut, p_cut, meas_idx);

% split everything in different variables for readibility
% position
pos_error = hat_error(4:5,:);
std_pos_hat = sqrt(std_hat(4)^2 + std_hat(5)^2);
std_pos_meas = sqrt(std_meas(1)^2 + std_meas(2)^2);
improvement = (std_pos_meas / std_pos_hat - 1) * 100;
pos_quality = sqrt(quality(4,:).^2 + quality(5,:).^2);

% velocity
vel_error = hat_error(2:3,:);
vel_quality = quality(2:3,:);

% azimuth
alpha_error = hat_error(1,:);
alpha_quality = quality(1,:);


%% PLOTS ==================================================================
% trajectory
figure
set(gcf,'Position',[100 100 650 650])
axis equal
%axis([-550 550 -550 550])
hold all
plot(state(:,2),state(:,3)) % nominal trajectory
plot(X_INS_tilda(:,2),X_INS_tilda(:,3))
plot(X_INS_hat(5,:),X_INS_hat(4,:))
title(['Trajectory of the rocket: ', plot_title])
xlabel('East-position of the vehicle, x_2^l [m]')
ylabel('North-position of the vehicle, x_1^l [m]')
legend('True','GPS','KF')

%heading
figure
hold all
plot(time,measurements(:,1)) % nominal heading
plot(time,measurements(:,1)) % nominal heading
legend('Nominal','INS')


% velocity
figure
hold all
plot(time,state(:,2)) % nominal x velocity
plot(time,state(:,4)) % nominal x velocity
plot(time,state(:,6)) % nominal x acceleration
legend('vx','vz','ax','az')

% errors
figure
subplot(3,1,1);
plot(t,rad2deg(alpha_error));
xline(tGPS(11))
title(['Azimuth error: ', plot_title])
xlabel('Time, t [s]')
ylabel('\alpha^{true} - \alpha^{KF} [deg]')
set(gcf,'Position',[100 -100 800 600])
subplot(3,1,2);
plot(t,vel_error);
xline(tGPS(11))
title(['Velocity error: ', plot_title])
xlabel('Time, t [s]')
ylabel('v^{true} - v^{KF} [m/s]')
legend('North','East')
subplot(3,1,3);
plot(t,pos_error);
xline(tGPS(11))
title(['Position error: ', plot_title])
xlabel('Time, t [s]')
ylabel('z^{true} - z^{KF} [m/s]')
legend('North','East')

% innovation and predicted quality
figure
set(gcf,'Position',[100 -100 800 600])
subplot(2,1,1);
semilogy(t,pos_quality);
xline(tGPS(11))
yl = yline(std_pos_meas,'-.b','GPS');
yl.LabelVerticalAlignment =  'bottom';
yline(std_pos_hat,':r','KF');
title(['Evolution of positioning quality: ', plot_title])
xlabel('Time, t [s]')
ylabel('Position quality, \sigma_{xy} [m]')
subplot(2,1,2);
plot(tGPS,innovation);
xline(tGPS(11))
title(['Evolution of innovation: ', plot_title])
xlabel('Time, t [s]')
ylabel('Innovation, $$Z_t^{GPS}-H \tilde{x}_t$$ [m]', 'Interpreter', 'LaTeX')


% histogram
figure
set(gcf,'Position',[100 -100 600 300])
histogram(innovation(:,11:end))
axis([-6 6 0 50])
title(['Histogram of innovation: ', plot_title])
xlabel('Innovation, $$Z_t^{GPS}-H \tilde{x}_t$$ [m]', 'Interpreter', 'LaTeX')
xline(tGPS(11))