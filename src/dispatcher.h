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

public:
	enum class Action
	{
		None,
		Registration,
		Auth,
		Message
	};

	enum class ErrorCode
	{
		Ok,
		Error,
		LoginExists,
		NoLogin,
		Password
	};

private:
	std::atomic_bool active_;
	QObject *rootItem_;
	Socket socket_;

private:
	explicit Dispatcher(QObject *parent = nullptr);

public:
	virtual ~Dispatcher();

signals:
	void registration(int code);
	void auth(int code);

private slots:
	void messageReceived(const QString &message);

public:
	bool start(QObject *rootItem);
	void stop();
	Q_INVOKABLE void regContact(const QVariantMap &data);
	Q_INVOKABLE void authContact(const QVariantMap &data);
	Q_INVOKABLE void sendMessage(const QVariantMap &data);

private:
	QString convertImage(const QString &fileName) const;
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
