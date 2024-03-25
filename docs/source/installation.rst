Installation
============

YAGIT requires C++ compiler that supports C++17 standard.
It also requires `CMake`_ (3.16 or newer).
It works on Linux and Windows.

.. _CMake: https://cmake.org/

YAGIT uses these dependencies:

.. rst-class:: list

- `GDCM`_ -- used for reading DICOM files.
- `xsimd`_ -- used for vectorization. Used only when ``GAMMA_VERSION=SIMD`` or ``GAMMA_VERSION=THREADS_SIMD``.
- `GoogleTest`_ -- used for unit tests. Used only when ``BUILD_TESTING=ON``.

.. _GDCM: https://github.com/malaterre/GDCM
.. _xsimd: https://github.com/xtensor-stack/xsimd
.. _GoogleTest: https://github.com/google/googletest

You can build and install YAGIT using `setup script <#setup-script-install>`_
or doing it `manually <#manual-install>`_.


Setup script installation
-------------------------

To build YAGIT, simply run *setup.sh* or *setup.bat* script, depending on your operating system.

On Linux, run:

.. rst-class:: code
.. code-block::

   ./setup.sh

On Windows, run:

.. rst-class:: code
.. code-block::

   setup.bat


Note that, by default these scripts will only build the library but not install it.
To install it, configure the options appropriately in the script file.

For now, the Windows script can only install dependencies using `Conan`_ package manager.

.. _Conan: https://conan.io/


Script options
~~~~~~~~~~~~~~

You can freely configure options that are in the script files.
Some of the options are the same as in the `CMake YAGIT options`_ section as they are simply passed to CMake.

.. rst-class:: wrap-text
.. table::
   :align: center

   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | Option                        | Values                 | Default     | Description                                |
   +===============================+========================+=============+============================================+
   | ``BUILD_TYPE``                | ``Release``, ``Debug``,| ``Release`` | Build type used in the script              |
   |                               | ``RelWithDebInfo``,    |             | by CMake for single-target generators      |
   |                               | ``MinSizeRel``         |             | (e.g. Makefile Generators) and             |
   |                               |                        |             | by a compiler for multi-target generators  |
   |                               |                        |             | (e.g. Visual Studio Generators).           |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``BUILD_SHARED_LIBS``         | ``ON``, ``OFF``        | ``OFF``     | Equivalent to the CMake YAGIT option.      |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``INSTALL_DEPENDENCIES``      | ``OFF``, ``LOCAL``     | ``LOCAL``   | Method of installing dependencies.         |
   |                               | ``GLOBAL``, ``CONAN``  |             | Value ``OFF`` doesn't install any          |
   |                               |                        |             | dependencies, and it assumes               |
   |                               |                        |             | they are already installed system-wide.    |
   |                               |                        |             | Value ``LOCAL`` installs dependencies      |
   |                               |                        |             | locally to *build/deps* directory.         |
   |                               |                        |             | Value ``GLOBAL`` installs dependencies     |
   |                               |                        |             | system-wide. It requires admin privileges. |
   |                               |                        |             | Value ``CONAN`` installs dependencies      |
   |                               |                        |             | using `Conan`_ package manager.            |
   |                               |                        |             | It requires installed Conan.               |
   |                               |                        |             | This option is only availble in *setup.sh*.|
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``GAMMA_VERSION``             | ``SEQUENTIAL``,        | ``THREADS`` | Equivalent to the CMake YAGIT option.      |
   |                               | ``THREADS``,           |             |                                            |
   |                               | ``SIMD``,              |             |                                            |
   |                               | ``THREADS_SIMD``       |             |                                            |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``SIMD_EXTENSION``            | ``DEFAULT``,           | ``DEFAULT`` | Equivalent to the CMake YAGIT option.      |
   |                               | ``SSE2``, ``SSE3``,    |             |                                            |
   |                               | ``SSSE3``, ``SSE4.1``, |             |                                            |
   |                               | ``SSE4.2``, ``AVX``,   |             |                                            |
   |                               | ``AVX2``, ``AVX512``,  |             |                                            |
   |                               | ``NATIVE``             |             |                                            |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``ENABLE_FMA``                | ``ON``, ``OFF``        | ``OFF``     | Equivalent to the CMake YAGIT option.      |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``BUILD_EXAMPLES``            | ``ON``, ``OFF``        | ``ON``      | Equivalent to the CMake YAGIT option.      |
   |                               |                        |             | Additionally it runs examples.             |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``BUILD_TESTING``             | ``ON``, ``OFF``        | ``OFF``     | Equivalent to the CMake YAGIT option.      |
   |                               |                        |             | Additionally it runs unit tests.           |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``BUILD_PERFORMANCE_TESTING`` | ``ON``, ``OFF``        | ``OFF``     | Equivalent to the CMake YAGIT option.      |
   |                               |                        |             | Additionally it runs performance tests.    |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``REF_IMG``                   | ---                    | ---         | Path to a reference image                  |
   |                               |                        |             | used in examples and performance tests.    |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``EVAL_IMG``                  | ---                    | ---         | Path to an evaluated image                 |
   |                               |                        |             | used in examples and performance tests.    |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``BUILD_DOCUMENTATION``       | ``ON``, ``OFF``        | ``OFF``     | Build documentation (*docs* directory).    |
   |                               |                        |             | It requires `Doxygen`_,                    |
   |                               |                        |             | `Python`_ and libraries                    |
   |                               |                        |             | listed in *docs/requirements.txt*.         |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``INSTALL``                   | ``ON``, ``OFF``        | ``OFF``     | Install YAGIT library.                     |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``INSTALL_DIR``               | ---                    | ./yagit     | Directory where YAGIT will be installed    |
   |                               |                        |             | (only if ``INSTALL=ON``).                  |
   |                               |                        |             | If this is set to an empty string,         |
   |                               |                        |             | then it will be installed system-wide.     |
   +-------------------------------+------------------------+-------------+--------------------------------------------+


