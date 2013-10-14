
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
    
    virtual void paint();
    virtual void resize(
        int width
    ,   int height);

    virtual void update();
    virtual void update(const QList<QOpenGLShaderProgram *> & programs);

protected:
    Camera * m_camera;

    UnitCube * m_cube;
    QOpenGLShaderProgram * m_program;

    QOpenGLShader * m_fragShader;
    QOpenGLShader * m_vertShader;
};
