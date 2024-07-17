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
		QueryContact,
		LinkContact,
		UnlinkContact,
		AddHistory,
		ModifyHistory,
		RemoveHistory,
		ClearHistory,
		NewHistory
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
	void setSelfId(int id);

private slots:
	void connected();
	void processMessage(const QString &message);

public:
	bool start(QObject *rootItem);
	void stop();

	SearchModel *searchModel() { return &searchModel_; }
	Q_INVOKABLE void addContact(QVariantMap &data);
	Q_INVOKABLE void addSearchContact(const QVariantMap &data);
	Q_INVOKABLE void removeContact(const QVariantMap &data);
	Q_INVOKABLE void regContact(const QVariantMap &data);
	Q_INVOKABLE void authContact(const QVariantMap &data, bool autologin);
	Q_INVOKABLE void searchContact(const QString &text);
	Q_INVOKABLE void sendMessage(int rid, const QString &message);
	Q_INVOKABLE QVariantMap selfContactInfo() const;

	Q_INVOKABLE void addHistory(const QVariantMap &data);
	Q_INVOKABLE void removeHistory(int id);
	Q_INVOKABLE void modifyHistory(const QVariantMap &data);
	Q_INVOKABLE void clearHistory(int cid);

private:
	void actionRegistration(const QJsonObject &root);
	void actionAuth(const QJsonObject &root);
	void actionSearch(QJsonObject &root);
	void actionMessage(const QJsonObject &root);
	void actionQueryContact(const QJsonObject &root);
	void actionNewHistory(const QJsonObject &root);
	bool waitConnected();
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
