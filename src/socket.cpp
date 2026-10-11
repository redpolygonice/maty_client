#include "socket.h"
#include "settings.h"
#include "log.h"

#include <QUrl>
#include <QFile>
#include <QDir>
#include <QSslKey>

Socket::Socket(bool secure)
	: secure_(secure)
{
	if (secure_)
		setSslConfiguration();
}

Socket::~Socket()
{
	close();
}

bool Socket::open()
{
	QString protocol = "ws://";
	if (secure_)
		protocol = "wss://";

	QUrl url = protocol + GetSettings()->params()["server1"].toString() + ":" +
		GetSettings()->params()["port"].toString();

	connect(&socket_, &QWebSocket::connected, this, &Socket::connected);
	connect(&socket_, &QWebSocket::disconnected, this, &Socket::closed);
	connect(&socket_, &QWebSocket::errorOccurred, this, &Socket::errorOccurred);
	connect(&socket_, &QWebSocket::sslErrors, this, &Socket::sslErrors);

	socket_.ignoreSslErrors();
	socket_.open(url);
	return true;
}

void Socket::close()
{
	socket_.close();
}

void Socket::sendMessage(const QString &message)
{
	socket_.sendTextMessage(message);
}

void Socket::connected()
{
	connect(&socket_, &QWebSocket::textMessageReceived, this, &Socket::textMessageReceived);
	emit opened();
}

void Socket::closed()
{
}

void Socket::textMessageReceived(const QString &message)
{
	emit messageReceived(message);
}

void Socket::errorOccurred(QAbstractSocket::SocketError error)
{
	LOGE("WebSocket error " << error << ", " << socket_.errorString().toStdString());
}

void Socket::sslErrors(const QList<QSslError> &errors)
{
	LOGE("WebSocket Ssl errors:");
	for (const QSslError& error : errors)
		LOGE(error.errorString().toStdString());
}

bool Socket::setSslConfiguration()
{
	QSslConfiguration sslConfiguration;

	QFile certFile(GetSettings()->certPath() + QDir::separator() + kCertName);
	if (!certFile.open(QIODevice::ReadOnly))
	{
		LOGE("Can't open cert file!");
		return false;
	}

	QFile keyFile(GetSettings()->certPath() + QDir::separator() + kKeyName);
	if (!keyFile.open(QIODevice::ReadOnly))
	{
		LOGE("Can't open key file!");
		return false;
	}

	QSslCertificate certificate(&certFile, QSsl::Pem);
	sslConfiguration.setLocalCertificate(certificate);

	QSslKey privateKey(&keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
	sslConfiguration.setPrivateKey(privateKey);

	sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
	socket_.setSslConfiguration(sslConfiguration);
	return true;
}
