#include "dispatcher.h"
#include "database.h"
#include "log.h"
#include "settings.h"
#include "crypt.h"

#include <QDebug>
#include <QVariantList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUuid>
#include <QFile>
#include <QDir>
#include <QtConcurrent>
#include <QFuture>

Dispatcher::Dispatcher(QObject *parent)
	: QObject{parent}
	, connected_(false)
	, rootItem_(nullptr)
	, socket_(QSharedPointer<Socket>::create())
{
}

Dispatcher::~Dispatcher()
{
	stop();
}

bool Dispatcher::start(QObject *rootItem)
{
	rootItem_ = rootItem;
	socket_->open();
	connect(socket_.get(), &Socket::opened, this, &Dispatcher::connected, Qt::QueuedConnection);
	return true;
}

void Dispatcher::stop()
{
	socket_->close();
}

void Dispatcher::connected()
{
	LOG("WebSocket connected!");
	connected_ = true;
	socket_->sendMessage("Hello!");
	connect(socket_.get(), &Socket::messageReceived, this, &Dispatcher::processMessage);
}

void Dispatcher::regContact(const QVariantMap &data)
{
	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["action"] = static_cast<int>(Action::Registration);
	if (!object["image"].toString().isEmpty())
		object["image"] = convertImageToBase64(object["image"].toString());
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::authContact(const QVariantMap &data, bool autologin)
{
	auto authServer = [this, data, autologin]() {
		QJsonObject object = QJsonObject::fromVariantMap(data);
		object["action"] = static_cast<int>(Action::Auth);
		object["cid"] = GetSettings()->params()["cid"].toInt();

		if (autologin)
		{
			Crypt crypt;
			QByteArray encrypted = QByteArray::fromHex(data["password"].toString().toLocal8Bit());
			object["password"] = crypt.decrypt(encrypted);
		}

		socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
	};

	emit connectState(static_cast<int>(ConnectState::Connecting));
	LOG("Connecting to server ..");
	QFuture<bool> future = QtConcurrent::run([this]() {
		int count = 30;
		while (!connected_ && count-- > 0)
			QThread::msleep(100);
		return connected_;
	});

	if (future.result())
	{
		emit connectState(static_cast<int>(ConnectState::Connected));
		LOG("Connected to server!");
		authServer();
	}
	else
	{
		emit connectState(static_cast<int>(ConnectState::NotConneted));
		LOGW("Not connected!");
	}
}

void Dispatcher::sendMessage(const QVariantMap &data)
{
	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["action"] =static_cast<int>(Action::Message);
	object["cid"] = GetSettings()->params()["cid"].toInt();
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::searchContact(const QString &text)
{
	LOG("Search contact: " << text.toStdString());
	QJsonObject object;
	object["action"] =static_cast<int>(Action::Search);
	object["cid"] = GetSettings()->params()["cid"].toInt();
	object["text"] = text;
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::processMessage(const QString &message)
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

	else if (action == Action::Search)
	{
		SearchResult result = static_cast<SearchResult>(rootObject["searchResult"].toInt());
		if (result == SearchResult::Found)
		{
			QJsonArray contactsArray = rootObject["contacts"].toArray();
			QJsonArray contacts;
			for (const QJsonValue &contact : contactsArray)
			{
				QJsonObject object = contact.toObject();
				object["image"] = convertImageFromBase84(object["image"].toString());
				contacts.push_back(object);
			}

			rootObject["contacts"] = contacts;
			searchModel_.update(rootObject);
		}

		emit searched(static_cast<int>(result));
	}

	else if (action == Action::Message)
	{
		int cid = rootObject["cid"].toInt();
		QString text = rootObject["text"].toString();
		QVariantList args{ cid, 0, text };
		GetDatabase()->appendHistory(args);
	}
}

QString Dispatcher::convertImageToBase64(const QString &fileName) const
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

QString Dispatcher::convertImageFromBase84(const QString &base64) const
{
	QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
	QByteArray imageData = QByteArray::fromBase64(base64.toLocal8Bit());
	QString fileName = GetSettings()->imagePath() + QDir::separator() + uuid;

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
