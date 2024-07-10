#ifndef DATABASE_H
#define DATABASE_H

#include "historymodel.h"
#include "contactsmodel.h"

#include <QObject>
#include <QString>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QVariant>
#include <QVariantList>
#include <QDateTime>
#include <QSharedPointer>

using HistoryRecord = std::tuple<QString, QString, QDateTime>;

// Sqlite database
class Database : public QObject
{
	friend class QSharedPointer<Database>;

	Q_OBJECT

private:
	QSqlDatabase db_;
	HistoryModel historyModel_;
	ContactsModel contactsModel_;

private:
	Database();

public:
	virtual ~Database();

	Database(const Database&) = delete;
	Database(Database&&) = delete;
	Database& operator= (const Database&) = delete;
	Database& operator= (Database&&) = delete;

public:
	Q_INVOKABLE bool appendHistory(const QVariantList &list);
	Q_INVOKABLE bool removeHistory(int id);
	Q_INVOKABLE bool modifyHistory(int id, const QString &text);
	Q_INVOKABLE bool clearHistory(int cid);

	Q_INVOKABLE bool appendContact(const QVariantList &list);
	Q_INVOKABLE bool modifyContact(int id, const QVariantList &list);
	Q_INVOKABLE bool removeContact(int id);
	Q_INVOKABLE QVariantList contactCard(int id);

public:
	bool open();
	void close();
	bool isOpen() const { return db_.isOpen(); }
	HistoryModel *historyModel() { return &historyModel_; }
	ContactsModel *contactsModel() { return &contactsModel_; }

private:
	bool createTables();
};

using DatabasePtr = QSharedPointer<Database>;

inline DatabasePtr GetDatabase()
{
	static DatabasePtr database = nullptr;
	if (database == nullptr)
		database = QSharedPointer<Database>::create();
	return database;
}

#endif // DATABASE_H
