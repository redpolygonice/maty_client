#include "contactsmodel.h"
#include "dbnames.h"

ContactsModel::ContactsModel(QObject *parent)
	: QSqlQueryModel(parent)
{
	roleNames_[IdRole] = "id";
	roleNames_[NameRole] = "name";
	roleNames_[ImageRole] = "imgname";
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
	setQuery("SELECT id, name, imgname, phone FROM " +
			 QString(kContactsName));
}

QVariantList ContactsModel::card(int row)
{
	if (row < 0)
		return QVariantList({ "", "", "" });

	QVariantList list{ data(index(row, 1), NameRole),
					 data(index(row, 2), ImageRole),
					 data(index(row, 3), PhoneRole) };
	return list;
}
