#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include <QStandardItemModel>
#include <QJsonObject>

class SearchModel : public QStandardItemModel
{
	Q_OBJECT

public:
	enum Roles
	{
		IdRole = Qt::UserRole + 1,
		NameRole,
		LoginRole,
		ImageRole,
		PhoneRole,
		DateRole
	};

private:
	QHash<int, QByteArray> roleNames_;

public:
	explicit SearchModel(QObject *parent = 0);
	virtual ~SearchModel() = default;

	SearchModel(const SearchModel&) = delete;
	SearchModel(SearchModel&&) = delete;
	SearchModel& operator= (const SearchModel&) = delete;
	SearchModel& operator= (SearchModel&&) = delete;

public:
	Q_INVOKABLE void update(const QJsonObject &root);

public:
	QHash<int, QByteArray> roleNames() const override { return roleNames_; }
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // SEARCHMODEL_H
