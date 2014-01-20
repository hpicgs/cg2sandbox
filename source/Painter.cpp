
#include <cassert>
#include <time.h>

#include <QKeyEvent>
#include <QVector4D>
#include <QRectF>
#include <QPolygonF>

#include "MathMacros.h"
#include "FileAssociatedShader.h"
#include "FileAssociatedTexture.h"
#include "Camera.h"
#include "Canvas.h"

#include "ScreenAlignedQuad.h"

#include "Painter.h"

namespace
{
    const int ParticleStep  = AbstractPainter::PaintMode9 + 1;
    const int ParticleBlurH = AbstractPainter::PaintMode9 + 2;
    const int ParticleBlurV = AbstractPainter::PaintMode9 + 3;
}


Painter::Painter()
: m_quad(nullptr)
, m_positions(-1)
, m_velocities(-1)
, m_forces(-1)
, m_envmap(-1)
, m_color(-1)
, m_stepFBO(-1)
, m_colorFBO(-1)
, m_glowFBO(-1)
, m_depth(-1)
, m_elapsed(0.f)
{
    setMode(PaintMode1);
    m_materials[0] = -1;
    m_materials[1] = -1;
}

Painter::~Painter()
{
    if (m_positions != -1)
        glDeleteTextures(1, &m_positions);
    if (m_velocities != -1)
        glDeleteTextures(1, &m_velocities);
    if (m_color != -1)
        glDeleteTextures(1, &m_color);

    for (int i = 0; i < 2; ++i)
        if (m_glows[i] != -1)
            glDeleteTextures(1, &m_glows[i]);

    for (int i = 0; i < 2; ++i)
        if (m_materials[i] != -1)
            glDeleteTextures(1, &m_materials[i]);

    if (m_forces != -1)
        glDeleteTextures(1, &m_forces);
    if (m_envmap != -1)
        glDeleteTextures(1, &m_envmap);

    if (m_colorFBO != -1)
        glDeleteFramebuffers(1, &m_colorFBO);
    if (m_glowFBO != -1)
        glDeleteFramebuffers(1, &m_glowFBO);
    if (m_stepFBO != -1)
        glDeleteFramebuffers(1, &m_stepFBO);

    if (m_depth != -1)
        glDeleteRenderbuffers(1, &m_depth);

    qDeleteAll(m_programs);
    qDeleteAll(m_shaders);

    FileAssociatedTexture::clean(*this);
}

