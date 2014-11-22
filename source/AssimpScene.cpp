

#include <cassert>

#include <QFileInfo>
#include <qDebug>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>

#include "MathMacros.h"
#include "FileAssociatedTexture.h"
#include "AssimpScene.h"


AssimpScene::AssimpScene(
    OpenGLFunctions & gl
,   const QString & filePath
,   const bool normalize)
:   m_valid(false)
{
    Assimp::Importer importer;

    const aiScene * scene = import(importer, filePath);
    if (!scene)
        return;

    m_aabb = retrieveAxisAlignedBoundingBox(scene);

    if (normalize)
    {
        QVector3D d = m_aabb.urb() - m_aabb.llf();
        m_scale = abs(1.f / ma(d.x(), ma(d.y(), d.z())));

        m_normalize.scale(m_scale);
        m_normalize.translate(-m_aabb.center());
    }
    setupVAOs(gl, scene);

    m_valid = true;
}

AssimpScene::~AssimpScene()
{
    for (unsigned int i = 0; i < m_meshes.size(); ++i)
        delete m_meshes[i];

    m_meshes.clear();
}

const QMatrix4x4 & AssimpScene::transform() const
{
    return m_transform;
}

QMatrix4x4 & AssimpScene::transform()
{
    return m_transform;
}

const aiScene * AssimpScene::import(
    Assimp::Importer & importer
,   const QString & filePath)
{
    QFileInfo fi(filePath);
    if (!fi.exists())
    {
        qWarning() << filePath << " does not exist.";
        return nullptr;
    }

    const aiScene * scene;
    scene = importer.ReadFile(filePath.toStdString(), aiProcessPreset_TargetRealtime_Quality);

    if (!scene)
    {
        qWarning() << "Reading file via assimp failed: " << importer.GetErrorString();
        return nullptr;
    }

    // Now we can access the file's contents.
    qDebug() << "Importing " << filePath << " succeeded";

    return scene;
}

AxisAlignedBoundingBox AssimpScene::retrieveAxisAlignedBoundingBox(const aiScene * scene)
{
    AxisAlignedBoundingBox aabb;

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
    {
        aiMesh * mesh = scene->mMeshes[m];

        for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
        {
            const QVector3D vec3(
                mesh->mVertices[v].x
            ,   mesh->mVertices[v].y
            ,   mesh->mVertices[v].z);

            aabb.extend(vec3);
        }
    }
    return aabb;
}

