#include "contacts.h"
#include "log.h"
#include "database.h"
#include "settings.h"
#include "dispatcher.h"
#include "image.h"

#include <QFile>
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

ContactsService::ContactsService()
{
}

ContactsService::~ContactsService()
{
}

void ContactsService::add(QVariantMap& data)
{
	if (GetDatabase()->contactExists(data["id"].toInt()))
		return;

	data["image"] = Image::convertFromBase84(data["image"].toString(), GetSettings()->imagePath());
	if (!GetDatabase()->appendContact(data))
	{
		LOGW("Can't append contact, try again later!");
		return;
	}

	QJsonObject object;
	object["action"] =static_cast<int>(Action::LinkContact);
	object["cid"] = GetSettings()->params()["id"].toInt();
	object["rid"] = data["id"].toInt();
	object["approved"] = data["approved"].toBool();

	GetDispatcher()->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void ContactsService::addSearch(const QVariantMap& data)
{
	if (!GetDatabase()->appendContact(data))
	{
		LOGW("Can't append contact, try again later!");
		return;
	}

	QString oldName = GetSettings()->tempPath() + QDir::separator() + data["image"].toString();
	QString newName = GetSettings()->imagePath() + QDir::separator() + data["image"].toString();
	QFile::rename(oldName, newName);

	QJsonObject object;
	object["action"] =static_cast<int>(Action::LinkContact);
	object["cid"] = GetSettings()->params()["id"].toInt();
	object["rid"] = data["id"].toInt();
	object["approved"] = true;

	GetDispatcher()->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void ContactsService::remove(const QVariantMap& data)
{
	int id = data["id"].toInt();
	if (!GetDatabase()->removeContact(id))
	{
		LOGW("Can't remove contact, try again later!");
		return;
	}

	QString imageName = GetSettings()->imagePath() + QDir::separator() + data["image"].toString();
	QFile::remove(imageName);

	GetDatabase()->clearHistory(id);

	QJsonObject object;
	object["action"] =static_cast<int>(Action::UnlinkContact);
	object["cid"] = GetSettings()->params()["id"].toInt();
	object["rid"] = id;

	GetDispatcher()->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void ContactsService::search(const QString& text)
{
	LOG("Search contact: " << text.toStdString());
	QJsonObject object;
	object["action"] = static_cast<int>(Action::Search);
	object["id"] = GetSettings()->params()["id"].toInt();
	object["text"] = text;

	GetDispatcher()->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

QVariantMap ContactsService::selfInfo() const
{
	QVariantMap contact{
		{"id", 0},
		{"name", ""},
		{"login", ""},
		{"image", ""},
		{"phone", ""}
	};

	contact["id"] = GetSettings()->params()["id"].toInt();
	contact["name"] = GetSettings()->params()["name"].toString();
	contact["login"] = GetSettings()->params()["login"].toString();
	contact["image"] = GetSettings()->params()["image"].toString();
	contact["phone"] = GetSettings()->params()["phone"].toString();

	return contact;
}

void ContactsService::actionSearch(QJsonObject& root)
{
	SearchResult result = static_cast<SearchResult>(root["searchResult"].toInt());
	if (result == SearchResult::Found)
	{
		QJsonArray contactsArray = root["contacts"].toArray();
		QJsonArray contacts;
		for (const QJsonValue &contact : contactsArray)
		{
			QJsonObject object = contact.toObject();
			object["image"] = Image::convertFromBase84(object["image"].toString(), GetSettings()->tempPath());
			contacts.push_back(object);
		}

		root["contacts"] = contacts;
		GetDispatcher()->searchModel()->update(root);
	}

	GetDispatcher()->setSearchResult(static_cast<int>(result));
}

void ContactsService::actionQuery(const QJsonObject& root)
{
	QJsonObject contactObject = root["contact"].toObject();
	QVariantMap contact = contactObject.toVariantMap();
	contact["approved"] = false;
	add(contact);
	GetDatabase()->contactsModel()->update();
}
