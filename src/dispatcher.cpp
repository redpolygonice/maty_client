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
	connect(rootItem_, SIGNAL(newText(int,QString)), this, SLOT(sendMessage(int,QString)));
}

void Dispatcher::addContact(const QVariantMap &data)
{
	if (!GetDatabase()->appendContact(data))
	{
		LOGW("Can't append contact, try again later!");
		return;
	}

	QString oldName = GetSettings()->tempPath() + QDir::separator() + data["image"].toString();
	QString newName = GetSettings()->imagePath() + QDir::separator() + data["image"].toString();
	QFile::rename(oldName, newName);

	QJsonObject object;
	object["action"] =static_cast<int>(Action::LinkContact);
	object["cid"] = GetSettings()->params()["cid"].toInt();
	object["rid"] = data["cid"].toInt();
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::removeContact(const QVariantMap &data)
{
	int cid = data["cid"].toInt();
	if (!GetDatabase()->removeContact(cid))
	{
		LOGW("Can't remove contact, try again later!");
		return;
	}

	QString imageName = GetSettings()->imagePath() + QDir::separator() + data["image"].toString();
	QFile::remove(imageName);

	GetDatabase()->clearHistory(cid);

	QJsonObject object;
	object["action"] =static_cast<int>(Action::UnlinkContact);
	object["cid"] = GetSettings()->params()["cid"].toInt();
	object["rid"] = cid;
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::clearHistory(int cid)
{
	if (!GetDatabase()->clearHistory(cid))
	{
		LOGW("Can't clear history, try again later!");
		return;
	}
}

void Dispatcher::regContact(const QVariantMap &data)
{
	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["action"] = static_cast<int>(Action::Registration);

	if (!object["image"].toString().isEmpty())
	{
		QString newName;
		object["image"] = convertImageToBase64(object["image"].toString(), newName);
		GetSettings()->params()["image"] = newName;
		GetSettings()->save();
	}

	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::authContact(const QVariantMap &data, bool autologin)
{
	auto authServer = [this, data, autologin]() {
		QJsonObject object = QJsonObject::fromVariantMap(data);
		object["action"] = static_cast<int>(Action::Auth);
		object["cid"] = GetSettings()->params()["cid"].toInt();
		object["querydata"] = GetSettings()->params()["cid"].toString().isEmpty() ||
			GetSettings()->params()["name"].toString().isEmpty();

		if (autologin)
		{
			Crypt crypt;
			QByteArray encrypted = QByteArray::fromHex(data["password"].toString().toLocal8Bit());
			object["password"] = crypt.decrypt(encrypted);
		}

		socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
	};

	emit connectStatus(static_cast<int>(ConnectState::Connecting));
	LOG("Connecting to server ..");
	QFuture<bool> future = QtConcurrent::run([this]() {
		int count = 30;
		while (!connected_ && count-- > 0)
			QThread::msleep(100);
		return connected_;
	});

	if (future.result())
	{
		emit connectStatus(static_cast<int>(ConnectState::Connected));
		LOG("Connected to server!");
		authServer();
	}
	else
	{
		emit connectStatus(static_cast<int>(ConnectState::NotConneted));
		LOGW("Not connected!");
	}
}

void Dispatcher::sendMessage(int rid, const QString &message)
{
	QJsonObject object;
	object["action"] = static_cast<int>(Action::Message);
	object["cid"] = GetSettings()->params()["cid"].toInt();
	object["rid"] = rid;
	object["message"] = message;
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::searchContact(const QString &text)
{
	LOG("Search contact: " << text.toStdString());
	QJsonObject object;
	object["action"] = static_cast<int>(Action::Search);
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

	if (action == Action::Registration)
	{
		actionRegistration(rootObject);
	}

	else if (action == Action::Auth)
	{
		actionAuth(rootObject);
	}

	else if (action == Action::Search)
	{
		actionSearch(rootObject);
	}

	else if (action == Action::Message)
	{
		actionMessage(rootObject);
	}
}

void Dispatcher::actionRegistration(QJsonObject &root)
{
	ErrorCode code = static_cast<ErrorCode>(root["code"].toInt());
	GetSettings()->params()["cid"] = root["cid"].toInt();
	GetSettings()->save();
	emit registration(static_cast<int>(code));
}

void Dispatcher::actionAuth(const QJsonObject &root)
{
	ErrorCode code = static_cast<ErrorCode>(root["code"].toInt());
	if (code == ErrorCode::Ok)
	{
		if (root["update"].toBool())
		{
			GetSettings()->params()["cid"] = root["cid"].toInt();
			GetSettings()->params()["name"] = root["name"].toString();
			GetSettings()->params()["login"] = root["login"].toString();
			GetSettings()->params()["image"] = convertImageFromBase84(root["image"].toString(), GetSettings()->imagePath());
			GetSettings()->params()["phone"] = root["phone"].toString();
			GetSettings()->save();

			QJsonArray links = root["links"].toArray();
			for (const QJsonValue &link : links)
			{
				QJsonObject object = link.toObject();
				object["image"] = convertImageFromBase84(object["image"].toString(), GetSettings()->imagePath());
				if (!GetDatabase()->appendContact(object.toVariantMap()))
				{
					LOGW("Can't link contact!");
					continue;
				}
			}
		}
	}

	emit auth(static_cast<int>(code));
}

void Dispatcher::actionSearch(QJsonObject &root)
{
	SearchResult result = static_cast<SearchResult>(root["searchResult"].toInt());
	if (result == SearchResult::Found)
	{
		QJsonArray contactsArray = root["contacts"].toArray();
		QJsonArray contacts;
		for (const QJsonValue &contact : contactsArray)
		{
			QJsonObject object = contact.toObject();
			object["image"] = convertImageFromBase84(object["image"].toString(), GetSettings()->tempPath());
			contacts.push_back(object);
		}

		root["contacts"] = contacts;
		searchModel_.update(root);
	}

	emit searchResult(static_cast<int>(result));
}

void Dispatcher::actionMessage(const QJsonObject &root)
{
	int cid = root["cid"].toInt();
	QString text = root["text"].toString();
	QVariantList args{ cid, 0, text };
	GetDatabase()->appendHistory(args);
}

QString Dispatcher::convertImageToBase64(const QString &fileName, QString &newName) const
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

QString Dispatcher::convertImageFromBase84(const QString &base64, const QString &dir) const
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

QVariantMap Dispatcher::selfContactInfo() const
{
	QVariantMap contact{
		{"cid", 0},
		{"name", ""},
		{"login", ""},
		{"image", ""},
		{"phone", ""}
	};

	contact["cid"] = GetSettings()->params()["cid"].toInt();
	contact["name"] = GetSettings()->params()["name"].toString();
	contact["login"] = GetSettings()->params()["login"].toString();
	contact["image"] = GetSettings()->params()["image"].toString();
	contact["phone"] = GetSettings()->params()["phone"].toString();

	return contact;
}
