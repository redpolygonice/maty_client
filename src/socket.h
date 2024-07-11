#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QString>
#include <QWebSocket>

// Communication class
class Socket : public QObject
{
	Q_OBJECT

private:
	QWebSocket socket_;

public:
	Socket();
	~Socket();

signals:
	void messageReceived(const QString &message);

private slots:
	void connected();
	void closed();
	void textMessageReceived(const QString &message);
	void errorOccurred(QAbstractSocket::SocketError error);
	void sslErrors(const QList<QSslError> &errors);

public:
	bool open();
	void close();
	void sendMessage(const QString &message);
};

#endif // SOCKET_H
