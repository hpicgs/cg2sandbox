#include "UnitCube.h"

#include <QOpenGLFunctions_3_2_Core>

namespace 
{
	struct vec3 
    {
        vec3(float a, float b, float c)
        {
            _[0] = a; _[1] = b; _[2] = c;
        }
        float _[3]; 
    };

	typedef std::vector<vec3> Strip3;

	const Strip3 strip()
	{
		static vec3 vertices[] =
		{ 
		    vec3(-.5f,-.5f,-.5f)
		,   vec3(-.5f,-.5f, .5f)
		,   vec3(-.5f, .5f,-.5f)
		,   vec3(-.5f, .5f, .5f)
		,   vec3( .5f,-.5f,-.5f)
		,   vec3( .5f,-.5f, .5f)
		,   vec3( .5f, .5f,-.5f)
		,   vec3( .5f, .5f, .5f)
		};

		static vec3 normals[] =
		{ 
			vec3(-1.f, 0.f, 0.f)
		,   vec3( 1.f, 0.f, 0.f)
		,   vec3( 0.f,-1.f, 0.f)
		,   vec3( 0.f, 1.f, 0.f)
		,   vec3( 0.f, 0.f,-1.f)
		,   vec3( 0.f, 0.f, 1.f)
		,   vec3( 0.f, 0.f, 0.f)  // dummy
		};

		// use an interleaved array
        static vec3 strip[] =
        {
		    vertices[7], normals[6]
		,   vertices[3], normals[6]
		,   vertices[5], normals[5]
		,   vertices[1], normals[5]
		,   vertices[0], normals[2]
		,   vertices[3], normals[0]
		,   vertices[2], normals[0]
		,   vertices[7], normals[3]
		,   vertices[6], normals[3]
		,   vertices[5], normals[1]
		,   vertices[4], normals[1]
		,   vertices[0], normals[2]
		,   vertices[6], normals[4] 
		,   vertices[2], normals[4]
		};

        Strip3 strip2;
        strip2.assign(strip, strip + 28);

        return strip2;
	}
}

UnitCube::UnitCube(
    QOpenGLFunctions_3_2_Core & gl
,   const GLuint vertexAttribLocation
,   const GLuint normalAttribLocation)
:   m_strip(QOpenGLBuffer::Type::VertexBuffer)
{
    const Strip3 raw(strip());

    m_vao.create();
    m_vao.bind();

    m_strip.create();
    m_strip.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_strip.bind();
    m_strip.allocate(raw.data(), sizeof(vec3) * 2 * raw.size());

    gl.glVertexAttribPointer(vertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) * 2, nullptr);
    gl.glEnableVertexAttribArray(vertexAttribLocation);

    gl.glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) * 2, reinterpret_cast<void*>(sizeof(vec3)));
    gl.glEnableVertexAttribArray(normalAttribLocation);

    m_vao.release();
}

void UnitCube::draw(QOpenGLFunctions_3_2_Core & gl)
{
    gl.glEnable(GL_DEPTH_TEST);

    m_vao.bind();
    gl.glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
    m_vao.release();
}