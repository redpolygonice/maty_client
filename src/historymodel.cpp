#include "historymodel.h"
#include "dbnames.h"

#include <QString>
#include <QDebug>

HistoryModel::HistoryModel(QObject *parent)
	: QSqlQueryModel(parent)
{
	roleNames_[IdRole] = "id";
	roleNames_[SenderRole] = "sender";
	roleNames_[TextRole] = "message";
	roleNames_[DateRole] = "date";
	roleNames_[NameRole] = "name";
	roleNames_[ImageRole] = "imgname";
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

void HistoryModel::update(int cid)
{
	if (cid < 0)
		return;

	setQuery("SELECT history.id, history.sender, history.text, history.ts,"
			 " contacts.name AS name, contacts.imgname AS imgname"
			 " FROM " + QString(kHistoryName) +
			 " LEFT JOIN " + QString(kContactsName) + " ON contacts.id = cid"
			 " WHERE cid = " + QString::number(cid));
}
