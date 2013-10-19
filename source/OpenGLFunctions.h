
#pragma once

// using a simple typedef or define does not work, since precproc is ignored by moc

#ifdef NO_OPENGL_320

#include <QOpenGLFunctions_3_0>
class OpenGLFunctions : public QOpenGLFunctions_3_0
{
};

#else

#include <QOpenGLFunctions_3_2_Core>
class OpenGLFunctions : public QOpenGLFunctions_3_2_Core
{
};

#endif
