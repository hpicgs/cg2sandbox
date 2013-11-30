
#include <cassert>

#include <QKeyEvent>

#include "Terrain.h"
#include "FileAssociatedShader.h"
#include "FileAssociatedTexture.h"
#include "Camera.h"
#include "Canvas.h"
#include "AssimpScene.h"
#include "DistanceTransform.h"

#include "ScreenAlignedQuad.h"

#include "Painter.h"

namespace
{
    const int ShadowMapSize = 2048;

    const int LabelAlphaMappingProgram  = AbstractPainter::PaintMode9 + 1;
    const int LabelDistanceMapProgram   = AbstractPainter::PaintMode9 + 2;
    const int ShadowMapProgram          = AbstractPainter::PaintMode9 + 3;
    const int ShadowMapDumpProgram      = AbstractPainter::PaintMode9 + 4;

    // const int OtherProgram = AbstractPainter::PaintMode9 + 2;
    // ...
}


Painter::Painter()
: m_camera(nullptr)
, m_quad(nullptr)

, m_hpicgs(nullptr)
, m_plane(nullptr)
, m_portcc(nullptr)

, m_hpicgsLabel(nullptr)
, m_portccLabel(nullptr)

, m_hpicgsLabelAM(-1)
, m_portccLabelAM(-1)
, m_hpicgsLabelDM(-1)
, m_portccLabelDM(-1)

, m_depthFbo(-1)
, m_depthTex(-1)
{
    setMode(PaintMode0);
}

Painter::~Painter()
{
    qDeleteAll(m_programs);
    qDeleteAll(m_shaders);

    delete m_quad;
    delete m_hpicgs;
    delete m_plane;
    delete m_portcc;

    delete m_hpicgsLabel;
    delete m_portccLabel;

    if (-1 != m_depthTex)
        glDeleteTextures(1, &m_depthTex);
    if (-1 != m_depthFbo)
        glDeleteFramebuffers(1, &m_depthFbo);

    FileAssociatedTexture::clean(*this);
}

bool Painter::initialize()
{
    initializeOpenGLFunctions();

    // Note: As always, feel free to change/modify parameters .. as long as its possible to 
    // see the terrain and navigation basically works, everything is ok. 

    glClearColor(1.f, 1.f, 1.f, 0.f);

    m_quad = new ScreenAlignedQuad(*this);

    m_light = QVector3D(2.f, 3.f, 2.f);

    QOpenGLShaderProgram * program;

    // setup default assimp scene shader (rendering the meshes...)

    // ToDo: If you render your own scene (e.g., a terrain, keep in mind
    // to extend your shaders for supporting shadowmaps as well as in assimp shaders...

    program = createBasicShaderProgram("data/assimp.vert", "data/assimp.frag");
    m_programs[PaintMode1] = program;
    program->bindAttributeLocation("a_vertex", 0);
    program->bindAttributeLocation("a_normal", 1);
    program->bindAttributeLocation("a_texc", 2);    
    program->link();

    // load labeling shaders and resources

    m_programs[LabelAlphaMappingProgram]  = createBasicShaderProgram("data/label.vert", "data/labelAM.frag");
    m_programs[LabelDistanceMapProgram] = createBasicShaderProgram("data/label.vert", "data/labelDM.frag");
    
    m_hpicgsLabelAM = FileAssociatedTexture::getOrCreate2D("data/hpicgs_label.png", *this);
    m_portccLabelAM = FileAssociatedTexture::getOrCreate2D("data/companion_cube_label.png", *this);

    // load and position 3d models in the scene 
    // Note: feel free to modify / add / remove here... there should be at least 
    // a few scene objects visible ;)

    m_hpicgs = new AssimpScene(*this, "data/hpicgs.obj", true);
    m_hpicgs->transform().scale(0.7f, 0.7f, 0.7f);
    m_hpicgs->transform().translate(-0.9f, 0.8f, -0.3f);
    m_hpicgs->transform().rotate( 56.0f, 0.f, 1.0f, 0.f);
    m_hpicgs->transform().rotate(0.0f, 1.f, 0.0f, 0.f);
    m_plane = new AssimpScene(*this, "data/plane.obj", false);
    m_plane->transform().translate(0.f, -0.1f, 0.f);
    m_portcc = new AssimpScene(*this, "data/companion_cube.obj", true);
    m_portcc->transform().scale(0.5f, 0.5f, 0.5f);
    m_portcc->transform().translate(1.1f, 0.73f, 1.2f);
    m_portcc->transform().rotate(8.0f, 0.f, 1.0f, 0.f);

    // initialize label positions


    // Task_3_1 - ToDo Begin

    // Modify the transformation matrix T to position, scale, and orient 
    // the labels to appropriate places, based on the initial view! No
    // interaction should be required to be able to see both labels.

    m_hpicgsLabel = new ScreenAlignedQuad(*this, 0);
    QMatrix4x4 T;

    // ToDo: use T.translate/scale/rotate ...

    m_transforms << T;

    m_portccLabel = new ScreenAlignedQuad(*this, 0);

    // ToDo: use T.translate/scale/rotate ...

    m_transforms << T;

    // Task_3_1 - ToDo End


    // initialize shader program attribute input

    program = m_programs[LabelAlphaMappingProgram];
    program->bind();
    program->bindAttributeLocation("a_vertex", 0);
    program->release();

    program = m_programs[LabelDistanceMapProgram];
    program->bind();
    program->bindAttributeLocation("a_vertex", 0);
    program->release();

    // create distance transforms of label images

    glGenTextures(1, &m_hpicgsLabelDM);
    glBindTexture(GL_TEXTURE_2D, m_hpicgsLabelDM);
        
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    {
        QImage image("data/hpicgs_label_bitmask.png");

        // ToDo: pre resize?

        DistanceTransform DT(image, 512, 128, 0.0625f);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 512, 128, 0, GL_RED, GL_FLOAT, DT.data());
    }
    glGenerateMipmap(GL_TEXTURE_2D);


    glGenTextures(1, &m_portccLabelDM);
    glBindTexture(GL_TEXTURE_2D, m_portccLabelDM);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    {
        QImage image("data/companion_cube_label_bitmask.png");

        // ToDo: pre resize?
        // image = image.scaled(? , ? , Qt::FastTransformation);

        DistanceTransform DT(image, 512, 128, 0.0625f);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 512, 128, 0, GL_RED, GL_FLOAT, DT.data());
    }        
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    // create fbo

    m_programs[ShadowMapProgram] = createBasicShaderProgram("data/shadowmap.vert", "data/shadowmap.frag");
    m_programs[ShadowMapDumpProgram] = createBasicShaderProgram("data/dump.vert", "data/dump.frag");

    glGenTextures(1, &m_depthTex);
    glBindTexture(GL_TEXTURE_2D, m_depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, ShadowMapSize, ShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &m_depthFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthFbo);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "depth fbo invalid";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}


