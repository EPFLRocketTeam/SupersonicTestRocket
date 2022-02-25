import os

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from filterpy.kalman import KalmanFilter
from filterpy.common import Q_discrete_white_noise

dataFolder = "./data"
extension = "txt"

# constants
P0 = 101325
CELSIUS_TO_KELVIN = 273.15
PRESSURE_EXPONENT = 1 / 5.257
LAPSE_RATE = 0.0065
RHO = 1.225
G = 9.80665

# Standard deviation of sensors
std_a = 0.05
std_h = 0.11

dt = 0.05

cd_a_reference = np.array([.233])
time_reference = np.linspace(0, 5, 50)

filenames = []
datasets = []


def pressure_to_altitude(pressure, temperature):
    altitude = ((P0 / pressure) ** (PRESSURE_EXPONENT) - 1) \
               * (temperature + CELSIUS_TO_KELVIN) / LAPSE_RATE
    return altitude

# load files
for fileName in os.listdir(dataFolder):
    if fileName.endswith(extension):
        print(f"Loading {fileName}")
        filenames.append(fileName)
        dataset = pd.read_csv(os.path.join(dataFolder, fileName),
                              skipinitialspace=True)
        datasets.append(dataset)
        # altitude = dataset

# process data
for dataset in datasets:
    dataset["measTime"] = dataset["measTime"]/1000
    dataset["accX"] = dataset["accX"] * G
    dataset["accY"] = dataset["accY"] * G
    dataset["accZ"] = dataset["accZ"] * G
    dataset["acceleration"] = np.sqrt(dataset["accX"] ** 2
                                      + dataset["accY"] ** 2
                                      + dataset["accZ"] ** 2)
    dataset["measAltitude"] = dataset["measAltitude"] \
                              - dataset["measAltitude"][0]
    dataset["height_calc"] = pressure_to_altitude(dataset["pressure"], 5)
    dataset["height_calc"] = dataset["height_calc"] - dataset["height_calc"][0]

    kf = KalmanFilter(dim_x=3, dim_z=2)
    kf.H = np.array([[1, 0, 0], [0, 0, 1]])
    kf.F = np.array([[1, dt, dt * dt * 0.5], [0, 1, dt], [0, 0, 1]])

    # initial process covariance
    kf.P = np.array([[std_h * std_h, 0, 0], [0, 1, 0], [0, 0, std_a * std_a]])

    # Process noise matrix
    std = 0.004
    var = std * std
    kf.Q = Q_discrete_white_noise(dim=3, dt=dt, var=var)

    # Measurement covariance
    kf.R *= np.array([[std_h * std_h, 0], [0, std_a * std_a]])

    n = dataset.shape[0]
    x_est_kf = np.zeros(n)
    v_est_kf = np.zeros(n)
    a_est_kf = np.zeros(n)

    # assign initial position
    kf.x = np.array([0, 0, -G])

    for i in range(n):
        x_est_kf[i] = kf.x[0]
        v_est_kf[i] = kf.x[1]
        a_est_kf[i] = kf.x[2]
        kf.predict()
        kf.update(np.array([dataset["measAltitude"][i],
                            dataset["acceleration"][i] - G]))
    dataset["x_est_kf"] = x_est_kf
    dataset["v_est_kf"] = v_est_kf
    dataset["a_est_kf"] = a_est_kf


# plot
plt.close('all')
fig_h, ax_h = plt.subplots()
fig_h.suptitle("height according to BMP")
fig_h2, ax_h2 = plt.subplots()
fig_h2.suptitle("height according to ISA")
fig_h_kf, ax_h_kf = plt.subplots()
fig_h_kf.suptitle("height according to KF")
fig_v_kf, ax_v_kf = plt.subplots()
fig_v_kf.suptitle("velocity according to KF")
fig_a, ax_a = plt.subplots()
fig_a.suptitle("raw acceleration")
fig_a_kf, ax_a_kf = plt.subplots()
fig_a_kf.suptitle("acceleration according to KF")
for filename, dataset in zip(filenames, datasets):
    ax_h.plot(dataset["measTime"], dataset["measAltitude"], label=filename)
    ax_h2.plot(dataset["measTime"], dataset["height_calc"], label=filename)
    ax_h_kf.plot(dataset["measTime"], dataset["x_est_kf"], label=filename)
    ax_v_kf.plot(dataset["measTime"], dataset["v_est_kf"], label=filename)
    ax_a.plot(dataset["measTime"], dataset["acceleration"] - G, label=filename)
    ax_a_kf.plot(dataset["measTime"], dataset["a_est_kf"], label=filename)
ax_h.legend()
ax_h2.legend()
ax_h_kf.legend()
ax_v_kf.legend()
ax_a.legend()
ax_a_kf.legend()
plt.show()