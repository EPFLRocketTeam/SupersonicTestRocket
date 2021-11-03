# -*- coding: utf-8 -*-
"""
Created on Wed Oct 13 20:25:35 2021

@author: newbi
"""

import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# constants
R = 287
GAMMA = 1.4

# variables for paths
root_path = "./data/decodedData/cernier_supersonic_09_10_2021"
altimax_subpath = "Cernier_Launch_[08.11.21].xlsx"
or_data_path = "OR_cernier_09_10_2021.csv"

T = 282 # K
    
# load the data
altimax_data = pd.read_excel(os.path.join(root_path, altimax_subpath))
or_data = pd.read_csv(os.path.join(root_path, or_data_path),
                      comment='#', encoding ='latin1')

# find where to cut off OR data
cutoff_time = altimax_data["TIME"].iloc[-1]
cutoff_index_or = np.argmin(np.abs(or_data["Time (s)"] - cutoff_time))

# calculate speed of sound 
a = np.sqrt(R*GAMMA*T)

# altitude vs t graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of height vs time")
ax.plot(altimax_data["TIME"],
        altimax_data["HEIGHT FILTER [m]"], label="Altimax filtered")
ax.plot(altimax_data["TIME"],
        altimax_data["HEIGHT RAW [m]"], label="Altimax raw")
ax.plot(or_data["Time (s)"][:cutoff_index_or],
        or_data["Altitude (m)"][:cutoff_index_or], label="OR")
ax.set_xlabel("Time, t [s]")
ax.set_ylabel("Altitude, y [m]")
ax.legend()

# velocity vs t graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of velocity vs time")
ax.plot(altimax_data["TIME"], altimax_data["SPEED [m/s]"], label="Altimax")
ax.plot(or_data["Time (s)"][:cutoff_index_or],
        or_data["Vertical velocity (m/s)"][:cutoff_index_or], label="OR")
ax.set_xlabel("Time, t [s]")
ax.set_ylabel("Vertical velocity, v [m/s]")
ax.legend()

# Mach vs t graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of Mach number vs time")
ax.plot(altimax_data["TIME"], altimax_data["SPEED [m/s]"] / a, label="Altimax")
ax.plot(or_data["Time (s)"][:cutoff_index_or],
        or_data["Vertical velocity (m/s)"][:cutoff_index_or] / a, label="OR")
ax.set_xlabel("Time, t [s]")
ax.set_ylabel("Mach number, M [-]")
ax.legend()

# acceleration vs t graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of acceleration vs time")
ax.plot(altimax_data["TIME"], altimax_data["ACCEL [m/s2]"], label="Altimax")
ax.plot(or_data["Time (s)"][:cutoff_index_or],
        or_data["Vertical acceleration (m/s²)"][:cutoff_index_or], label="OR")
ax.set_xlabel("Time, t [s]")
ax.set_ylabel("Vertical acceleration, a [m/s^2]")
ax.legend()

# pressure vs t graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of pressure vs time")
ax.plot(altimax_data["TIME"],
        altimax_data["PRESS RAW [hpa]"], label="Altimax")
ax.plot(or_data["Time (s)"][:cutoff_index_or],
        or_data["Air pressure (mbar)"][:cutoff_index_or] * 100, label="OR")
ax.set_xlabel("Time, t [s]")
ax.set_ylabel("Pressure, p [Pa]")
ax.legend()

# altitude vs Mach graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of altitude vs Mach")
ax.plot(altimax_data["SPEED [m/s]"] / a,
        altimax_data["HEIGHT FILTER [m]"], label="Altimax filtered")
ax.plot(or_data["Vertical velocity (m/s)"][:cutoff_index_or] / a,
        or_data["Altitude (m)"][:cutoff_index_or], label="OR")
ax.set_xlabel("Mach number, M [-]")
ax.set_ylabel("Altitude, y [m]")
ax.legend()

# acceleration vs Mach graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of acceleration vs Mach")
ax.plot(altimax_data["SPEED [m/s]"] / a,
        altimax_data["ACCEL [m/s2]"], label="Altimax")
ax.plot(or_data["Vertical velocity (m/s)"][:cutoff_index_or] / a,
        or_data["Vertical acceleration (m/s²)"][:cutoff_index_or], label="OR")
ax.set_xlabel("Mach number, M [-]")
ax.set_ylabel(r"Vertical acceleration, a [$m/s^2$]")
ax.legend()

# pressure vs Mach graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of pressure vs Mach")
ax.plot(altimax_data["SPEED [m/s]"] / a,
        altimax_data["PRESS RAW [hpa]"], label="Altimax")
ax.plot(or_data["Vertical velocity (m/s)"][:cutoff_index_or] / a,
        or_data["Air pressure (mbar)"][:cutoff_index_or] * 100, label="OR")
ax.set_xlabel("Mach number, M [-]")
ax.set_ylabel("Pressure, p [Pa]")
ax.legend()