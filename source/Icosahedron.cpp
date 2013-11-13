
#include <cmath>
#include <iterator>

#include <QVector3D>
#include <QOpenGLVertexArrayObject.h>
#include <QOpenGLBuffer.h>

#include "Icosahedron.h"


const std::vector<Icosahedron::vec3> Icosahedron::vertices()
{
    static const float t = (1.f + sqrtf(5.f)) * 0.5f; // 2.118
    static const float i =  1.f / sqrt(t * t + 1.f);  // 0.427
    static const float a = t * i;                     // 0.904

    // icosahedron vertices (normalized) form three orthogonal golden rectangles
    // http://en.wikipedia.org/wiki/Icosahedron#Cartesian_coordinates

    std::vector<vec3> data;
    data.push_back(vec3(-i, a, 0));
    data.push_back(vec3( i, a, 0));
    data.push_back(vec3(-i,-a, 0));
    data.push_back(vec3( i,-a, 0));
    data.push_back(vec3( 0,-i, a));
    data.push_back(vec3( 0, i, a));
    data.push_back(vec3( 0,-i,-a));
    data.push_back(vec3( 0, i,-a));
    data.push_back(vec3( a, 0,-i));
    data.push_back(vec3( a, 0, i));
    data.push_back(vec3(-a, 0,-i));
    data.push_back(vec3(-a, 0, i));

    return data;
}

const std::vector<Icosahedron::uvec3> Icosahedron::indices()
{
    std::vector<uvec3> data;

    data.push_back(uvec3( 0, 11,  5));
    data.push_back(uvec3( 0,  5,  1));
    data.push_back(uvec3( 0,  1,  7));
    data.push_back(uvec3( 0,  7, 10));
    data.push_back(uvec3( 0, 10, 11));

    data.push_back(uvec3( 1,  5,  9));
    data.push_back(uvec3( 5, 11,  4));
    data.push_back(uvec3(11, 10,  2));
    data.push_back(uvec3(10,  7,  6));
    data.push_back(uvec3( 7,  1,  8));

    data.push_back(uvec3( 3,  9,  4));
    data.push_back(uvec3( 3,  4,  2));
    data.push_back(uvec3( 3,  2,  6));
    data.push_back(uvec3( 3,  6,  8));
    data.push_back(uvec3( 3,  8,  9));

    data.push_back(uvec3( 4,  9,  5));
    data.push_back(uvec3( 2,  4, 11));
    data.push_back(uvec3( 6,  2, 10));
    data.push_back(uvec3( 8,  6,  7));
    data.push_back(uvec3( 9,  8,  1));

    return data;
}

Icosahedron::Icosahedron(
    OpenGLFunctions & gl
,   const GLsizei iterations)
:   m_vao(new QOpenGLVertexArrayObject)
,   m_vertices(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))
,   m_indices(new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer))
{
    std::vector<vec3> v(vertices());
    std::vector<uvec3> i(indices());

    refine(v, i, qBound(0, iterations, 8));

    m_vao->create();
    m_vao->bind();

    m_vertices->create();
    m_vertices->setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_indices->create();
    m_indices->setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_vertices->bind();
    m_vertices->allocate(v.data(), v.size() * sizeof(vec3));

    m_indices->bind();
    m_indices->allocate(i.data(), i.size() * sizeof(uvec3));

    m_size = i.size() * 3;

    gl.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), nullptr);
    gl.glEnableVertexAttribArray(0);

    m_vao->release();
}

Icosahedron::~Icosahedron()
{
    delete m_vao;
    delete m_vertices;
    delete m_indices;
}

void Icosahedron::draw(
    OpenGLFunctions & gl
,   const GLenum mode)
{
    m_vao->bind();
    gl.glPointSize(8.f);
    gl.glDrawElements(mode, m_size, GL_UNSIGNED_SHORT, nullptr);
    m_vao->release();
}

void Icosahedron::refine(
    std::vector<Icosahedron::vec3> & vertices
,   std::vector<Icosahedron::uvec3> & indices
,   const unsigned char levels)
{
    std::map<unsigned int, unsigned short> cache;

    for(int i = 0; i < levels; ++i)
    {
        const int size(static_cast<int>(indices.size()));

        for(int f = 0; f < size; ++f)
        {
            uvec3 & face(indices[f]);

            const unsigned short a(face.x);
            const unsigned short b(face.y);
            const unsigned short c(face.z);

            const unsigned short ab(split(a, b, vertices, cache));
            const unsigned short bc(split(b, c, vertices, cache));
            const unsigned short ca(split(c, a, vertices, cache));

            face = uvec3(ab, bc, ca);

            indices.push_back(uvec3(a, ab, ca));
            indices.push_back(uvec3(b, bc, ab));
            indices.push_back(uvec3(c, ca, bc));
        }
    }    
}

unsigned short Icosahedron::split(
    const unsigned short a
,   const unsigned short b
,   std::vector<Icosahedron::vec3> & points
,   std::map<unsigned int, unsigned short> & cache)
{
    const bool aSmaller(a < b);

    const unsigned short smaller(aSmaller ? a : b);
    const unsigned short greater(aSmaller ? b : a);
    const unsigned int hash((smaller << 16) + greater);

    std::map<unsigned int, unsigned short>::const_iterator h(cache.find(hash));
    if(cache.end() != h)
        return h->second;

    QVector3D va(points[a].x, points[a].y, points[a].z);
    QVector3D vb(points[b].x, points[b].y, points[b].z);
    
    QVector3D vab((va + vb) * .5f);
    vab.normalize();

    points.push_back(vec3(vab.x(), vab.y(), vab.z()));

    const unsigned short i(static_cast<unsigned short>(points.size()) - 1);

    cache[hash] = i;

    return i;
}
