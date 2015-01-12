#pragma once


#include <QMap>
#include <QMatrix3x3>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "OpenGLFunctions.h"

class QOpenGLShaderProgram;

class PatchedTerrain
{
    enum PatchIdentifier
    {
        Tile0
    ,   Tile1
    ,   Tile2
    ,   Strip00
    ,   Strip01
    ,   Strip02
    ,   Strip11
    ,   Strip12
    ,   Strip22
    };

public:
    PatchedTerrain(
        unsigned int LOD
    ,   OpenGLFunctions & gl);
    virtual ~PatchedTerrain();

    void drawReset();
    void drawPatch(
        const QVector3D & position
    ,   float scale
    ,   unsigned char bottomLOD
    ,   unsigned char rightLOD
    ,   unsigned char topLOD
    ,   unsigned char leftLOD);

    void draw(
        OpenGLFunctions & gl
    ,   QOpenGLShaderProgram & program
    ,   bool lineStrip);

protected:
    void setupVertices();
    void setupUniformBuffer(
        OpenGLFunctions & gl
    ,   PatchIdentifier strip);

    void setupTiles();
    void setupStrips();

    void setupStrip001122(PatchIdentifier strip);
    void setupStrip010212(PatchIdentifier strip);

    inline void draw(
        OpenGLFunctions & gl
    ,   GLint binding
    ,   PatchIdentifier strip
    ,   bool lineStrip);

    PatchIdentifier getStrip(
        unsigned char lod0
    ,   unsigned char lod1
    ,   bool & mirror);

    inline int sum(int a) const;

private:
    const unsigned int m_maxLOD; // is input LOD + 2 for three different tiles and 6 strips

    GLint m_maxUniformBlockSize;
    GLint m_maxInstancesPerPatch;

    QOpenGLVertexArrayObject m_vao;

    QOpenGLBuffer m_vertices;
    QMap<PatchIdentifier, QOpenGLBuffer *> m_indices;
    QMap<PatchIdentifier, GLsizei> m_counts;
    QMap<PatchIdentifier, GLuint> m_ubos;

    struct mat3
    {
        mat3()
        {
            QMatrix4x4 matrix;
            memcpy(m, matrix.data(), sizeof(float) * 16);
        }
        mat3(const QMatrix4x4 & matrix)
        {
            memcpy(m, matrix.data(), sizeof(float) * 16);
        }
        float m[16];
    };

    QMap<PatchIdentifier, QVector<mat3>> m_transforms;
    bool m_cached;
};
