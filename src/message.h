#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QString>
#include <QStringList>

class Message : public QObject
{
	Q_OBJECT

private:
	QStringList data_;

public:
	explicit Message(QObject *parent = nullptr);

public:
	QString generateText();

};

#endif // MESSAGE_H