void AssimpScene::setupVAOs(
    OpenGLFunctions & gl
,   const aiScene * scene)
{
    assert(scene);

    // For each mesh
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
    {
        const aiMesh * mesh = scene->mMeshes[m];

        // create array with faces
        // have to convert from Assimp format to array
        std::vector<unsigned int> indices(mesh->mNumFaces * 3);

        for (unsigned int f = 0, i = 0; f < mesh->mNumFaces; ++f, i += 3)
        {
            const aiFace * face = &mesh->mFaces[f];
            indices[i + 0] = face->mIndices[0];
            indices[i + 1] = face->mIndices[1];
            indices[i + 2] = face->mIndices[2];
        }

        AssimpMesh * amesh = new AssimpMesh();
        amesh->faces = mesh->mNumFaces;

        amesh->vao.create();
        amesh->vao.bind();

        // create buffers

        amesh->indices = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        amesh->indices->create();
        amesh->indices->setUsagePattern(QOpenGLBuffer::StaticDraw);
        amesh->indices->bind();
        amesh->indices->allocate(indices.data(), indices.size() * sizeof(unsigned int));

        if (mesh->HasPositions())
        {
            amesh->vertices = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
            amesh->vertices->create();
            amesh->vertices->setUsagePattern(QOpenGLBuffer::StaticDraw);
            amesh->vertices->bind();
            amesh->vertices->allocate(mesh->mVertices, mesh->mNumVertices * sizeof(float) * 3);

            gl.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
            gl.glEnableVertexAttribArray(0);
        }
        if (mesh->HasNormals())
        {
            amesh->normals = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
            amesh->normals->create();
            amesh->normals->setUsagePattern(QOpenGLBuffer::StaticDraw);
            amesh->normals->bind();
            amesh->normals->allocate(mesh->mNormals, mesh->mNumVertices * sizeof(float) * 3);

            gl.glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, nullptr);
            gl.glEnableVertexAttribArray(1);
        }
        if (mesh->HasTextureCoords(0))
        {
            float * texcs = new float[2 * mesh->mNumVertices];
            for (unsigned int t = 0; t < mesh->mNumVertices; ++t)
            {
                texcs[t * 2 + 0] = mesh->mTextureCoords[0][t].x;
                texcs[t * 2 + 1] = mesh->mTextureCoords[0][t].y;
            }

            amesh->texcs= new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
            amesh->texcs->create();
            amesh->texcs->setUsagePattern(QOpenGLBuffer::StaticDraw);
            amesh->texcs->bind();
            amesh->texcs->allocate(texcs, mesh->mNumVertices * sizeof(float) * 2);

            gl.glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);
            gl.glEnableVertexAttribArray(2);
        }

        amesh->vao.release();

        AssimpMaterial & material(amesh->material);

        // create material uniform buffer
        aiMaterial * mtl = scene->mMaterials[mesh->mMaterialIndex];

        // support single texture on diffuse channel only for now... TODO

        aiString path;
        if (AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &path))
        {
            material.texture = FileAssociatedTexture::getOrCreate2D(QString(path.C_Str()), gl);
            material.texCount = 1;
        }
        else
            material.texCount = 0;

        retrieveColor(mtl, AI_MATKEY_COLOR_DIFFUSE, material.diffuse, 0.8f, 0.8f, 0.8f, 1.0f);
        retrieveColor(mtl, AI_MATKEY_COLOR_AMBIENT, material.ambient, 0.2f, 0.2f, 0.2f, 1.0f);
        retrieveColor(mtl, AI_MATKEY_COLOR_SPECULAR, material.specular, 0.0f, 0.0f, 0.0f, 1.0f);
        retrieveColor(mtl, AI_MATKEY_COLOR_EMISSIVE, material.emissive, 0.0f, 0.0f, 0.0f, 1.0f);

        material.shininess = 5.f;
        unsigned int max;
        aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &material.shininess, &max);

        m_meshes.push_back(amesh);
    }
}

inline void AssimpScene::retrieveColor(
    const aiMaterial * mtl
,   const char * pKey
,   const unsigned int type
,   const unsigned int index
,   QVector4D & color
,   const float r
,   const float g
,   const float b
,   const float a) 
{
    aiColor4D acolor;
    if (AI_SUCCESS == aiGetMaterialColor(mtl, pKey, type, index, &acolor))
    {
        color.setX(acolor.r);
        color.setY(acolor.g);
        color.setZ(acolor.b);
        color.setW(acolor.a);
    }
    else
    {
        color.setX(r);
        color.setY(g);
        color.setZ(b);
        color.setW(a);
    }
}

void AssimpScene::draw(
    OpenGLFunctions & gl
,   QOpenGLShaderProgram & program
,   const GLenum mode)
{
    if (!m_valid)
        return;

    if (!program.isLinked())
        return;

    std::vector<AssimpMesh *>::const_iterator i = m_meshes.begin();
    const std::vector<AssimpMesh *>::const_iterator iEnd = m_meshes.end();

    program.bind();
    program.setUniformValue("model", m_transform * m_normalize);

    for (; i != iEnd; ++i)
    {
        AssimpMesh * mesh(*i);

        program.setUniformValue("diffuse", mesh->material.diffuse);
        program.setUniformValue("ambient", mesh->material.ambient);
        program.setUniformValue("specular", mesh->material.specular);
        program.setUniformValue("emissive", mesh->material.emissive);
        program.setUniformValue("shininess", mesh->material.shininess);
        program.setUniformValue("texCount", mesh->material.texCount);

        if (mesh->material.texCount > 0)
        {
            program.setUniformValue("difftex", 0);
            gl.glActiveTexture(GL_TEXTURE0);            
            gl.glBindTexture(GL_TEXTURE_2D, mesh->material.texture);
        }

        mesh->vao.bind();
        gl.glDrawElements(mode, mesh->faces * 3, GL_UNSIGNED_INT, nullptr);
        mesh->vao.release();

        if (mesh->material.texCount > 0)
            gl.glBindTexture(GL_TEXTURE_2D, 0);
    }
    program.release();
}
