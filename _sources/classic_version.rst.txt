Classic version
===============

Algorithm
.........

Calculating Gamma Index function for every point |ref| in the reference image matrix requires finding a minimum of the |Gamma| function specified in the domain (|ref|, |tar|), where |tar| is a voxel in the target image. In the classic version of the algorithm |tar| can be any point within the target image, but often, in order to speed up the computation, some sort of limitation is introduced - the |tar| set is reduced to an arbitrarily specified rectangle with the center in the currently considered |ref| point.
The formula for |Gamma| (|ref|, |tar|) function is as follows:

.. math::

   \Gamma (r_r, r_e) = \sqrt {{\frac{|r_e - r_r|}{DTA}^2 + \frac{(D_e(r_e) - D_r(r_r))}{(DD*refVal)}^2}}

The first component under the root is calculated based on the distance between considered voxels (\|\ |tar| - |ref|\|). This distance is then normalized by a parameter called DTA (as this component comes from the metric called *Distance To Agreement*).

The second component is calculated based on the difference of values in the given points :math:`(D_r(r_r), D_e(r_e)` - values of the reference and target images in voxels |ref| and |tar|, respectively). The normalization value is calculated as a product of two factors: the first one is an arbitrary parameter DD (the name after *Dose Difference* metric), the second is *refVal* and it varies depending on the algorithm version:

 - in *global* version (most commonly used) the *refVal* is constant and  set to the maximal value in the reference image.
 - in *local* version the *refVal* is set to the value of currently considered |ref| voxel.

As a result of the whole process, an image containing Gamma Index values is produced. This image has the same spatial parameters (location in space, the voxel size and the resolution) as the reference image.

Unfortunately, the classic version requires visiting every |tar| point for each |ref| voxel, which results in dramatically high complexity (:math:`O(N^{2k})`, where *N* is the length of the image's side in voxels, and *k* is the number of image's dimensions). This makes the calculation very time consuming, thus the classic version cannot be used on the daily basis and some changes in the algorithm had to be made in order to make this metric useful. That is why we created the spiral version of the algorithm.

.. |ref| replace:: *r*\ :sub:`r`
.. |tar| replace:: *r*\ :sub:`e`
.. |Gamma| replace:: :math:`\Gamma`

Example
.......

This section presents an example of the classic version of Gamma Index algorithm showing step by step the comparison of two 2D data sets of size 2x2 with a bit different location in space.

.. image:: images/classic_example_1.png

Matrices *M1*, *M2* are attached below. We will perform comparison in which *M1* will be the reference matrix, and *M2* will be the target matrix. The matrix *M1* is composed of pixels 1,2,3,4. The matrix *M2* is composed of pixels I,II,III,IV.

.. image:: images/classic_example_2.png

The resulting Gamma Index matrix has the same size, spacing and position in space as the reference matrix. The initial value is defined by user in the limit parameter.

.. image:: images/classic_example_3.png

The comparison parameters:
 - DD - 3%
 - DTA - 3 mm
 - limit - 20.0
 - version - global
 - refVal - the maximal value of the reference image (which in our case is 1.0)

The initial Gamma Index matrix has then the following form.

.. image:: images/classic_example_4.png

The reference array is composed of 4 pixels, so the process of calculating the matrix can be divided into 4 main stages - in each there will be calculated Gamma Index value for a subsequent reference point.
The formula for the value of the Gamma Index is as follows:

.. image:: images/classic_example_4_1.png

First stage.

+------------+----------+------------------------------------------+-------------+------------------+
|| Reference || Target  ||               Distance                  || Dose value || GI for given    |
|| point     || point   ||                                         || difference || pair of points  |
+============+==========+==========================================+=============+==================+
|  1         |  I       |    :math:`\sqrt{1^2 + 1^2} = \sqrt{2}`   |    0.03     |      1.0554      |
+------------+----------+------------------------------------------+-------------+------------------+
|  1         |  II      |    :math:`\sqrt{1^2 + 2^2} = \sqrt{5}`   |    0.06     |      2.1344      |
+------------+----------+------------------------------------------+-------------+------------------+
|  1         |  III     |    :math:`\sqrt{2^2 + 1^2} = \sqrt{5}`   |    0.02     |      1.0         |
+------------+----------+------------------------------------------+-------------+------------------+
|  1         |  IV      |    :math:`\sqrt{2^2 + 2^2} = \sqrt{8}`   |    0.0      |  **0.9428**      |
+------------+----------+------------------------------------------+-------------+------------------+

