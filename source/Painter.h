
#pragma once

#include <QMatrix4x4>
#include <QMap>
#include <QVector2D>

#include "AbstractPainter.h"

class QOpenGLShader;
class QOpenGLShaderProgram;

class Camera;
class AssimpScene;
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
    void paintPhong(float timef);
    void paintDiffuse(float timef);
    void paintNormal(float timef);
    void paintSpecular(float timef);
    void paintEmission(float timef);
    void paintCombined(float timef);
    void paintCombinedWithBlur(float timef);

protected:
    GLuint getOrCreateTexture(const QString & fileName);

    QOpenGLShaderProgram * createBasicShaderProgram(
        const QString & vertexShaderFileName
    ,   const QString & fragmentShaderFileName);
    QOpenGLShaderProgram * createBasicShaderProgram(
        const QString & vertexShaderFileName
        , const QString & geometryShaderFileName
        , const QString & fragmentShaderFileName);

protected:
    Camera * m_camera;

    AssimpScene * m_model;

    QVector3D m_light;

    QList<QMatrix4x4> m_transforms;

    QMap<int, QOpenGLShaderProgram *> m_programs;
    QList<QOpenGLShader *> m_shaders;

    unsigned int m_diffuseTexture;
    unsigned int m_normalsTexture;
    unsigned int m_specularTexture;
    unsigned int m_emissionTexture;

    // Task_4_4 - ToDo Begin

    // Add members to store identifiers to FBOs, textures and renderbuffers needed for the bloom effect

    // Task_4_4 - ToDo End

    ScreenAlignedQuad * m_quad;
};