void Painter::reset_5()
{
    // Choose appropriate width and height for the current number of particles
    int size = 16;

    m_width = static_cast<int>(sqrt(static_cast<float>(size)));
    m_height = m_width;
    const int remain = size - (m_height * m_width);
    m_height += remain / m_width + (remain % m_width == 0 ? 0 : 1);

    float * rawPositions  = new float[size * 4]; // position + radius
    float * rawVelocities = new float[size * 4]; // velocities + dummy (e.g. active)
    float * rawMaterials0 = new float[size * 4]; 
    float * rawMaterials1 = new float[size * 4]; 

    for (int i = 0; i < size; ++i)
    {
        rawPositions[i * 4 + 0] = randf(-2.f, 2.f);
        rawPositions[i * 4 + 1] = randf(-2.f, 2.f);
        rawPositions[i * 4 + 2] = randf(-2.f, 2.f);
        rawPositions[i * 4 + 3] = randf(0.4f, 0.8f);

        rawVelocities[i * 4 + 0] = randf(-1.f, 1.f);
        rawVelocities[i * 4 + 1] = randf(-1.f, 1.f);
        rawVelocities[i * 4 + 2] = randf(-1.f, 1.f);
        rawVelocities[i * 4 + 3] = 0.f;

        float S = randf(0.f, 1.f);

        rawMaterials0[i * 4 + 0] = S;  // specular R
        rawMaterials0[i * 4 + 1] = S;  // specular G
        rawMaterials0[i * 4 + 2] = S;  // specular B
        rawMaterials0[i * 4 + 3] = randf(0.f, 0.66f);  // refrectence

        rawMaterials1[i * 4 + 0] = randf(0.f, 1.f);  // diffuse R
        rawMaterials1[i * 4 + 1] = randf(0.f, 1.f);  // diffuse G
        rawMaterials1[i * 4 + 2] = randf(0.f, 1.f);  // diffuse B
        rawMaterials1[i * 4 + 3] = randf(0.f, 0.33f);  // roughness
    }

    // Read positions and velocities into textures

    assert(m_positions  != -1);
    assert(m_velocities != - 1);

    glBindTexture(GL_TEXTURE_2D, m_positions);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, rawPositions);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, m_velocities);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, rawVelocities);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, m_materials[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, rawMaterials0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, m_materials[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, rawMaterials1);
    glBindTexture(GL_TEXTURE_2D, 0);

    delete rawPositions;
    delete rawVelocities;
    delete rawMaterials0;
    delete rawMaterials1;

    // random fill forces

    static const int fdimx(5);
    static const int fdimy(5);
    static const int fdimz(5);

    float * rawForces = new float[fdimx * fdimy * fdimz * 3]; // this has center axises and allows for random rings etc..

    for (int i = 0; i < fdimx * fdimy * fdimz; ++i)
    {
    QVector3D f(randf(-1.f, 1.f), randf(-1.f, 1.f), randf(-1.f, 1.f));
    f.normalize();

    rawForces[i * 3 + 0] = f.x();
    rawForces[i * 3 + 1] = f.y();
    rawForces[i * 3 + 2] = f.z();
    }


    glBindTexture(GL_TEXTURE_3D, m_forces);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, fdimx, fdimy, fdimz, 0, GL_RGB, GL_FLOAT, rawForces);
    glBindTexture(GL_TEXTURE_3D, 0);

    delete rawForces;
}

