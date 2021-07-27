# -*- coding: utf-8 -*-
""" Class that contains one subplot with the relevant widgets
Created on Mon Jul 26 21:04:06 2021

@author: newbi
"""

import matplotlib.pyplot as plt

class Dashboard_Subplot():      
    def __init__(self, ax):
        self.ax = ax
        
    def setData(self):
        self.text = self.ax.text(0.75, 0.5, "Hello",
                     fontsize=14, transform=plt.gcf().transFigure)
        
    def changeText(self, textstr):
        self.text.set_text(textstr)