.. _Doxygen: https://www.doxygen.nl/
.. _Python: https://www.python.org/


Manual installation
-------------------

If you prefer to perform the installation manually without the script,
then follow the commands below to build and install YAGIT on Linux.
Before running them, ensure that you have installed the dependencies
(e.g. using a package manager or by building from source).

.. code-block::

   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build . -j
   sudo cmake --install .


If you don't have admin privileges or you want to install in a specific directory, then use this command:

.. rst-class:: code
.. code-block::

   cmake --install . --prefix <yagit installation path>


CMake YAGIT options
-------------------

.. rst-class:: wrap-text
.. table::
   :align: center

   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | Option                        | Values                 | Default     | Description                                |
   +===============================+========================+=============+============================================+
   | ``BUILD_SHARED_LIBS``         | ``ON``, ``OFF``        | ``OFF``     | Build YAGIT as shared or static library.   |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``BUILD_EXAMPLES``            | ``ON``, ``OFF``        | ``OFF``     | Build examples                             |
   |                               |                        |             | (*examples* directory).                    |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``BUILD_TESTING``             | ``ON``, ``OFF``        | ``OFF``     | Build unit tests                           |
   |                               |                        |             | (*tests/unit* directory).                  |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``BUILD_PERFORMANCE_TESTING`` | ``ON``, ``OFF``        | ``OFF``     | Build performance tests                    |
   |                               |                        |             | (*tests/performance* directory).           |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``GAMMA_VERSION``             | ``SEQUENTIAL``,        | ``THREADS`` | Implementation of an algorithm calculating |
   |                               | ``THREADS``,           |             | the gamma index.                           |
   |                               | ``SIMD``,              |             |                                            |
   |                               | ``THREADS_SIMD``       |             |                                            |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``SIMD_EXTENSION``            | ``DEFAULT``,           | ``DEFAULT`` | SIMD instruction set extension.            |
   |                               | ``SSE2``, ``SSE3``,    |             | Works only when ``GAMMA_VERSION`` is       |
   |                               | ``SSSE3``, ``SSE4.1``, |             | set to ``SIMD`` or ``THREADS_SIMD``.       |
   |                               | ``SSE4.2``, ``AVX``,   |             | Value ``DEFAULT`` adds no additional       |
   |                               | ``AVX2``, ``AVX512``,  |             | compilation flags.                         |
   |                               | ``NATIVE``             |             | Value ``NATIVE`` adds *-march=native*      |
   |                               |                        |             | if compiler supports it.                   |
   +-------------------------------+------------------------+-------------+--------------------------------------------+
   | ``ENABLE_FMA``                | ``ON``, ``OFF``        | ``OFF``     | Enable fused multiply-add (FMA)            |
   |                               |                        |             | when building YAGIT library.               |
   +-------------------------------+------------------------+-------------+--------------------------------------------+

To use these options, pass them to CMake during configuration using ``-D<option>=<value>``
(e.g. ``cmake .. -DGAMMA_VERSION=THREADS_SIMD -DSIMD_EXTENSION=AVX2``).


CMake YAGIT integration
-----------------------

Here is an example of adding YAGIT to your CMake project.

.. code-block:: cmake

   find_package(yagit REQUIRED)
   add_executable(program program.cpp)
   target_link_libraries(program PRIVATE yagit::yagit)

If YAGIT has been installed in a custom location, then you need to set the ``CMAKE_PREFIX_PATH`` option
to the path of the installed YAGIT. This is necessary for locating the *yagitConfig.cmake* file.


Including YAGIT
---------------

To include whole YAGIT library in your code, use the line below.

.. code-block:: c++

   #include <yagit/yagit.hpp>
