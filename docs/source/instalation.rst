Installation Guide
==================

This section explains how to install and run the *yet Another Gamma Index Tool*.

Prerequisites
.............
- C++ Compiler (GCC, Clang or Visual Studio)
- CMake 2.8.4+
- make, wget, unzip (Optionally and only for Linux, in case of building from script)

Obtaining the project sources
.............................
1. Clone project the `repository <https://bitbucket.iisg.agh.edu.pl/scm/pp2016/gamma-index.git>`_
2. Fetch and checkout to branch *final_version*

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

The script will download and install two libraries:

- Imebra C++ Distribution 4.0.8.1+ (https://imebra.com/)
- wxWidgets C++ Library 2.9.3+ (http://wxwidgets.org/)

Also, the following will be created:

a. *imebra/* - directory with the *Imebra* library
b. *gi_core/libgi_core.a* - core module of the *yAGIT* project compiled to a static library
c. *gi_additions/libgi_additions.a* - module of additions of the *yAGIT* project compiled to a static library
d. *gi_wrapper_cpp/libgi_wrapper.a* - wrapper module of the *yAGIT* project compiled to a static library
e. *examples/core_2d_client* - executable file presenting how to use the core module
f. *examples/wrapper_client* - executable file presenting how to use the core module
   (before running this example, the paths to DICOM images have to be updated manually in the source code, also, the images need to be provided by user)
g. *gi_gui/yAGIT* - executable file for running the Graphical User Interface of the *yAGIT* project


Building the project using CMake tool
`````````````````````````````````````

Building the dependency libraries
'''''''''''''''''''''''''''''''''
1. Build *Imebra* library:

   In the main directory of the project create *imebra/* folder. Download and unzip the `Imebra sources <https://imebra.com/get-it/>`_ (make sure that all *Imebra* subdirectories (like: *docs/*, *library/*, *wrappers/*, …) are placed directly in the *imebra/* folder)

2. Build *wxWidgets* library:

   .. code-block:: bash

      apt-get install libwxgtk3.0-dev


Building the project module
'''''''''''''''''''''''''''
1. In order to build a chosen module (*gi_core*, *gi_additions*, *gi_wrapper*, *gi_gui*, *examples*) go to the appropriate module directory
2. Inside the module directory create and change directory to *build/* folder
3. Build the module using two following commands:

   .. code-block:: bash

      cmake ..
      cmake --build . --config release

4. In the main directory of the module should appear:

   a. executable files (*yAGIT* or *core_2d_client* and *wrapper_client*) (in case of building modules *gi_gui* or *examples*) or
   b. static library files *\*.a* (in case of building other modules)

5. In order to build a module there is no need for building any of the other modules. Each module builds its dependencies itself.
6. Before building *examples* module user should provide paths to images under comparison in the source code files.



Compiling the Windows version of *yAGIT*
----------------------------------------

Building the *wxWidgets* library
````````````````````````````````
1. Download the latest version (let's call it *<WX_VERSION>*) of `wxWidgets sources <https://wxwidgets.org/downloads/>`_ for the Windows System.
2. Create folder *C:\\wxWidgets-<WX_VERSION>* (e.g. *C:\\wxWidgets-3.1.0*)
3. Unzip the archives into the created folder
4. Set the environment variable *WXWIN* to the path to the *wxWidgets* library (*C:\\wxWidgets-<WX_VERSION>*)

   a. Open: *Control Panel -> System and Security -> System*
   b. From the left menu choose: *Advanced system settings*
   c. Click the button: *Environment variables…*
   d. Under the list: *User variables…* click the button: *New…*
   e. Fill the field: *Variable name* with *WXWIN*
   f. Fill the field: *Variable value* with *C:\\wxWidgets-<WX_VERSION>*

5. Build the *wxWidgets* library

   a. Run the MS Visual Studio Console (e.g. in the Start Menu find VS2015 x86 x64 Cross Tools Command Prompt or something similar)
   b. Change directory to the library source folder:

      .. code-block:: batch

         cd C:\wxWidgets-<WX_VERSION>\build\msw

   c. Find installed version of Visual Studio (let's name it *<VS_VERSION>*):

      .. code-block:: batch

         msbuild /version

   d. Build the library:

      .. code-block:: batch

         msbuild /m:2 /v:n /p:Platform="Win32" /p:Configuration="Release" wx_vc<VS_VERSION>.sln

      (in case there was no solution (*\*.sln* file) in the version matching  the installed version of Visual Studio choose the closest one)


Building the project module
```````````````````````````
1. In order to build a chosen module (*gi_core*, *gi_additions*, *gi_wrapper*, *gi_gui*, *examples*) go to the appropriate module directory
2. Inside the module directory create and change directory to *build/* folder
3. Build the module using two following commands:

.. code-block:: batch

   cmake ..
   cmake --build . --config release

4. In the main directory of the module should appear:

   a. executable files (*yAGIT* or *core_2d_client* and *wrapper_client*) (in case of building modules *gi_gui* or *examples*) or
   b. static library files *\*.a* (in case of building other modules)

5. In order to build a module there is no need for building any of the other modules. Each module builds its dependencies itself.
6. Before building *examples* module user should provide paths to images under comparison in the source code files.
