#include "database.h"
#include "dbnames.h"
#include "settings.h"

#include <QVariant>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

Database::Database()
{
}

Database::~Database()
{
	close();
}

bool Database::open()
{
	// Create database path
	QDir dbDir = Settings::dataPath() + QDir::separator() + "db";
	if (!dbDir.exists())
		dbDir.mkpath(".");

	// Open or create database
	QString dbFile = dbDir.absolutePath() + QDir::separator() + kDbName;
	db_ = QSqlDatabase::addDatabase("QSQLITE");
	db_.setHostName(kDbHostName);
	db_.setDatabaseName(dbFile);

	if (db_.open())
	{
		if (db_.tables().empty())
			return createTables();
	}
	else
		return false;

	return true;
}

bool Database::createTables()
{
	QSqlQuery query(db_);
	if (!query.exec("CREATE TABLE " + QString(kHistoryName) + " ("
					"id INTEGER PRIMARY KEY AUTOINCREMENT, "
					"cid INTEGER KEY NOT NULL, "
					"sender BOOLEAN NOT NULL, "
					"text TEXT NOT NULL,"
					"ts TIMESTAMP NOT NULL)"))
	{
		qDebug() << query.lastError().text();
		return false;
	}

	if (!query.exec("CREATE TABLE " + QString(kContactsName) + " ("
					"id INTEGER PRIMARY KEY AUTOINCREMENT, "
					"name VARCHAR(50) NOT NULL,"
					"imgname VARCHAR(50),"
					"phone VARCHAR(20),"
					"ts TIMESTAMP NOT NULL)"))
	{
		qDebug() << query.lastError().text();
		return false;
	}

	appendContact({"me", "empty", ""});
	return true;
}

void Database::close()
{
	db_.close();
}

bool Database::appendHistory(const QVariantList &list)
{
	if (list[0].toInt() <= 0)
		return false;

	QSqlQuery query(db_);
	query.prepare("INSERT INTO " + QString(kHistoryName) + " (cid, sender, text, ts)"
					" VALUES (:cid, :sender, :text, :ts)");

	query.bindValue(":cid", list[0]);
	query.bindValue(":sender", list[1]);
	query.bindValue(":text", list[2]);
	query.bindValue(":ts", QVariant(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")));

	if (!query.exec())
	{
		qDebug() << query.lastError().text();
		return false;
	}

	return true;
}

bool Database::removeHistory(int id)
{
	QSqlQuery query(db_);
	query.prepare("DELETE FROM " + QString(kHistoryName) + " WHERE id = :id");
	query.bindValue(":id", id);

	if (!query.exec())
	{
		qDebug() << query.lastError().text();
		return false;
	}

	return true;
}

bool Database::modifyHistory(int id, const QString &text)
{
	QSqlQuery query(db_);
	query.prepare("UPDATE " + QString(kHistoryName) + " SET text = :text WHERE id = :id");
	query.bindValue(":id", id);
	query.bindValue(":text", text);

	if (!query.exec())
	{
		qDebug() << query.lastError().text();
		return false;
	}

	return true;
}

bool Database::clearHistory(int cid)
{
	QSqlQuery query(db_);
	query.prepare("DELETE FROM " + QString(kHistoryName) + " WHERE cid = :cid");
	query.bindValue(":cid", cid);

	if (!query.exec())
	{
		qDebug() << query.lastError().text();
		return false;
	}

	return true;
}

bool Database::appendContact(const QVariantList &list)
{
	QSqlQuery query(db_);
	query.prepare("INSERT INTO " + QString(kContactsName) + " (name, imgname, phone, ts)"
												" VALUES (:name, :imgname, :phone, :ts)");

	query.bindValue(":name", list[0]);
	query.bindValue(":imgname", list[1]);
	query.bindValue(":phone", list[2]);
	query.bindValue(":ts", QVariant(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")));

	if (!query.exec())
	{
		qDebug() << query.lastError().text();
		return false;
	}

	return true;
}

bool Database::modifyContact(int id, const QVariantList &list)
{
	QSqlQuery query(db_);
	query.prepare("UPDATE " + QString(kContactsName) + " SET name = :name, imgname = :imgname, phone = :phone"
														" WHERE id = :id");

	query.bindValue(":id", id);
	query.bindValue(":name", list[0]);
	query.bindValue(":imgname", list[1]);
	query.bindValue(":phone", list[2]);

	if (!query.exec())
	{
		qDebug() << query.lastError().text();
		return false;
	}

	return true;
}

bool Database::removeContact(int id)
{
	QSqlQuery query(db_);
	query.prepare("DELETE FROM " + QString(kContactsName) + " WHERE id = :id");
	query.bindValue(":id", id);

	if (!query.exec())
	{
		qDebug() << query.lastError().text();
		return false;
	}

	return true;
}

QVariantList Database::contactCard(int id)
{
	QVariantList result;

	QSqlQuery query(db_);
	query.prepare("SELECT name, imgname, phone FROM " +
				  QString(kContactsName) + " WHERE id = " + QString::number(id));

	if (!query.exec())
	{
		qDebug() << query.lastError().text();
		return result;
	}

	query.first();
	result.push_back(query.value(0));
	result.push_back(query.value(1));
	result.push_back(query.value(2));
	return result;
}
