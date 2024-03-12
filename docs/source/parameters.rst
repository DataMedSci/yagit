Parameters
==========

Gamma index is configurable by adjusting its parameters. They impact the results and the computation time.


General parameters
------------------

Algorithms calculating the gamma index has several parameters:

.. rst-class:: list

- DD acceptance criterion :math:`\Delta D` [%],
- DTA acceptance criterion :math:`\Delta d` [mm],
- normalization:

    - global -- a predetermined value, typically the maximum value in the reference image,
    - local -- value at the current point in the reference image,

- dose cutoff (DCO) -- the gamma index will not be calculated for reference points
  with radiation dose values below this threshold.

Frequently used parameters in radiotherapy are values within the range of 1--3% for the acceptance criterion DD
with global normalization and 1--3 mm for the acceptance criterion DTA.
These parameters are shortened, e.g. 3%G/3mm (3% DD criterion, global normalization, 3 mm DTA criterion).

In YAGIT, in the case of local normalization, when the dose value at the current reference point is 0,
that point is skipped to avoid division by zero.
Similarly, in the case of a dose cutoff value greater than 0,
if a reference point with a value below this threshold is encountered, such a point is also skipped.
In both cases, the gamma index value will be set to NaN.
When calculating GIPR, NaN values are not taken into account in either the numerator or the denominator.

Wendling method parameters
--------------------------

There are also two parameters exclusive to the Wendling method:

.. rst-class:: list

- maximum search distance [mm] -- the radius of the circle/sphere that will be searched in the evaluated image,
- step size [mm] -- the spacing between searched points inside the circle/sphere.

The maximum search distance should be at least equal to the DTA acceptance criterion,
but it is preferable to be several times greater for better gamma index results.
The step size should be :math:`\frac{1}{10}` of the DTA acceptance criterion,
as recommended by the authors of this method, to achieve better results.

Instead of using maximum search distance, some other tools use the maximum gamma searched for.
The relation between these two parameters is as follows

.. math::
    g = \frac{s}{\Delta d}

| where:
| :math:`g` -- maximum gamma searched for, so that :math:`\frac{r(\vec{r_r}, \vec{r_e})}{\Delta d} \le g`,
| :math:`s` -- maximum search distance, so that :math:`r(\vec{r_r}, \vec{r_e}) \le s`,
| :math:`\Delta d` -- DTA acceptance criterion.


Impact of parameters changes on computation time
------------------------------------------------

DD and DTA
~~~~~~~~~~

The change of DD and DTA parameters in the classic method has no impact on the computation time,
because changing them doesn't affect the number of operations performed.

In Wendling method, the computation time is inversely proportional to the value of the DD criterion
(:math:`t \sim \frac{1}{\Delta D}`) and proportional to the value of the DTA criterion (:math:`t \sim \Delta d`).
The proportionality with the DTA criterion holds true when only the DTA criterion is changed,
without adjusting maximum search distance and step size to it.
Changing DD and DTA acceptance criteria affects when the stopping condition will occur during the search
of the circle/sphere, thus also impacting the computation time.

Normalization
~~~~~~~~~~~~~

In the classical method, changing the normalization from global to local reduces computation time
due to skipping all reference points with a value of 0.
There are many such points (usually around 50%) in an average reference image.

However, in Wendling method, the runtime increases because when using local normalization
for reference points with small doses, the stopping condition occurs much later.
These problematic points can be skipped by appropriately setting the dose cutoff.

Dose cutoff
~~~~~~~~~~~

Changing the dose cutoff value can significantly affect computation time because typical images contain 
a large background region (an area where the dose value is equal to 0)
and numerous low-dose regions (areas where the dose value is relatively low compared to other regions).
In both methods, the larger the dose cutoff, the shorter the computation time.
