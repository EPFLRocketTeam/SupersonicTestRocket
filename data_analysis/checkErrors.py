# -*- coding: utf-8 -*-
"""
Created on Sat Jul 31 18:05:23 2021

@author: newbi
"""

import pandas as pd
import os

directory = "data/decodedDataError/000006"

a=pd.read_csv(os.path.join(directory,"ADIS16470_0.csv"))
print(a.loc[a["measInvalid"] == 1])
print(a.loc[a["accZ (g)"] == 0])
print(a["timestamp (us)"].max()/1000000)
b=pd.read_csv(os.path.join(directory,"RSC_pressure_1.csv"))
print(b.loc[b["measInvalid"] == 1])