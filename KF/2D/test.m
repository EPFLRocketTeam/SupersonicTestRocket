% initialization error
alpha0_error = deg2rad(0);                          % [rad]
vx0_error = 0;                                      % [m/s]
vz0_error = 0;                                      % [m/s]

%%
fileName = './data/i600_1000Hz.csv';
[time, state, measurements] = extractNominalOR(fileName);

nT = length(time);
%% INS
% initialize matrices
X_INS_tilda = zeros(nT,5);
X_INS_hat = zeros(nT,5);

% initial conditions
X_INS_tilda(1,:) = state(1,1:5) + [alpha0_error, 0, 0, vx0_error, vz0_error];
X_INS_hat(1,:) = X_INS_tilda(1,:);

for i = 2:nT
    dt = time(i)-time(i-1);
    meas = measurements(i-1:i,1:3);
    %meas = [state(i-1:i,1),measurements(i-1:i,2:3)];
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

% compare heading error
figure
hold all
plot(time,state(:,1))
plot(time,X_INS_tilda(:,1))
% compare velocity error
figure
hold all
plot(time,state(:,2))
plot(time,X_INS_tilda(:,2))
% compare velocity error
figure
hold all
plot(time,state(:,3))
plot(time,X_INS_tilda(:,3))

% compare velocity error
figure
hold all
plot(time,state(:,4))
plot(time,X_INS_tilda(:,4))

% compare velocity error
figure
hold all
plot(time,state(:,5))
plot(time,X_INS_tilda(:,5))