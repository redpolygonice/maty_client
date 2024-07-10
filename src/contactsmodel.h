#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include "dbnames.h"

#include <QObject>
#include <QSqlQueryModel>

class ContactsModel : public QSqlQueryModel
{
	Q_OBJECT

public:
	enum Roles
	{
		IdRole = Qt::UserRole + 1,
		NameRole,
		ImageRole,
		PhoneRole,
		DateRole
	};

private:
	QHash<int, QByteArray> roleNames_;

public:
	explicit ContactsModel(QObject *parent = 0);
	virtual ~ContactsModel() = default;

	ContactsModel(const ContactsModel&) = delete;
	ContactsModel(ContactsModel&&) = delete;
	ContactsModel& operator= (const ContactsModel&) = delete;
	ContactsModel& operator= (ContactsModel&&) = delete;

public:
	Q_INVOKABLE void update();
	Q_INVOKABLE QVariantList card(int row);

public:
	QHash<int, QByteArray> roleNames() const override { return roleNames_; }
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // CONTACTSMODEL_H
