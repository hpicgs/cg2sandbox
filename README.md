cg2sandbox
==========

### Table of Contents

 * [General Remarks](#general-remarks)
  * [Requirements](#requirements)
  * [Disclaimer](#disclaimer)
 * [Build Instructions](#build-instructions)
  * [Windows](#windows)
  * [Linux](#linux)
  * [OS X](#os-x)
 * [Third Party Libraries](#third-party-libraries)

## General Remarks

### Requirements

+ CMake 2.8.12
+ Qt 5.1
+ OpenGL support >= 3.2 (3.1 or below might work in some cases - use cmake option ```OPTION_NO_OPENGL_320```)
+ XCode/QtCreator/gcc/mingw/VisualStudio/etc..

### Disclaimer

+ FOR ASSESSMENT WE BUILD YOUR SUBMISSION IN WINDOWS WITH MSVC11 ONLY!
+ WE OFFICIALLY SUPPORT WINDOWS WITH MSVC11 ONLY!
+ Still, if you have problems on other platforms please let us know. 
We are dedicated to provide a framework that is working on all common desktop plattforms.


## Build Instructions

### Windows

General Requirements:

+ install cmake from
    http://www.cmake.org/cmake/resources/software.html
+ install qt5.1 from (for this exercise we used visual studio 2012 x64 and 2013 x64)
	http://qt-project.org/downloads
+ install visual studio or qt creator


### Windows - Visual Studio (the preferred way)

#### Prerequisites

+ Having a Visual Studio installed (MSVC10 or UP)

#### Building and Running an Exercise

+ Start cmake-gui with batch or manually (but provide QT_DIR and make sure the Qt binaries are on the path)
+ Open CMakeLists.txt with cmake-gui 
+ Specify path to build binaries (recommended in extra build directory) -> "build" folder
+ Press configure and generate
+ Open solution with batch
+ setup working directory of cg2sandbox to ".." for each configuration you want to run
+ compile and run as required...


### Windows - Qt Creator

#### Prerequisites

+ Use the qt creator 2.4 or newer (at best, the one that comes with qt 5.1)
+ Set the path for the CMake executable in Tools > Options > Build & Run > CMake.
+ Now you should be able to open CMakeLists.txt files (if not check http://doc.qt.digia.com/qtcreator-2.4/creator-project-cmake.html)
+ NOTE: you need a CDB if you want to compile with MSVC... http://msdn.microsoft.com/de-de/windows/hardware/hh852363 
+ Installing the Debugging Tools should suffice.

#### Building and Running an Exercise

+ File > Open File or Project
+ select the CMakeLists.txt
+ specify Build Directory
+ Press Run-CMake, then finish
+ Specify Working Directory via Projects > Run > Working Directory (see http://doc.qt.digia.com/qtcreator-2.3/creator-run-settings.html)
  to point to exercise folder, not the build folder
+ Compile (e.g. with [Ctrl] + [R])


### Windows - NMake

#### Prerequisites

+ Cmake is available via command line
+ You habe nmake installed and you can use ae.g.  Visual Studio Command Promt

#### Building and Running an Exercise

+ set PATH=%PATH%;"C:\Qt\5.1.0\qtbase\bin\"
+ mkdir build
+ cd build
+ cmake -DCMAKE_PREFIX_PATH="C:\Qt\5.1.0\qtbase\lib\cmake" -G "NMake Makefiles" .. # this worked for us... (perhaps you need to specify platform/compiler...)
+ nmake
+ cd ..
+ set QT_DIR=C:\Qt\5.1.0
+ set PATH=%PATH%;%QT_DIR%\qtbase\bin;
+ ./build/exercise1


### Linux

Only tested on Ubuntu 13.04 x64.

####  Gathering Requirements

+ install cmake, this should give you 2.8.10 or above (in Ubuntu 13.04)
	+ sudo apt-get install cmake
+ install a C++ compiler with support for C++11 (GCC 4.7 or higher is recommended)
	+ e.g. for Ubuntu 12.04:
		- add-apt-repository -y ppa:george-edison55/gcc4.7-precise
		- apt-get -y install gcc-4.7 g++-4.7
+ download qt 5.1.0 (http://qt-project.org/downloads)
+ suppose the filename is qt.run (replace appropriately)
+ in downloaded folder do:
	+ chmod u+x qt.run
	+ sudo ./qt.run

#### Building and Running an Exercise

in your cg2 exercise1 (or exercise#) source folder do:
+ mkdir build
+ cd build
+ PATH="/opt/Qt5.1.0/5.1.0/gcc_64/bin;$PATH" cmake ..  # this worked for us...
	+ you might need to set the newer compiler manually: -DCMAKE_CXX_COMPILER=/usr/bin/gcc-4.7
+ make
+ cd ..
+ ./build/exercise1


### OS X

#### Building and Running an Exercise

+ The following instructions are used for XCode. You can also use other IDEs like QtCreator or Netbeans.
+ Start CMake.
+ Choose generator, e.g., XCode or Unix Makefiles.
+ Configure and Generate.
+ Open XCode project and run make


## Third Party Libraries
In order to make the libraries compile with our compiler setup, the following modifications had to be made:

The following modifications where made in order to get things running. Note: these are not modifications you need to do when using this repo.

### Assimp Modifications 

##### assimp/CMakeLists.txt

+ ENABLE_BOOST_WORKAROUND was set to ON by default
+ disabled the uninstall capability

##### assimp/code/CMakeLists.txt

+ COMPILE_FLAGS "-D_CRT_SECURE_NO_WARNINGS" was added to avoid secure warnings in msvc  
+ COMPILE_FLAGS "-D_SCL_SECURE_NO_WARNINGS" was added to avoid secure warnings in msvc  

#####  assimp/code/LWOAnimation.cpp

+ \#include <functional> was added in order to compile in windows

### Qt 5.1.1 Compiling for Visual Studio 2013 RC

+ http://qt-project.org/forums/viewthread/32508
+ set CL=/MP
+ set QMAKEPATH=C:/Qt/5.1.1.12/qtbase/bin
+ configure -opensource -confirm-license -nomake examples -nomake tests -opengl desktop -platform win32-msvc2013
