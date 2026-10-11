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
	bool secure_ = false;

public:
	Socket(bool secure = false);
	~Socket();

signals:
	void messageReceived(const QString &message);
	void opened();

private slots:
	void connected();
	void closed();
	void textMessageReceived(const QString &message);
	void errorOccurred(QAbstractSocket::SocketError error);
	void sslErrors(const QList<QSslError> &errors);

private:
	bool setSslConfiguration();

public:
	bool open();
	void close();
	void sendMessage(const QString &message);
	bool valid() const { return socket_.isValid(); }
};

#endif // SOCKET_H
