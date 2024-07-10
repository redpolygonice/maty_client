#include "settings.h"
#include "log.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

Settings::Settings(QObject *parent)
	: QObject{parent}
{
	params_["server1"] = "127.0.0.1";
	params_["server2"] = "127.0.0.1";
	params_["port"] = 1978;
}

QString Settings::imagePath() const
{
	return Settings::dataPath() + QDir::separator() + "img";
}

QString Settings::dataPath()
{
	QDir appDir = QCoreApplication::applicationDirPath();

#ifdef QT_DEBUG
	appDir.cdUp();
#endif

	QDir dataDir = appDir.path() + QDir::separator() + "data";
	if (!dataDir.exists())
		dataDir.mkpath(".");

	return dataDir.path();
}

bool Settings::load()
{
	if (!QFile::exists(settingsFile_))
	{
		LOGW("Settings file does not exist!");
		return false;
	}

	QFile file(settingsFile_);
	if (!file.open(QIODevice::ReadOnly))
	{
		LOGE("Can't open settings file!");
		return false;
	}

	QByteArray data = file.readAll();

	QJsonParseError error;
	QJsonDocument document = QJsonDocument::fromJson(data, &error);
	if (error.error != QJsonParseError::NoError)
	{
		LOGE(error.errorString().toStdString());
		return false;
	}

	QJsonObject rootObject = document.object();
	QJsonObject authObject = rootObject["Auth"].toObject();
	if (!authObject.isEmpty())
	{
		params_["autologin"] = authObject["autologin"].toBool();
		params_["login"] = authObject["login"].toString();
		params_["password"] = authObject["password"].toString();
	}

	QJsonObject netObject = rootObject["Net"].toObject();
	if (netObject.isEmpty())
	{
		params_["server1"] = "127.0.0.1";
		params_["server2"] = "127.0.0.1";
		params_["port"] = 1978;
	}

	return true;
}

bool Settings::save()
{
	QFile file(settingsFile_);
	if (!file.open(QIODevice::WriteOnly | QIODeviceBase::Truncate))
	{
		LOG("Can't open settings file for writing!");
		return false;
	}

	QJsonObject rootObject;
	QJsonObject authObject;
	authObject["autologin"] = params_["autologin"].toBool();
	authObject["login"] = params_["login"].toString();
	authObject["password"] = params_["password"].toString();
	rootObject["Auth"] = authObject;

	QJsonObject netObject;
	netObject["server1"] = params_["server1"].toString();
	netObject["server2"] = params_["server2"].toString();
	netObject["port"] = params_["port"].toInt();
	rootObject["Net"] = netObject;

	file.write(QJsonDocument(rootObject).toJson());
	return true;
}

void Settings::saveAuthData(const QVariantMap &map)
{
	params_["autologin"] = map["autologin"].toBool();
	params_["name"] = map["name"].toString();
	params_["login"] = map["login"].toString();
	params_["password"] = map["password"].toString();
}
