
#pragma once

#include <QMatrix4x4>
#include <QMap>

#include "AbstractPainter.h"

class QOpenGLShader;
class QOpenGLShaderProgram;

class Camera;
class Terrain;
class UnitCube;

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

protected:
    void paint_1_1(float timef);
    void paint_1_2(float timef);
    void paint_1_3(float timef);
    void paint_1_4(float timef);
    //void paint_1_5(float timef);
    //...

    void paint_2_1(float timef);
    void paint_2_2(float timef);
    void paint_2_3(float timef);
    void paint_2_4(float timef);
    //void paint_2_5(float timef);
    //...

protected:
    GLuint getOrCreateTexture(const QString & fileName);

    QOpenGLShaderProgram * createBasicShaderProgram(
        const QString & vertexShaderFileName
    ,   const QString & fragmentShaderFileName);

protected:
    Camera * m_camera;

    UnitCube * m_cube; // Note: This is for the test rendering

    QList<Terrain *> m_terrains;
    QList<QMatrix4x4> m_transforms;

    QMap<int, QOpenGLShaderProgram *> m_programs;
    QList<QOpenGLShader *> m_shaders;

    GLuint m_height;
    GLuint m_ground;
};