bool Painter::initialize()
{
    initializeOpenGLFunctions();

    srand(time(NULL));

    glGenTextures(1, &m_positions);
    glBindTexture(GL_TEXTURE_2D, m_positions);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &m_velocities);
    glBindTexture(GL_TEXTURE_2D, m_velocities);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(2, m_materials);
    for (int i = 0; i < 2; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, m_materials[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &m_forces);
    glBindTexture(GL_TEXTURE_3D, m_forces);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, 0);

    // fill buffers with data
    reset_5();

    glGenTextures(1, &m_color);
    glBindTexture(GL_TEXTURE_2D, m_color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(2, m_glows);
    for (int i = 0; i < 2; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, m_glows[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // create write access to buffers 

    glGenRenderbuffers(1, &m_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_width, m_height);



    glGenFramebuffers(1, &m_colorFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_colorFBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth);

    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "color buffer is invalid." << fboStatus;

    glGenFramebuffers(1, &m_glowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_glowFBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_glows[0], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_glows[1], 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth);

    fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "glow buffer is invalid." << fboStatus;


    glGenFramebuffers(1, &m_stepFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_stepFBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_positions, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_velocities, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth);

    fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        qDebug() << "no write access to positions and velocities." << fboStatus;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // setup execution

    m_quad = new ScreenAlignedQuad(*this);

    QOpenGLShaderProgram * program;

    program = createBasicShaderProgram("data/particle_step.vert", "data/particle_step.frag");
    m_programs[ParticleStep] = program;
    program->bindAttributeLocation("a_vertex", 0);
    program->link();


    program = createBasicShaderProgramVF2("data/particle_draw_5.vert", "data/particle_draw_5_1.frag", "data/metaballs.frag");
    m_programs[PaintMode1] = program;
    program->bindAttributeLocation("a_vertex", 0);
    program->link();

    program = createBasicShaderProgramVF3("data/particle_draw_5.vert", "data/particle_draw_5_2.frag", "data/metaballs.frag", "data/cooktorrance.frag");
    m_programs[PaintMode2] = program;
    program->bindAttributeLocation("a_vertex", 0);
    program->link();

    program = createBasicShaderProgramVF3("data/particle_draw_5.vert", "data/particle_draw_5_3.frag", "data/metaballs.frag", "data/cooktorrance.frag");
    m_programs[PaintMode3] = program;
    program->bindAttributeLocation("a_vertex", 0);
    program->link();

    program = createBasicShaderProgram("data/particle_glow_5_4.vert", "data/gaussh.frag");
    m_programs[ParticleBlurH] = program;
    program->bindAttributeLocation("a_vertex", 0);
    program->link();

    program = createBasicShaderProgram("data/particle_glow_5_4.vert", "data/gaussv.frag");
    m_programs[ParticleBlurV] = program;
    program->bindAttributeLocation("a_vertex", 0);
    program->link();

    program = createBasicShaderProgram("data/particle_glow_5_4.vert", "data/particle_glow_5_4.frag");
    m_programs[PaintMode4] = program;
    program->bindAttributeLocation("a_vertex", 0);
    program->link();

    m_envmap = FileAssociatedTexture::getOrCreateCube("data/env_cube_?.png", *this);

    return true;
}


void Painter::keyPressEvent(QKeyEvent * event)
{
    switch (event->key())
    {
    case Qt::Key_Q:
        reset_5();
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

QOpenGLShaderProgram * Painter::createBasicShaderProgramVF2(
    const QString & vertexShaderFileName
    , const QString & fragmentShaderFileName0
    , const QString & fragmentShaderFileName1)
{
    QOpenGLShaderProgram * program = new QOpenGLShaderProgram();

    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Vertex, vertexShaderFileName, *program);
    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Fragment, fragmentShaderFileName0, *program);
    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Fragment, fragmentShaderFileName1, *program);
    program->bindAttributeLocation("a_vertex", 0);
    program->link();

    return program;
}

QOpenGLShaderProgram * Painter::createBasicShaderProgramVF3(
    const QString & vertexShaderFileName
,   const QString & fragmentShaderFileName0
,   const QString & fragmentShaderFileName1
,   const QString & fragmentShaderFileName2)
{
    QOpenGLShaderProgram * program = new QOpenGLShaderProgram();

    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Vertex, vertexShaderFileName, *program);
    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Fragment, fragmentShaderFileName0, *program);
    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Fragment, fragmentShaderFileName1, *program);
    m_shaders << FileAssociatedShader::getOrCreate(
        QOpenGLShader::Fragment, fragmentShaderFileName2, *program);
    program->bindAttributeLocation("a_vertex", 0);
    program->link();

    return program;
}


