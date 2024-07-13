#include "searchmodel.h"

#include <QJsonObject>
#include <QJsonArray>

SearchModel::SearchModel(QObject *parent)
	: QStandardItemModel(parent)
{
	roleNames_[IdRole] = "id";
	roleNames_[NameRole] = "name";
	roleNames_[LoginRole] = "login";
	roleNames_[ImageRole] = "image";
	roleNames_[PhoneRole] = "phone";
}

void SearchModel::update(const QJsonObject &root)
{
	QJsonArray contacts = root["contacts"].toArray();
	if (contacts.empty())
		return;

	for (const QJsonValue &contact : contacts)
	{
		QJsonObject object = contact.toObject();
		QStandardItem *item = new QStandardItem();
		item->setData(object["name"].toString(), NameRole);
		item->setData(object["login"].toString(), LoginRole);
		item->setData(object["image"].toString(), ImageRole);
		item->setData(object["phone"].toString(), PhoneRole);
		appendRow(item);
	}
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
	QStandardItem *item = itemFromIndex(index);

	if (role == NameRole || role == LoginRole || role == ImageRole || role == PhoneRole)
		return item->data(role);

	return QVariant();
}
