# -*- coding: utf-8 -*-
"""
Created on Wed Oct 13 20:25:35 2021

@author: newbi
"""

import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d


# constants
R = 287
GAMMA = 1.4
G = 9.81
C_TO_K = 273.15
T = 15.5 + C_TO_K  # K
A = np.sqrt(R * GAMMA * T)  # speed of sound

# rocket variables
nosecone_mass = np.array([0.25,0.275,0.3])
nosecone_drag_fraction = 0.07

# variables for paths
root_path = "./data/logs/decodedData/cernier_supersonic_09_10_2021"
altimax_subpath = "Cernier_Launch_[08.11.21].xlsx"
or_data_path = "OR_cernier_09_10_2021.csv"
or_mod_data_path = "OR_cernier_09_10_2021_modified_thrust_curve.csv"
ais_data_path = "AISx120SX_0.csv"
sft_data_path = "LabVIEW_thrust_I600.csv"
# launch start/end window. obtained after graphs first shown
launch_start_ais = 669.315
launch_end_ais = 680
altimax_offset = 0.134
sft_start = 263.74
sft_end = 265.25

sft_freq = 50  # Hz

# thrust curve modification
thrust_curve_level_mod = 1.18
thrust_curve_length_mod = 1.18

# load the data
altimax_data = pd.read_excel(os.path.join(root_path, altimax_subpath))
or_data = pd.read_csv(os.path.join(root_path, or_data_path),
                      comment='#', encoding='latin1')
or_mod_data = pd.read_csv(os.path.join(root_path, or_mod_data_path),
                          comment='#', encoding='latin1')
ais_data = pd.read_csv(os.path.join(root_path, ais_data_path))
sft_data = pd.read_csv(os.path.join(root_path, sft_data_path),
                       comment='#', delimiter=';')

# change the time to be consistent between data sources
ais_data["timestamp (s)"] = ais_data["timestamp (us)"] / 1000000
altimax_data["TIME"] += altimax_offset
sft_data["timestamp (s)"] = sft_data["échantillon"] / sft_freq

# find where to cut off data
cutoff_index_start_ais = (ais_data["timestamp (s)"] -
                          launch_start_ais).abs().argmin()
cutoff_index_end_ais = (ais_data["timestamp (s)"] -
                        launch_end_ais).abs().argmin()
cutoff_index_start_sft = (sft_data["timestamp (s)"] -
                          sft_start).abs().argmin()
cutoff_index_end_sft = (sft_data["timestamp (s)"] -
                        sft_end).abs().argmin()
cutoff_time = altimax_data["TIME"].iloc[-1]
cutoff_index_or = np.argmin(np.abs(or_data["Time (s)"] - cutoff_time))
cutoff_index_or_mod = np.argmin(np.abs(or_mod_data["Time (s)"] - cutoff_time))

# cut off data and change timestamps accordingly
ais_data = ais_data[cutoff_index_start_ais:cutoff_index_end_ais]
sft_data = sft_data[cutoff_index_start_sft:cutoff_index_end_sft]
ais_data["timestamp (s)"] = (ais_data["timestamp (s)"] - launch_start_ais)
sft_data["timestamp (s)"] = (sft_data["timestamp (s)"] - sft_start)
or_data = or_data[:cutoff_index_or]
or_mod_data = or_mod_data[:cutoff_index_or_mod]

# calculate thrust from sensor acceleration measurements
# this uses the drag and mass simulation from OR
or_drag_interp = interp1d(or_data["Time (s)"], or_data["Drag force (N)"],
                          fill_value="extrapolate")
or_mass_interp = interp1d(or_data["Time (s)"], or_data["Mass (g)"],
                          fill_value="extrapolate")
# interpolate OR simulation onto the timesteps from the sensors
ais_drag = or_drag_interp(ais_data["timestamp (s)"])
ais_mass = or_mass_interp(ais_data["timestamp (s)"]) / 1000
altimax_drag = or_drag_interp(altimax_data["TIME"])
altimax_mass = or_mass_interp(altimax_data["TIME"]) / 1000
# calculate the thrust
ais_thrust = ais_mass * (ais_data["accX (g)"] * -G + G) + ais_drag
altimax_thrust = altimax_mass * (
        altimax_data["ACCEL [m/s2]"] + G) + altimax_drag

# calculate contact force
fins_mass = or_data["Mass (g)"][:, None] / 1000 - nosecone_mass
nosecone_drag = or_data["Drag force (N)"][:, None] * nosecone_drag_fraction
fins_drag = or_data["Drag force (N)"][:, None] - nosecone_drag
contact_force_fins = (fins_mass *
                      (G + or_data["Vertical acceleration (m/s²)"][:, None])
                      - or_data["Thrust (N)"][:, None] + fins_drag)
contact_force_nosecone = - (nosecone_mass * (G +
                            or_data["Vertical acceleration (m/s²)"][:, None])
                            + nosecone_drag)

plt.close('all')

# altitude vs t graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of height vs time")
ax.plot(altimax_data["TIME"],
        altimax_data["HEIGHT FILTER [m]"], label="Altimax filtered")
ax.plot(altimax_data["TIME"],
        altimax_data["HEIGHT RAW [m]"], label="Altimax raw")
