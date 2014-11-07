
#pragma once

#include <QMatrix4x4>
#include <QMap>
#include <QVector3D>

#include "AbstractPainter.h"

class QOpenGLShader;
class QOpenGLShaderProgram;
class QKeyEvent;

class Camera;
class Terrain;
class ScreenAlignedQuad;
class Icosahedron;


class Painter : public AbstractPainter
{
public:
    Painter();
    virtual ~Painter();

    virtual bool initialize();
    
    virtual void paint(float timef);
    virtual void resize(
        int width
    ,   int height);

    virtual void update();
    virtual void update(const QList<QOpenGLShaderProgram *> & programs);

    void keyPressEvent(QKeyEvent * event);

protected:
    void paint_1_1(float timef);
    void paint_1_2(float timef);
    void paint_1_3(float timef);
    void paint_1_4(float timef);
    //void paint_1_5(float timef);
    //...

    void bindEnvMaps(GLenum target);
    void unbindEnvMaps(GLenum target);

    void paint_2_1_envmap(const int programIndex, float timef);
    void paint_2_2_sphere(const int programIndex, float timef);

    void paint_2_3_terrain(const int programIndex, float timef);
    void paint_2_3_water(const int programIndex, float timef);

    void paint_2_1(float timef);
    void paint_2_2(float timef);
    void paint_2_3(float timef);
    void paint_2_4(float timef);
    //void paint_2_5(float timef);
    //...

    enum EnvironmentMapping
    {
        CubeMapping
    ,   PolarMapping
    ,   ParaboloidMapping
    ,   SphereMapping
    };

protected:
    QOpenGLShaderProgram * createBasicShaderProgram(
        const QString & vertexShaderFileName
    ,   const QString & fragmentShaderFileName);

    QOpenGLShaderProgram * createBasicShaderProgram(
        const QString & vertexShaderFileName
    ,   const QString & geometryShaderFileName
    ,   const QString & fragmentShaderFileName);

protected:
    Camera * m_camera;
    
    ScreenAlignedQuad * m_quad;
    
    EnvironmentMapping m_mapping;
    QMap<EnvironmentMapping, GLuint> m_envmaps;

    Icosahedron * m_icosa;
    QVector3D m_icosa_center;

    QList<Terrain *> m_terrains;
    QList<QMatrix4x4> m_transforms;

    QMap<int, QOpenGLShaderProgram *> m_programs;
    QList<QOpenGLShader *> m_shaders;

    GLuint m_height;
    GLuint m_ground;
    GLuint m_caustics;

    GLuint m_cubeFBO;
    GLuint m_cubeTex;
    GLuint m_cubeDepthRB;


    // ############
    // ToDo: Remove
    GLuint m_waterheights;
    GLuint m_waternormals;
};
