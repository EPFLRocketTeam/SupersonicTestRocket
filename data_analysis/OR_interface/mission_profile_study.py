# standard imports
import numpy as np
import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
from sklearn.linear_model import LinearRegression

# variables
data_file_path = "../data/simulations/hermes2_mission_profile_study.csv"
marker_symbols = ("o", "x", "<", ">")
linestyles = ("-", ":", "--", "-.")

if __name__ == "__main__":
    data = pd.read_csv(data_file_path)

    # prepare the graph
    fig, ax = plt.subplots()
    fig.suptitle("Hermes II mission profile study")
    ax.set_xlabel("Mach number, M [-]")
    ax.set_ylabel(r"Apogee, $y_{max}$ [m]")
    ax.grid()

    # split data by motor
    for i, (motor, motor_dataframe) in enumerate(data.groupby(["Motor"])):
        # split data by mass
        for j, (mass, mass_dataframe) in enumerate(
                motor_dataframe.groupby(["Dry mass [g]"])):
            sort_idx = mass_dataframe["Mach number [-]"].argsort()
            X = mass_dataframe.iloc[sort_idx]["Mach number [-]"].\
                values.reshape(-1, 1)
            Y = mass_dataframe.iloc[sort_idx]["Apogee [m]"].\
                values.reshape(-1, 1)
            linear_regressor = LinearRegression()  # create object for the
            # class
            linear_regressor.fit(X, Y)  # perform linear regression
            Y_pred = linear_regressor.predict(X)  # make predictions
            ax.scatter(X, Y, label=f"{motor}, " r"$m_{dry}$ = " f"{mass}g",
                       color=list(mcolors.TABLEAU_COLORS.values())[j],
                       marker=marker_symbols[i])
            plt.plot(X, Y_pred,
                     color=list(mcolors.TABLEAU_COLORS.values())[j],
                     linestyle=linestyles[i])
    ax.legend()

    plt.show()