ax.plot(or_data["Time (s)"], or_data["Altitude (m)"], label="OR")
ax.plot(or_mod_data["Time (s)"], or_mod_data["Altitude (m)"], label="OR (mod)")
ax.set_xlabel("Time, t [s]")
ax.set_ylabel("Altitude, y [m]")
ax.legend()

# velocity vs t graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of velocity vs time")
ax.plot(altimax_data["TIME"], altimax_data["SPEED [m/s]"], label="Altimax")
ax.plot(or_data["Time (s)"], or_data["Vertical velocity (m/s)"], label="OR")
ax.plot(or_mod_data["Time (s)"], or_mod_data["Vertical velocity (m/s)"],
        label="OR (mod)")
ax.set_xlabel("Time, t [s]")
ax.set_ylabel("Vertical velocity, v [m/s]")
ax.legend()

# Mach vs t graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of Mach number vs time")
ax.plot(altimax_data["TIME"], altimax_data["SPEED [m/s]"] / A, label="Altimax")
ax.plot(or_data["Time (s)"],
        or_data["Vertical velocity (m/s)"] / A, label="OR")
# ax.plot(or_mod_data["Time (s)"],
#         or_mod_data["Vertical velocity (m/s)"] / A, label="OR (mod)")
ax.set_xlabel("Time, t [s]")
ax.set_ylabel("Mach number, M [-]")
ax.hlines(1,0,1.4)
ax.legend()

# acceleration vs t graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of acceleration vs time")
ax.plot(altimax_data["TIME"], altimax_data["ACCEL [m/s2]"], label="Altimax")
ax.plot(or_data["Time (s)"],
        or_data["Vertical acceleration (m/s²)"], label="OR")
# ax.plot(or_mod_data["Time (s)"],
#         or_mod_data["Vertical acceleration (m/s²)"], label="OR (mod)")
ax.plot(ais_data["timestamp (s)"], ais_data["accX (g)"] * -G, label="AIS")
ax.set_xlabel("Time, t [s]")
ax.set_ylabel("Vertical acceleration, a [m/s^2]")
ax.legend()

# contact force vs t graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of contact force vs time")
# ax.plot(or_data["Time (s)"],
#         contact_force_fins, label="Fins")
ax.plot(or_data["Time (s)"], contact_force_nosecone)
ax.set_xlabel("Time, t [s]")
ax.set_ylabel("Contact force, C [N]")
labels = [f"Nosecone mass = {mass:.3f} kg" for mass in nosecone_mass]
ax.legend(labels)

# thrust vs t graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of thrust vs time")
ax.plot(altimax_data["TIME"], altimax_thrust,
        label="Altimax (using OR drag and mass)")
ax.plot(or_data["Time (s)"],
        or_data["Thrust (N)"], label="OR")
ax.plot(or_mod_data["Time (s)"],
        or_mod_data["Thrust (N)"], label="OR (full mod)")
ax.plot(or_data["Time (s)"] * thrust_curve_length_mod,
        or_data["Thrust (N)"] / thrust_curve_level_mod, label="OR (part mod)")
ax.plot(ais_data["timestamp (s)"], ais_thrust,
        label="AIS (using OR drag and mass)")
ax.plot(sft_data["timestamp (s)"], sft_data["N"], label="SFT")
ax.plot(sft_data["timestamp (s)"], sft_data["N"] * 1.0428,
        label="SFT (corrected)")
ax.set_xlabel("Time, t [s]")
ax.set_ylabel("Motor thrust, T [N]")
ax.legend()

# pressure vs t graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of pressure vs time")
ax.plot(altimax_data["TIME"],
        altimax_data["PRESS RAW [hpa]"], label="Altimax")
ax.plot(or_data["Time (s)"], or_data["Air pressure (mbar)"] * 100, label="OR")
ax.set_xlabel("Time, t [s]")
ax.set_ylabel("Pressure, p [Pa]")
ax.legend()

# altitude vs Mach graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of altitude vs Mach")
ax.plot(altimax_data["SPEED [m/s]"] / A,
        altimax_data["HEIGHT FILTER [m]"], label="Altimax filtered")
ax.plot(or_data["Vertical velocity (m/s)"] / A,
        or_data["Altitude (m)"], label="OR")
ax.set_xlabel("Mach number, M [-]")
ax.set_ylabel("Altitude, y [m]")
ax.legend()

# acceleration vs Mach graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of acceleration vs Mach")
ax.plot(altimax_data["SPEED [m/s]"] / A,
        altimax_data["ACCEL [m/s2]"], label="Altimax")
ax.plot(or_data["Vertical velocity (m/s)"] / A,
        or_data["Vertical acceleration (m/s²)"], label="OR")
ax.set_xlabel("Mach number, M [-]")
ax.set_ylabel(r"Vertical acceleration, a [$m/s^2$]")
ax.legend()

# pressure vs Mach graph
fig, ax = plt.subplots()
fig.suptitle("Comparison of pressure vs Mach")
ax.plot(altimax_data["SPEED [m/s]"] / A,
        altimax_data["PRESS RAW [hpa]"], label="Altimax")
ax.plot(or_data["Vertical velocity (m/s)"] / A,
        or_data["Air pressure (mbar)"] * 100, label="OR")
ax.set_xlabel("Mach number, M [-]")
ax.set_ylabel("Pressure, p [Pa]")
ax.legend()

plt.show()
