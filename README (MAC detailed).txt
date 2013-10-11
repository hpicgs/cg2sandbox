Prerequisites

1) install cmake (version 2.8.9 or above) from http://www.cmake.org/cmake/resources/software.html

2) install QT (version 5.0.2 or above) from http://qt-project.org/downloads

3) install macports (or a similar tool, e.g. fink/homebrew) from http://www.macports.org/

	3.1) open a terminal window and log in as user with admin privileges.
	3.2) with macports type the following on the command line (without the ">". you probably need to enter the password of the admin user).
		> sudo install gcc46

	3.3) you should see some messages scrolling down on the command line with a success message at the end.
	3.4) check that gcc/g++ is installed in "/opt/local/bin/". using macports you should find the executables named "gcc-mp-46" and "g++-mp-46" (vor gcc/g++ version 4.6).

4) [suggested] install the NetBeans IDE from https://netbeans.org/

	4.1)  if you did not install a NetBeans bundle including the C/C++ language plugin, install the plugin using the IDE itself.



Build Instructions

1) start cmake. after the installation you should find cmake in the "/Applications" ("/Programme" for german language installations) folder.

	1.1) set the "source" directory in cmake to the directory where the "CMakeList.txt" file resides in.

	1.2) set the "build" directory to the same directory as the "source" directory and append "/build" to it.

	1.3) run the cmake "configure" command

		1.3.1) you (probably) get a message that asks if you want to create the "build" directory (if not the directory already exists. in that case be careful not to override any existing files).

		1.3.2) you get another message dialog. in this dialog select "UnixMakefiles" from the drop-down list and select the "Specify native compilers" radio button

		1.3.3) as C compiler select "/opt/local/bin/gcc-mp-46" (as installed using macports), as C++ compiler select "/opt/local/bin/g++-mp-46" (as installed using macports). ignore the Fortran compiler. 

		1.3.4) [If you don't get an error message skip to step 1.3.5] you probably get an error message now that says "Error in configuration process...". Click OK and look at the list item in cmake that is named "Ungrouped Entries". unfolding this item in the list should unveil an item named "QT5Core_DIR". Here you need to edit the "value" attribute. Set the value to "/Path/where/you/installed/QT5/5.0.x/clang_64/lib/cmake/Qt5Core". re-run configure. you probably get another error message for the item "QT5Gui_DIR". set the value as described above for "QT5Core" but now choose the "Qt5Gui" directory instead of "Qt5Core". re-run configure of cmake. another error message will appear for "QT5Widgets_DIR". Do the same as above for the "QTWidgets" directory. re-run configure, and again do the same thing as above (a last time) for the "QT5OpenGL_DIR". if you re-run configure now, no more error messages should appear. 

		1.3.5) run the cmake "generate" command.


2) open the NetBeans IDE

	2.1) open the NetBeans preferences (menu item "NetBeans -> Preferences")

		2.1.1) in the dialog go to the "C/C++" category. select "GNU" in the "Tool Collection" list on the left. set "C-Compiler" and "C++-Compiler" to the executables as described above for cmake

		2.1.2). further, set the path to cmake, that is "/usr/bin/cmake". click "Ok"

	2.2)  select "File -> New Project..."

		2.2.1) select "C/C++ Project with Existing Sources" in the dialog window and click "Next".

		2.2.2) specify/select the directory "... containing existing sources". note that this is the directory that was previously created by cmake (/path/to/your/cg1/exerciseX/build)! If not already set, choose the "Automatic" radio button for the "Select Configuration Mode" category. as "Tool Collection" choose "GNU Mac Compiler" from the drop-down menu. click "Finish" 

		2.2.3) you should now see the IDE compiling the project.

	2.3) right-click on the root node of your project in the "Project-Navigator" of the IDE 

		2.3.1) in the dialog that pops up select the "Run" category on the left. set the "Run Directory" value (in the list on the right) to ".." instead of ".". 

	2.4) "clean and build" the project (this did not produce any compilation errors for us) and "run" the application.


3) have fun!