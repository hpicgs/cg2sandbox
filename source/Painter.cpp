
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

    m_cube = new UnitCube(*this);

    m_program = new QOpenGLShaderProgram();

    m_fragShader = FileAssociatedShader::getOrCreate(
        QOpenGLShader::Fragment, "data/default.frag", *m_program);
    m_vertShader = FileAssociatedShader::getOrCreate(
        QOpenGLShader::Vertex, "data/default.vert", *m_program);
    m_program->link();

    return true;
}

void Painter::resize(
    int width
,   int height)
{
    glViewport(0, 0, width, height);

    const float aspect(static_cast<float>(width) / static_cast<float>(height));

    m_program->bind();
    m_program->setUniformValue("transform", camera()->viewProjection());
    m_program->release();
}

void Painter::update()
{
    m_program->bind();
    m_program->setUniformValue("transform", camera()->viewProjection());
    m_program->release();
}

void Painter::update(const QList<QOpenGLShaderProgram *> & programs)
{
    if (programs.contains(m_program))
    {
        m_program->bind();
        m_program->setUniformValue("transform", m_transform);
        m_program->release();
    }
}

void Painter::paint()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();
    m_cube->draw(*this);
    m_program->release();
}
