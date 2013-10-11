
REQUIREMENTS: CMake 2.8.11 and Qt 5.1 and XCode/QtCreator/gcc/mingw/VisualStudio/etc..

DISCLAIMER: 
1. FOR ASSESSMENT WE BUILD YOUR SUBMISSION IN WINDOWS WITH MSVC11 ONLY!
2. WE OFFICIALLY SUPPORT WINDOWS WITH MSVC11 ONLY!

Still, if you have problems on other platforms please let us know. 
We are dedicated to provide a framework that is working on all common desktop plattforms.


INSTRUCTIONS
------------


WINDOWS
-------

Gathering Requirements

	# install cmake from
	http://www.cmake.org/cmake/resources/software.html
	# install qt5 from (for this exercise we used visual studio 2012 x64, but older version should work fine...)
	http://qt-project.org/downloads
	# install visual studio or qt creator


WINDOWS - MSVC - THE GUI WAY - PREFERED
---------------------------------------

Prerequisites

	# Having a Visual Studio installed (MSVC10 or UP)

Building and Running an Exercise

	# Start cmake-gui with batch or manually (but provide QT_DIR and make sure the Qt binaries are on the path)
	# Open CMakeLists.txt with cmake-gui 
	# Specify path to build binaries (recommended in extra build directory) -> "build" folder
	# Press configure and generate
	# Open solution with batch
	# setup working directory of exercise executable to ".." for each configuration you want to start your program in
	# compile and run as required...


WINDOWS - QT-CREATOR
--------------------

Prerequisites

	# Use the qt creator 2.4 or newer (at best, the one that comes with qt 5.1)
	# Set the path for the CMake executable in Tools > Options > Build & Run > CMake.
	# Now you should be able to open CMakeLists.txt files (if not check http://doc.qt.digia.com/qtcreator-2.4/creator-project-cmake.html)
	NOTE: you need a CDB if you want to compile with MSVC... http://msdn.microsoft.com/de-de/windows/hardware/hh852363 
	Installing the Debugging Tools should suffice.

Building and Running an Exercise

	> File > Open File or Project
	> select the CMakeLists.txt
	> specify Build Directory
	> Press Run-CMake, then finish
	> Specify Working Directory via Projects > Run > Working Directory (see http://doc.qt.digia.com/qtcreator-2.3/creator-run-settings.html)
	  to point to exercise folder, not the build folder
	> Compile (e.g. with [Ctrl] + [R])


WINDOWS - NMAKE - COMMAND LINE
-----------------------------

Prerequisites
	# Cmake is available via command line
	# You habe nmake installed and you can use ae.g.  Visual Studio Command Promt

Building and Running an Exercise

	> set PATH=%PATH%;"C:\Qt\5.1.0\qtbase\bin\"
	> mkdir build
	> cd build
	> cmake -DCMAKE_PREFIX_PATH="C:\Qt\5.1.0\qtbase\lib\cmake" -G "NMake Makefiles" .. # this worked for us... (perhaps you need to specify platform/compiler...)
	> nmake
	> cd ..
	> set QT_DIR=C:\Qt\5.1.0
	> set PATH=%PATH%;%QT_DIR%\qtbase\bin;
	> ./build/exercise1


LINUX - Tested on Ubuntu 12.10 x64
----------------------------------

Gathering Requirements

	# install cmake, this should give you 2.8.9 or above ...
	# if version is below you probably need to update to a newer version or compile cmake for youself
	> sudo apt-get install cmake 
	> download qt 5.0.2
	
	# suppose the filename is qt.run (replace appropriately)
	# in downloaded folder do:
	> chmod u+x qt.run
	> ./qt.run

Building and Running an Exercise

	# in your cg2 exercise1 (or exercise#) source folder do:
	> mkdir build
	> cd build
	> cmake -DCMAKE_PREFIX_PATH="/opt/Qt5.1.0/5.1.0/gcc_64" ..  # this worked for us...
	> make
	> cd ..
	> ./build/exercise1


MAC
---

Building and Running an Exercise

	# The following instructions are used for XCode. You can also use other IDEs like QtCreator or Netbeans.

	# Start CMake.
	# Choose generator, e.g., XCode or Unix Makefiles.
	# Configure and Generate.
	# Open XCode project and run make