import os

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from filterpy.kalman import KalmanFilter
from filterpy.common import Q_discrete_white_noise

# variables ===================================================================

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
pressure_datasets = []
acceleration_datasets = []


# user functions ==============================================================

def pressure_to_altitude(pressure, temperature):
    altitude = ((P0 / pressure) ** PRESSURE_EXPONENT - 1) \
               * (temperature + CELSIUS_TO_KELVIN) / LAPSE_RATE
    return altitude


# mains script ================================================================

if __name__ == "__main__":

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
        # fix data to be in a format we expect
        dataset["measTime"] = dataset["measTime"] / 1000
        dataset["accX"] = dataset["accX"] * G
        dataset["accY"] = dataset["accY"] * G
        dataset["accZ"] = dataset["accZ"] * G
        dataset["acceleration"] = np.sqrt(dataset["accX"] ** 2
                                          + dataset["accY"] ** 2
                                          + dataset["accZ"] ** 2)
        dataset["measAltitude"] = dataset["measAltitude"] \
                                  - dataset["measAltitude"][0]
        dataset["height_calc"] = pressure_to_altitude(dataset["pressure"], 5)
        dataset["height_calc"] = dataset["height_calc"] - \
                                 dataset["height_calc"][0]

        # remove duplicate pressure readings
        duplicates = dataset.duplicated(["measAltitude", "pressure"])
        # dataset.loc[duplicates, "measAltitude"] = None
        # dataset.loc[duplicates, "pressure"] = None

        # kalman filter!
        kf = KalmanFilter(dim_x=3, dim_z=2)
        kf.H = np.array([[1, 0, 0],
                         [0, 0, 1]])
        kf.F = np.array([[1, dt, dt * dt * 0.5],
                         [0, 1, dt],
                         [0, 0, 1]])

        # initial process covariance
        kf.P = np.array([[std_h * std_h, 0, 0],
                         [0, 1, 0],
                         [0, 0, std_a * std_a]])

        # Process noise matrix
        std = 0.004
        var = std * std
        kf.Q = Q_discrete_white_noise(dim=3, dt=dt, var=var)

        # Measurement covariance
        kf.R *= np.array([[std_h * std_h, 0], [0, std_a * std_a]])

        # assign initial state
        kf.x = np.array([0, 0, -G])

        # prepare columns to store the KF states
        dataset["x_est_kf"] = np.nan
        dataset["v_est_kf"] = np.nan
        dataset["a_est_kf"] = np.nan

        for idx, row in dataset.iterrows():
            # save the state into the dataset
            dataset.loc[idx, "x_est_kf"] = kf.x[0]
            dataset.loc[idx, "v_est_kf"] = kf.x[1]
            dataset.loc[idx, "a_est_kf"] = kf.x[2]
            # start calculating things using the kalman filter
            # figure out the timestep for this measurement
            if idx == 0:
                timestep = row["measTime"]
            else:
                timestep = row["measTime"] - dataset.loc[idx-1, "measTime"]
            # update F to use the right timestep
            F = np.array([[1, timestep, timestep * timestep * 0.5],
                         [0, 1, timestep],
                         [0, 0, 1]])
            kf.predict(F=F)
            kf.update(np.array([row["measAltitude"],
                                row["acceleration"] - G]))

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
        ax_a.plot(dataset["measTime"], dataset["acceleration"] - G,
                  label=filename)
        ax_a_kf.plot(dataset["measTime"], dataset["a_est_kf"], label=filename)
    ax_h.legend()
    ax_h2.legend()
    ax_h_kf.legend()
    ax_v_kf.legend()
    ax_a.legend()
    ax_a_kf.legend()
    plt.show()
