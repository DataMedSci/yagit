Gamma Index
===========

Concept 
-------
*Gamma Index* is a metric used for comparing two images (called reference and target) of dose distribution. It allows to express the difference between the images in a given point in space with a number, which is very important for performing a Quality Assurance.

This technique, developed by `Low at al. (1998) <https://dx.doi.org/10.1118/1.598248>`_, is based on two other metrics of dose distribution comparison: *Dose Difference* (DD) and *Distance To Agreement* (DTA). Gamma Index as a metric is quite sophisticated in comparison to the other two, but it combines them in such way that eliminates their deficiencies and exposes their advantages.

Despite the high complexity, Gamma Index has gained lot of popularity in the radiotherapy treatment - it is used for planning and evaluating the radiation dose that is administered to a patient. However, to become useful on a daily basis, it requires a smart tool for performing the Gamma Index calculations, which would make the computation time acceptable for the user.

The gamma test (Gamma Index is also sometimes called that way) is considered as passed if its value is lower than 1. It is achievable only if its both components (DD and DTA) also have the value below 1.

Constituent metrics
--------------------
*Dose Difference* - a metric that calculates a difference between doses in two corresponding points of reference and target images. If this difference is smaller then some certain value (e.g. 3% of the maximum of the reference image), then these points pass the test. *Dose Difference*  is most efficient in areas of small gradient and is less reliable in parts with high variability.

*Distance To Agreement* - a metric based on the distance between given point of the reference image and the closest point of the target image, with the equal value. This test is considered as passed, if the distance is smaller than some arbitrary parameter (e.g. 3mm). It it most efficient in high gradient, but does very poorly in small gradient regions.

Detailed descriptions of DTA and DD can be found on:
http://medphys365.blogspot.com/2012/05/comparing-dose-distributions-dta-and.html
