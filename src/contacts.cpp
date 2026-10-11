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

void ContactsService::add(const QVariantMap& data)
{
	if (GetDatabase()->contactExists(data["id"].toInt()))
		return;

	data["image_name"] = Image::convertFromBase64(data["image_data"].toString(), GetSettings()->imagePath());
	if (!GetDatabase()->appendContact(data))
	{
		LOGW("Can't append contact, try again later!");
		return;
	}

	QJsonObject object;
	object["action"] = static_cast<int>(Action::LinkContact);
	object["cid"] = GetSettings()->params()["id"].toInt();
	object["rid"] = data["id"].toInt();
	object["approved"] = data["approved"].toBool();

	GetDispatcher()->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void ContactsService::modifySelf(const QVariantMap& data)
{
	GetSettings()->params()["name"] = data["name"];
	GetSettings()->params()["phone"] = data["phone"];
	GetSettings()->params()["about"] = data["about"];
	GetSettings()->save();

	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["action"] = static_cast<int>(Action::ModifyContact);

	QString imagePath = GetSettings()->imagePath() + QDir::separator() + GetSettings()->params()["image_name"].toString();
	object["image_data"] = Image::convertToBase64(imagePath);
	object["image_hash"] = GetSettings()->params()["image_hash"].toString();

	GetDispatcher()->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void ContactsService::addSearch(const QVariantMap& data)
{
	QString imageName = data["image"].toString();
	QString oldName = GetSettings()->tempPath() + QDir::separator() + imageName;
	QString newName = GetSettings()->imagePath() + QDir::separator() + imageName;
	QFile::rename(oldName, newName);

	QVariantMap contact = data;
	contact["image_name"] = imageName;
	QString imageData = std::move(Image::convertToBase64(newName));
	QString imageHash = std::move(QString(QCryptographicHash::hash(imageData.toUtf8(),
		QCryptographicHash::Sha256).toHex()));
	contact["image_hash"] = imageHash;

	if (!GetDatabase()->appendContact(contact))
	{
		LOGW("Can't append contact, try again later!");
		return;
	}

	QJsonObject object;
	object["action"] = static_cast<int>(Action::LinkContact);
	object["cid"] = GetSettings()->params()["id"].toInt();
	object["rid"] = contact["id"].toInt();
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

	QString imageName = GetSettings()->imagePath() + QDir::separator() + data["image_name"].toString();
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
		{"phone", ""},
		{"about", ""}
	};

	contact["id"] = GetSettings()->params()["id"].toInt();
	contact["name"] = GetSettings()->params()["name"].toString();
	contact["login"] = GetSettings()->params()["login"].toString();
	contact["image"] = GetSettings()->params()["image_name"].toString();
	contact["phone"] = GetSettings()->params()["phone"].toString();
	contact["about"] = GetSettings()->params()["about"].toString();

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
			object["image_name"] = Image::convertFromBase64(object["image_data"].toString(), GetSettings()->tempPath());
			contacts.push_back(object);
		}

		root["contacts"] = contacts;
		GetDispatcher()->searchModel()->update(root);
	}

	GetDispatcher()->emitSearchResult(static_cast<int>(result));
}

void ContactsService::actionQuery(const QJsonObject& root)
{
	QJsonObject contactObject = root["contact"].toObject();
	QVariantMap contact = contactObject.toVariantMap();
	contact["approved"] = false;
	add(contact);
	GetDatabase()->contactsModel()->update();
}
