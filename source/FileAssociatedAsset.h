
#pragma once

#include <QObject>

class QFileSystemWatcher;

class FileAssociatedAsset : public QObject
{
public:
    static FileAssociatedAsset * instance();
	static QFileSystemWatcher * const fileSystemWatcher();

public slots:
    void fileChanged(const QString & path);

protected:
	FileAssociatedAsset();
	virtual ~FileAssociatedAsset();

	static QQueue<QString> s_queue;

    QFileSystemWatcher * m_fileSystemWatcher;
    static FileAssociatedAsset * s_instance;
};
