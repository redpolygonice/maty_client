#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "socket.h"
#include "searchmodel.h"
#include "contacts.h"
#include "history.h"

#include <QObject>
#include <QThread>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QVariantMap>
#include <QFutureWatcher>

using SocketPtr = QSharedPointer<Socket>;
using IntList = QList<int>;

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

class Dispatcher : public QObject
{
	friend class QSharedPointer<Dispatcher>;

	Q_OBJECT

private:
	std::atomic_bool connected_;
	bool update_;
	QObject *rootItem_;
	SocketPtr socket_;
	SearchModel searchModel_;
	ContactsServicePtr contacts_;
	HistoryServicePtr history_;
	QFutureWatcher<int> connectWatcher_;

private:
	explicit Dispatcher(QObject *parent = nullptr);

public:
	virtual ~Dispatcher();

signals:
	void reg(int code);
	void auth(int code);
	void connectState(int state);
	void searchResult(int result);
	void historyUpdated();
	void contactsUpdated();
	void setSelfId(int id);

private slots:
	void connected();
	void processMessage(const QString &message);

public:
	bool start(QObject *rootItem);
	void stop();

	void sendMessage(const QString &message);
	SocketPtr socket() { return socket_; }
	SearchModel *searchModel() { return &searchModel_; }
	void emitSearchResult(int result) { emit searchResult(result); }
	void emitHistoryUpdate() { emit historyUpdated(); }
	void emitContactsUpdate() { emit contactsUpdated(); }

	Q_INVOKABLE void regContact(const QVariantMap &data);
	Q_INVOKABLE void authContact(const QVariantMap &data);
	Q_INVOKABLE bool isConnected() const { return connected_; }
	Q_INVOKABLE void addContact(QVariantMap &data) { contacts_->add(data); }
	Q_INVOKABLE void addSearchContact(const QVariantMap &data) { contacts_->addSearch(data); }
	Q_INVOKABLE void removeContact(const QVariantMap &data) { contacts_->remove(data); }
	Q_INVOKABLE void searchContact(const QString &text) { contacts_->search(text); }
	Q_INVOKABLE QVariantMap selfContactInfo() const { return contacts_->selfInfo(); }
	Q_INVOKABLE void addHistory(const QVariantMap &data) { history_->add(data); }
	Q_INVOKABLE void modifyHistory(const QVariantMap &data) { history_->modify(data); }
	Q_INVOKABLE void removeHistory(const QVariantMap& data) { history_->remove(data); }
	Q_INVOKABLE void clearHistory(int cid) { history_->clear(cid); }

private:
	void actionReg(const QJsonObject &root);
	void actionAuth(const QJsonObject &root);
	void actionUpdateAll(const QJsonObject &root);
	void logMessage(const QString &message);
	template <typename Function> void waitConnected(Function &&func);
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
