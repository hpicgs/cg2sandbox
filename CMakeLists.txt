cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)


# PROJECT DESCRIPTION

set(META_PROJECT_NAME        "cg2sandbox")
set(META_VERSION_MAJOR       "cg2ws1314")
set(META_EXERCISE            "1")
set(META_AUTHOR1_MATNR       "000000")
set(META_AUTHOR2_MATNR       "000000")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_EXERCISE}.${META_AUTHOR1_MATNR}.${META_AUTHOR2_MATNR}")
set(META_AUTHOR_ORGANIZATION "hpicgs group")
set(META_AUTHOR_DOMAIN       "https://moodle.hpi3d.de/")


# OPTIONS

option(OPTION_LIMIT_CONFIGS  "Generate limited configs (Release; Debug)" ON)
option(OPTION_LOCAL_INSTALL  "Install to a local directory instead of the system" OFF)
option(OPTION_NO_OPENGL_320  "Try using the 3.0 OpenGL profile in evil compatibility mode." OFF)

set(CMAKE_INSTALL_LOCAL_ONLY OFF)

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

if (MSVC)
	cmake_policy(SET CMP0020 NEW)
endif()

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

configure_file(source/version.h.in version.h)

add_subdirectory(source)

# Group source files
set(SANDBOX_SOURCE_GROUP "Source Files")
source_group_by_path(
    ${CMAKE_CURRENT_SOURCE_DIR}/source "\\\\.h$|\\\\.hpp$|\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$" 
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

# PACKAGING

# Package information

set(CPACK_PACKAGE_NAME                  "${META_PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION               "${META_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR         "${META_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR         "${META_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH         "${META_AUTHOR1_MATNR}.${META_AUTHOR2_MATNR}")
set(CPACK_PACKAGE_RELOCATABLE           OFF)

# Package name
    
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
SET(CPACK_SOURCE_INSTALLED_DIRECTORIES 
    "${CMAKE_CURRENT_SOURCE_DIR}/source;/source;${CMAKE_CURRENT_SOURCE_DIR}/data;/data")

get_filename_component(CPACK_PATH ${CMAKE_COMMAND} PATH)
set(CPACK_COMMAND "${CPACK_PATH}/cpack")

add_custom_target(pack
    COMMAND ${CPACK_COMMAND} --config ${CMAKE_BINARY_DIR}/CPackSourceConfig.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

include(CPack)