Gamma Index matrix after the first stage.

.. image:: images/classic_example_5.png

Second stage:

+------------+----------+------------------------------------------+-------------+------------------+
|| Reference || Target  ||               Distance                  || Dose value || GI for given    |
|| point     || point   ||                                         || difference || pair of points  |
+============+==========+==========================================+=============+==================+
|  2         |  I       |    :math:`\sqrt{0^2 + 1^2} = \sqrt{1}`   |    0.0      |    **0.3333**    |
+------------+----------+------------------------------------------+-------------+------------------+
|  2         |  II      |    :math:`\sqrt{1^2 + 1^2} = \sqrt{2}`   |    0.03     |      1.1055      |
+------------+----------+------------------------------------------+-------------+------------------+
|  2         |  III     |    :math:`\sqrt{0^2 + 2^2} = 2`          |    0.05     |      1.7951      |
+------------+----------+------------------------------------------+-------------+------------------+
|  2         |  IV      |    :math:`\sqrt{1^2 + 2^2} = \sqrt{5}`   |    0.03     |      1.2472      |
+------------+----------+------------------------------------------+-------------+------------------+

Gamma Index matrix after the second stage.

.. image:: images/classic_example_6.png

Third stage.

+------------+----------+------------------------------------------+-------------+------------------+
|| Reference || Target  ||               Distance                  || Dose value || GI for given    |
|| point     || point   ||                                         || difference || pair of points  |
+============+==========+==========================================+=============+==================+
|  3         |  I       |    :math:`\sqrt{1^2 + 0^2} = 1`          |    0.07     |      2.3570      |
+------------+----------+------------------------------------------+-------------+------------------+
|  3         |  II      |    :math:`\sqrt{1^2 + 1^2} = \sqrt{2}`   |    0.02     |    **0.8165**    |
+------------+----------+------------------------------------------+-------------+------------------+
|  3         |  III     |    :math:`\sqrt{2^2 + 0^2} = 2`          |    0.1      |      3.993       |
+------------+----------+------------------------------------------+-------------+------------------+
|  3         |  IV      |    :math:`\sqrt{2^2 + 1^2} = \sqrt{5}`   |    0.04     |      2.2360      |
+------------+----------+------------------------------------------+-------------+------------------+

Gamma Index matrix after the third stage.

.. image:: images/classic_example_7.png

Fourth stage.

+------------+----------+------------------------------------------+-------------+------------------+
|| Reference || Target  ||               Distance                  || Dose value || GI for given    |
|| point     || point   ||                                         || difference || pair of points  |
+============+==========+==========================================+=============+==================+
|  4         |  I       |    :math:`\sqrt{0^2 + 0^2} = 0`          |    0.05     |      1.6667      |
+------------+----------+------------------------------------------+-------------+------------------+
|  4         |  II      |    :math:`\sqrt{1^2 + 0^2} = \sqrt{1}`   |    0.08     |      2.6874      |
+------------+----------+------------------------------------------+-------------+------------------+
|  4         |  III     |    :math:`\sqrt{0^2 + 1^2} = \sqrt{1}`   |    0.0      |    **0.3333**    |
+------------+----------+------------------------------------------+-------------+------------------+
|  4         |  IV      |    :math:`\sqrt{1^2 + 1^2} = \sqrt{2}`   |    0.02     |      0.8165      |
+------------+----------+------------------------------------------+-------------+------------------+

Gamma Index matrix after the final stage.

.. image:: images/classic_example_8.png
