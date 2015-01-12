
#pragma once

#include <QObject>
#include <QMap>
#include <QMultiMap>
#include <QQueue>

#include "OpenGLFunctions.h"

class QImage;
class QFileSystemWatcher;
class Painter;

/** ToDo: does not work for multiple contexts yet...
	ToDo: remove "singletoness"
	ToDo: allow removal of textures...
*/


class FileAssociatedTexture : public QObject
{
public:
    static GLuint getOrCreate2Dus16(
		const QString & fileName
    ,   const int width
    ,   const int height
    ,   OpenGLFunctions & gl
    ,   std::vector<unsigned short> * data = nullptr
	,	const GLenum wrap_s = GL_CLAMP_TO_EDGE
    ,   const GLenum wrap_t = GL_CLAMP_TO_EDGE
    ,   const GLenum mag_filter = GL_LINEAR_MIPMAP_NEAREST
    ,   const GLenum min_filter = GL_LINEAR_MIPMAP_NEAREST);

    static GLuint getOrCreate2D(
		const QString & fileName
    ,   OpenGLFunctions & gl
	,	const GLenum wrap_s = GL_CLAMP_TO_EDGE
    ,   const GLenum wrap_t = GL_CLAMP_TO_EDGE
    ,   const GLenum mag_filter = GL_LINEAR_MIPMAP_NEAREST
    ,   const GLenum min_filter = GL_LINEAR_MIPMAP_NEAREST);

    static void clean(OpenGLFunctions & gl);

	/** filePaths should contain a questionmark '?' that is replaced
		by cubemap extensions 'px', 'nx', 'py', etc. 
		e.g. "resources/starmap?.png" points to "resources/starmap_px.png" etc.
	*/
	static GLuint getOrCreateCube(
        const QString & fileNames
    ,   OpenGLFunctions & gl
    ,	const GLenum mag_filter = GL_LINEAR_MIPMAP_NEAREST
    ,   const GLenum min_filter = GL_LINEAR_MIPMAP_NEAREST
	);

	static void process(OpenGLFunctions & gl);
	static QImage image(const QString & filePath);

protected:
	static FileAssociatedTexture * instance();

protected slots:
    void fileChanged(const QString & path);

protected:
    static QImage getOrCreateImage(
        const QString & filePath
    ,   bool forceReload = false);

    static GLuint image2D(
		GLuint texture
	,	const QImage & image
	,	OpenGLFunctions & gl);

    static GLuint image2D(
        GLuint texture
    ,   const int width
    ,   const int height
    ,   const unsigned short * data
    ,   OpenGLFunctions & gl);

	static GLuint setupTexture2D(
		GLuint texture
	,	const GLenum wrap_s
	,	const GLenum wrap_t
	,	const GLenum mag_filter
	,	const GLenum min_filter
	,	OpenGLFunctions & gl);

	static GLuint loadTextureCube(
		GLuint texture
    ,   const QMap<GLenum, QImage> & images
    ,   OpenGLFunctions & gl);
    static GLuint loadTextureCube(
        GLuint texture
    ,   const GLenum mag_filter
    ,   const GLenum min_filter
    ,   const QMap<GLenum, QImage> & images
	,	OpenGLFunctions & gl);


private:
	FileAssociatedTexture();
	virtual ~FileAssociatedTexture();

protected:
    static QMap<QString, QImage> s_imagesByFilePath;
	static QMap<QString, GLuint> s_texturesByFilePath;
    
    struct CubeFaceOfTexture
    {
        GLuint texture;
        GLenum face;
    };
    static QMultiMap<QString, CubeFaceOfTexture> s_cubefacesByFilePath;

	static QQueue<QString> s_queue;

	QFileSystemWatcher * m_fileSystemWatcher;

	static FileAssociatedTexture * s_instance;
};
