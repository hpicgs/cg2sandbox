
#include <cassert>

#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QTextStream>
#include <QImage>
#include <QStringList>

#include "Painter.h"
#include "FileAssociatedTexture.h"

QMap<QString, QImage> FileAssociatedTexture::s_imagesByFilePath;
QMap<QString, GLuint> FileAssociatedTexture::s_texturesByFilePath;
QMultiMap<QString, FileAssociatedTexture::CubeFaceOfTexture> 
    FileAssociatedTexture::s_cubefacesByFilePath;

QQueue<QString> FileAssociatedTexture::s_queue;

FileAssociatedTexture * FileAssociatedTexture::s_instance = nullptr;

FileAssociatedTexture::FileAssociatedTexture()
: m_fileSystemWatcher(new QFileSystemWatcher())
{
    connect(m_fileSystemWatcher, &QFileSystemWatcher::fileChanged
		, this, &FileAssociatedTexture::fileChanged);
}

FileAssociatedTexture::~FileAssociatedTexture()
{
    delete m_fileSystemWatcher;
}

void FileAssociatedTexture::clean(OpenGLFunctions & gl)
{
    instance()->m_fileSystemWatcher->removePaths(s_texturesByFilePath.keys());

    foreach(const GLuint & texture, s_texturesByFilePath.values())
        gl.glDeleteTextures(1, &texture);

    s_queue.clear();
    s_texturesByFilePath.clear();
    s_imagesByFilePath.clear();
}

FileAssociatedTexture * FileAssociatedTexture::instance()
{
	if (!s_instance)
		s_instance = new FileAssociatedTexture();

	return s_instance;
}

GLuint FileAssociatedTexture::getOrCreate2D(
	const QString & fileName
,   OpenGLFunctions & gl
,	const GLenum wrap_s
,	const GLenum wrap_t
,	const GLenum mag_filter
,	const GLenum min_filter)
{
	QFileInfo fi(fileName);
	if (!fi.exists())
	{
		qWarning() << fileName << " does not exist: texture has no associated file.";
		return -1;
	}
	QString filePath(fi.absoluteFilePath());

	if (s_texturesByFilePath.contains(filePath))
		return s_texturesByFilePath[filePath];

    QImage image = getOrCreateImage(filePath);
	if (image.isNull())
		return -1;

	instance()->m_fileSystemWatcher->addPath(filePath);

	GLuint texture = loadTexture2D(-1, wrap_s, wrap_t, mag_filter, min_filter, image, gl);
	s_texturesByFilePath[filePath] = texture;

	return texture;
}

GLuint FileAssociatedTexture::getOrCreateCube(
	const QString & fileNames
,   OpenGLFunctions & gl
,	const GLenum mag_filter
,	const GLenum min_filter)
{
    if (s_texturesByFilePath.contains(fileNames))
        return s_texturesByFilePath[fileNames];

    QString px(fileNames); px.replace("?", "px");
    QString nx(fileNames); nx.replace("?", "nx");
    QString py(fileNames); py.replace("?", "py");
    QString ny(fileNames); ny.replace("?", "ny");
    QString pz(fileNames); pz.replace("?", "pz");
    QString nz(fileNames); nz.replace("?", "nz");

    QStringList files = QStringList() << px << nx << py << ny << pz << nz;
    QStringList absolutes;
    foreach(const QString & file, files)
    {
        QFileInfo fi(file);
        if (!fi.exists())
        {
            qWarning() << file << " does not exist: texture has no associated file.";
            return -1;
        }
        absolutes << fi.absoluteFilePath();
    }

    QMap<GLenum, QImage> images;
    images[GL_TEXTURE_CUBE_MAP_POSITIVE_X] = getOrCreateImage(absolutes[0]);
    images[GL_TEXTURE_CUBE_MAP_NEGATIVE_X] = getOrCreateImage(absolutes[1]);
    images[GL_TEXTURE_CUBE_MAP_POSITIVE_Y] = getOrCreateImage(absolutes[2]);
    images[GL_TEXTURE_CUBE_MAP_NEGATIVE_Y] = getOrCreateImage(absolutes[3]);
    images[GL_TEXTURE_CUBE_MAP_POSITIVE_Z] = getOrCreateImage(absolutes[4]);
    images[GL_TEXTURE_CUBE_MAP_NEGATIVE_Z] = getOrCreateImage(absolutes[5]);

    foreach(const QImage & image, images.values())
    {
        if (image.isNull())
            // ToDo: remove other related images ...

            return -1;
    }

    GLuint texture = loadTextureCube(-1, mag_filter, min_filter, images, gl);

    int i = 0; // hope for consequtive definitions
    foreach(const QString & filePath, absolutes)
    {
        instance()->m_fileSystemWatcher->addPath(filePath);

        CubeFaceOfTexture cfot;
        cfot.face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        cfot.texture = texture;
        s_cubefacesByFilePath.insert(filePath, cfot);

        ++i;
    }
    s_texturesByFilePath[fileNames] = texture;
	return texture;
}

