#pragma once

#include <hash_map>
#include <vector>

#include <qgl.h>

#include <QVector3D>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class QOpenGLFunctions_3_2_Core;

class UnitCube
{
    struct vec3 {
        float _[3]; };

    using Strip3 = std::vector<vec3>;

    static const Strip3 strip();

public:
    UnitCube(
        QOpenGLFunctions_3_2_Core & gl
    ,   const GLuint vertexAttribLocation = 0
    ,   const GLuint normalAttribLocation = 1);

    /** draws the unit cube as single triangle strip. Note: interleaved array 
        is used, so the normal comes from the provoking vertex (expected to be
        the default GL_LAST_VERTEX_CONVENTION). use flat interpolation to get 
        the face normal within the fragment shader.
    */
    void draw(QOpenGLFunctions_3_2_Core & gl);

private:
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_strip;
};
