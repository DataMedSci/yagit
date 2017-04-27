Gamma Index Core 2D example
===========================

In this section we describe an example of how to use the core functionality of yAGIT project - calculating Gamma Index of two linearized arrays.

In order to use the main functionality of the yAGIT application user has to include the following header files:

.. code-block:: cpp

    #include "additions.h"
    #include "dispatcher.h"

To call function :ref:`calculateGamma() <link-dispatcher>` it is required to pass such arguments as:

.. code-block:: cpp

    int algorithm = 2, dims = 2;
    double percentage = 3.0, dta = 3.0, referenceValue = -1, limit = 20.0;
    bool local = false;

    int refXNumber, refYNumber, refZNumber, tarXNumber, tarYNumber, tarZNumber;
    double refXStart, refYStart, refZStart, tarXStart, tarYStart, tarZStart,
           refXSpacing, refYSpacing, refZSpacing, tarXSpacing, tarYSpacing, tarZSpacing;

Though some parameters might not be used for a particular comparison (like the ...Z... parameters while comparing 2D images),
all of them must be passed into :ref:`calculateGamma() <link-dispatcher>` function.

There are also parameters not used in the 2D Gamma Index analysis,
which don't have to be passed into :ref:`calculateGamma() <link-dispatcher>` function.
They are used during 2.5D or 2D for 3D comparison:

.. code-block:: cpp

    int plane;
    int refSlice;
    int tarSlice;

First of all it is required to obtain reference and target images for the comparison.
For this example we prepared sample text files containing such images and *read2DMatrixFromTxt()* function that parses the image files and returns them as an linearized array with image spatial parameters.
These parameters are: coordinates of top left corner (start), pixel spacing and  number of pixels along each dimensions (resolution).
The image samples can be found under *gi_clients/images*.

.. code-block:: cpp

    string referencePath = "images" + string(OS_SEP) + "1.txt";
    string targetPath = "images" + string(OS_SEP) + "2.txt";

    double* reference = read2DMatrixFromTxt(referencePath,
                                            refXStart, refXSpacing, refXNumber,
                                            refYStart, refYSpacing, refYNumber);
    double* target = read2DMatrixFromTxt(targetPath,
                                         tarXStart, tarXSpacing, tarXNumber,
                                         tarYStart, tarYSpacing, tarYNumber);

After acquiring the images some preprocessing can be performed.
In the *additions module* there are prepared functions for applying linear mapping or filtering the noise.

.. code-block:: cpp

    applyLinearMappingToImage(refXNumber * refYNumber, reference, 1.1, 0.0);  // This will multiply the image by 1.1 and add 0.0.

    applyNoiseFilteringToImage(refXNumber * refYNumber, reference, 5.0);  // This will filter evey value in reference image
                                                                          // below threshold = minVal + 5% * (maxVal - minVal)..

The next step is the main stage - performing Gamma Index comparison using :ref:`calculateGamma() <link-dispatcher>` function.
Function returns linearized matrix with Gamma Index values.

.. code-block:: cpp

	double *gamma = calculateGamma(algorithm, dims, reference, target,
                                   refXStart, refXSpacing, refXNumber,
                                   refYStart, refYSpacing, refYNumber,
                                   refZStart, refZSpacing, refZNumber,
                                   tarXStart, tarXSpacing, tarXNumber,
                                   tarYStart, tarYSpacing, tarYNumber,
                                   tarZStart, tarZSpacing, tarZNumber,
                                   percentage, dta, local, referenceValue, limit);

After all operations user has to free the allocated memory themselves.

.. code-block:: cpp

    delete [] reference;
    delete [] target;
    delete [] gamma;
