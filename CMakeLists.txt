cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)


# PROJECT DESCRIPTION

set(META_PROJECT_NAME        "cg2sandbox")
set(META_VERSION_MAJOR       "cg2ws1314")
set(META_EXERCISE            "0")
set(META_AUTHOR1_MATNR       "000000")
set(META_AUTHOR2_MATNR       "000000")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_EXERCISE}.${META_AUTHOR1_MATNR}.${META_AUTHOR2_MATNR}")
set(META_AUTHOR_ORGANIZATION "hpicgs group")
set(META_AUTHOR_DOMAIN       "https://moodle.hpi3d.de/")


# OPTIONS

option(OPTION_LIMIT_CONFIGS  "Generate limited configs (Release; Debug)" ON)
option(OPTION_LOCAL_INSTALL  "Install to a local directory instead of the system" OFF)
option(OPTION_NO_OPENGL_320  "Try using the 3.0 OpenGL profile in evil compatibility mode." OFF)

# CMAKE CONFIGURATION

set(SANDBOX_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Include cmake modules from ./cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find the WinSDK libs
if (WIN32)
    set(CMAKE_PREFIX_PATH "C:\\Program Files (x86)\\Windows Kits\\8.0\\Lib\\win8\\um\\x64")
    cmake_policy(SET CMP0020 NEW)
endif (WIN32)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Set configuration types
if(OPTION_LIMIT_CONFIGS)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Limited Configs" FORCE)
endif()

# Project
project(${META_PROJECT_NAME} C CXX)

# Generate folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Include custom cmake functions
include(cmake/Custom.cmake)


# PLATFORM AND ARCHITECTURE

# Architecture (32/64 bit)
set(X64 OFF)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(X64 ON)
endif()

# Check for linux
if(UNIX AND NOT APPLE)
    set(LINUX 1)
endif()

# Setup platform specifics (compile flags, etc., ...)
if(MSVC)
    message(STATUS "Configuring for platform Windows/MSVC.")
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformWindowsMSVC.cmake)
elseif(WIN32 AND CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "Configuring for platform Windows/GCC.")
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformWindowsGCC.cmake)
elseif(LINUX AND CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "Configuring for platform Linux/GCC.")
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformLinuxGCC.cmake)
elseif(APPLE)
    message(STATUS "Configuring for platform MacOS.")
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformMacOS.cmake)
else()
    # Unsupported system/compiler
    message(WARNING "Unsupported platform/compiler combination")
endif()


# Install locations

set(project ${META_PROJECT_NAME})
if(WIN32)
    set(INSTALL_BIN           ".")                      # C:\Programme\<project>
    set(INSTALL_SHARED        ".")                      # C:\Programme\<project>
    set(INSTALL_LIB           "lib")                    # C:\Programme\<project>\lib
    set(INSTALL_INCLUDE       "include")                # C:\Programme\<project>\include
    set(INSTALL_DOC           "doc")                    # C:\Programme\<project>\doc
    set(INSTALL_APPLICATIONS  "")                       # Not available under Windows
    set(INSTALL_ICONS         "")                       # Not available under Windows
    set(INSTALL_INIT          "")                       # Not available under Windows
    set(INSTALL_DATA          ".")                      # C:\Programme\<project>
    set(INSTALL_EXAMPLES      ".")                      # C:\Programme\<project>
elseif(LINUX)
    set(INSTALL_BIN           "bin")                    # /usr/[local]/bin
    set(INSTALL_SHARED        "lib")                    # /usr/[local]/lib
    set(INSTALL_LIB           "lib")                    # /usr/[local]/lib
    set(INSTALL_INCLUDE       "include")                # /usr/[local]/include
    set(INSTALL_DOC           "share/doc/${project}")   # /usr/[local]/share/doc/<project>
    set(INSTALL_APPLICATIONS  "share/applications")     # /usr/[local]/share/applications
    set(INSTALL_ICONS         "share/pixmaps")          # /usr/[local]/share/pixmaps
    set(INSTALL_INIT          "/etc/init")              # /etc/init (upstart init scripts)
    set(INSTALL_DATA          "share/${project}")       # /usr/[local]/share/<project>
    set(INSTALL_EXAMPLES      "share/${project}")       # /usr/[local]/share/<project>

    # Adjust target paths for Local installs
    if(OPTION_LOCAL_INSTALL)
        # Put binaries in root directory and keep data directory name
        set(INSTALL_BIN                       ".")           # /<INSTALL_PREFIX>
        set(INSTALL_DATA                      ".")           # /<INSTALL_PREFIX>

        # We have to change the RPATH of binaries to achieve a usable local install.
        # [TODO] For binaries, "$ORIGIN/lib" is right, so that libraries are found in ./lib.
        #        However, I have not yet tested what happens when libraries use other libraries.
        #        In that case, they might need the rpath $ORIGIN instead ...
        set(CMAKE_SKIP_BUILD_RPATH            FALSE)         # Use automatic rpath for build
        set(CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE)         # Use specific rpath for INSTALL
        set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)         # NO automatic rpath for INSTALL
        set(CMAKE_INSTALL_RPATH               "$ORIGIN/lib") # Libraries are relative to binary
    endif()
endif()


# Target
set(target cg2sandbox)

# Find externals
#find_package(OpenGL REQUIRED)

# qt5

set(CMAKE_AUTOMOC ON)
set(AUTOMOC_MOC_OPTIONS PROPERTIES FOLDER CMakeAutomocTargets)

