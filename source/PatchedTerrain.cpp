
#include <cassert>

#include <QVector>
#include <QVector2D>
#include <QMatrix4x4>
#include <QMatrix>
#include <QOpenGLShaderProgram>

#include "OpenGLFunctions.h"

#include "PatchedTerrain.h"

namespace
{
    const GLuint PrimitiveRestartIndex(-1);
}

PatchedTerrain::PatchedTerrain(
    unsigned int LOD
,   OpenGLFunctions & gl)
:   m_maxLOD(LOD >= 0 ? LOD + 2 : 2)
,   m_vertices(QOpenGLBuffer::VertexBuffer)
,   m_cached(false)
{
    gl.glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &m_maxUniformBlockSize);
    m_maxInstancesPerPatch = m_maxUniformBlockSize / (sizeof(float) * 16);

    setupUniformBuffer(gl, Tile0);
    setupUniformBuffer(gl, Tile1);
    setupUniformBuffer(gl, Tile2);

    setupUniformBuffer(gl, Strip00);
    setupUniformBuffer(gl, Strip01);
    setupUniformBuffer(gl, Strip02);
    setupUniformBuffer(gl, Strip11);
    setupUniformBuffer(gl, Strip12);
    setupUniformBuffer(gl, Strip22);

    m_indices[Tile0]   = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_indices[Tile1]   = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_indices[Tile2]   = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_indices[Strip00] = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_indices[Strip01] = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_indices[Strip02] = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_indices[Strip11] = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_indices[Strip12] = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_indices[Strip22] = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

	m_vao.create();
	m_vao.bind();

    setupVertices();

    gl.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
    gl.glEnableVertexAttribArray(0);

    m_vao.release(); 

    setupTiles();
    setupStrips();
}

PatchedTerrain::~PatchedTerrain()
{
    qDeleteAll(m_indices.values());
}

