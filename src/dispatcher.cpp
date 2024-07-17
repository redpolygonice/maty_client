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

void Dispatcher::addContact(QVariantMap& data)
{
	data["image"] = convertImageFromBase84(data["image"].toString(), GetSettings()->imagePath());

	if (!GetDatabase()->appendContact(data))
	{
		LOGW("Can't append contact, try again later!");
		return;
	}

	QJsonObject object;
	object["action"] =static_cast<int>(Action::LinkContact);
	object["cid"] = GetSettings()->params()["id"].toInt();
	object["rid"] = data["id"].toInt();
	object["approved"] = data["approved"].toBool();
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::addSearchContact(const QVariantMap &data)
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
	object["cid"] = GetSettings()->params()["id"].toInt();
	object["rid"] = data["id"].toInt();
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::removeContact(const QVariantMap &data)
{
	int id = data["id"].toInt();
	if (!GetDatabase()->removeContact(id))
	{
		LOGW("Can't remove contact, try again later!");
		return;
	}

	QString imageName = GetSettings()->imagePath() + QDir::separator() + data["image"].toString();
	QFile::remove(imageName);

	GetDatabase()->clearHistory(id);

	QJsonObject object;
	object["action"] =static_cast<int>(Action::UnlinkContact);
	object["cid"] = GetSettings()->params()["id"].toInt();
	object["rid"] = id;
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
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

	if (waitConnected())
		socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::authContact(const QVariantMap &data, bool autologin)
{
	auto authServer = [this, data, autologin]() {
		QJsonObject object = QJsonObject::fromVariantMap(data);
		object["action"] = static_cast<int>(Action::Auth);
		object["id"] = GetSettings()->params()["id"].toInt();
		object["querydata"] = (object["id"].toInt() == 0);

		if (autologin)
		{
			Crypt crypt;
			QByteArray encrypted = QByteArray::fromHex(data["password"].toString().toLocal8Bit());
			object["password"] = crypt.decrypt(encrypted);
		}

		socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
	};

	if (waitConnected())
		authServer();
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
	object["id"] = GetSettings()->params()["id"].toInt();
	object["text"] = text;
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::addHistory(const QVariantMap& data)
{
	int hid = GetDatabase()->appendHistory(data);
	if (hid == 0)
	{
		LOGW("Can't append history!");
		return;
	}

	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["hid"] = hid;
	object["action"] = static_cast<int>(Action::AddHistory);
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::modifyHistory(const QVariantMap& data)
{
	if (!GetDatabase()->modifyHistory(data))
	{
		LOGW("Can't modify history!");
		return;
	}

	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["action"] = static_cast<int>(Action::ModifyHistory);
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::removeHistory(int id)
{
	if (!GetDatabase()->removeHistory(id))
	{
		LOGW("Can't remove history!");
		return;
	}

	QJsonObject object;
	object["id"] = id;
	object["action"] = static_cast<int>(Action::RemoveHistory);
	socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::clearHistory(int cid)
{
	if (!GetDatabase()->clearHistory(cid))
	{
		LOGW("Can't clear history, try again later!");
		return;
	}

	QJsonObject object;
	object["cid"] = cid;
	object["action"] = static_cast<int>(Action::ClearHistory);
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

	else if (action == Action::QueryContact)
	{
		actionQueryContact(rootObject);
	}

	else if (action == Action::NewHistory)
	{
		actionNewHistory(rootObject);
	}
}

void Dispatcher::actionRegistration(const QJsonObject &root)
{
	ErrorCode code = static_cast<ErrorCode>(root["code"].toInt());
	GetSettings()->params()["id"] = root["id"].toInt();
	GetSettings()->save();
	emit setSelfId(root["id"].toInt());
	emit registration(static_cast<int>(code));
}

void Dispatcher::actionAuth(const QJsonObject &root)
{
	ErrorCode code = static_cast<ErrorCode>(root["code"].toInt());
	if (code == ErrorCode::Ok)
	{
		if (root["update"].toBool())
		{
			// Self contact
			GetSettings()->params()["id"] = root["id"].toInt();
			GetSettings()->params()["name"] = root["name"].toString();
			GetSettings()->params()["login"] = root["login"].toString();
			GetSettings()->params()["image"] = convertImageFromBase84(root["image"].toString(), GetSettings()->imagePath());
			GetSettings()->params()["phone"] = root["phone"].toString();
			GetSettings()->save();

			// Link contacts
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

			// History
			actionNewHistory(root);

			// Set self id to Qml windows
			emit setSelfId(root["id"].toInt());
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
}

void Dispatcher::actionQueryContact(const QJsonObject& root)
{
	if (GetDatabase()->contactExists(root["id"].toInt()))
		return;

	QJsonObject contactObject = root["contact"].toObject();
	QVariantMap contact = contactObject.toVariantMap();
	contact["approved"] = false;
	addContact(contact);
	GetDatabase()->contactsModel()->update();
}

void Dispatcher::actionNewHistory(const QJsonObject& root)
{
	QJsonArray historyArray = root["history"].toArray();
	for (const QJsonValue &data : historyArray)
	{
		QJsonObject object = data.toObject();
		QVariantMap histoty = object.toVariantMap();
		if (GetDatabase()->appendHistory(histoty) == 0)
		{
			LOGW("Can't append history!");
			return;
		}

		// Update history in HiwsoryView
		GetDatabase()->historyModel()->update(histoty["cid"].toInt());

		// Self contact
		if (histoty["cid"].toInt() == GetSettings()->params()["id"].toInt())
			continue;

		// Check contacts with history
		if (!GetDatabase()->contactExists(histoty["cid"].toInt()))
		{
			QJsonObject rootObject;
			rootObject["id"] = histoty["cid"].toInt();
			rootObject["action"] = static_cast<int>(Action::QueryContact);
			socket_->sendMessage(QJsonDocument(rootObject).toJson(QJsonDocument::Compact));
		}
	}
}

bool Dispatcher::waitConnected()
{
	bool result = false;
	emit connectStatus(static_cast<int>(ConnectState::Connecting));
	LOG("Connecting to server ..");
	QFuture<bool> future = QtConcurrent::run([this]() {
		int count = 30;
		while (!connected_ && count-- > 0)
			QThread::msleep(100);
		return connected_;
	});

	result = future.result();
	if (result)
	{
		emit connectStatus(static_cast<int>(ConnectState::Connected));
		LOG("Connected to server!");
	}
	else
	{
		emit connectStatus(static_cast<int>(ConnectState::NotConneted));
		LOGW("Not connected!");
	}

	return result;
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
		{"id", 0},
		{"name", ""},
		{"login", ""},
		{"image", ""},
		{"phone", ""}
	};

	contact["id"] = GetSettings()->params()["id"].toInt();
	contact["name"] = GetSettings()->params()["name"].toString();
	contact["login"] = GetSettings()->params()["login"].toString();
	contact["image"] = GetSettings()->params()["image"].toString();
	contact["phone"] = GetSettings()->params()["phone"].toString();

	return contact;
}