# Probably works in the next cmake release -> http://www.cmake.org/Bug/view.php?id=13788
# What we do not want is automocs beside the project -> http://www.cmake.org/Bug/view.php?id=13688

set_property(GLOBAL PROPERTY AUTOMOC_FOLDER CMakeAutomocTargets)

# try to find either qt4 or qt5, favoring qt5
# good resource: http://www.kdab.com/using-cmake-with-qt-5/

# http://qt-project.org/forums/viewthread/30006/

cmake_policy(SET CMP0020 NEW)

find_package(Qt5Core    5.1 REQUIRED)
find_package(Qt5Gui     5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED) # this is important in order to get uic working 
find_package(Qt5OpenGL  5.1 REQUIRED)


# assimp

set(ASSIMP_INSTALL_BY_DEFAULT OFF)

option(ASSIMP_INSTALL "Install ASSIMP libs, includes, and CMake scripts" ${ASSIMP_INSTALL_BY_DEFAULT})
option(ASSIMP_INSTALL_DOC "Install ASSIMP documentation" OFF)
option(ASSIMP_BUILD_SHARED "Build ASSIMP shared libraries" OFF)
option(ASSIMP_BUILD_STATIC "Build ASSIMP static libraries" ON)
option(ASSIMP_BUILD_EXAMPLES "Build ASSIMP examples" OFF)

# The ASSIMP library.
add_subdirectory(assimp)


# Include directories
include_directories(
    ${CMAKE_BINARY_DIR} # for Meta.h
#    ${OPENGL_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs)

if(OPTION_NO_OPENGL_320)
    add_definitions("-DNO_OPENGL_320")
endif()

set(source_path "${CMAKE_CURRENT_SOURCE_DIR}/source")

# Generate meta-header
configure_file(${source_path}/version.h.in version.h)

set(sources
    ${source_path}/main.cpp

    # based on or targeted for glwhite
    ${source_path}/AbstractPainter.cpp
    ${source_path}/AbstractPainter.h
    ${source_path}/Application.cpp
    ${source_path}/Application.h
    ${source_path}/Canvas.cpp
    ${source_path}/Canvas.h
    ${source_path}/MessageHandler.cpp
    ${source_path}/MessageHandler.h
    ${source_path}/OpenGLFunctions.h
    ${source_path}/Painter.cpp
    ${source_path}/Painter.h
    ${source_path}/Viewer.cpp
    ${source_path}/Viewer.h
    ${source_path}/Viewer.ui

    ${source_path}/FileAssociatedShader.cpp
    ${source_path}/FileAssociatedShader.h
    
    ${source_path}/CachedValue.h
    ${source_path}/CachedValue.hpp

    # from libglow
    ${source_path}/AdaptiveGrid.cpp
    ${source_path}/AdaptiveGrid.h
    ${source_path}/AutoTimer.cpp
    ${source_path}/AutoTimer.h
    ${source_path}/ChronoTimer.cpp
    ${source_path}/ChronoTimer.h
	${source_path}/MathMacros.h
    ${source_path}/Plane3.cpp
    ${source_path}/Plane3.h
    ${source_path}/Timer.cpp
    ${source_path}/Timer.h
    ${source_path}/UnitCube.cpp
    ${source_path}/UnitCube.h

    ${source_path}/AbstractCoordinateProvider.h
    ${source_path}/Boundary.cpp
    ${source_path}/Boundary.h
    ${source_path}/Camera.cpp
    ${source_path}/Camera.h
    ${source_path}/Navigation.cpp
    ${source_path}/Navigation.h
    ${source_path}/NavigationMath.cpp
    ${source_path}/NavigationMath.h    
)

# Group source files
set(SANDBOX_SOURCE_GROUP "Source Files")
source_group_by_path(${source_path} "\\\\.h$|\\\\.hpp$|\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$" 
    ${SANDBOX_SOURCE_GROUP} ${sources})

# wrap ui files and group wrap headers
list_extract(uis "\\\\.ui$" ${sources})
qt5_wrap_ui(uihs ${uis})

source_group_by_path(${CMAKE_CURRENT_BINARY_DIR} "\\\\.h$" 
    ${SANDBOX_SOURCE_GROUP} ${uihs})


# Build library
add_executable(${target} ${sources} ${uihs})

qt5_use_modules(${target} Core OpenGL Gui Widgets)

target_link_libraries(${target}
    assimp
    zlib
)

set_target_properties(${target}
    PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_DEFINITIONS_DEBUG   "${DEFAULT_COMPILE_DEFS_DEBUG}"
    COMPILE_DEFINITIONS_RELEASE "${DEFAULT_COMPILE_DEFS_RELEASE}"
    COMPILE_FLAGS               "${DEFAULT_COMPILE_FLAGS}"
    LINK_FLAGS_DEBUG            "${DEFAULT_LINKER_FLAGS_DEBUG}"
    LINK_FLAGS_RELEASE          "${DEFAULT_LINKER_FLAGS_RELEASE}"
    DEBUG_POSTFIX               "d${DEBUG_POSTFIX}"
)

# Deployment
install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_BIN}
    LIBRARY DESTINATION ${INSTALL_SHARED}
    ARCHIVE DESTINATION ${INSTALL_LIB}
)
#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_INCLUDE})

install(FILES LICENSE AUTHORS DESTINATION .)

# DEPLOYMENT

# Data files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
    DESTINATION ${INSTALL_DATA})

# Packaging
add_subdirectory(packages)
