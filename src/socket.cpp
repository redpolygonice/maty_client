#include "socket.h"
#include "settings.h"
#include "log.h"

#include <QUrl>

Socket::Socket()
{
}

Socket::~Socket()
{
	close();
}

bool Socket::open()
{
	QUrl url = "ws://" + GetSettings()->params()["server1"].toString() + ":" +
		GetSettings()->params()["port"].toString();

	connect(&socket_, &QWebSocket::connected, this, &Socket::connected);
	connect(&socket_, &QWebSocket::disconnected, this, &Socket::closed);
	connect(&socket_, &QWebSocket::errorOccurred, this, &Socket::errorOccurred);
	connect(&socket_, &QWebSocket::sslErrors, this, &Socket::sslErrors);

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
	socket_.sendTextMessage("Hello!");
}

void Socket::closed()
{
}

void Socket::textMessageReceived(const QString &message)
{
	LOG("Message received: " << message.toStdString());
}

void Socket::errorOccurred(QAbstractSocket::SocketError error)
{
	LOGE("Error occurred: " << error);
}

void Socket::sslErrors(const QList<QSslError> &errors)
{
	LOGE("Ssl errors");
}
