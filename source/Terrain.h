#pragma once


#include <vector>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "OpenGLFunctions.h"

class Terrain
{
public:
	Terrain(
		unsigned short size
	,	OpenGLFunctions & gl);

	void draw(
		OpenGLFunctions & gl
	,	GLenum mode = GL_TRIANGLE_STRIP);

protected:
    static void strip(
        unsigned short size
    ,   QOpenGLBuffer & vertices
    ,   QOpenGLBuffer & indices
    ,   GLuint primitiveRestartIndex);

private:
    QOpenGLVertexArrayObject m_vao;

    QOpenGLBuffer m_vertices;
	QOpenGLBuffer m_indices;
};