void Painter::keyPressEvent(QKeyEvent * event)
{
    switch (event->key())
    {
    case Qt::Key_X:
        m_light += QVector3D(event->modifiers() && Qt::Key_Shift ? 0.05f : -0.05f, 0.f, 0.f);
        update();
        break;
    case Qt::Key_Y:
        m_light += QVector3D(0.f, event->modifiers() && Qt::Key_Shift ? 0.05f : -0.05f, 0.f);
        update();
        break;
    case Qt::Key_Z:
        m_light += QVector3D(0.f, 0.f, event->modifiers() && Qt::Key_Shift ? 0.05f : -0.05f);
        update();
        break;
    default:
        break;
    }
}


QOpenGLShaderProgram * Painter::createBasicShaderProgram(
    const QString & vertexShaderFileName
    , const QString & fragmentShaderFileName)
{
    QOpenGLShaderProgram * program = new QOpenGLShaderProgram();

    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Vertex, vertexShaderFileName, *program);
    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Fragment, fragmentShaderFileName, *program);
    program->bindAttributeLocation("a_vertex", 0);
    program->link();

    return program;
}

QOpenGLShaderProgram * Painter::createBasicShaderProgram(
    const QString & vertexShaderFileName
    , const QString & geometryShaderFileName
    , const QString & fragmentShaderFileName)
{
    QOpenGLShaderProgram * program = new QOpenGLShaderProgram();

    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Vertex, vertexShaderFileName, *program);
    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Geometry, geometryShaderFileName, *program);
    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Fragment, fragmentShaderFileName, *program);
    program->bindAttributeLocation("a_vertex", 0);
    program->link();

    return program;
}

void Painter::resize(
    int width
    , int height)
{
    glViewport(0, 0, width, height);
    update();
}

void Painter::update()
{
    update(m_programs.values());
}

void Painter::update(const QList<QOpenGLShaderProgram *> & programs)
{
    foreach(const int i, m_programs.keys())
    {
        QOpenGLShaderProgram * program(m_programs[i]);

        if (programs.contains(program) && program->isLinked())
        {
            program->bind();

            switch (i)
            {
            case PaintMode0:
            case PaintMode9:
            case PaintMode8:
            case PaintMode7:
            case PaintMode6:
            case PaintMode5:
            case PaintMode4:
            case PaintMode3:
            case PaintMode2:
            case PaintMode1:
                program->setUniformValue("projection", camera()->projection());
                program->setUniformValue("view", camera()->view());
                program->setUniformValue("shadowmap", 1);
                program->setUniformValue("shadow", false);
                break;
            case LabelAlphaMappingProgram:
            case LabelDistanceMapProgram:
                program->setUniformValue("label", 0);
                break;
            case ShadowMapProgram:
                program->setUniformValue("viewProjection", camera()->viewProjection());
                break;
            case ShadowMapDumpProgram:
                program->setUniformValue("source", 0);
                break;
                //case OtherProgram: // feel free to use more than one program per mode...
                //    break;
            }

            program->release();
        }
    }
}

