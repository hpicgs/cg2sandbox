
#pragma once

#include <QMatrix4x4>
#include <QMap>
#include <QVector3D>

#include "AbstractPainter.h"

class QOpenGLShader;
class QOpenGLShaderProgram;

class Camera;
class Terrain;
class ScreenAlignedQuad;
class AssimpScene;


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

    void bindEnvMaps(GLenum target);
    void unbindEnvMaps(GLenum target);

    void paint_3_1(float timef);
    void paint_3_1_scene(bool shadow, float timef);
    void paint_3_1_label(const QMatrix4x4 & viewProjection, float timef);
    void paint_3_2(float timef); 
    void paint_3_2_label(const QMatrix4x4 & viewProjection, float timef);
    void paint_3_3(float timef);
    void paint_3_3_shadowmap(float timef);
    void paint_3_4(float timef);

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

    QList<QMatrix4x4> m_transforms;

    QMap<int, QOpenGLShaderProgram *> m_programs;
    QList<QOpenGLShader *> m_shaders;

    AssimpScene * m_hpicgs;
    AssimpScene * m_plane;
    AssimpScene * m_portcc;

    QVector3D m_light;

    ScreenAlignedQuad * m_hpicgsLabel;
    ScreenAlignedQuad * m_portccLabel;

    GLuint m_hpicgsLabelAM;
    GLuint m_portccLabelAM;

    GLuint m_hpicgsLabelDM;
    GLuint m_portccLabelDM;

    GLuint m_depthFbo;
    GLuint m_depthTex;
};
