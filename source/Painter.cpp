
#include <cassert>

#include <QImageReader>
#include <QKeyEvent>

#include "UnitCube.h"
#include "FileAssociatedShader.h"
#include "FileAssociatedTexture.h"
#include "Camera.h"
#include "Canvas.h"
#include "AssimpScene.h"
#include "ScreenAlignedQuad.h"

#include "Painter.h"

namespace
{
    const int PhongProgram = AbstractPainter::PaintMode1;
    const int DiffuseProgram = AbstractPainter::PaintMode2;
    const int NormalProgram = AbstractPainter::PaintMode3;
    const int SpecularProgram = AbstractPainter::PaintMode4;
    const int EmissionProgram = AbstractPainter::PaintMode5;
    const int RenderProgram = AbstractPainter::PaintMode6;
    const int RenderToTextureProgram = AbstractPainter::PaintMode7;

    // Task_4_4 - ToDo Begin

    // Add symbolic constants to identify additional Programs needed for bloom effect and deferred shading

    // Task_4_4 - ToDo End
}


Painter::Painter()
: m_camera(nullptr)
, m_model(nullptr)
, m_diffuseTexture(static_cast<unsigned int>(-1))
, m_normalsTexture(static_cast<unsigned int>(-1))
, m_specularTexture(static_cast<unsigned int>(-1))
, m_emissionTexture(static_cast<unsigned int>(-1))

// Task_4_4 - ToDo Begin

// Initialize all additional FBOs, textures and renderbuffers needed for the bloom effect with the invalid value -1

// Task_4_4 - ToDo End

{
}

Painter::~Painter()
{
    qDeleteAll(m_programs);
    qDeleteAll(m_shaders.toSet());

    if (-1 != static_cast<int>(m_diffuseTexture))
        glDeleteTextures(1, &m_diffuseTexture);
    if (-1 != static_cast<int>(m_normalsTexture))
        glDeleteTextures(1, &m_normalsTexture);
    if (-1 != static_cast<int>(m_specularTexture))
        glDeleteTextures(1, &m_specularTexture);
    if (-1 != static_cast<int>(m_emissionTexture))
        glDeleteTextures(1, &m_emissionTexture);

    // Task_4_4 - ToDo Begin

    // Free all additional FBOs, textures and renderbuffers needed for the bloom effect

    // Task_4_4 - ToDo End

    delete m_model;
}

bool Painter::initialize()
{
    initializeOpenGLFunctions();

    m_programs[PhongProgram] = createBasicShaderProgram("data/render_flat.vert", "data/phong.frag");
    m_programs[DiffuseProgram] = createBasicShaderProgram("data/render_flat.vert", "data/diffuse.frag");
    m_programs[NormalProgram] = createBasicShaderProgram("data/render_flat.vert", "data/normal.frag");
    m_programs[SpecularProgram] = createBasicShaderProgram("data/render_flat.vert", "data/specular.frag");
    m_programs[EmissionProgram] = createBasicShaderProgram("data/render_flat.vert", "data/emission.frag");
    m_programs[RenderProgram] = createBasicShaderProgram("data/render.vert", "data/render.frag");

    // 3D model
    m_model = new AssimpScene(*this, "data/Imrod.obj", true);

    // Light
    m_light = QVector3D(40.f, 40.f, 20.f);

    // ScreenAlignedQuad
    m_quad = new ScreenAlignedQuad(*this);

    // Material

    // Task_4_1 - ToDo Begin

    // Load all textures from the 3D model into m_diffuseTexture, m_normalsTexture, m_specularTexture, and m_illuminationTexture
    // m_diffuseTexture = FileAssociatedTexture::getOrCreate2D("...", *this);

    // Task_4_1 - ToDo End

    // Render stage

    // Task_4_4 - ToDo Begin

    // Create all needed FBOs, textures and renderbuffers for the offscreen-rendering of the model

    // glGenFramebuffers(1, &...);
    // glBindFramebuffer(GL_FRAMEBUFFER, ...);

    // glGenTextures(1, &...);
    // glBindTexture(GL_TEXTURE_2D, ...);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ...);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ...);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ...);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ...);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    // glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ..., 0);

    // continue for other needed textures and renderbuffers

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Task_4_4 - ToDo End


    // Blur stage

    // Task_4_4 - ToDo Begin

    // Create the FBO, the result texture and the program for the blur

    // glGenFramebuffers(1, &...);
    // glBindFramebuffer(GL_FRAMEBUFFER, ...);

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // m_programs[...] = createBasicShaderProgram("data/screenalignedquad.vert", "... .frag");

    // Task_4_4 - ToDo End

    // Combine stage

    // Task_4_4 - ToDo Begin

    // m_programs[CombineProgram] = createBasicShaderProgram("data/screenalignedquad.vert", "... .frag");

    // Task_4_4 - ToDo End

    // Note: As always, feel free to change/modify parameters .. as long as its possible to 
    // see the terrain and navigation basically works, everything is ok.

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0);

    m_transforms << QMatrix4x4();

    return true;
}

