Spiral version
==============

The idea
........

In the classic version for given |ref| point, |tar| points are visited in the same way that you read this text: row by row, from left to right, starting from the upper left corner. It is  not the most beneficial way of performing the search for the minimum of Gamma function, since the most probable place to find the match is close to the |ref| point, where the *dta* component has the smallest value (*dta* component is proportional to the distance between |ref| and |tar| point).

So we decided that, instead of starting in the upper left corner, we would begin the calculations in the |tar| point with the same coordinates as the current |ref| point. Then the next points will be chosen with the increasing distance from |ref| point in some kind of spiral (that is why we call this version spiral).

Thanks to this approach we are able to stop the computations when the *dta* component is so big that it exceeds the already found minimum of Gamma Index. We have then the certainty that none of the furthest points on the spiral will give a better Gamma Index value. 

Spiral construction
...................

In order to maximize the computation acceleration we needed a way of constructing a universal spiral, that is a spiral that would be created once per comparison of two images, at the beginning of the calculations, and then for each |ref| point it would be, at most, slightly modified. This way we could save some time because we would not need to create the spiral multiple times.

We benefited from the fact that the images are specified on a regular grid. Thanks to this, it was possible to create a spiral that would preserve its shape for all the |ref| points and only its center point would be moved to the currently considered |ref| point.

While constructing the spiral, first we find the maximal possible distance between |ref| and |tar| point along each axis. For this purpose we calculate the difference of coordinates between the extreme points of the images. Then, taking into consideration the spacing of the target image, we calculate how many points along each axis should create the spiral and then we create the matrix of this size. The matrix is then filled with the distance of the given point from the center of the spiral. At the end we sort the matrix, and by that we obtain the order of the points in the spiral.

We would like to emphasize that nowhere in the construction process did we specify the location of the center of the spiral. We only specified its shape. Thanks to this, while performing calculations for consecutive |ref| points, we only need to move the spiral so that it starts in the given |ref| point (of course, spiral exists in the target image, what we mean is that the spiral should begin in the |tar| point with the coordinates same as the current |ref| point). Also during computations we need to check, whether the current point on the spiral does not lie outside of the target image (the universal spiral is greater than the target image so that it could cover all potential cases).

All the above actions are performed by the *SpiralSolver* member function: *getIndicesOrder()*. It returns the array of objects of type *Voxel* specifying the consecutive points of the spiral (their coordinates and distance from the center of the spiral).


Termination condition
.....................

As mentioned before, the termination condition was introduced in order to prevent performing computations that have no chance of giving a desired result, when the *dta* component exceeds the value of the minimum of Gamma Index.

Unfortunately, we are not able to use so simple termination condition, like comparison between the *dta* component and the current Gamma Index value. The reason for this is that, despite the claims above, the center of the spiral may not overlap with the given |ref| point. This can happen for a very simple reason - such |tar| point may not exist, the images are specified only in certain point, on a grid, and since our algorithm does not use interpolation we have no way of obtaining the value of target image in the desired |tar| point.

Because of this, between the |ref| point and the center of the spiral there may emerge a gap, we called this distance *closest*. While comparing points from spiral with current |ref| point, there may occur a situation, when *dta* component exceeds the Gamma Index value, and according to the above assumptions we should end the computations for given |ref| point. But because of the gap, it may turn out that the next |tar| point on the spiral, despite being located further or as close as the previous one form the center of the spiral, lies a bit closer to the |ref| point. In the face of this fact we had no choice but to weaken our termination condition. While checking the stopping criterion we calculate *dta* based on the distance of current |tar| point from the center of the spiral reduced by the value of *closest*. This gives us the certainty that if given point fails the termination condition, then all the next points will not have the *dta* component smaller than the already found Gamma Index value.

Summary
.......

Thanks to the modifications described above, we perform only a few comparisons per a reference point and we do not need to run through the whole target image for each reference point.

The classic version for high resolution images or 3-dimensional ones required a huge amount of time (counted in minutes or even hours) to perform a comparison. But for our spiral version it takes only about a minute, and often, in case of quite simmilar images, the computation lasts only a few seconds.


.. |ref| replace:: *r*\ :sub:`r`
.. |tar| replace:: *r*\ :sub:`e`
.. |Gamma| replace:: :math:`\Gamma`
