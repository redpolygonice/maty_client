#ifndef HISTORY_H
#define HISTORY_H

#include <QObject>
#include <QThread>
#include <QSharedPointer>
#include <QVariantMap>

class HistoryService;
using HistoryServicePtr = QSharedPointer<HistoryService>;

class HistoryService : public QObject
{
	Q_OBJECT

public:
	HistoryService();
	~HistoryService();

public:
	static HistoryServicePtr create() { return HistoryServicePtr::create(); }
	void add(const QVariantMap &data);
	void remove(const QVariantMap& data);
	void modify(const QVariantMap &data);
	void clear(int cid);
	void actionNew(const QJsonObject &root);
};

#endif // HISTORY_H
