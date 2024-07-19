#include "history.h"
#include "log.h"
#include "database.h"
#include "settings.h"
#include "dispatcher.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

HistoryService::HistoryService()
{
}

HistoryService::~HistoryService()
{
}

void HistoryService::add(const QVariantMap& data)
{
	int hid = GetDatabase()->appendHistory(data);
	if (hid == 0)
	{
		LOGW("Can't append history, try again later!");
		return;
	}

	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["hid"] = hid;
	object["action"] = static_cast<int>(Action::AddHistory);

	GetDispatcher()->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void HistoryService::modify(const QVariantMap& data)
{
	if (!GetDatabase()->modifyHistory(data))
	{
		LOGW("Can't modify history, try again later!");
		return;
	}

	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["action"] = static_cast<int>(Action::ModifyHistory);

	GetDispatcher()->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void HistoryService::remove(const QVariantMap& data)
{
	if (!GetDatabase()->removeHistory(data["hid"].toInt()))
	{
		LOGW("Can't remove history, try again later!");
		return;
	}

	QJsonObject object = QJsonObject::fromVariantMap(data);
	object["action"] = static_cast<int>(Action::RemoveHistory);

	GetDispatcher()->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void HistoryService::clear(int cid)
{
	if (!GetDatabase()->clearHistory(cid))
	{
		LOGW("Can't clear history, try again later!");
		return;
	}

	QJsonObject object;
	object["cid"] = cid;
	object["action"] = static_cast<int>(Action::ClearHistory);

	GetDispatcher()->sendMessage(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void HistoryService::actionNew(const QJsonObject& root)
{
	IntList contacts;
	QJsonArray historyArray = root["history"].toArray();
	for (const QJsonValue &data : historyArray)
	{
		// Append history record
		QJsonObject object = data.toObject();
		QVariantMap histoty = object.toVariantMap();
		histoty["sync"] = true;
		if (GetDatabase()->appendHistory(histoty) == 0)
		{
			LOGW("Can't append history!");
			return;
		}

		int contactId = 0;

		// I'm sender
		if (histoty["cid"].toInt() == GetSettings()->params()["id"].toInt())
			contactId = histoty["rid"].toInt();

		// I'm receiver
		if (histoty["rid"].toInt() == GetSettings()->params()["id"].toInt())
		{
			contactId = histoty["cid"].toInt();
			if (!root["update"].toBool())
				GetDatabase()->historyModel()->update(contactId);
		}

		// Contact is already added
		if (contacts.contains(contactId))
			continue;

		// If contact does not exist but there is in history
		if (!GetDatabase()->contactExists(contactId))
		{
			contacts.push_back(contactId);
			QJsonObject rootObject;
			rootObject["id"] = contactId;
			rootObject["action"] = static_cast<int>(Action::QueryContact);
			GetDispatcher()->sendMessage(QJsonDocument(rootObject).toJson(QJsonDocument::Compact));
		}
	}
}