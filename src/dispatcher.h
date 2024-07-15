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
		Search,
		LinkContact,
		UnlinkContact,
		ClearHistory
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
	void connectStatus(int status);
	void searchResult(int result);

private slots:
	void connected();
	void processMessage(const QString &message);

public:
	bool start(QObject *rootItem);
	void stop();

	SearchModel *searchModel() { return &searchModel_; }
	Q_INVOKABLE void addContact(const QVariantMap &data);
	Q_INVOKABLE void removeContact(const QVariantMap &data);
	Q_INVOKABLE void clearHistory(int cid);
	Q_INVOKABLE void regContact(const QVariantMap &data);
	Q_INVOKABLE void authContact(const QVariantMap &data, bool autologin);
	Q_INVOKABLE void searchContact(const QString &text);
	Q_INVOKABLE void sendMessage(int rid, const QString &message);
	Q_INVOKABLE QVariantMap selfContactInfo() const;

private:
	void actionRegistration(QJsonObject &root);
	void actionAuth(const QJsonObject &root);
	void actionSearch(QJsonObject &root);
	void actionMessage(const QJsonObject &root);
	QString convertImageToBase64(const QString &fileName, QString &newName) const;
	QString convertImageFromBase84(const QString &base64, const QString &dir) const;
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
