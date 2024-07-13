#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "socket.h"
#include "searchmodel.h"

#include <QObject>
#include <QThread>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QVariantMap>

using SocketPtr = QSharedPointer<Socket>;

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
		Message,
		Search
	};

	enum class ErrorCode
	{
		Ok,
		Error,
		LoginExists,
		NoLogin,
		Password
	};

	enum class ConnectState
	{
		Connecting,
		Connected,
		NotConneted
	};

	enum class SearchResult
	{
		Found,
		NotFound
	};

private:
	bool connected_;
	QObject *rootItem_;
	SocketPtr socket_;
	SearchModel searchModel_;

private:
	explicit Dispatcher(QObject *parent = nullptr);

public:
	virtual ~Dispatcher();

signals:
	void registration(int code);
	void auth(int code);
	void connectState(int state);
	void searched(int result);

private slots:
	void connected();
	void processMessage(const QString &message);

public:
	bool start(QObject *rootItem);
	void stop();

	Q_INVOKABLE void regContact(const QVariantMap &data);
	Q_INVOKABLE void authContact(const QVariantMap &data, bool autologin);
	Q_INVOKABLE void sendMessage(const QVariantMap &data);
	Q_INVOKABLE void searchContact(const QString &text);
	SearchModel *searchModel() { return &searchModel_; }

private:
	QString convertImageToBase64(const QString &fileName) const;
	QString convertImageFromBase84(const QString &base64) const;
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
