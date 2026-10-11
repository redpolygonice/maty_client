#include "contactsmodel.h"
#include "dbnames.h"

ContactsModel::ContactsModel(QObject *parent)
	: QSqlQueryModel(parent)
{
	roleNames_[IdRole] = "id";
	roleNames_[NameRole] = "name";
	roleNames_[LoginRole] = "login";
	roleNames_[ImageRole] = "image";
	roleNames_[PhoneRole] = "phone";
	roleNames_[AboutRole] = "about";
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
	setQuery("SELECT id, name, login, image_name AS image, phone, about FROM " +
			 QString(kContactsName));
}

QVariantMap ContactsModel::card(int row)
{
	QVariantMap contact{
		{"id", 0},
		{"name", ""},
		{"login", ""},
		{"image", ""},
		{"phone", ""},
		{"about", ""}
	};

	if (row < 0)
		return contact;

	contact["id"] = data(index(row, 0), IdRole);
	contact["name"] = data(index(row, 1), NameRole);
	contact["login"] = data(index(row, 2), LoginRole);
	contact["image"] = data(index(row, 3), ImageRole);
	contact["phone"] = data(index(row, 4), PhoneRole);
	contact["about"] = data(index(row, 5), AboutRole);

	return contact;
}
