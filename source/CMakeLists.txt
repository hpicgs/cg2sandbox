
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

if(MSVC)
    cmake_policy(SET CMP0020 NEW)
endif()

set(CMAKE_AUTOMOC ON)
set(AUTOMOC_MOC_OPTIONS PROPERTIES FOLDER CMakeAutomocTargets)

# Probably works in the next cmake release -> http://www.cmake.org/Bug/view.php?id=13788
# What we do not want is automocs beside the project -> http://www.cmake.org/Bug/view.php?id=13688
set_property(GLOBAL PROPERTY AUTOMOC_FOLDER CMakeAutomocTargets)

set(target cg2sandbox)
message(STATUS "App ${target}")

#
# External libraries
#

find_package(Qt5Core    5.1 REQUIRED)
find_package(Qt5Gui     5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED)
find_package(Qt5OpenGL  5.1 REQUIRED)

#
# Includes
#

include_directories(
)

include_directories(
    BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

#
# Libraries
#

set(libs
)

#
# Compiler definitions
#

if (OPTION_BUILD_STATIC)
    add_definitions("-D${META_PROJECT_NAME_UPPER}_STATIC")
endif()

#
# Sources
#

set(header_path "${CMAKE_CURRENT_SOURCE_DIR}")
set(source_path "${CMAKE_CURRENT_SOURCE_DIR}")
set(ui_path "${CMAKE_CURRENT_SOURCE_DIR}")

set(sources
    ${source_path}/AbstractPainter.cpp
    ${source_path}/AdaptiveGrid.cpp
    ${source_path}/Application.cpp
    ${source_path}/AutoTimer.cpp
    ${source_path}/AxisAlignedBoundingBox.cpp
    ${source_path}/Camera.cpp
    ${source_path}/Canvas.cpp
    ${source_path}/ChronoTimer.cpp
    ${source_path}/CyclicTime.cpp
    ${source_path}/FileAssociatedAsset.cpp
    ${source_path}/FileAssociatedShader.cpp
    ${source_path}/FileAssociatedTexture.cpp
    ${source_path}/Icosahedron.cpp
    ${source_path}/main.cpp
    ${source_path}/MessageHandler.cpp
    ${source_path}/Navigation.cpp
    ${source_path}/NavigationMath.cpp
    ${source_path}/Painter.cpp
    ${source_path}/Plane3.cpp
    ${source_path}/ScreenAlignedQuad.cpp
    ${source_path}/Terrain.cpp
    ${source_path}/Timer.cpp
    ${source_path}/UnitCube.cpp
    ${source_path}/Viewer.cpp
)

set(headers
    ${include_path}/AbstractCoordinateProvider.h
    ${include_path}/AbstractPainter.h
    ${include_path}/AdaptiveGrid.h
    ${include_path}/Application.h
    ${include_path}/AutoTimer.h
    ${include_path}/AxisAlignedBoundingBox.h
    ${include_path}/CachedValue.h
    ${include_path}/CachedValue.hpp
    ${include_path}/Camera.h
    ${include_path}/Canvas.h
    ${include_path}/ChronoTimer.h
    ${include_path}/CyclicTime.h
    ${include_path}/FileAssociatedAsset.h
    ${include_path}/FileAssociatedShader.h
    ${include_path}/FileAssociatedTexture.h
    ${include_path}/Icosahedron.h
    ${include_path}/MathMacros.h
    ${include_path}/MessageHandler.h
    ${include_path}/Navigation.h
    ${include_path}/NavigationMath.h
    ${include_path}/OpenGLFunctions.h
    ${include_path}/Painter.h
    ${include_path}/PainterMode.h
    ${include_path}/Plane3.h
    ${include_path}/ScreenAlignedQuad.h
    ${include_path}/Terrain.h
    ${include_path}/Timer.h
    ${include_path}/UnitCube.h
    ${include_path}/version.h.in
    ${include_path}/Viewer.h
)

set(uis
    ${ui_path}/Viewer.ui
)

# Group source files
set(header_group "Header Files")
set(source_group "Source Files")
set(ui_group "UI Files")
source_group_by_path(${header_path} "\\\\.h$|\\\\.hpp$" 
    ${header_group} ${headers})
source_group_by_path(${source_path} "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$" 
    ${source_group} ${sources})
source_group_by_path(${ui_path} "\\\\.ui$" 
    ${ui_group} ${uis})

#
# UI Files
#

qt5_wrap_ui(compiled_ui_files ${uis})

#
# Build executable
#

add_executable(${target} ${sources} ${compiled_ui_files})

target_link_libraries(${target} ${libs})
qt5_use_modules(${target} Core Gui Widgets OpenGL)

set_target_properties(${target}
    PROPERTIES
    LINKER_LANGUAGE              CXX
    FOLDER                      "${IDE_FOLDER}"
    COMPILE_DEFINITIONS_DEBUG   "${DEFAULT_COMPILE_DEFS_DEBUG}"
    COMPILE_DEFINITIONS_RELEASE "${DEFAULT_COMPILE_DEFS_RELEASE}"
    COMPILE_FLAGS               "${DEFAULT_COMPILE_FLAGS}"
    LINK_FLAGS_DEBUG            "${DEFAULT_LINKER_FLAGS_DEBUG}"
    LINK_FLAGS_RELEASE          "${DEFAULT_LINKER_FLAGS_RELEASE}"
    DEBUG_POSTFIX               "d${DEBUG_POSTFIX}")

#
# Deployment
#

install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_BIN}
#   LIBRARY DESTINATION ${INSTALL_SHARED}
#   ARCHIVE DESTINATION ${INSTALL_LIB}
)