void Painter::paint(float timef)
{
    switch (mode())
    {
    case PaintMode0:
    case PaintMode1:
        paint_3_1(timef); break;
    case PaintMode2:
        paint_3_2(timef); break;
    case PaintMode3:
        paint_3_3(timef); break;
    case PaintMode4:
        paint_3_4(timef); break;
        //paint_1_4(timef); break;
        //case PaintMode5:
        //    paint_1_5(timef); break;
        // ...
    default:
        break;
    }
}

// Note: Feel free to remove your old code and start on minor cleanups and refactorings....

void Painter::paint_3_1_scene(const bool shadow, float timef)
{
    QOpenGLShaderProgram * program(m_programs[PaintMode1]);

    program->bind();
    program->setUniformValue("light", m_light);
    program->setUniformValue("useshadow", shadow);
    program->release();

    m_hpicgs->draw(*this, *program);
    m_portcc->draw(*this, *program);
    m_plane->draw(*this, *program);
}

void Painter::paint_3_1(float timef)
{
    paint_3_1_scene(false, timef);
    paint_3_1_label(camera()->viewProjection(), timef);
}

void Painter::paint_3_1_label(
    const QMatrix4x4 & viewProjection
,   float timef)
{
    QOpenGLShaderProgram * program;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);

    program = m_programs[LabelAlphaMappingProgram];
    program->bind();

    glBindTexture(GL_TEXTURE_2D, m_hpicgsLabelAM);
    program->setUniformValue("mvp", viewProjection * m_transforms[0]);
    m_hpicgsLabel->draw(*this);

    glBindTexture(GL_TEXTURE_2D, m_portccLabelAM);
    program->setUniformValue("mvp", viewProjection * m_transforms[1]);
    m_portccLabel->draw(*this);

    program->release();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void Painter::paint_3_2(float timef)
{
    paint_3_1_scene(false, timef);
    paint_3_2_label(camera()->viewProjection(), timef);
}

void Painter::paint_3_2_label(
    const QMatrix4x4 & viewProjection
,   float timef)
{
    QOpenGLShaderProgram * program;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);

    program = m_programs[LabelDistanceMapProgram];
    program->bind();

    glBindTexture(GL_TEXTURE_2D, m_hpicgsLabelDM);
    program->setUniformValue("mvp", viewProjection * m_transforms[0]);
    m_hpicgsLabel->draw(*this);

    glBindTexture(GL_TEXTURE_2D, m_portccLabelDM);
    program->setUniformValue("mvp", viewProjection * m_transforms[1]);
    m_portccLabel->draw(*this);

    program->release();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void Painter::paint_3_3_shadowmap(float timef)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthFbo);
    glViewport(0, 0, ShadowMapSize, ShadowMapSize);

    glClearDepth(1.f);
    glClear(GL_DEPTH_BUFFER_BIT);

    QOpenGLShaderProgram * program(m_programs[ShadowMapProgram]);


    // Task_3_3 - ToDo Begin

    QMatrix4x4 L;

    // L ...

    // Task_3_3 - ToDo End


    program->bind();
    // program->setUniformValue("viewProjection", L);
    program->release();

    m_hpicgs->draw(*this, *program);
    m_portcc->draw(*this, *program);

    paint_3_2_label(L, timef);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, camera()->viewport().width(), camera()->viewport().height());
}

void Painter::paint_3_3(float timef)
{
    paint_3_3_shadowmap(timef);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_depthTex);

    QOpenGLShaderProgram * program(m_programs[ShadowMapDumpProgram]);

    program->bind();
    m_quad->draw(*this);
    program->release();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Painter::paint_3_4(float timef)
{
    paint_3_3_shadowmap(timef);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_depthTex);

    glActiveTexture(GL_TEXTURE0);

    QOpenGLShaderProgram * program(m_programs[PaintMode1]);


    // Task_3_3 - ToDo Begin

    // QMatrix4x4 ...
    
    program->bind();
    program->setUniformValue("light", m_light);
    // program->setUniformValue("todo", ?);
    program->release();

    // Task_3_3 - ToDo End


    paint_3_1_scene(true, timef);
    paint_3_2_label(camera()->viewProjection(), timef);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}
