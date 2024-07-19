#ifndef CONTACTS_H
#define CONTACTS_H

#include <QObject>
#include <QThread>
#include <QSharedPointer>
#include <QVariantMap>

class ContactsService;
using ContactsServicePtr = QSharedPointer<ContactsService>;

class ContactsService : public QObject
{
	Q_OBJECT

public:
	ContactsService();
	~ContactsService();

public:
	static ContactsServicePtr create() { return ContactsServicePtr::create(); }
	void add(QVariantMap &data);
	void addSearch(const QVariantMap &data);
	void remove(const QVariantMap &data);
	void search(const QString &text);
	QVariantMap selfInfo() const;
	void actionSearch(QJsonObject &root);
	void actionQuery(const QJsonObject &root);
};



#endif // CONTACTS_H
