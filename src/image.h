#ifndef IMAGE_H
#define IMAGE_H

#include <QString>

class Image
{
public:
	static QString convertToBase64(const QString &fileName, QString &newName);
	static QString convertFromBase64(const QString &base64, const QString &dir);
};

#endif // IMAGE_H
