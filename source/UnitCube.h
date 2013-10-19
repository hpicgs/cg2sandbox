#pragma once

#include <hash_map>
#include <vector>

#include <qgl.h>

#include <QVector3D>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "OpenGLFunctions.h"

class UnitCube
{
public:
    UnitCube(
        OpenGLFunctions & gl
    ,   const GLuint vertexAttribLocation = 0
    ,   const GLuint normalAttribLocation = 1);

    /** draws the unit cube as single triangle strip. Note: interleaved array 
        is used, so the normal comes from the provoking vertex (expected to be
        the default GL_LAST_VERTEX_CONVENTION). use flat interpolation to get 
        the face normal within the fragment shader.
    */
    void draw(OpenGLFunctions & gl);

private:
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_strip;
};
