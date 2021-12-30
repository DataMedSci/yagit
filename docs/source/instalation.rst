Installation Guide
==================

This section explains how to install and run the *yet Another Gamma Index Tool*.

Download
.............
You can get the installer for Windows and the binaries for Linux from the `release page <https://github.com/DataMedSci/gi-yagit/releases/latest>`_.

Prerequisites
.............
// TODO

Obtaining the project sources
.............................
1. Clone project the `repository <https://github.com/DataMedSci/gi-yagit.git>`_

Building the project
....................

Compiling the Linux version of *yAGIT*
--------------------------------------
The project on the Linux system can be installed in two ways: by using a deployment script or building only chosen modules using CMake tool.


Automatic installation of the project
`````````````````````````````````````
From the main directory of the project run the command:

.. code-block:: bash

    ./deploy.sh

The script will // TODO

Building the project using CMake tool
`````````````````````````````````````

Building the dependency libraries
'''''''''''''''''''''''''''''''''
// TODO


Building the project module
'''''''''''''''''''''''''''
1. In order to build a chosen module (*gi_core*, *gi_wrapper*, *gi_gui*) go to the appropriate module directory
2. Inside the module directory create and change directory to *build/* folder
3. Build the module using two following commands:

   .. code-block:: bash

      cmake ..
      cmake --build . --config release

4. In the main directory of the module should appear:

   a. executable files (*yAGIT* or // TODO) (in case of // TODO) or
   b. static library files *\*.a* (in case of building other modules)

5. In order to build a module there is no need for building any of the other modules. Each module builds its dependencies itself.


Compiling the Windows version of *yAGIT*
----------------------------------------

Building the project module
```````````````````````````
1. In order to build a chosen module (*gi_core*, *gi_wrapper*, *gi_gui*) go to the appropriate module directory
2. Inside the module directory create and change directory to *build/* folder
3. Build the module using two following commands:

.. code-block:: batch

   cmake ..
   cmake --build . --config release

4. In the main directory of the module should appear:

   a. executable files (*yAGIT* or // TODO) (in case of // TODO) or
   b. static library files *\*.a* (in case of building other modules)

5. In order to build a module there is no need for building any of the other modules. Each module builds its dependencies itself.
