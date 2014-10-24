
#include <vector>
#include <cassert>

#include "OpenGLFunctions.h"

#include "Terrain.h"


Terrain::Terrain(
	unsigned short size
,	OpenGLFunctions & gl)
: m_vertices(QOpenGLBuffer::VertexBuffer)
, m_indices(QOpenGLBuffer::IndexBuffer  )
{
	m_vao.create();
	m_vao.bind();

	m_vertices.create();
	m_vertices.setUsagePattern(QOpenGLBuffer::StaticDraw);

	m_indices.create();
	m_indices.setUsagePattern(QOpenGLBuffer::StaticDraw);

    // Task_1_1 - ToDo Begin

    strip(size, m_vertices, m_indices, -1);

    // Configure your Vertex Attribute Pointer based on your vertex buffer (mainly number of coefficients ;)).

    gl.glVertexAttribPointer(0, 0, GL_FLOAT, GL_FALSE, sizeof(float)* 0, nullptr);
    gl.glEnableVertexAttribArray(0);

    // Task_1_1 - ToDo End

    m_vao.release();
}

void Terrain::draw(
	OpenGLFunctions & gl
,	const GLenum mode)
{
    // Task_1_1 - ToDo Begin

    // Setup the OpenGL state appropriate to the given and your personal drawing requirements.
    // You might take depth tests, primitive restart, and culling into account.

    // gl.glEnable(...
    // ...

    m_vao.bind();
    // gl.glDrawElements(...);
    m_vao.release();

    // Remember to "clean up" the state after rendering.

    // gl.glDisable(...
    // ...

    // Task_1_1 - ToDo End
}

void Terrain::strip(
	const unsigned short size
,	QOpenGLBuffer & vertices
,	QOpenGLBuffer & indices
,	const GLuint primitiveRestartIndex)
{
    // Task_1_1 - ToDo Begin

    // perhaps, check for pointless parameter input

    // tips: probably use some for loops to fill up a std or qt container with 
    // floats or other structs, fitting the purpose. further, remember that 
    // gradually summing up lots of small floats might lead to major inaccuracies ...

    // ...

	//vertices.bind();
	//vertices.allocate(...);

	// ...

	//indices.bind();
	//indices.allocate(...);

    // Task_1_1 - ToDo End
}