QOpenGLShaderProgram * Painter::createBasicShaderProgram(
    const QString & vertexShaderFileName
,   const QString & fragmentShaderFileName)
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
,   int height)
{
    glViewport(0, 0, width, height);

    // Task_4_4 - ToDo Begin

    // Bind and resize all textures and renderbuffers for offscreen-rendering
    // These includes the offscreen rendering of the model as well as the blurring

    // Task_4_4 - ToDo End

    foreach(const int i, m_programs.keys())
    {
        QOpenGLShaderProgram * program(m_programs[i]);
        if (program->isLinked())
        {
			program->bind();

            program->setUniformValue("viewProjection"
                , camera()->viewProjection() * m_transforms[0]);
            program->setUniformValue("view"
                , camera()->view() * m_transforms[0]);
            program->setUniformValue("camera", camera()->eye());
            program->setUniformValue("normalMatrix", camera()->viewInverted().transposed());
            program->setUniformValue("screenSize", camera()->viewport());
            program->setUniformValue("light", m_light);

            program->release();
        }
    }
}

void Painter::keyPressEvent(QKeyEvent * event)
{
    switch (event->key())
    {
    case Qt::Key_X:
        m_light += QVector3D(event->modifiers() && Qt::Key_Shift ? 5.0f : -5.0f, 0.f, 0.f);
        update();
        break;
    case Qt::Key_Y:
        m_light += QVector3D(0.f, event->modifiers() && Qt::Key_Shift ? 5.0f : -5.0f, 0.f);
        update();
        break;
    case Qt::Key_Z:
        m_light += QVector3D(0.f, 0.f, event->modifiers() && Qt::Key_Shift ? 5.0f : -5.0f);
        update();
        break;
    default:
        break;
    }
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
            
            // Note: transform might be the modelview projection matrix, or similar.
            // the identifier was choosen to show that all transform is done before
            // GPU, to reduce vertex shader workload. So feel free to modify...

            switch (i)
            {
            case PhongProgram:
            case DiffuseProgram:
            case NormalProgram:
            case SpecularProgram:
            case EmissionProgram:
            case RenderProgram:
            case RenderToTextureProgram:
                program->setUniformValue("viewProjection"
                    , camera()->viewProjection() * m_transforms[0]);
                program->setUniformValue("view"
                    , camera()->view() * m_transforms[0]);
                program->setUniformValue("camera", camera()->eye());
                program->setUniformValue("normalMatrix", camera()->viewInverted().transposed());

                program->setUniformValue("diffuseTexture", 0);
                program->setUniformValue("normalsTexture", 1);
                program->setUniformValue("specularTexture", 2);
                program->setUniformValue("emissionTexture", 3);
                program->setUniformValue("light", m_light);

                break;

            // Task_4_4 - ToDo Begin

            // Update uniforms of created programs

            // case ...:
            // break;

            // Task_4_4 - ToDo End

            }

            program->release();
        }
    }
}

