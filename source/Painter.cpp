
#include <cassert>

#include "Terrain.h"
#include "UnitCube.h"
#include "FileAssociatedShader.h"
#include "Camera.h"
#include "Canvas.h"

#include "Painter.h"

namespace
{
    const int TestProgram = AbstractPainter::PaintMode9 + 1;
    // const int OtherProgram = AbstractPainter::PaintMode9 + 2;
    // ...
}


Painter::Painter()
: m_camera(nullptr)
, m_cube(nullptr)
{
}

Painter::~Painter()
{
    qDeleteAll(m_terrains);
    qDeleteAll(m_programs);
    qDeleteAll(m_shaders);

    delete m_cube;
}

bool Painter::initialize()
{
    initializeOpenGLFunctions();


    // Initialize Test draw - ToDo: remove this!

    m_programs[TestProgram] = createBasicShaderProgram("data/default.vert", "data/default.frag");

    // Note: this is on way to use attribute location (the badd way...)
    m_cube = new UnitCube(*this
        , m_programs[TestProgram]->attributeLocation("a_vertex")
        , m_programs[TestProgram]->attributeLocation("a_normal"));



    // Note: As always, feel free to change/modify parameters .. as long as its possible to 
    // see the terrain and navigation basically works, everything is ok. 

    glClearColor(1.f, 1.f, 1.f, 0.f);

    m_transforms << QMatrix4x4();
    m_transforms[0].scale(2.f, 0.16f, 2.f);
    m_transforms[0].translate(-.5f, 0.f, -.5f);

    m_terrains << new Terrain(8, *this);

    m_transforms << QMatrix4x4();
    m_transforms[1].scale(64.f, 8.f, 64.f);
    m_transforms[1].translate(-.5f, 0.f, -.5f);

    m_terrains << new Terrain(256, *this);
    // m_terrains << new Terrain(2, *this); // this should give you a plane that you might use as a water plane ;)


    // Note: You can absolutely modify/paint/change these textures if you like.
    m_height = getOrCreateTexture("data/height.png"); // e.g., there is a height2 or use L3DT (see moodle)
    m_ground = getOrCreateTexture("data/ground.png");


    // uebung 1_1
    m_programs[PaintMode1] = createBasicShaderProgram("data/terrain_1_1.vert", "data/terrain_1_1.frag");

    // uebung 1_2
    m_programs[PaintMode2] = createBasicShaderProgram("data/terrain_1_2.vert", "data/terrain_1_2.frag");

    // uebung 1_3
    m_programs[PaintMode3] = createBasicShaderProgram("data/terrain_1_3.vert", "data/terrain_1_3.frag");

    // uebung 1_4 +
    m_programs[PaintMode4] = createBasicShaderProgram("data/terrain_1_4.vert", "data/terrain_1_4.frag"); 
    //m_programs[PaintMode5] = createBasicShaderProgram("data/terrain_1_5.vert", "data/terrain_1_5.frag");
    //...

    //m_programs[OtherProgram] = createBasicShaderProgram("data/other_1.vert", "data/other_1.frag");

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

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
        if (program->isLinked() && i != TestProgram)
        {
            program->bind();
            program->setUniformValue("transform"
                , camera()->viewProjection() * m_transforms[i == PaintMode1 ? 0 : 1]);
            program->release();
        }
        else if (i == TestProgram)
        {
            program->bind();
            program->setUniformValue("transform", camera()->viewProjection());
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

        if (programs.contains(program) && program->isLinked() && i != TestProgram)
        {
            program->bind();
            
            // Note: transform might be the modelview projection matrix, or similar.
            // the identifier was choosen to show that all transform is done before
            // GPU, to reduce vertex shader workload. So feel free to modify...
            program->setUniformValue("transform"
                , camera()->viewProjection() * m_transforms[i == PaintMode1 ? 0 : 1]);

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
                program->setUniformValue("ground",   1);
            case PaintMode2:
                program->setUniformValue("height",   0);
            case PaintMode1:
                break;
            //case OtherProgram: // feel free to use more than one program per mode...
            //    break;
            default:
                break;
            }

            program->release();
        }
        else if (i == TestProgram)
        {
            program->bind();
            program->setUniformValue("transform", camera()->viewProjection());
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
    case PaintMode2:
        paint_1_2(timef); break;
    case PaintMode3:
        paint_1_3(timef); break;
    case PaintMode4:
        paint_1_4(timef); break;
    //case PaintMode5:
    //    paint_1_5(timef); break;
    // ...
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

    QOpenGLShaderProgram * program(m_programs[PaintMode1]);
    Terrain * terrain(m_terrains[0]);

    if (program->isLinked())
    {
        // Note: you can use glLineWidth(...) to adjust ... :D - remember to setup default when done painting.
        program->bind();
        program->setUniformValue("color", QVector3D(0.4f, 0.4f, 0.4f)); // yes, you can change that
		terrain->draw(*this, GL_LINE_STRIP);
        program->setUniformValue("color", QVector3D(0.8f, 0.8f, 0.8f)); // ... and that one too. if you like.
        terrain->draw(*this);
        program->release();
    }
}

void Painter::paint_1_2(float /*timef*/)
{
    QOpenGLShaderProgram * program(m_programs[PaintMode2]);
    Terrain * terrain(m_terrains[1]);

    if (program->isLinked())
    {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_height);

        program->bind();
        terrain->draw(*this);
        program->release();

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

void Painter::paint_1_3(float /*timef*/)
{
    QOpenGLShaderProgram * program(m_programs[PaintMode3]);
    Terrain * terrain(m_terrains[1]);

    if (program->isLinked())
    {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_height);

        glActiveTexture(GL_TEXTURE1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_ground);

        program->bind();
        terrain->draw(*this);
        program->release();

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

void Painter::paint_1_4(float /*timef*/)
{
    // ... 
}
