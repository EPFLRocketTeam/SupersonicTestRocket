# -*- coding: utf-8 -*-
"""
Temporary file before the dashboard is create to plot flight raw data.
Created on Sat September 04 20:28:00 2021

@author: newbi
"""

import pandas as pd
import numpy as np
import os
import matplotlib.pyplot as plt

# variables
directory = "data/decodedData/cernier_supersonic_09_10_2021"
# launch start/end window. obtained after graphs first shown
launch_start = 669e6
launch_end = 1200e6

# load the raw data
ADIS16470_data = pd.read_csv(os.path.join(directory,"ADIS16470_0.csv"))
AIS1120SX_data = pd.read_csv(os.path.join(directory,"AISx120SX_0.csv"))
RSC1_pressure_data = pd.read_csv(os.path.join(directory,"RSC_pressure_1.csv"))
RSC1_temp_data = pd.read_csv(os.path.join(directory,"RSC_temp_1.csv"))
MAX0_data = pd.read_csv(os.path.join(directory,"MAX_0.csv"))
MAX1_data = pd.read_csv(os.path.join(directory,"MAX_1.csv"))
MAX2_data = pd.read_csv(os.path.join(directory,"MAX_2.csv"))

# filter the data according to the window given above
ADIS16470_data = ADIS16470_data.loc[
    (ADIS16470_data["timestamp (us)"] >= launch_start) &
    (ADIS16470_data["timestamp (us)"] <= launch_end)]
AIS1120SX_data = AIS1120SX_data.loc[
    (AIS1120SX_data["timestamp (us)"] >= launch_start) &
    (AIS1120SX_data["timestamp (us)"] <= launch_end)]
RSC1_pressure_data = RSC1_pressure_data.loc[
    (RSC1_pressure_data["timestamp (us)"] >= launch_start) &
    (RSC1_pressure_data["timestamp (us)"] <= launch_end) &
    (RSC1_pressure_data["measInvalid"] == 0)]
RSC1_temp_data = RSC1_temp_data.loc[
    (RSC1_temp_data["timestamp (us)"] >= launch_start) &
    (RSC1_temp_data["timestamp (us)"] <= launch_end) &
    (RSC1_temp_data["measInvalid"] == 0)]
MAX0_data = MAX0_data.loc[
    (MAX0_data["timestamp (us)"] >= launch_start) &
    (MAX0_data["timestamp (us)"] <= launch_end)]
MAX1_data = MAX1_data.loc[
    (MAX1_data["timestamp (us)"] >= launch_start) &
    (MAX1_data["timestamp (us)"] <= launch_end)]
MAX2_data = MAX2_data.loc[
    (MAX2_data["timestamp (us)"] >= launch_start) &
    (MAX2_data["timestamp (us)"] <= launch_end)]


# plot acceleration graph
fig, ax_acc = plt.subplots()
ax_acc.set_xlabel("Time, t [s]")
ax_acc.set_ylabel("Acceleration, a [g]")
ax_acc.plot(ADIS16470_data["timestamp (us)"] / 1e6, ADIS16470_data["accX (g)"],
        label="ADIS16470 X-accel")
ax_acc.plot(ADIS16470_data["timestamp (us)"] / 1e6, ADIS16470_data["accY (g)"],
        label="ADIS16470 Y-accel")
ax_acc.plot(ADIS16470_data["timestamp (us)"] / 1e6, ADIS16470_data["accZ (g)"],
        label="ADIS16470 Z-accel")
ax_acc.plot(AIS1120SX_data["timestamp (us)"] / 1e6, -AIS1120SX_data["accX (g)"],
        label="AIS1120SX_data X-accel")
ax_acc.legend()

# plot gyro graph
fig, ax_gyro = plt.subplots()
ax_gyro.set_xlabel("Time, t [s]")
ax_gyro.set_ylabel("Angular rotation, $\omega$ [deg/s]")
ax_gyro.plot(ADIS16470_data["timestamp (us)"] / 1e6, ADIS16470_data["gyroX (deg/s)"],
        label="ADIS16470 X-gyro")
ax_gyro.plot(ADIS16470_data["timestamp (us)"] / 1e6, ADIS16470_data["gyroY (deg/s)"],
        label="ADIS16470 Y-gyro")
ax_gyro.plot(ADIS16470_data["timestamp (us)"] / 1e6, ADIS16470_data["gyroZ (deg/s)"],
        label="ADIS16470 Z-gyro")
ax_gyro.legend()

# plot pressure graph
fig, ax_pres = plt.subplots()
ax_pres.set_xlabel("Time, t [s]")
ax_pres.set_ylabel("Pressure, P [psi]")
ax_pres.plot(RSC1_pressure_data["timestamp (us)"] / 1e6,
             RSC1_pressure_data["pressure (PSI)"], label="Total Pressure")
ax_pres.legend()

# plot temperature graph
fig, ax_temp = plt.subplots()
ax_temp.set_xlabel("Time, t [s]")
ax_temp.set_ylabel("Temperature, T [degC]")
ax_temp.plot(RSC1_temp_data["timestamp (us)"] / 1e6,
             RSC1_temp_data["temperature (degC)"], label="RSC1 temp")
ax_temp.plot(MAX0_data["timestamp (us)"] / 1e6,
             MAX0_data["probeTemp (degC)"], label="MAX0 probe temp")
# ax_temp.plot(MAX0_data["timestamp (us)"] / 1e6,
#              MAX0_data["ambientTemp (degC)"], label="MAX0 ambient temp")
ax_temp.plot(MAX1_data["timestamp (us)"] / 1e6,
             MAX1_data["probeTemp (degC)"], label="MAX1 probe temp")
# ax_temp.plot(MAX1_data["timestamp (us)"] / 1e6,
#              MAX1_data["ambientTemp (degC)"], label="MAX1 ambient temp")
ax_temp.plot(MAX2_data["timestamp (us)"] / 1e6,
             MAX2_data["probeTemp (degC)"], label="MAX2 probe temp")
# ax_temp.plot(MAX2_data["timestamp (us)"] / 1e6,
#              MAX2_data["ambientTemp (degC)"], label="MAX2 ambient temp")
ax_temp.legend()
