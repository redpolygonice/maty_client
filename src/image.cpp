#include "image.h"
#include "log.h"
#include "settings.h"

#include <QUuid>
#include <QFile>
#include <QDir>

QString Image::convertToBase64(const QString& fileName, QString& newName)
{
	QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
	QString oldName = fileName;
#ifdef WIN32
	oldName = oldName.replace('/', '\\').removeFirst();
#endif
	QString copyName = GetSettings()->imagePath() + QDir::separator() + uuid;
	QFile::copy(oldName, copyName);

	QFile file(copyName);
	if (!file.open(QIODevice::ReadOnly))
	{
		LOGW("Can't open image file " << newName.toStdString());
		return "";
	}

	QByteArray imageData = file.readAll();
	QString encoded = QString(imageData.toBase64());
	newName = uuid;
	return encoded;
}

QString Image::convertFromBase84(const QString& base64, const QString& dir)
{
	QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
	QByteArray imageData = QByteArray::fromBase64(base64.toLocal8Bit());
	QString fileName = dir + QDir::separator() + uuid;

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		LOGW("Can't create image file " << fileName.toStdString());
		return "";
	}

	file.write(imageData);
	file.flush();
	return uuid;
}
