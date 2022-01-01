import SimpleITK as sitk
import numpy as np
from array import array
import sys

def load_itk_image(filename):
    itkimage = sitk.ReadImage(filename)
    numpyImage = sitk.GetArrayFromImage(itkimage)
    return numpyImage, itkimage.GetOrigin(), itkimage.GetSpacing(), (bool)(itkimage.GetPixelID() == sitk.sitkFloat32)


if __name__ == '__main__':
    input_filename = sys.argv[1]
    output_filename = sys.argv[2]
    image, origin, spacing, isfloat32 = load_itk_image(input_filename)
    with open(output_filename, "wb") as result:
        print(sys.byteorder)
        print(f'isfloat32: {isfloat32}')
        print(f'shape: {image.shape}')
        print(f'origin: {origin}')
        print(f'spacing: {spacing}')

        format_type = array('Q', [1 if isfloat32 else 0, image.shape[0], image.shape[1], image.shape[2]])# image shape (width/height/depth)
        format_type.tofile(result)
        metadata_array = array('f' if isfloat32 else 'd', [
            origin[0], origin[1], origin[2], # image position (x,y,z)
            spacing[0], spacing[1], spacing[2], # pixel spacing (x,y,z)
        ])
        metadata_array.tofile(result)
        image.astype(np.float32 if isfloat32 else np.float64).tofile(result)