void PatchedTerrain::setupUniformBuffer(
    OpenGLFunctions & gl
,   const PatchIdentifier strip)
{
    gl.glGenBuffers(1, &m_ubos[strip]);
    gl.glBindBuffer(GL_UNIFORM_BUFFER, m_ubos[strip]);
    gl.glBufferData(GL_UNIFORM_BUFFER, m_maxInstancesPerPatch * sizeof(float) * 16, 0, GL_STREAM_DRAW);
    gl.glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

inline int PatchedTerrain::sum(const int a) const
{
    return a * (a + 1) / 2;
}

void PatchedTerrain::setupVertices()
{
    struct vec2
    {
        vec2(float x = 0.f, float y = 0.f)
        : x(x), y(y) { }

        float x;
        float y;
    };

    // creates the bottom triangular tile in range x[0;1], y[0;0.5]

    const int numLanes = pow(2, m_maxLOD) * 6;

    const float xStep = 1.f / static_cast<float>(numLanes);
    const float yStep = xStep * 0.5f;

    // create vertices

    QVector<vec2> rawv;

    // patch center point
    rawv << vec2(0.5f, 0.5f);

    // strip extra points
    for (int l = 0; l < numLanes / 4 - 1; ++l) // lane for lod0
        rawv << vec2(xStep * l * 2.0, yStep * 4.0 * (l + 2));
    for (int l = 0; l < numLanes / 2 - 1; ++l) // lane for lod1
        rawv << vec2(xStep * l * 1.0, yStep * 2.0 * (l + 2));
    for (int l = 0; l < numLanes / 1 - 1; ++l) // lane for lod2
        rawv << vec2(xStep * l * 0.5, yStep * 1.0 * (l + 2));

    const int so = numLanes - 1 + numLanes / 2 - 1 + numLanes / 4 - 1; // strip offset
    assert(1 + so == rawv.size());

    // tile vertices; starting with single left starting triangle

    for (int l = 0; l < numLanes; ++l)
        for (int i = 0; i < numLanes - l; ++i)
            rawv << vec2(xStep * l + i * xStep * 0.5, i * yStep);

    
    assert(sum(numLanes) + 1 + so == rawv.size());

    m_vertices.create();
    m_vertices.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_vertices.bind();
    m_vertices.allocate(rawv.data(), sizeof(float) * rawv.size() * 2);
}

void PatchedTerrain::setupTiles()
{
    QVector<unsigned int> rawi;

    const int numLanes = pow(2, m_maxLOD) * 6;
    const int so = numLanes - 1 + numLanes / 2 - 1 + numLanes / 4 - 1; // strip offset

    for (int tile = 0; tile < 3; ++tile)
    {
        rawi.clear();
        const int t = pow(2, tile);

        for (int l = t; l < numLanes - t; l += t)
        {
            const int i0 = sum(numLanes) - sum(numLanes - l) + 1;
            const int id = sum(numLanes) - sum(numLanes - l - t) - i0 - t + 1;

            rawi << so + i0;

            for (int i = 0; i < numLanes - l - t; i += t)
            {
                rawi << so + i0 + i + t;
                rawi << so + i0 + i + t + id;
            }
            rawi << PrimitiveRestartIndex;
        }

        PatchIdentifier id = static_cast<PatchIdentifier>(Tile2 - tile);
        QOpenGLBuffer * indices(m_indices[id]);

        indices->create();
        indices->setUsagePattern(QOpenGLBuffer::StaticDraw);

        indices->bind();
        indices->allocate(rawi.data(), sizeof(unsigned int)* rawi.size());
        indices->release();

        m_counts[id] = rawi.size();
    }
}

void PatchedTerrain::setupStrips()
{
    setupStrip001122(Strip00);
    setupStrip001122(Strip11);
    setupStrip001122(Strip22);

    setupStrip010212(Strip01);
    setupStrip010212(Strip02);
    setupStrip010212(Strip12);
}

void PatchedTerrain::setupStrip001122(const PatchIdentifier strip)
{
    int tile = -1;
    switch (strip)
    {
    case Strip00:
        tile = 2; break;
    case Strip11:
        tile = 1; break;
    case Strip22:
        tile = 0; break;
    default:
        return;
    }

    QOpenGLBuffer * indices(m_indices[strip]);
    if (indices->isCreated())
        return;

    const int numLanes = pow(2, m_maxLOD) * 6;
    const int so = numLanes - 1 + numLanes / 2 - 1 + numLanes / 4; // strip offset

    int i0 = 1;

    if (tile < 2)
        i0 += numLanes / 4 - 1;
    if (tile < 1)
        i0 += numLanes / 2 - 1;

    const int t = pow(2, tile);

    QVector<unsigned int> rawi;

    rawi << so;
    for (int i = 0; i < numLanes / t - 1; ++i)
    {
        rawi << i0 + i;
        rawi << so + (i + 1) * t;
    }
    rawi << 0;
    rawi << PrimitiveRestartIndex;

    const int id = sum(numLanes) - sum(numLanes - t);
    rawi << so;

    for (int i = 0; i < numLanes - t; i += t)
    {
        rawi << so + i + t;
        rawi << so + i + id;
    }
    rawi << 0;
    rawi << PrimitiveRestartIndex;

    indices->create();
    indices->setUsagePattern(QOpenGLBuffer::StaticDraw);

    indices->bind();
    indices->allocate(rawi.data(), sizeof(unsigned int) * rawi.size());
    indices->release();

    m_counts[strip] = rawi.size();
}

void PatchedTerrain::setupStrip010212(const PatchIdentifier strip)
{
    int l0 = -1;
    int l1 = -1;

    switch (strip)
    {
    case Strip01:
        l0 = 2; l1 = 1; break;
    case Strip02:
        l0 = 2; l1 = 0; break;
    case Strip12:
        l0 = 1; l1 = 0; break;
    default:
        return;
    }

    QOpenGLBuffer * indices(m_indices[strip]);
    if (indices->isCreated())
        return;

    const int numLanes = pow(2, m_maxLOD) * 6;
    const int so = numLanes - 1 + numLanes / 2 - 1 + numLanes / 4; // strip offset

    QVector<unsigned int> rawi;

    const int t0 = pow(2, l0);
    const int t1 = pow(2, l1);

    int id = sum(numLanes) - sum(numLanes - t0);

    // normal lane connected to tile with l0

    rawi << so + t0;
    rawi << so;

    for (int i = 0; i < numLanes / t0; i += 2)
    {
        if (i > 0)
            rawi << so + id + i * t0 - t0;

        rawi << so + id + i * t0;
        if (i < numLanes / t0 - 2)
        {
            rawi << so + id + i * t0 + t0;
            rawi << so +      i * t0 + t0 * 2;
        }
        else
            rawi << 0;
        rawi << PrimitiveRestartIndex;

        if (i < numLanes / t0 - 2)
        {
            rawi << so + i * t0 + t0 * 3;
            rawi << so + i * t0 + t0 * 2;
        }
    }

    // target lane connected to target from l0 to l1

    if (l0 == 2 && l1 == 1)
        id = - (numLanes - 4 + numLanes / 2) - 1;
    else
        id = - (numLanes - t0);

    rawi << so + t0;
    rawi << so;

    int t2 = l1 == 0 ? t0 : t1;

    for (int i = 0; i < numLanes / t0 - 1; ++i)
    {
        if (l1 == 0 && l0 == 2 && i == 0)
            rawi << so + id - 3;
        if (l1 == 0 && l0 == 2)
            rawi << so + id + i * t2 - 2;

        rawi << so + id + i * t2 - 1;
        rawi << so + id + i * t2;
        rawi << so + id + i * t2 + 1;
        if (l1 == 0 && l0 == 2)
            rawi << so + id + i * t2 + 2;

        if (l1 == 0 && l0 == 2 && i >= numLanes / t0 - 2)
            rawi << so + id + i * t2 + 3;
        if (i >= numLanes / t0 - 2)
            rawi << 0;

        rawi << PrimitiveRestartIndex;

        if (i < numLanes / t0 - 2)
        {
            rawi << so + i * t0 + t0 * 2;
            rawi << so + i * t0 + t0;
        }
    }


    indices->create();
    indices->setUsagePattern(QOpenGLBuffer::StaticDraw);

    indices->bind();
    indices->allocate(rawi.data(), sizeof(unsigned int) * rawi.size());
    indices->release();

    m_counts[strip] = rawi.size();
}

void PatchedTerrain::drawReset()
{
    m_cached = false;

    m_transforms[Tile0].clear();
    m_transforms[Tile1].clear();
    m_transforms[Tile2].clear();
    
    m_transforms[Strip00].clear();
    m_transforms[Strip01].clear();
    m_transforms[Strip02].clear();
    m_transforms[Strip11].clear();
    m_transforms[Strip12].clear();
    m_transforms[Strip22].clear();
}

void PatchedTerrain::drawPatch(
    const QVector3D & position
,   float scale
,   unsigned char bottomLOD
,   unsigned char rightLOD
,   unsigned char topLOD
,   unsigned char leftLOD)
{
    QMatrix4x4 PS;
    PS.translate(position);
    PS.scale(scale, 1.f, scale);
    
    static QVector<QMatrix4x4> T;
    if (T.isEmpty())
    {
        QMatrix4x4 t;
        
        t.setToIdentity();
        t.translate(-0.5f, 0.f, -0.5f);
        T << t;

        t.setToIdentity();
        t.rotate(90.f, 0.f, 1.f, 0.f);
        t.translate(-0.5f, 0.f, -0.5f);
        T << t;

        t.setToIdentity();
        t.rotate(180.f, 0.f, 1.f, 0.f);
        t.translate(-0.5f, 0.f, -0.5f);
        T << t;

        t.setToIdentity();
        t.rotate(270.f, 0.f, 1.f, 0.f);
        t.translate(-0.5f, 0.f, -0.5f);
        T << t;

        T << QMatrix4x4(
            0.f, 0.f, 1.f, 0.f
        ,   0.f, 1.f, 0.f, 0.f
        ,   1.f, 0.f, 0.f, 0.f
        ,   0.f, 0.f, 0.f, 1.f);
    }

    // prepare tiles

    const QMatrix4x4 TPS[4] = { PS * T[0], PS * T[1], PS * T[2], PS * T[3] };

    if (bottomLOD < 3)
        m_transforms[static_cast<PatchIdentifier>(Tile0 + bottomLOD)] << TPS[0];
    if (rightLOD  < 3)
        m_transforms[static_cast<PatchIdentifier>(Tile0 + rightLOD)]  << TPS[1];
    if (topLOD    < 3)
        m_transforms[static_cast<PatchIdentifier>(Tile0 + topLOD)]    << TPS[2];
    if (leftLOD   < 3)
        m_transforms[static_cast<PatchIdentifier>(Tile0 + leftLOD)]   << TPS[3];

    // retrieve and prepare strips

    bool mirror;

    PatchIdentifier strip;
    if (bottomLOD < 3 && rightLOD < 3)
    {
        strip = getStrip(bottomLOD, rightLOD, mirror);
        m_transforms[strip] << (mirror ? TPS[0] * T[4] : TPS[0]);
    }
    if (rightLOD < 3 && topLOD < 3)
    {
        strip = getStrip(rightLOD, topLOD, mirror);
        m_transforms[strip] << (mirror ? TPS[1] * T[4] : TPS[1]);
    }
    if (topLOD < 3 && leftLOD < 3)
    {
        strip = getStrip(topLOD, leftLOD, mirror);
        m_transforms[strip] << (mirror ? TPS[2] * T[4] : TPS[2]);
    }
    if (leftLOD < 3 && bottomLOD < 3)
    {
        strip = getStrip(leftLOD, bottomLOD, mirror);
        m_transforms[strip] << (mirror ? TPS[3] * T[4] : TPS[3]);
    }
}

PatchedTerrain::PatchIdentifier PatchedTerrain::getStrip(
    unsigned char lod0
,   unsigned char lod1
,   bool & mirror)
{
    PatchIdentifier P;

    if       (lod0 == 0 && lod1 == 0)
        P = Strip00;
    else if  (lod0 == 1 && lod1 == 1)
        P = Strip11;
    else if  (lod0 == 2 && lod1 == 2)
        P = Strip22;
    else if ((lod0 == 0 && lod1 == 1)
          || (lod0 == 1 && lod1 == 0))
        P = Strip01;
    else if ((lod0 == 0 && lod1 == 2)
          || (lod0 == 2 && lod1 == 0))
        P = Strip02;
    else if ((lod0 == 1 && lod1 == 2)
          || (lod0 == 2 && lod1 == 1))
        P = Strip12;

    mirror = (P == Strip01 && lod0 == 1 && lod1 == 0)
          || (P == Strip02 && lod0 == 2 && lod1 == 0)
          || (P == Strip12 && lod0 == 2 && lod1 == 1);

    return P;
}

void PatchedTerrain::draw(
    OpenGLFunctions & gl
,   QOpenGLShaderProgram & program
,   const bool lineStrip)
{
    gl.glEnable(GL_DEPTH_TEST);
    gl.glDisable(GL_CULL_FACE);

    gl.glEnable(GL_PRIMITIVE_RESTART);
    gl.glPrimitiveRestartIndex(PrimitiveRestartIndex);


    program.bind();
    
    GLuint pid = program.programId();

    static const GLuint binding = 0;
    GLint blockIndex = gl.glGetUniformBlockIndex(pid, "transforms");

    gl.glUniformBlockBinding(pid, blockIndex, binding);

    m_vao.bind();

    draw(gl, binding, Tile0, lineStrip);
    draw(gl, binding, Tile1, lineStrip);
    draw(gl, binding, Tile2, lineStrip);

    draw(gl, binding, Strip00, lineStrip);
    draw(gl, binding, Strip01, lineStrip);
    draw(gl, binding, Strip02, lineStrip);
    draw(gl, binding, Strip11, lineStrip);
    draw(gl, binding, Strip12, lineStrip);
    draw(gl, binding, Strip22, lineStrip);
    
    m_cached = true;

    m_vao.release();

    program.release();

    gl.glDisable(GL_PRIMITIVE_RESTART);
}

inline void PatchedTerrain::draw(
    OpenGLFunctions & gl
,   const GLint binding
,   const PatchIdentifier patch
,   const bool lineStrip)
{
    if (m_transforms[patch].isEmpty())
        return;

    gl.glBindBuffer(GL_UNIFORM_BUFFER, m_ubos[patch]);

    if (!m_cached)
        gl.glBufferSubData(GL_UNIFORM_BUFFER, 0, m_transforms[patch].size() * sizeof(float)* 16, m_transforms[patch].data());

    gl.glBindBufferRange(GL_UNIFORM_BUFFER, binding, m_ubos[patch], 0, m_transforms[patch].size() * sizeof(float)* 16);

 /*     gl.glPointSize(4.f);
        m_vertices.bind();
        gl.glDrawArrays(GL_POINTS, 0, m_vertices.size());
*/
    GLenum mode = GL_TRIANGLE_STRIP;
    if (patch == Strip01 || patch == Strip02 || patch == Strip12)
        mode = GL_TRIANGLE_FAN;

    if (lineStrip)
        mode = GL_LINE_STRIP;

    m_indices[patch]->bind();
    gl.glDrawElementsInstanced(mode, m_counts[patch], GL_UNSIGNED_INT, nullptr, m_transforms[patch].size());
    m_indices[patch]->release();
}
