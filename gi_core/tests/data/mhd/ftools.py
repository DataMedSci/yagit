# Import of FREDtools and other useful modules
import numpy as np
import matplotlib.pyplot as plt
import time
import SimpleITK as sitk
import sys
from array import array

import fredtools as ft
print("Current FREDtools version", ft.__version__)

imgRef = ft.readMHD(sys.argv[1], displayInfo=True) # voxel size [2.5 2.5 2.4]
imgEval = ft.readMHD(sys.argv[2], displayInfo=True) # voxel size [1.5 1.5 1.5]

# Many options can be changed when calculating the GI. Refer to the documentation for more details

"""
By default it is only needed to specify the reference and evaluation images and the calculation criteria, 
like dose-difference, distance-to-agreement and dose-cutoff.
"""
start_time = time.time()
imgGI=ft.calcGammaIndex(imgRef, imgEval, DD=2, DTA=200, DCO=0.05, displayInfo=True)
end_time = time.time()

"""
To calculate the GI statistics use the following function which returns a dictionary with the statistical parameters.
"""

statGI=ft.getGIstat(imgGI, displayInfo=True)

print(f'Time taken: {end_time - start_time}')

def load_itk_image(itkimage):
    numpyImage = sitk.GetArrayFromImage(itkimage)
    return numpyImage, itkimage.GetOrigin(), itkimage.GetSpacing(), (bool)(itkimage.GetPixelID() == sitk.sitkFloat32)

input_filename = sys.argv[1]
output_filename = sys.argv[2]
image, _, _, isfloat32 = load_itk_image(imgGI)
with open(f'comp_{sys.argv[1]}_{sys.argv[2]}.out', "wb") as result:
    print(sys.byteorder)
    print(f'isfloat32: {isfloat32}')
    print(f'shape: {image.shape}')

    format_type = array('Q', [1 if isfloat32 else 0, image.shape[0], image.shape[1], image.shape[2]])# image shape (width/height/depth)
    format_type.tofile(result)
    image.astype(np.float32 if isfloat32 else np.float64).tofile(result)