void Painter::resize(
    int width
    , int height)
{
    glViewport(0, 0, width, height);

    if (m_color != -1)
    {
        glBindTexture(GL_TEXTURE_2D, m_color);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }
    if (m_glows[0] != -1)
    {
        glBindTexture(GL_TEXTURE_2D, m_glows[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width / 4, height / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }
    if (m_glows[1] != -1)
    {
        glBindTexture(GL_TEXTURE_2D, m_glows[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width / 4, height / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
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
                program->setUniformValue("color", 0);
                program->setUniformValue("glow", 1);
                break;
            case PaintMode3:
            case PaintMode2:
                program->setUniformValue("materials0", 1);
                program->setUniformValue("materials1", 2);
            case PaintMode1:
                program->setUniformValue("positions", 0);
                program->setUniformValue("envmap", 3);
                program->setUniformValue("eye", camera()->eye());
                program->setUniformValue("view", camera()->view());
                program->setUniformValue("viewProjection", camera()->viewProjection());
                program->setUniformValue("projectionInverted", camera()->projectionInverted());
                program->setUniformValue("viewProjectionInverted", camera()->viewProjectionInverted());
                break;
            case ParticleStep:
                program->setUniformValue("positions",  0);
                program->setUniformValue("velocities", 1);
                program->setUniformValue("forces",     2);
                break;
            case ParticleBlurH:
            case ParticleBlurV:
                program->setUniformValue("source", 0);
                program->setUniformValue("size", camera()->viewport().width(), camera()->viewport().height());
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
        paint_5_1(timef); break;
    case PaintMode2:
        paint_5_2(timef); break;
    case PaintMode3:
        paint_5_3(timef); break;
    case PaintMode4:
        paint_5_4(timef); break;
        //paint_1_4(timef); break;
        //case PaintMode5:
        //    paint_1_5(timef); break;
        // ...
    default:
        break;
    }
}

void Painter::step(float deltaf)
{
    QOpenGLShaderProgram * program = m_programs[ParticleStep];
    if (!program->isLinked())
        return;

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_positions);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_velocities);

    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, m_forces);

    glBindFramebuffer(GL_FRAMEBUFFER, m_stepFBO);

    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    program->bind();
    program->setUniformValue("elapsed", deltaf);

    glViewport(0, 0, m_width, m_height);
    m_quad->draw(*this);
    glViewport(0, 0, camera()->viewport().width(), camera()->viewport().height());

    glDrawBuffer(GL_BACK);

    program->release();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindTexture(GL_TEXTURE_3D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, 0);

}

void Painter::draw(float timef, const int mode)
{
    QOpenGLShaderProgram * program = m_programs[mode];
    if (!program->isLinked())
        return;

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_positions);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_materials[0]);

    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_materials[1]);

    glActiveTexture(GL_TEXTURE3);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_envmap);

    program->bind();
    program->setUniformValue("timef", timef);

    m_quad->draw(*this);

    program->release();

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Painter::glow(float timef)
{
    QOpenGLShaderProgram * program;

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_color);

    glBindFramebuffer(GL_FRAMEBUFFER, m_glowFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    program = m_programs[ParticleBlurH];
    if (!program->isLinked())
        return;

    glViewport(0, 0, camera()->viewport().width() / 2, camera()->viewport().height() / 2);

    program->bind();
    m_quad->draw(*this);
    program->release();


    glBindTexture(GL_TEXTURE_2D, m_glows[0]);

    glDrawBuffer(GL_COLOR_ATTACHMENT1);

    program = m_programs[ParticleBlurV];
    if (!program->isLinked())
        return;

    program->bind();
    m_quad->draw(*this);
    program->release();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, m_color);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_glows[1]);

    program = m_programs[PaintMode4];
    if (!program->isLinked())
        return;

    glViewport(0, 0, camera()->viewport().width(), camera()->viewport().height());

    program->bind();
    m_quad->draw(*this);
    program->release();

    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Painter::paint_5_1(float timef)
{
    glDisable(GL_DEPTH_TEST);

    const float delta = timef - m_elapsed;
    m_elapsed = timef;

    step(delta * 10.f);
    draw(timef, PaintMode1);

    glEnable(GL_DEPTH_TEST);
}

void Painter::paint_5_2(float timef)
{
    glDisable(GL_DEPTH_TEST);

    const float delta = timef - m_elapsed;
    m_elapsed = timef;

    step(delta * 10.f);
    draw(timef, PaintMode2);

    glEnable(GL_DEPTH_TEST);
}

void Painter::paint_5_3(float timef)
{
    glDisable(GL_DEPTH_TEST);

    const float delta = timef - m_elapsed;
    m_elapsed = timef;

    step(delta * 10.f);
    draw(timef, PaintMode3);

    glEnable(GL_DEPTH_TEST);
}

void Painter::paint_5_4(float timef)
{
    glDisable(GL_DEPTH_TEST);

    const float delta = timef - m_elapsed;
    m_elapsed = timef;

    step(delta * 10.f);

    glBindFramebuffer(GL_FRAMEBUFFER, m_colorFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    draw(timef, PaintMode3);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glow(timef);

    glEnable(GL_DEPTH_TEST);
}