
#pragma once

#include <QMap>
#include <QVector>
#include <QVector3D>

#include "AbstractPainter.h"

class QOpenGLShader;
class QOpenGLShaderProgram;

class Camera;
class ScreenAlignedQuad;


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

    void paint_5_1(float timef);
    void paint_5_2(float timef);
    void paint_5_3(float timef);
    void paint_5_4(float timef);

    void reset_5();

    void step(float deltaf);
    void draw(float timef, const int mode);
    void glow(float timef);

protected:
    QOpenGLShaderProgram * createBasicShaderProgram(
        const QString & vertexShaderFileName
    ,   const QString & fragmentShaderFileName);

    QOpenGLShaderProgram * createBasicShaderProgramVF2(
        const QString & vertexShaderFileName
    ,   const QString & fragmentShaderFileName0
    ,   const QString & fragmentShaderFileName1);

    QOpenGLShaderProgram * createBasicShaderProgramVF3(
        const QString & vertexShaderFileName
    ,   const QString & fragmentShaderFileName0
    ,   const QString & fragmentShaderFileName1
    ,   const QString & fragmentShaderFileName2);

    // ...

protected:
    ScreenAlignedQuad * m_quad;

    QMap<int, QOpenGLShaderProgram *> m_programs;
    QList<QOpenGLShader *> m_shaders;

    GLuint m_positions;
    GLuint m_velocities;
    GLuint m_materials[2];
    GLuint m_forces;
    GLuint m_envmap;
    GLuint m_glows[2];
    GLuint m_color;

    GLuint m_stepFBO;
    GLuint m_colorFBO;
    GLuint m_glowFBO;
    GLuint m_depth;


    GLuint m_width;
    GLuint m_height;

    float m_elapsed;
};
