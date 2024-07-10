#include "dispatcher.h"
#include "database.h"
#include "log.h"

#include <QDebug>
#include <QVariantList>
#include <QJsonObject>
#include <QJsonDocument>

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
	active_ = true;
	thread_.reset(QThread::create([this] () { run(); }));
	thread_->start();
	connect(rootItem_, SIGNAL(newText(int,QString)), this, SLOT(getText(int,QString)));
	return true;
}

void Dispatcher::stop()
{
	active_ = false;
	thread_->wait();
}

void Dispatcher::regContact(const QVariantMap &data)
{
	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["action"] = 1;
	socket_.sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Dispatcher::run()
{
	while (active_)
	{
		QThread::msleep(50);
	}
}

void Dispatcher::getText(int cid, const QString &text)
{
	QString answer = message_.generateText();
	QVariantList args{ cid, 1, answer };
	GetDatabase()->appendHistory(args);
}

void Dispatcher::sendMessage(const QVariantMap &data)
{
	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["action"] = 2;
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
	int action = rootObject["action"].toInt();
	int code = rootObject["code"].toInt();

	if (action == 1 && code == 0)
	{
		emit registration(code);
		return;
	}

	if (action == 2)
	{
		int cid = rootObject["cid"].toInt();
		QString text = rootObject["text"].toString();
		QVariantList args{ cid, 0, text };
		GetDatabase()->appendHistory(args);
	}
}