void FileAssociatedTexture::fileChanged(const QString & filePath)
{
	s_imagesByFilePath[filePath] = getOrCreateImage(filePath, true);
	s_queue.append(filePath);
}

QImage FileAssociatedTexture::getOrCreateImage(
    const QString & filePath
,   const bool forceReload)
{
    QFileInfo fi(filePath);

    if (!forceReload && s_imagesByFilePath.contains(filePath))
        return s_imagesByFilePath[filePath];

	QImage image(filePath);
	if (image.isNull())
	{
		qDebug() << "Loading image from" << filePath << "failed.";
		return QImage();
	}
	return image.convertToFormat(QImage::Format_ARGB32);
}

void FileAssociatedTexture::process(OpenGLFunctions & gl)
{
	while (!s_queue.isEmpty())
	{
		QString filePath = s_queue.first();
		s_queue.removeFirst();

		loadTexture2D(s_texturesByFilePath[filePath], s_imagesByFilePath[filePath], gl);
	}
}

GLuint FileAssociatedTexture::loadTexture2D(
	GLuint texture
	, const GLenum wrap_s
	, const GLenum wrap_t
	, const GLenum mag_filter
	, const GLenum min_filter
	, const QImage & image
	, OpenGLFunctions & gl)
{
	if (texture == -1)
	{
		gl.glGenTextures(1, &texture);
		gl.glBindTexture(GL_TEXTURE_2D, texture);

		gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
		gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

		gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
		gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	}
	
	return loadTexture2D(texture, image, gl);
}

GLuint FileAssociatedTexture::loadTexture2D(
	GLuint texture
,	const QImage & image
,	OpenGLFunctions & gl)
{
	gl.glBindTexture(GL_TEXTURE_2D, texture);
	gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height()
		, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
    gl.glGenerateMipmap(GL_TEXTURE_2D);

    return texture;
}

GLuint FileAssociatedTexture::loadTextureCube(
    GLuint texture
,   const GLenum mag_filter
,   const GLenum min_filter
,   const QMap<GLenum, QImage> & images
,   OpenGLFunctions & gl)
{
    if (texture == -1)
    {
        gl.glGenTextures(1, &texture);
        gl.glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        gl.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mag_filter);
        gl.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, min_filter);

        gl.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl.glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    return loadTextureCube(texture, images, gl);
}

GLuint FileAssociatedTexture::loadTextureCube(
    GLuint texture
,   const QMap<GLenum, QImage> & images
,   OpenGLFunctions & gl)
{
    gl.glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    QList<GLenum> faces = QList<GLenum>()
        << GL_TEXTURE_CUBE_MAP_POSITIVE_X << GL_TEXTURE_CUBE_MAP_NEGATIVE_X
        << GL_TEXTURE_CUBE_MAP_POSITIVE_Y << GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        << GL_TEXTURE_CUBE_MAP_POSITIVE_Z << GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

    foreach(GLenum face, faces)
    {
        const QImage & image(images[face]);

        gl.glTexImage2D(face, 0, GL_RGB, image.width(), image.height()
            , 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits());
    }
    gl.glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    return texture;
}

QImage FileAssociatedTexture::image(const QString & filePath)
{
	if (s_imagesByFilePath.contains(filePath))
		return s_imagesByFilePath[filePath];

	return QImage();
}
