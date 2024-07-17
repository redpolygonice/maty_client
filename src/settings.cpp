#include "settings.h"
#include "log.h"
#include "crypt.h"

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

QString Settings::logPath() const
{
	QDir logDir = Settings::dataPath() + QDir::separator() + "log";
	if (!logDir.exists())
		logDir.mkpath(".");

#ifdef WIN32
	QString logPath = logDir.path().replace('/', '\\');
#else
	QString logPath = logDir.path();
#endif

	return logPath;
}

QString Settings::tempPath() const
{
	QDir tempDir = Settings::dataPath() + QDir::separator() + "temp";
	if (!tempDir.exists())
		tempDir.mkpath(".");

#ifdef WIN32
	QString tempPath = tempDir.path().replace('/', '\\');
#else
	QString tempPath = tempDir.path();
#endif

	return tempPath;
}

QString Settings::imagePath() const
{
	QDir imgDir = Settings::dataPath() + QDir::separator() + "img";
	if (!imgDir.exists())
		imgDir.mkpath(".");

#ifdef WIN32
	QString imgPath = imgDir.path().replace('/', '\\');
#else
	QString imgPath = imgDir.path();
#endif

	return imgPath;
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

#ifdef WIN32
	QString dataPath = dataDir.path().replace('/', '\\');
#else
	QString dataPath = dataDir.path();
#endif

	return dataPath;
}

QString Settings::settingsPath() const
{
	QDir settingsDir = Settings::dataPath() + QDir::separator() + "settings";
	if (!settingsDir.exists())
		settingsDir.mkpath(".");
	return settingsDir.path();
}

bool Settings::load()
{
	QString fileName = settingsPath() + QDir::separator() + settingsFile_;
	if (!QFile::exists(fileName))
	{
		LOGW("Settings file does not exist!");
		return false;
	}

	QFile file(fileName);
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
		params_["id"] = authObject["id"].toInt();
		params_["autologin"] = authObject["autologin"].toBool();
		params_["login"] = authObject["login"].toString();
		params_["password"] = authObject["password"].toString();
		params_["name"] = authObject["name"].toString();
		params_["image"] = authObject["image"].toString();
		params_["phone"] = authObject["phone"].toString();
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
	QString fileName = settingsPath() + QDir::separator() + settingsFile_;
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODeviceBase::Truncate))
	{
		LOG("Can't open settings file for writing!");
		return false;
	}

	QJsonObject rootObject;
	QJsonObject authObject;
	authObject["id"] = params_["id"].toInt();
	authObject["autologin"] = params_["autologin"].toBool();
	authObject["login"] = params_["login"].toString();
	authObject["password"] = params_["password"].toString();
	authObject["name"] = params_["name"].toString();
	authObject["image"] = params_["image"].toString();
	authObject["phone"] = params_["phone"].toString();
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
	params_["login"] = map["login"].toString();

	if ( map.contains("name"))
		params_["name"] = map["name"].toString();

	if (map.contains("password"))
	{
		Crypt crypt;
		QByteArray encrypted = crypt.encrypt(map["password"].toString());
		params_["password"] = encrypted.toHex();
	}
}
