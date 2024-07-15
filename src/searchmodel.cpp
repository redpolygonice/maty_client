#include "searchmodel.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QList>

SearchModel::SearchModel(QObject *parent)
	: QStandardItemModel(parent)
{
	roleNames_[IdRole] = "cid";
	roleNames_[NameRole] = "name";
	roleNames_[LoginRole] = "login";
	roleNames_[ImageRole] = "image";
	roleNames_[PhoneRole] = "phone";
}

void SearchModel::update(const QJsonObject &root)
{
	clear();
	QJsonArray contacts = root["contacts"].toArray();
	if (contacts.empty())
		return;

	for (const QJsonValue &contact : contacts)
	{
		QJsonObject object = contact.toObject();
		QStandardItem *item = new QStandardItem();
		item->setData(object["cid"].toInt(), IdRole);
		item->setData(object["name"].toString(), NameRole);
		item->setData(object["login"].toString(), LoginRole);
		item->setData(object["image"].toString(), ImageRole);
		item->setData(object["phone"].toString(), PhoneRole);
		appendRow(item);
	}

	setRowCount(contacts.size());
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
	QStandardItem *item = itemFromIndex(index);

	if (role == IdRole || role == NameRole || role == LoginRole || role == ImageRole || role == PhoneRole)
		return item->data(role);

	return QVariant();
}

QVariantMap SearchModel::card(int row)
{
	QVariantMap contact{
		{"cid", 0},
		{"name", ""},
		{"login", ""},
		{"image", ""},
		{"phone", ""}
	};

	if (row < 0)
		return contact;

	QStandardItem *item = QStandardItemModel::item(row);
	if (item == nullptr)
		return contact;

	contact["cid"] = item->data(IdRole);
	contact["name"] = item->data(NameRole);
	contact["login"] = item-> data(LoginRole);
	contact["image"] = item->data(ImageRole);
	contact["phone"] = item->data(PhoneRole);

	return contact;
}
