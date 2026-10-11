#include "dispatcher.h"
#include "database.h"
#include "log.h"
#include "settings.h"
#include "image.h"

#include <QDebug>
#include <QVariantList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtConcurrent>

Dispatcher::Dispatcher(QObject *parent)
	: QObject{parent}
	, connected_(false)
	, update_(true)
	, rootItem_(nullptr)
	, socket_(QSharedPointer<Socket>::create(true))
	, contacts_(ContactsService::create())
	, history_(HistoryService::create())
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
	QDir(GetSettings()->tempPath()).removeRecursively();
	return true;
}

void Dispatcher::stop()
{
	socket_->close();
}

void Dispatcher::sendMessage(const QString& message)
{
	socket_->sendMessage(message);
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
	auto regServer = [this, data]() {
		QJsonObject object = QJsonObject::fromVariantMap(data);
		object["action"] = static_cast<int>(Action::Registration);

		if (!object["image_name"].toString().isEmpty())
		{
			QString newName;
			QString imageData = std::move(Image::convertToBase64(object["image_name"].toString(), newName));
			QString imageHash = std::move(QString(QCryptographicHash::hash(imageData.toUtf8(),
				QCryptographicHash::Sha256).toHex()));
			object["image_data"] = imageData;
			object["image_hash"] = imageHash;
			GetSettings()->params()["image_name"] = newName;
			GetSettings()->params()["image_hash"] = imageHash;
			object.remove("image_name");
		}

		GetSettings()->params()["password"] = std::move(QString(crypt_.encrypt(data["password"].toString()).toHex()));
		GetSettings()->save();
		object["password"] = std::move(QString(QCryptographicHash::hash(data["password"].toString().toUtf8(),
			QCryptographicHash::Sha256).toHex()));

		LOG("Reg, login: " << object["login"].toString().toStdString());
		if (static_cast<ConnectState>(connectWatcher_.result()) == ConnectState::Connected)
			socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
	};

	waitConnected(std::move(regServer));
}

void Dispatcher::authContact(const QVariantMap &data)
{
	auto authServer = [this, data]() {
		QJsonObject object = QJsonObject::fromVariantMap(data);
		object["action"] = static_cast<int>(Action::Auth);
		object["id"] = GetSettings()->params()["id"].toInt();
		object["querydata"] = (object["id"].toInt() == 0);

		QString password;
		if (object["autologin"].toBool())
		{
			QString encrypted = GetSettings()->params()["password"].toString();
			QString decrypted = crypt_.decrypt(QByteArray::fromHex(encrypted.toUtf8()));
			password = std::move(QString(QCryptographicHash::hash(decrypted.toUtf8(),
				QCryptographicHash::Sha256).toHex()));
		}
		else
		{
			password = std::move(QString(QCryptographicHash::hash(data["password"].toString().toUtf8(),
				QCryptographicHash::Sha256).toHex()));
		}

		object["password"] = password;

		LOG("Auth, login: " << object["login"].toString().toStdString());
		if (static_cast<ConnectState>(connectWatcher_.result()) == ConnectState::Connected)
			socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
	};

	waitConnected(std::move(authServer));
}

void Dispatcher::processMessage(const QString &message)
{
	logMessage(message);

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
		actionReg(rootObject);
	else if (action == Action::Auth)
		actionAuth(rootObject);
	else if (action == Action::Search)
		contacts_->actionSearch(rootObject);
	else if (action == Action::QueryContact)
		contacts_->actionQuery(rootObject);
	else if (action == Action::ModifyContact)
		contacts_->actionQuery(rootObject);
	else if (action == Action::NewHistory)
		history_->actionNew(rootObject);
	else if (action == Action::ModifyHistory)
		history_->actionModify(rootObject);
	else if (action == Action::RemoveHistory)
		history_->actionRemove(rootObject);
	else if (action == Action::ClearHistory)
		history_->actionClear(rootObject);
}

void Dispatcher::actionReg(const QJsonObject &root)
{
	ErrorCode code = static_cast<ErrorCode>(root["code"].toInt());
	GetSettings()->params()["id"] = root["id"].toInt();
	GetSettings()->save();

	if (code == ErrorCode::Ok)
		LOG("Reg OK!");

	emit setSelfId(root["id"].toInt());
	emit reg(static_cast<int>(code));
}

