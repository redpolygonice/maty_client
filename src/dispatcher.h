#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "message.h"
#include "socket.h"

#include <QObject>
#include <QThread>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QVariantMap>

class Dispatcher : public QObject
{
	friend class QSharedPointer<Dispatcher>;

	Q_OBJECT

private:
	std::atomic_bool active_;
	QObject *rootItem_;
	QScopedPointer<QThread> thread_;
	Message message_;
	Socket socket_;

private:
	explicit Dispatcher(QObject *parent = nullptr);

public:
	virtual ~Dispatcher();

signals:
	void newMessage(const QString &text);
	void registration(int code);

private slots:
	void getText(int cid, const QString &text);
	void messageReceived(const QString &message);

public:
	bool start(QObject *rootItem);
	void stop();
	Q_INVOKABLE void regContact(const QVariantMap &data);
	Q_INVOKABLE void sendMessage(const QVariantMap &data);

private:
	void run();
};

using DispatcherPtr = QSharedPointer<Dispatcher>;

inline DispatcherPtr GetDispatcher()
{
	static DispatcherPtr dispatcher = nullptr;
	if (dispatcher == nullptr)
		dispatcher = QSharedPointer<Dispatcher>::create();
	return dispatcher;
}

#endif // DISPATCHER_H
