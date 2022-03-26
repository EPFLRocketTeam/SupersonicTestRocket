# standard imports
import numpy as np
import pandas as pd

# variables
data_file_path = "../data/simulations/cernier_09_10_2021_square_fins.csv"
data_file_path = "C:\\Users\\newbi\\Google Drive\\Rocket " \
                 "Team\\Wildhorn_N5800.csv"
bin_ends = np.array([0, 0.8, 1.2, 3.1])  # Mach number limits for regimes
total_impulse = 640  # N-s

# constants
G = 9.81  # m/s^2

if __name__ == "__main__":
    data = pd.read_csv(data_file_path, comment='#', encoding='latin1')

    # only look at the ascent
    apogee_idx = data["Altitude (m)"].argmax()
    data_ascent = data[:apogee_idx]
    # split in the acceleration and deceleration phases
    max_speed_idx = data_ascent["Vertical velocity (m/s)"].argmax()
    data_accel = data_ascent[:max_speed_idx]
    max_mach = data_accel.iloc[-1]["Mach number (?)"]
    data_decel = data_ascent[max_speed_idx:]

    # calculate energy loss and efficiency
    max_v_theoretical = total_impulse / (data["Mass (g)"][0] / 1000)
    max_v_theoretical_energy = (data["Mass (g)"][0] / 1000) \
                               * max_v_theoretical ** 2 / 2
    max_v_simulation = data_ascent["Vertical velocity (m/s)"][max_speed_idx]
    max_v_simulation_energy = (data_ascent["Mass (g)"][max_speed_idx] / 1000) \
                              * max_v_simulation ** 2 / 2
    max_h_simulation = data_ascent["Altitude (m)"][apogee_idx - 1]
    max_h_simulation_energy = (data_ascent["Mass (g)"][apogee_idx - 1] /
                               1000) \
                              * max_h_simulation * G
    print(f"Maximum theoretical energy: {max_v_theoretical_energy:.2f} J")
    print(f"Maximum simulation kinetic energy: {max_v_simulation_energy:.2f}"
          f" J")
    print(f"Maximum simulation potential energy: {max_h_simulation_energy:.2f}"
          f" J\n")

    displacement_accel = data_accel["Altitude (m)"].diff()
    avg_drag_accel = data_accel["Drag force (N)"].rolling(2).mean()
    avg_mach_accel = data_accel["Mach number (?)"].rolling(2).mean()
    work_drag_accel = displacement_accel * avg_drag_accel

    work_bins_accel = np.zeros(len(bin_ends) - 1)
    for i, (mach_start, mach_end) in enumerate(zip(bin_ends[:-1],
                                                   bin_ends[1:])):
        idx_start_accel = (avg_mach_accel - mach_start).abs().argmin()
        idx_end_accel = (avg_mach_accel - mach_end).abs().argmin()
        work_bins_accel[i] = np.sum(
            work_drag_accel[idx_start_accel:idx_end_accel])
        print(f"Work done by drag accelerating between Mach {mach_start} "
              f"and {min(mach_end, max_mach):.2f}: {work_bins_accel[i]:.2f} J")
    print("")

    displacement_decel = data_decel["Altitude (m)"].diff()
    avg_drag_decel = data_decel["Drag force (N)"].rolling(2).mean()
    avg_mach_decel = data_decel["Mach number (?)"].rolling(2).mean()
    work_drag_decel = displacement_decel * avg_drag_decel

    work_bins_decel = np.zeros(len(bin_ends) - 1)
    for i, (mach_start, mach_end) in enumerate(zip(np.flip(bin_ends[:-1]),
                                                   np.flip(bin_ends[1:]))):
        idx_start_decel = (avg_mach_decel - mach_end).abs().argmin()
        idx_end_decel = (avg_mach_decel - mach_start).abs().argmin()
        work_bins_decel[len(work_bins_decel) - i - 1] = np.sum(
            work_drag_decel[idx_start_decel:idx_end_decel])
        print("Work done by drag decelerating between Mach "
              f"{min(mach_end, max_mach):.2f} "
              f"and {mach_start}: "
              f"{work_bins_decel[len(work_bins_decel) - i - 1]:.2f} J")
    print("")

    total_drag_work = np.sum(work_bins_accel + work_bins_decel)
    for i, (mach_start, mach_end) in enumerate(zip(bin_ends[:-1],
                                                   bin_ends[1:])):
        print(f"Total work done by drag between Mach {mach_start} "
              f"and {min(mach_end, max_mach):.2f}: "
              f"{work_bins_accel[i] + work_bins_decel[i]:.2f} J "
              f"({100 * (work_bins_accel[i] + work_bins_decel[i]) / total_drag_work:.2f} %)")

    print("\nTotal work done by drag: "
          f"{total_drag_work:.2f} J")