void Dispatcher::actionAuth(const QJsonObject &root)
{
	ErrorCode code = static_cast<ErrorCode>(root["code"].toInt());
	if (code == ErrorCode::Ok)
	{
		LOG("Auth OK!");
		if (root["update"].toBool() || update_)
			actionUpdateAll(root);
	}

	emit auth(static_cast<int>(code));
}

void Dispatcher::actionUpdateAll(const QJsonObject& root)
{
	GetDatabase()->remove();
	GetDatabase()->open();
	QDir(GetSettings()->imagePath()).removeRecursively();

	// Self contact
	GetSettings()->params()["id"] = root["id"].toInt();
	GetSettings()->params()["name"] = root["name"].toString();
	GetSettings()->params()["login"] = root["login"].toString();
	GetSettings()->params()["image_name"] = Image::convertFromBase64(root["image_data"].toString(), GetSettings()->imagePath());
	GetSettings()->params()["image_hash"] = root["image_hash"].toString();
	GetSettings()->params()["phone"] = root["phone"].toString();
	GetSettings()->params()["about"] = root["about"].toString();
	GetSettings()->save();

	// Link contacts
	QJsonArray links = root["links"].toArray();
	for (const QJsonValue &link : links)
	{
		QJsonObject object = link.toObject();
		object["image_name"] = Image::convertFromBase64(object["image_data"].toString(), GetSettings()->imagePath());
		if (!GetDatabase()->appendContact(object.toVariantMap()))
		{
			LOGW("Can't link contact!");
			continue;
		}
	}

	// History
	history_->actionNew(root);

	// Update contacts
	GetDatabase()->contactsModel()->update();

	// Set self id to Qml windows
	emit setSelfId(root["id"].toInt());
}

void Dispatcher::logMessage(const QString& message)
{
	if (!message.contains("image_data"))
	{
		LOG("Message received: " << message.toStdString());
		return;
	}

	QJsonParseError error;
	QJsonDocument document = QJsonDocument::fromJson(message.toUtf8(), &error);
	if (error.error != QJsonParseError::NoError)
	{
		LOGE(error.errorString().toStdString());
		return;
	}

	QJsonObject rootObject = document.object();

	if (rootObject.contains("image_data"))
		rootObject["image_data"] = "base64";

	if (rootObject.contains("links"))
	{
		QJsonArray newLinks;
		QJsonArray links = rootObject["links"].toArray();
		for (const QJsonValue &link : links)
		{
			QJsonObject object = link.toObject();
			object["image_data"] = "base64";
			newLinks.push_back(object);
		}

		rootObject["links"] = newLinks;
	}

	if (rootObject.contains("contacts"))
	{
		QJsonArray newContacts;
		QJsonArray contactsArray = rootObject["contacts"].toArray();
		for (const QJsonValue &contact : contactsArray)
		{
			QJsonObject object = contact.toObject();
			object["image_data"] = "base64";
			newContacts.push_back(object);
		}

		rootObject["contacts"] = newContacts;
	}

	LOG("Message received: " <<
		QJsonDocument(rootObject).toJson(QJsonDocument::Compact).toStdString());
}

template <typename Function> void Dispatcher::waitConnected(Function &&func)
{
	connect(&connectWatcher_, &QFutureWatcher<int>::finished, this, func);

	QFuture<int> future = QtConcurrent::run([this]() {
		ConnectState result = ConnectState::Connecting;
		emit connectState(static_cast<int>(result));
		LOG("Connecting to server ..");

		int count = 30;
		while (!connected_ && count-- > 0)
			QThread::msleep(100);

		if (connected_)
		{
			result = ConnectState::Connected;
			emit connectState(static_cast<int>(result));
			LOG("Connected to server!");
		}
		else
		{
			result = ConnectState::NotConneted;
			emit connectState(static_cast<int>(result));
			LOGW("Not connected!");
		}

		return static_cast<int>(result);
	});

	connectWatcher_.setFuture(future);
}
