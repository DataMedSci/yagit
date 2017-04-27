Gamma Index Wrapper example
===========================

This example shows how tp use yAGIT to compare two DICOM files.

In order to use the functionality of the yAGIT application described in this example user has to include following header files:

.. code-block:: cpp

    #include "additions.h"
    #include "dispatcher.h"
    #include "wrapper.h"
    #include "core_logger.h"
    #include "wrapper_logger.h" 

At first we need to specify a whole bunch of parameters.

.. code-block:: cpp

    string refFile = "images/dicoms/Dose_3D_org.dcm",
           tarFile = "images/dicoms/Dose_3D_meas.dcm",
           outFile = "images/dicoms/Dose_3D_org_meas.dcm";

    int algorithm = 2, precision = 3, tagStrategy = 4;
    double dims = 3.0, percentage = 3.0, dta = 3.0, referenceValue = -1.0, limit = 2.0, fillValue = 1.1*limit;
    bool local = false;

    // The tag set used to determine which tags should be transferred from reference DICOM into result DICOM file.
    // For more information on how to construct this set see applyDicomTagsRewriteStrategy().
    set <pair<int, int>> tagSet = {
                    {0x0018, 0x0050},	// Slice Thicknes
                    {0x0020, 0x0032},	// Image Position (Patient)
                    {0x0020, 0x0037},	// Image Orientation (Patient)
                    {0x0028, 0x0002},	// Samples per Pixel
                    {0x0028, 0x0008},	// Number of Frames
                    {0x0028, 0x0010},	// Rows
                    {0x0028, 0x0011},	// Columns
                    {0x0028, 0x0030},	// Pixel Spacing
                    {0x0028, 0x0100},	// Bits Allocated
                    {0x0028, 0x0101},	// Bits Stored
                    {0x0028, 0x0102},	// High Bit
                    {0x0028, 0x0103},	// Pixel Representation
                    {0x0028, 0x0004},	// Photometric Interpretation
                    {0x0028, 0x1052},	// Rescale Intercept
                    {0x0028, 0x1053},	// Rescale Slope
                    {0x3004, 0x0002},	// Dose Units
                    {0x3004, 0x0004},	// Dose Type
                    {0x3004, 0x000A},	// Dose Summation Type
                    {0x3004, 0x000C},	// Grid Frame Offset Vector
                    {0x3004, 0x000E},	// Dose Grid Scaling
                    {0x3004, 0x0014}	// Tissue Heterogeneity Correction
    };
    /***************************************/

    // Variables in which images parameters will be stored.
    int refNDims, tarNDims, refXNumber, refYNumber, refZNumber, tarXNumber, tarYNumber, tarZNumber;
    double refXStart, refYStart, refZStart, tarXStart, tarYStart, tarZStart,
           refXSpacing, refYSpacing, refZSpacing, tarXSpacing, tarYSpacing, tarZSpacing;

We can optionally enable a logging service which will store application logs in *log.txt* file.

.. code-block:: cpp

    initializeCoreLogger();
    initializeWrapperLogger();

Then we have to load the images from DICOM files. There is a special function *loadDicom()* prepared for this purpose.
It loads the image to a *DataSet* object and returns a pointer to this structure. Later this pointer should be passed to other functions.
Also the returned pointer should be put into an unique_ptr to avoid any troubles with memory management.

.. code-block:: cpp

    unique_ptr<imebra::DataSet> refDataSet(loadDicom(refFile));
    unique_ptr<imebra::DataSet> tarDataSet(loadDicom(tarFile));

Then using *acquireImage()* one can transform these *DataSets* into linearized arrays and extract the images spatial parameters.

.. code-block:: cpp

    double* reference = acquireImage(refDataSet.get(), refNDims,
                                     refXStart, refXSpacing, refXNumber,
                                     refYStart, refYSpacing, refYNumber,
                                     refZStart, refZSpacing, refZNumber);
    double* target = acquireImage(tarDataSet.get(), tarNDims,
                                  tarXStart, tarXSpacing, tarXNumber,
                                  tarYStart, tarYSpacing, tarYNumber,
                                  tarZStart, tarZSpacing, tarZNumber);

Now we are ready to perform Gamma Index comparison with :ref:`calculateGamma() <link-dispatcher>` function.

.. code-block:: cpp

    double* gamma = calculateGamma(algorithm, dims, reference, target,
                                   refXStart, refXSpacing, refXNumber,
                                   refYStart, refYSpacing, refYNumber,
                                   refZStart, refZSpacing, refZNumber,
                                   tarXStart, tarXSpacing, tarXNumber,
                                   tarYStart, tarYSpacing, tarYNumber,
                                   tarZStart, tarZSpacing, tarZNumber,
                                   percentage, dta, local, referenceValue, limit);

When the comparison is finished, the resulting Gamma Index image can be saved to a DICOM file using *saveImage()* function.

.. code-block:: cpp

    saveImage(dims, refDataSet.get(), outFile, gamma, refXNumber, refYNumber, refZNumber, precision, fillValue, tagStrategy, tagSet);

And the statistics can be also written to a file.

.. code-block:: cpp

    int gammaSize = calculateGammaArraySize(dims, refXNumber, refYNumber, refZNumber);
    string info =  createComputationDescription(refFile, tarFile, algorithm);
    string filepath = "stats.txt";
    prepareStatisticsFile(gammaSize, gamma, info, filepath);

At the end it is important to free the memory allocated for the images.

.. code-block:: cpp

    delete[] reference;
    delete[] target;
    delete[] gamma;
