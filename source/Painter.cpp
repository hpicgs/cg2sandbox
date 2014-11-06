
#include <cassert>

#include "UnitCube.h"
#include "FileAssociatedShader.h"
#include "Camera.h"
#include "Canvas.h"

#include "Painter.h"

namespace
{
    const int TestProgram = AbstractPainter::PaintMode0;
}


Painter::Painter()
: m_camera(nullptr)
, m_cube(nullptr)
{
}

Painter::~Painter()
{
    qDeleteAll(m_programs);
    qDeleteAll(m_shaders);

    delete m_cube;
}

bool Painter::initialize()
{
    initializeOpenGLFunctions();

    m_programs[TestProgram] = createBasicShaderProgram("data/default.vert", "data/default.frag");

    m_cube = new UnitCube(*this);

    // Note: this is on way to use attribute location (the badd way...)
    m_cube = new UnitCube(*this
        , m_programs[TestProgram]->attributeLocation("a_vertex")
        , m_programs[TestProgram]->attributeLocation("a_normal"));



    // Note: As always, feel free to change/modify parameters .. as long as its possible to 
    // see the terrain and navigation basically works, everything is ok. 

    glClearColor(1.f, 1.f, 1.f, 0.f);

    m_transforms << QMatrix4x4();

    return true;
}


// ToDo: this should be an extra asset class (similar to file associated shaders..)

GLuint Painter::getOrCreateTexture(const QString & fileName)
{
    QImage image(fileName);
    if (image.isNull())
    {
        qDebug() << "Loading image from" << fileName << "failed.";
        return -1;
    }

    image = image.convertToFormat(QImage::Format_ARGB32);

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // glGenerateMipmap(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height()
        , 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());

    return texture;
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

void Painter::resize(
    int width
,   int height)
{
    glViewport(0, 0, width, height);

    foreach(const int i, m_programs.keys())
    {
        QOpenGLShaderProgram * program(m_programs[i]);
        if (program->isLinked())
        {
			program->bind();
	        program->setUniformValue("transform"
                , camera()->viewProjection() * m_transforms[i]);
	        program->release();
            
        }
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
            program->setUniformValue("transform", camera()->viewProjection() * m_transforms[i]);

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
        paint_1_1(timef); break;
    default:
        break;
    }
}

void Painter::paint_1_1(float /*timef*/)
{
    // ToDo: if you habe seen the cube, remove this!
    if (m_programs[TestProgram]->isLinked())
    {
        m_programs[TestProgram]->bind();
        m_cube->draw(*this);
        m_programs[TestProgram]->release();
    }
}
