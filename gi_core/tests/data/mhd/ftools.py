# Import of FREDtools and other useful modules
import numpy as np
import matplotlib.pyplot as plt
import time


import fredtools as ft
print("Current FREDtools version", ft.__version__)

imgRef = ft.readMHD("Ref.mhd", displayInfo=True) # voxel size [2.5 2.5 2.4]
imgEval = ft.readMHD("Eval.mhd", displayInfo=True) # voxel size [1.5 1.5 1.5]

# Many options can be changed when calculating the GI. Refer to the documentation for more details

"""
By default it is only needed to specify the reference and evaluation images and the calculation criteria, 
like dose-difference, distance-to-agreement and dose-cutoff.
"""
imgGI=ft.calcGammaIndex(imgRef, imgEval, DD=2, DTA=2, DCO=0.05, displayInfo=True)


"""
To calculate the GI statistics use the following function which returns a dictionary with the statistical parameters.
"""
start_time = time.time()
statGI=ft.getGIstat(imgGI, displayInfo=True)
end_time = time.time()

print(f'Time taken: {end_time - start_time}')