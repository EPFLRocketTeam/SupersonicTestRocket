# -*- coding: utf-8 -*-
""" Class that contains the multiple graphs and widgets to display data
Created on Mon Jul 26 20:48:08 2021

@author: newbi
"""

import matplotlib.pyplot as plt
from Dashboard_Subplot import Dashboard_Subplot

class Dashboard:
    def __init__(self):
        # create the graphs
        self.fig = plt.figure()
        self.fig.suptitle("HERMES Ground Station Dashboard")
        self.positionGraph = Dashboard_Subplot(
            plt.subplot2grid((3,6), (0,0), colspan=2,
                             ylabel=r"$Position, \vec X\ [m]$"))
        self.velocityGraph = (
            plt.subplot2grid((3,6), (1,0), colspan=2,
                             sharex=self.positionGraph.ax,
                             ylabel=r"$Velocity, \vec V\ [m/s]$"))
        self.accelerationGraph = (
            plt.subplot2grid((3,6), (2,0), colspan=2,
                             sharex=self.positionGraph.ax,
                             xlabel=r"$Time, t\ [s]$",
                             ylabel=r"$Acceleration, \vec A\ [m/s^2]$"))
        self.rotationGraph = (
            plt.subplot2grid((3,6), (0,3), colspan=2,
                             sharex=self.positionGraph.ax,
                             ylabel=r"$Rotation, \vec \omega\ [^\circ/s]$"))
        self.pressureGraph = (
            plt.subplot2grid((3,6), (1,3), colspan=2,
                             sharex=self.positionGraph.ax,
                             ylabel=r"$Pressure, P\ [kPa]$"))
        self.temperatureGraph = (
            plt.subplot2grid((3,6), (2,3), colspan=2,
                             sharex=self.positionGraph.ax,
                             xlabel=r"$Time, t\ [s]$",
                             ylabel=r"$Temperature, T\ [^\circ C]$"))
        plt.subplots_adjust(left=0.05, right=0.95)
        
    def plotData(self):
        pass
        
        
if __name__ == "__main__":
    DSB = Dashboard()