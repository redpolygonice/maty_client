#ifndef SQLMODEL_H
#define SQLMODEL_H

#include <QObject>
#include <QSqlQueryModel>

class HistoryModel : public QSqlQueryModel
{
	Q_OBJECT

public:
	enum Roles
	{
		IdRole = Qt::UserRole + 1,
		SenderRole,
		TextRole,
		DateRole,
		NameRole,
		ImageRole
	};

private:
	QHash<int, QByteArray> roleNames_;

public:
	explicit HistoryModel(QObject *parent = 0);
	virtual ~HistoryModel() = default;

	HistoryModel(const HistoryModel&) = delete;
	HistoryModel(HistoryModel&&) = delete;
	HistoryModel& operator= (const HistoryModel&) = delete;
	HistoryModel& operator= (HistoryModel&&) = delete;

public:
	Q_INVOKABLE void update(int cid);

public:
	QHash<int, QByteArray> roleNames() const override { return roleNames_; }
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // SQLMODEL_H
