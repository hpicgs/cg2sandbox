
set(sources
    source/main.cpp

    # based on or targeted for glwhite
    source/AbstractPainter.cpp
    source/AbstractPainter.h
    source/Application.cpp
    source/Application.h
    source/Canvas.cpp
    source/Canvas.h
    source/MessageHandler.cpp
    source/MessageHandler.h
    source/OpenGLFunctions.h
    source/Painter.cpp
    source/Painter.h
	source/Terrain.cpp
	source/Terrain.h
    source/Viewer.cpp
    source/Viewer.h
    source/Viewer.ui

    source/FileAssociatedAsset.cpp
    source/FileAssociatedAsset.h
    source/FileAssociatedShader.cpp
    source/FileAssociatedShader.h
    source/FileAssociatedTexture.cpp
    source/FileAssociatedTexture.h

    source/CachedValue.h
    source/CachedValue.hpp

    # based on or targeted for libglow
    source/AdaptiveGrid.cpp
    source/AdaptiveGrid.h
    source/AutoTimer.cpp
    source/AutoTimer.h
    source/DistanceTransform.cpp
    source/DistanceTransform.h
    source/AssimpScene.cpp
    source/AssimpScene.h
    source/ChronoTimer.cpp
    source/ChronoTimer.h
    source/Icosahedron.cpp
    source/Icosahedron.h
	source/MathMacros.h
    source/Plane3.cpp
    source/Plane3.h
    source/ScreenAlignedQuad.cpp
    source/ScreenAlignedQuad.h
    source/Timer.cpp
    source/Timer.h
    source/UnitCube.cpp
    source/UnitCube.h

	source/PatchedTerrain.cpp
	source/PatchedTerrain.h

    source/AbstractCoordinateProvider.h
    source/AxisAlignedBoundingBox.cpp
    source/AxisAlignedBoundingBox.h
    source/Camera.cpp
    source/Camera.h
    source/CyclicTime.cpp
    source/CyclicTime.h
    source/Navigation.cpp
    source/Navigation.h
    source/NavigationMath.cpp
    source/NavigationMath.h
    source/FlightNavigation.cpp
    source/FlightNavigation.h

    PARENT_SCOPE)