void Painter::paint(float timef)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (mode())
    {
    case PaintMode1:
        paintPhong(timef); break;
    case PaintMode2:
        paintDiffuse(timef); break;
    case PaintMode3:
        paintNormal(timef); break;
    case PaintMode4:
        paintSpecular(timef); break;
    case PaintMode5:
        paintEmission(timef); break;
    case PaintMode6:
        paintCombined(timef); break;
    case PaintMode7:
        paintCombinedWithBlur(timef); break;
    default:
        break;
    }
}

void Painter::paintPhong(float /*timef*/)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (m_programs[PhongProgram]->isLinked())
    {
        m_model->draw(*this, *m_programs[PhongProgram]);
    }
}

void Painter::paintDiffuse(float /*timef*/)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (m_programs[DiffuseProgram]->isLinked())
    {
        // Task_4_1 - ToDo Begin

        // Bind m_diffuseTexture to active texture unit 0

        // Task_4_1 - ToDo End

        m_model->draw(*this, *m_programs[DiffuseProgram]);
    }
}

void Painter::paintNormal(float /*timef*/)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (m_programs[NormalProgram]->isLinked())
    {
        // Task_4_1 - ToDo Begin

        // Bind m_normalsTexture to active texture unit 1

        // Task_4_1 - ToDo End

        m_model->draw(*this, *m_programs[NormalProgram]);
    }
}

void Painter::paintSpecular(float /*timef*/)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (m_programs[SpecularProgram]->isLinked())
    {
        // Task_4_1 - ToDo Begin

        // Bind m_specularTexture to active texture unit 2

        // Task_4_1 - ToDo End

        m_model->draw(*this, *m_programs[SpecularProgram]);
    }
}

void Painter::paintEmission(float /*timef*/)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (m_programs[EmissionProgram]->isLinked())
    {
        // Task_4_1 - ToDo Begin

        // Bind m_illuminationTexture to active texture unit 3

        // Task_4_1 - ToDo End

        m_model->draw(*this, *m_programs[EmissionProgram]);
    }
}

void Painter::paintCombined(float timef)
{
    if (m_programs[RenderProgram]->isLinked())
    {
        m_programs[RenderProgram]->bind();
        m_programs[RenderProgram]->setUniformValue("timef", timef);

        // Task_4_2 - ToDo Begin

        // First render stage
        // Enable the 3 coloring textures (diffuse, specular, illumination) as active textures for access during the rendering

        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, ...);

        // Task_4_2 - ToDo End

        // Task_4_3 - ToDo Begin

        // Enable the normal texture for access during the rendering

        // Task_4_3 - ToDo End

        m_model->draw(*this, *m_programs[RenderProgram]);
    }
}

void Painter::paintCombinedWithBlur(float timef)
{
    // Task_4_4 - ToDo Begin

    // Render to separate FBO to postpone rendering to back buffer until bloom texture is computed
    // Enable all drawbuffers (GL_COLOR_ATTACHMENT0, ...)

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Copy rendering from Painter::paintCombined but use a new program (for render to texture) instead

    // remove this fallback draw call
    paintCombined(timef);

    // Add two extra render stages

    // (1)
    // Enable bloom texture as active texture
    // Render screen aligned quad (see class ScreenAlignedQuad) to blur the bloom texture rendered in first render stage

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // uncomment following line to clear default back buffer
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // (2)
    // Activate all textures needed for combination of final image (e.g., rasterized image and final bloom texture)
    // Render the quad using your program for combination

    // m_programs[...]->bind();
    // m_quad->draw(*this);
    // m_programs[...]->release();

    // Task_4_4 - ToDo End
}
