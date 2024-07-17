#include "historymodel.h"
#include "dbnames.h"

#include <QString>
#include <QDebug>

HistoryModel::HistoryModel(QObject *parent)
	: QSqlQueryModel(parent)
{
	roleNames_[IdRole] = "id";
	roleNames_[CidRole] = "cid";
	roleNames_[RidRole] = "rid";
	roleNames_[TextRole] = "message";
	roleNames_[DateRole] = "date";
	roleNames_[NameRole] = "name";
	roleNames_[ImageRole] = "image";
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
	if (role > Qt::UserRole)
	{
		int column = role - (Qt::UserRole + 1);
		QModelIndex modelIndex = QSqlQueryModel::index(index.row(), column);
		return modelIndex.data();
	}

	return QSqlQueryModel::data(index, role);
}

void HistoryModel::update(int rid)
{
	if (rid <= 0)
		return;

	setQuery("SELECT history.id, history.cid, history.rid, history.text, history.ts,"
			 " contacts.name AS name, contacts.image AS image"
			 " FROM " + QString(kHistoryName) +
			 " LEFT JOIN " + QString(kContactsName) + " ON (contacts.id = rid OR contacts.id = cid)"
			 " WHERE rid = " + QString::number(rid) +
			 " OR cid = " + QString::number(rid));
}
