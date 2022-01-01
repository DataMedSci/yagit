import numpy as np
import sys
from array import array
import pydicom


def load_dcm_image(filename):
    dcmimage = pydicom.dcmread(filename)
    scaling = dcmimage.get(0x3004000e)
    spacing = dcmimage.get(0x00280030)
    z_spacing = dcmimage.get(0x00180088)
    if z_spacing is None:
        z_spacing = dcmimage.get(0x3004000c)
        if z_spacing is not None:
            z_spacing = z_spacing.value
            z_spacing = max(z_spacing) / (len(z_spacing) - 1)
    origin = dcmimage.get(0x00200032)
    numpyImage = np.array(dcmimage.pixel_array)
    if scaling is not None:
        numpyImage = numpyImage / scaling.value
    numpyImage = numpyImage.astype(np.double)
    return numpyImage, origin.value, [spacing.value[0], spacing.value[1], z_spacing], False


if __name__ == '__main__':
    input_filename = sys.argv[1]
    output_filename = sys.argv[2]
    image, origin, spacing, isfloat32 = load_dcm_image(input_filename)
    with open(output_filename, "wb") as result:
        print(sys.byteorder)
        print(f'isfloat32: {isfloat32}')
        print(f'shape: {image.shape}')
        print(f'origin: {origin}')
        print(f'spacing: {spacing}')

        format_type = array('Q', [1 if isfloat32 else 0, image.shape[0], image.shape[1], image.shape[2]])  # image shape (width/height/depth)
        format_type.tofile(result)
        metadata_array = array('f' if isfloat32 else 'd', [
            origin[0], origin[1], origin[2],  # image position (x,y,z)
            spacing[0], spacing[1], spacing[2],  # pixel spacing (x,y,z)
        ])
        metadata_array.tofile(result)
        image.astype(np.float32 if isfloat32 else np.float64).tofile(result)
