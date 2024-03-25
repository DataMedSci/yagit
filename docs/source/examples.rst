Examples
========

Examples are located in the *examples* directory. To build them, set the option ``BUILD_EXAMPLES=ON``.


2D gamma index using classic method for simple images
-----------------------------------------------------

This example shows how to use YAGIT even without any input files.

.. literalinclude:: ../../examples/gammaSimple.cpp
    :language: cpp
    :linenos:
    :lines: 19-


3D gamma index using Wendling method
------------------------------------

This example shows the most useful use case -- 3D gamma index using the Wendling method.
The Wendling method is more preferred than the classic method
because it computes the gamma index faster and provides more accurate results,
thanks to the ability to set a small step size.

.. literalinclude:: ../../examples/gamma3D.cpp
    :language: cpp
    :linenos:
    :lines: 19-


2D gamma index using classic method with interpolation
------------------------------------------------------

This example demonstrates the classic method with interpolation of the evaluated image
to achieve more accurate results (though at the expense of longer computations).
In this example, the 2D version is used instead of the 3D version
because the classic method for the 3D version can take a very long time to compute.

.. literalinclude:: ../../examples/gammaWithInterp.cpp
    :language: cpp
    :linenos:
    :lines: 19-
