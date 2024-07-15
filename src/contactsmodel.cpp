#include "contactsmodel.h"
#include "dbnames.h"

ContactsModel::ContactsModel(QObject *parent)
	: QSqlQueryModel(parent)
{
	roleNames_[IdRole] = "cid";
	roleNames_[NameRole] = "name";
	roleNames_[LoginRole] = "login";
	roleNames_[ImageRole] = "image";
	roleNames_[PhoneRole] = "phone";
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
	if (role > Qt::UserRole)
	{
		int column = role - (Qt::UserRole + 1);
		QModelIndex modelIndex = QSqlQueryModel::index(index.row(), column);
		return modelIndex.data();
	}

	return QSqlQueryModel::data(index, role);
}

void ContactsModel::update()
{
	setQuery("SELECT cid, name, login, image, phone FROM " +
			 QString(kContactsName));
}

QVariantMap ContactsModel::card(int row)
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

	contact["cid"] = data(index(row, 0), IdRole);
	contact["name"] = data(index(row, 1), NameRole);
	contact["login"] = data(index(row, 2), LoginRole);
	contact["image"] = data(index(row, 3), ImageRole);
	contact["phone"] = data(index(row, 4), PhoneRole);

	return contact;
}
