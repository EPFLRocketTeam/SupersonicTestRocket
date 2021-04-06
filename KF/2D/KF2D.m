% KF2D Script for post-process Kalman filter on simulated 2D data
%% HEADER
% KF2D Script
% Author: Joshua Cayetano-Emond
% newbie856@gmail.com
% Date: 2021-02-20


%% General settings =======================================================
clear all; close all; clc;

seed = 45; % seed to assure repeatability
rng(seed); 

set(groot,'DefaultAxesFontSize',14)
set(groot,'DefaultLineLineWidth',1.5)
set(groot,'defaultfigureposition',[400 100 1000 400])


%% Load data ==============================================================
fileName = './data/i600_1000Hz.csv';
state = extractNominalOR(fileName);


%% Variables ==============================================================
g = 9.805;

%% Timing
T = state.time(end);
t_stationary = 0; % stationary time on the ground before launch

%% IMU values
% timing
fIMU = 20;                                          % [Hz]
tIMU = (0:1/fIMU:T)'; % IMU time vector             % [s]
nTIMU = length(tIMU);                               % [-]

%
IMU_angle = deg2rad(45);                             % [rad]

% initialization error
alpha0_error = deg2rad(0);                          % [rad]
vx0_error = 0;                                      % [m/s]
vz0_error = 0;                                      % [m/s]

%% INS values
% ADIS 16470 provided noise values
gyro_bias     = 0.2 * randn;                        % [deg/s]
gyro_WN =       0.34;                               % [deg/sqrt(h)]
gyro_GM_sigma = 0.5;                                % [deg/(s / sqrt(Hz))]
gyro_GM_tau =   100;                                % [s]
acc_bias =      4 * randn(1,2);                     % [mg]
acc_WN =        100;                                % [ug/sqrt(Hz)]

% MPU-6000 provided noise values
% gyro_bias     = 0 * randn;                         % [deg/s]
% gyro_WN =       0.05 *3600;                         % [deg/sqrt(h)]
% gyro_GM_sigma = 0.5;                                % [deg/(s / sqrt(Hz))]
% gyro_GM_tau =   100;                                % [s]
% acc_bias =      80 * randn(1,2);                    % [mg]
% acc_WN =        400;                                % [ug/sqrt(Hz)]

% % MMA6555KW provided noise values
% acc_LSB =       18.2;                               % [LSB/g]
% acc_bias =      1000 / acc_LSB * 0.25 * randn(1,2); % [mg]
% acc_WN =        1000 / acc_LSB;                     % [mg]

% convert to desired units
gyro_bias = deg2rad(gyro_bias);                     % [rad/s]
gyro_WN = (deg2rad(gyro_WN) / 60) * sqrt(fIMU);     % [rad/(s / sample)]
gyro_GM_sigma = deg2rad(gyro_GM_sigma);             % [rad/(s / sqrt(Hz))]
acc_bias = acc_bias / 1e3 * g;                      % [m/s^2]
acc_WN = acc_WN / 1e6 * g * sqrt(fIMU);             % [m/(s^2 / sample)]
% acc_WN = acc_WN / 1e3 * g;                         % [m/s^2]


%% Generate measurements ==================================================
% calculate the IMU measurements
IMU_measurements = generateNominalIMU(state.time, state.heading, ...
    state.angular_rate, state.acceleration, tIMU, IMU_angle);

% add stationary time on the ground
stationary_time = (-t_stationary:1/fIMU:-1/fIMU)';
stationary_gyro = zeros(size(stationary_time));
stationary_accel = [zeros(size(stationary_time)), ...
    -g * ones(size(stationary_time))];
IMU_measurements.time = [stationary_time; IMU_measurements.time];
IMU_measurements.angular_rate = [stationary_gyro; ...
    IMU_measurements.angular_rate];
IMU_measurements.acceleration = [stationary_accel; ...
    IMU_measurements.acceleration];

% add error
error_measurements = addINSerror(IMU_measurements, gyro_bias, gyro_WN, ...
    gyro_GM_sigma, 1/gyro_GM_tau, acc_bias, acc_WN);

measurements = [error_measurements.angular_rate, ...
    error_measurements.acceleration];



%% INS
% initialize matrices
X_INS_tilda = zeros(length(error_measurements.time),5);
X_INS_hat = zeros(length(error_measurements.time),5);

% initial conditions
X_INS_tilda(1,:) = [state.heading(1), state.position(1,:), ...
    state.velocity(1,:)] + [alpha0_error, 0, 0, vx0_error, vz0_error];
X_INS_hat(1,:) = X_INS_tilda(1,:);

launch_flag = false;

for i = 2:length(error_measurements.time)
    dt = error_measurements.time(i)-error_measurements.time(i-1);
    meas = measurements(i-1:i,1:3);
    
    if launch_flag
        X_INS_tilda(i,:) = step_INS_ODE(X_INS_tilda(i-1,:), meas, ...
                dt, IMU_angle);
    else
        if norm(error_measurements.acceleration(i,:)) >= 3 * g
            launch_flag = true;
            X_INS_tilda(i,:) = step_INS_ODE(X_INS_tilda(i-1,:), meas, ...
                dt, IMU_angle);
        else
            X_INS_tilda(i,:) = X_INS_tilda(i-1,:);
        end
    end
end


%% Plots ==================================================================

% trajectory
figure
set(gcf,'Position',[100 100 650 650])
axis equal
%axis([-550 550 -550 550])
hold all
plot(state.position(:,1),state.position(:,2)) % nominal trajectory
plot(X_INS_tilda(:,2),X_INS_tilda(:,3))
plot(X_INS_hat(5,:),X_INS_hat(4,:))
title(['Trajectory of the rocket: '])
xlabel('East-position of the vehicle, x_2^l [m]')
ylabel('North-position of the vehicle, x_1^l [m]')
legend('True','INS no error')

% compare heading
figure
%subplot(2,1,1)
hold all
plot(state.time,state.heading)
plot(IMU_measurements.time,X_INS_tilda(:,1))
title('heading')
%subplot(2,1,2)


% compare x position error
figure
hold all
plot(state.time,state.position(:,1))
plot(IMU_measurements.time,X_INS_tilda(:,2))
title('x-pos')

% compare velocity error
figure
hold all
plot(state.time,state.position(:,2))
plot(IMU_measurements.time,X_INS_tilda(:,3))
title('z-pos')

% compare velocity error
figure
hold all
plot(state.time,state.velocity(:,1))
plot(IMU_measurements.time,X_INS_tilda(:,4))
title('x-vel')


% compare velocity error
figure
hold all
plot(state.time,state.velocity(:,2))
plot(IMU_measurements.time,X_INS_tilda(:,5))
title('z-vel')