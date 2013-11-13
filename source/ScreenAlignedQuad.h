#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "OpenGLFunctions.h"

class ScreenAlignedQuad
{
public:
	ScreenAlignedQuad(
		OpenGLFunctions & gl
	,	const GLuint vertexAttribLocation = 0);

	void draw(OpenGLFunctions & gl);

protected:
	static void strip(QOpenGLBuffer & vertices);

private:
	QOpenGLVertexArrayObject m_vao;
	QOpenGLBuffer m_vertices;
};
