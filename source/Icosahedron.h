#pragma once

#include <map>
#include <vector>

#include "OpenGLFunctions.h"

class QOpenGLVertexArrayObject;
class QOpenGLBuffer;

class Icosahedron
{
public:
    struct vec3
    {
        vec3(float x = 0.f, float y = 0.f, float z = 0.f)
        : x(x), y(y), z(z) { }

        float x;
        float y;
        float z;
    };
    struct uvec3
    {
        uvec3(unsigned short x = 0, unsigned short y = 0, unsigned short z = 0)
        : x(x), y(y), z(z) { }

        unsigned short x;
        unsigned short y;
        unsigned short z;
    };

    static const std::vector<vec3> vertices();
    static const std::vector<uvec3> indices(); /// individual triangle indices (no strip, no fan)

   /**  Iterative triangle refinement: split each triangle into 4 new ones and 
        create points and indices appropriately.
    */
    static void refine(
        std::vector<vec3> & vertices
    ,   std::vector<uvec3> & indices
    ,   unsigned char levels);

public:
    Icosahedron(
        OpenGLFunctions & gl
    ,   GLsizei iterations = 0);

    virtual ~Icosahedron();


    /** draws the icosahedron as single triangles (TODO: generate set of triangle strips?)
    */
    void draw(
        OpenGLFunctions & gl
    ,   const GLenum mode = GL_TRIANGLES);

private:
    /** Splits a triangle edge by adding an appropriate new point (normalized
        on sphere) to the points (if not already cached) and returns the index
        to this point.
    */
    static unsigned short split(
        unsigned short a
    ,   unsigned short b
    ,   std::vector<vec3> & points
    ,   std::map<unsigned int, unsigned short> & cache);

private:
    QOpenGLVertexArrayObject * m_vao;

    QOpenGLBuffer * m_vertices;
    QOpenGLBuffer * m_indices;

    GLsizei m_size;
};
