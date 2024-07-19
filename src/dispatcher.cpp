#include "dispatcher.h"
#include "database.h"
#include "log.h"
#include "settings.h"
#include "crypt.h"
#include "image.h"

#include <QDebug>
#include <QVariantList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtConcurrent>

Dispatcher::Dispatcher(QObject *parent)
	: QObject{parent}
	, connected_(false)
	, rootItem_(nullptr)
	, socket_(QSharedPointer<Socket>::create())
	, contactsSvc_(ContactsService::create())
	, historySvc_(HistoryService::create())
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

		if (!object["image"].toString().isEmpty())
		{
			QString newName;
			object["image"] = Image::convertToBase64(object["image"].toString(), newName);
			GetSettings()->params()["image"] = newName;
			GetSettings()->save();
		}

		LOG("Performing reg, login: " << object["login"].toString().toStdString());
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

		if (object["autologin"].toBool())
		{
			Crypt crypt;
			QByteArray encrypted = QByteArray::fromHex(data["password"].toString().toLocal8Bit());
			object["password"] = crypt.decrypt(encrypted);
		}

		LOG("Performing auth, login: " << object["login"].toString().toStdString());
		if (static_cast<ConnectState>(connectWatcher_.result()) == ConnectState::Connected)
			socket_->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
	};

	waitConnected(std::move(authServer));
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
		actionReg(rootObject);
	}

	else if (action == Action::Auth)
	{
		actionAuth(rootObject);
	}

	else if (action == Action::Search)
	{
		contactsSvc_->actionSearch(rootObject);
	}

	else if (action == Action::QueryContact)
	{
		contactsSvc_->actionQuery(rootObject);
	}

	else if (action == Action::NewHistory)
	{
		historySvc_->actionNew(rootObject);
	}
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

		if (root["update"].toBool())
		{
			// Self contact
			GetSettings()->params()["id"] = root["id"].toInt();
			GetSettings()->params()["name"] = root["name"].toString();
			GetSettings()->params()["login"] = root["login"].toString();
			GetSettings()->params()["image"] = Image::convertFromBase84(root["image"].toString(), GetSettings()->imagePath());
			GetSettings()->params()["phone"] = root["phone"].toString();
			GetSettings()->save();

			// Link contacts
			QJsonArray links = root["links"].toArray();
			for (const QJsonValue &link : links)
			{
				QJsonObject object = link.toObject();
				object["image"] = Image::convertFromBase84(object["image"].toString(), GetSettings()->imagePath());
				if (!GetDatabase()->appendContact(object.toVariantMap()))
				{
					LOGW("Can't link contact!");
					continue;
				}
			}

			// History
			historySvc_->actionNew(root);

			// Set self id to Qml windows
			emit setSelfId(root["id"].toInt());
		}
	}

	emit auth(static_cast<int>(code));
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
