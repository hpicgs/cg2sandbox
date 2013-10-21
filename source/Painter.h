
#pragma once

#include <QMatrix4x4>

#include "AbstractPainter.h"

class QOpenGLShader;
class QOpenGLShaderProgram;

class Camera;
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
    void paint_1_5(float timef);

protected:
    Camera * m_camera;

    UnitCube * m_cube;
    QOpenGLShaderProgram * m_program;

    QOpenGLShader * m_fragShader;
    QOpenGLShader * m_vertShader;
};
