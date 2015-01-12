
#pragma once

#include <vector>

#include <qgl.h>
#include <QString>
#include <QVector4D>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <assimp/color4.h>

#include "OpenGLFunctions.h"
#include "AxisAlignedBoundingBox.h"

class QMatrix4x4;
class QOpenGLShaderProgram;

struct aiScene;
struct aiMaterial;

namespace Assimp
{
    class Importer;
}

// this class is based on the following tutorial 
// http://www.lighthouse3d.com/cg-topics/code-samples/importing-3d-models-with-assimp/

class AssimpScene
{
    struct AssimpMaterial
    {
        QVector4D diffuse;
        QVector4D ambient;
        QVector4D specular;
        QVector4D emissive;

        float shininess;
        int   texCount;

        GLuint texture;
    };

    struct AssimpMesh
    {
        AssimpMesh()
        : indices(nullptr)
        , vertices(nullptr)
        , normals(nullptr)
        , texcs(nullptr)
        {
        }

        ~AssimpMesh()
        {
            delete indices;
            delete vertices;
            delete normals;
            delete texcs;
        }

        unsigned int faces;

        QOpenGLVertexArrayObject vao;

        QOpenGLBuffer * indices;
        QOpenGLBuffer * vertices;
        QOpenGLBuffer * normals;
        QOpenGLBuffer * texcs;

        AssimpMaterial material;
    };

public:
    AssimpScene(
        OpenGLFunctions & gl
    ,   const QString & filePath
    ,   bool normalize);
    virtual ~AssimpScene();

    void draw(
        OpenGLFunctions & gl
    ,   QOpenGLShaderProgram & program
    ,   const GLenum mode = GL_TRIANGLES);

    const QMatrix4x4 & transform() const;
    QMatrix4x4 & transform();


    static const aiScene * import(
        Assimp::Importer & importer
    ,   const QString & filePath);
    static AxisAlignedBoundingBox retrieveAxisAlignedBoundingBox(const aiScene * scene);

protected:
    void setupVAOs(
        OpenGLFunctions & gl
    ,   const aiScene * scene);
    
    static inline void retrieveColor(
        const aiMaterial * mtl
    ,   const char * pKey
    ,   const unsigned int type
    ,   const unsigned int index
    ,   QVector4D & color
    ,   const float r
    ,   const float g
    ,   const float b
    ,   const float a);

protected:
    std::vector<AssimpMesh *> m_meshes;

    AxisAlignedBoundingBox m_aabb;

    float m_scale;
    QMatrix4x4 m_normalize;
    QMatrix4x4 m_transform;

    bool m_valid;
};

