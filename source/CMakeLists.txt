
set(sources
    main.cpp

    # based on or targeted for glwhite
    AbstractPainter.cpp
    AbstractPainter.h
    Application.cpp
    Application.h
    Canvas.cpp
    Canvas.h
    MessageHandler.cpp
    MessageHandler.h
    OpenGLFunctions.h
    Painter.cpp
    Painter.h
    Viewer.cpp
    Viewer.h
    Viewer.ui

    FileAssociatedShader.cpp
    FileAssociatedShader.h
    
    CachedValue.h
    CachedValue.hpp

    # based on or targeted for libglow
    AdaptiveGrid.cpp
    AdaptiveGrid.h
    AutoTimer.cpp
    AutoTimer.h
    ChronoTimer.cpp
    ChronoTimer.h
	MathMacros.h
    Plane3.cpp
    Plane3.h
    Timer.cpp
    Timer.h
    UnitCube.cpp
    UnitCube.h

    AbstractCoordinateProvider.h
    AxisAlignedBoundingBox.cpp
    AxisAlignedBoundingBox.h
    Camera.cpp
    Camera.h
    CyclicTime.cpp
    CyclicTime.h
    Navigation.cpp
    Navigation.h
    NavigationMath.cpp
    NavigationMath.h    
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
