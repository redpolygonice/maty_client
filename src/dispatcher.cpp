#include "dispatcher.h"
#include "database.h"
#include "log.h"
#include "settings.h"

#include <QDebug>
#include <QVariantList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUuid>
#include <QFile>
#include <QDir>

Dispatcher::Dispatcher(QObject *parent)
	: QObject{parent}
	, active_(false)
	, rootItem_(nullptr)
{
}

Dispatcher::~Dispatcher()
{
	stop();
}

bool Dispatcher::start(QObject *rootItem)
{
	rootItem_ = rootItem;
	socket_.open();
	connect(&socket_, &Socket::messageReceived, this, &Dispatcher::messageReceived);
	return true;
}

void Dispatcher::stop()
{
	socket_.close();
}

void Dispatcher::regContact(const QVariantMap &data)
{
	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["action"] = static_cast<int>(Action::Registration);
	if (!object["image"].toString().isEmpty())
		object["image"] = convertImage(object["image"].toString());
	socket_.sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::authContact(const QVariantMap &data)
{
	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["action"] = static_cast<int>(Action::Auth);
	object["cid"] = GetSettings()->params()["cid"].toInt();
	socket_.sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::sendMessage(const QVariantMap &data)
{
	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["action"] =static_cast<int>(Action::Message);
	object["cid"] = GetSettings()->params()["cid"].toInt();
	socket_.sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::messageReceived(const QString &message)
{
	QJsonParseError error;
	QJsonDocument document = QJsonDocument::fromJson(message.toUtf8(), &error);
	if (error.error != QJsonParseError::NoError)
	{
		LOGE(error.errorString().toStdString());
		return;
	}

	QJsonObject rootObject = document.object();
	Action action = static_cast<Action>(rootObject["action"].toInt());
	ErrorCode code = static_cast<ErrorCode>(rootObject["code"].toInt());

	if (action == Action::Registration)
	{
		GetSettings()->params()["cid"] = rootObject["cid"].toInt();
		GetSettings()->save();
		emit registration(static_cast<int>(code));
	}

	else if (action == Action::Auth)
	{
		emit auth(static_cast<int>(code));
	}

	else if (action == Action::Message)
	{
		int cid = rootObject["cid"].toInt();
		QString text = rootObject["text"].toString();
		QVariantList args{ cid, 0, text };
		GetDatabase()->appendHistory(args);
	}
}

QString Dispatcher::convertImage(const QString &fileName) const
{
	QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
	QString oldName = fileName;
#ifdef WIN32
	oldName = oldName.replace('/', '\\').removeFirst();
#endif
	QString newName = GetSettings()->imagePath() + QDir::separator() + uuid;
	QFile::copy(oldName, newName);

	GetSettings()->params()["image"] = uuid;
	GetSettings()->save();

	QFile file(newName);
	if (!file.open(QIODevice::ReadOnly))
	{
		LOGW("Can't open image file " << newName.toStdString());
		return "";
	}

	QByteArray imageData = file.readAll();
	QString encoded = QString(imageData.toBase64());
	return encoded;
}
