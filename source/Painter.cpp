
#include <cassert>

#include "UnitCube.h"
#include "FileAssociatedShader.h"
#include "Camera.h"

#include "Painter.h"


Painter::Painter()
: m_camera(nullptr)
, m_cube(nullptr)
, m_program(nullptr)
, m_fragShader(nullptr)
, m_vertShader(nullptr)
{
}

Painter::~Painter()
{
    delete m_cube;
    delete m_program;

    delete m_fragShader;
    delete m_vertShader;
}

bool Painter::initialize()
{
    initializeOpenGLFunctions();

    glClearColor(1.f, 1.f, 1.f, 0.f);

    m_program = new QOpenGLShaderProgram();

    m_fragShader = FileAssociatedShader::getOrCreate(
        QOpenGLShader::Fragment, "data/default.frag", *m_program);
    m_vertShader = FileAssociatedShader::getOrCreate(
        QOpenGLShader::Vertex, "data/default.vert", *m_program);
    m_program->link();

    m_cube = new UnitCube(*this
        , m_program->attributeLocation("a_vertex")
        , m_program->attributeLocation("a_normal"));

    return true;
}

void Painter::resize(
    int width
,   int height)
{
    glViewport(0, 0, width, height);

    if (m_program->isLinked())
    {
        m_program->bind();
        m_program->setUniformValue("transform", camera()->viewProjection());
        m_program->release();
    }
}

void Painter::update()
{
    if (m_program->isLinked())
    {
        m_program->bind();
        m_program->setUniformValue("transform", camera()->viewProjection());
        m_program->release();
    }
}

void Painter::update(const QList<QOpenGLShaderProgram *> & programs)
{
    if (programs.contains(m_program) && m_program->isLinked())
    {
        m_program->bind();
        m_program->setUniformValue("transform", camera()->viewProjection());
        m_program->release();
    }
}

void Painter::paint(float timef)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (mode())
    {
    case PaintMode1:
        paint_1_1(timef);
        break;

    case PaintMode2:
        paint_1_2(timef);
        break;

    case PaintMode3:
        paint_1_3(timef);
        break;

    case PaintMode4:
        paint_1_4(timef);
        break;

    case PaintMode5:
        paint_1_5(timef);
        break;

    case PaintMode0:
        break;

    default:
        break;
    }
}

void Painter::paint_1_1(float timef)
{
    if (m_program->isLinked())
    {
        m_program->bind();
        m_cube->draw(*this);
        m_program->release();
    }
}

void Painter::paint_1_2(float timef)
{
}

void Painter::paint_1_3(float timef)
{
}

void Painter::paint_1_4(float timef)
{
}

void Painter::paint_1_5(float timef)
{
}

