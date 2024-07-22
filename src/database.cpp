#include "database.h"
#include "dbnames.h"
#include "settings.h"
#include "log.h"

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
															  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
															  "hid INTEGER KEY NOT NULL, " // Id from server history
															  "cid INTEGER KEY NOT NULL," // Sender id
															  "rid INTEGER KEY NOT NULL," // Receyver id
															  "text TEXT NOT NULL,"
															  "sync BOOLEAN,"
															  "state INTEGER,"
															  "ts TIMESTAMP NOT NULL)"))
	{
		LOGE(query.lastError().text().toStdString());
		return false;
	}

	if (!query.exec("CREATE TABLE " + QString(kContactsName) + " ("
															   "id INTEGER PRIMARY KEY, "
															   "name VARCHAR(50) NOT NULL,"
															   "login VARCHAR(50) NOT NULL,"
															   "image VARCHAR(50),"
															   "phone VARCHAR(20),"
															   "about TEXT,"
															   "approved BOOLEAN,"
															   "ts TIMESTAMP NOT NULL)"))
	{
		LOGE(query.lastError().text().toStdString());
		return false;
	}

	return true;
}

void Database::close()
{
	db_.close();
}

void Database::remove()
{
	QDir dbDir = Settings::dataPath() + QDir::separator() + "db";
	QString dbFile = dbDir.absolutePath() + QDir::separator() + kDbName;

	if (!QFile::exists(dbFile))
		return;

	if (db_.isOpen())
	{
		QSqlDatabase::removeDatabase(db_.connectionName());
		db_.close();
	}

	QFile::remove(dbFile);
}

int Database::appendHistory(const QVariantMap &data)
{
	if (data["cid"].toInt() <= 0)
		return 0;

	QSqlQuery query(db_);
	query.prepare("INSERT INTO " + QString(kHistoryName) + " (hid, cid, rid, text, sync, ts)"
														   " VALUES (:hid, :cid, :rid, :text, :sync, :ts)");

	query.bindValue(":hid", data["hid"].toInt());
	query.bindValue(":cid", data["cid"].toInt());
	query.bindValue(":rid", data["rid"].toInt());
	query.bindValue(":text", data["text"].toString());
	query.bindValue(":sync", data["sync"].toInt());
	if (data["ts"].isNull() || data["ts"].toString().isEmpty())
		query.bindValue(":ts", QVariant(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")));
	else
		query.bindValue(":ts", data["ts"].toDateTime());

	if (!query.exec())
	{
		LOGE(query.lastError().text().toStdString());
		return 0;
	}

	// Query history id
	query.prepare("SELECT id FROM " + QString(kHistoryName));
	if (!query.exec())
	{
		LOGE(query.lastError().text().toStdString());
		return 0;
	}

	if (!query.last())
		return 0;

	return query.value(0).toInt();
}

bool Database::modifyHistory(const QVariantMap &data)
{
	QSqlQuery query(db_);
	if (data["update"].toBool())
		query.prepare("UPDATE " + QString(kHistoryName) + " SET text = :text WHERE hid = :hid");
	else
		query.prepare("UPDATE " + QString(kHistoryName) + " SET text = :text WHERE id = :hid");

	query.bindValue(":hid", data["hid"].toInt());
	query.bindValue(":text", data["text"].toString());

	if (!query.exec())
	{
		LOGE(query.lastError().text().toStdString());
		return false;
	}

	return true;
}

bool Database::removeHistory(const QVariantMap &data)
{
	QSqlQuery query(db_);
	if (data["update"].toBool())
		query.prepare("DELETE FROM " + QString(kHistoryName) + " WHERE hid = :hid");
	else
		query.prepare("DELETE FROM " + QString(kHistoryName) + " WHERE id = :hid");

	query.bindValue(":hid", data["hid"].toInt());

	if (!query.exec())
	{
		LOGE(query.lastError().text().toStdString());
		return false;
	}

	return true;
}

bool Database::clearHistory(int cid)
{
	QSqlQuery query(db_);
	query.prepare("DELETE FROM " + QString(kHistoryName) + " WHERE cid = :cid OR rid = :cid");
	query.bindValue(":cid", cid);

	if (!query.exec())
	{
		LOGE(query.lastError().text().toStdString());
		return false;
	}

	return true;
}

bool Database::historyExists(int hid)
{
	QSqlQuery query(db_);
	query.prepare("SELECT id FROM " + QString(kHistoryName) + " WHERE id = :hid");
	query.bindValue(":hid", hid);

	if (!query.exec())
	{
		LOGE(query.lastError().text().toStdString());
		return false;
	}

	if (!query.next())
		return false;

	return true;
}

bool Database::appendContact(const QVariantMap &contact)
{
	QSqlQuery query(db_);
	query.prepare("INSERT INTO " + QString(kContactsName) + " (id, name, login, image, phone, approved, ts)"
															" VALUES (:id, :name, :login, :image, :phone, :approved, :ts)");

	query.bindValue(":id", contact["id"].toInt());
	query.bindValue(":name", contact["name"].toString());
	query.bindValue(":login", contact["login"].toString());
	query.bindValue(":image", contact["image"].toString());
	query.bindValue(":phone", contact["phone"].toString());
	query.bindValue(":approved", contact["approved"].toBool());
	query.bindValue(":ts", QVariant(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")));

	if (!query.exec())
	{
		LOGE(query.lastError().text().toStdString());
		return false;
	}

	return true;
}

bool Database::modifyContact(const QVariantMap &contact)
{
	QSqlQuery query(db_);
	query.prepare("UPDATE " + QString(kContactsName) + " SET name = :name, login = :login image = :image, phone = :phone"
													   " WHERE id = :id");

	query.bindValue(":id", contact["id"].toInt());
	query.bindValue(":name", contact["name"].toString());
	query.bindValue(":login", contact["login"].toString());
	query.bindValue(":image", contact["image"].toString());
	query.bindValue(":phone", contact["phone"].toString());

	if (!query.exec())
	{
		LOGE(query.lastError().text().toStdString());
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
		LOGE(query.lastError().text().toStdString());
		return false;
	}

	return true;
}

QVariantMap Database::contactData(int id)
{
	QVariantMap contact;

	QSqlQuery query(db_);
	query.prepare("SELECT * FROM " +
				  QString(kContactsName) + " WHERE id = " + QString::number(id));

	if (!query.exec())
	{
		LOGE(query.lastError().text().toStdString());
		return contact;
	}

	if (query.first())
	{
		contact["id"] = query.value("id").toInt();
		contact["name"] = query.value("name").toString();
		contact["login"] = query.value("login").toString();
		contact["image"] = query.value("image").toString();
		contact["phone"] = query.value("phone").toString();
		contact["approved"] = query.value("approved").toBool();
	}

	return contact;
}

bool Database::contactExists(int id) const
{
	QSqlQuery query(db_);
	query.prepare("SELECT id FROM " +
				  QString(kContactsName) + " WHERE id = " + QString::number(id));

	if (!query.exec())
	{
		LOGE(query.lastError().text().toStdString());
		return false;
	}

	return query.next();
}
