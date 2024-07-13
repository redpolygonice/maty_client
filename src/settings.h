#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QVariantMap>

class Settings : public QObject
{
	friend class QSharedPointer<Settings>;

	Q_OBJECT
	Q_PROPERTY(QVariantMap params READ params WRITE setParams NOTIFY paramsChanged)

private:
	const QString settingsFile_ = "settings.json";
	QVariantMap params_;

private:
	explicit Settings(QObject *parent = nullptr);

signals:
	void paramsChanged();

public:
	QString logPath() const;
	QString tempPath() const;
	Q_INVOKABLE QString imagePath() const;
	Q_INVOKABLE void saveAuthData(const QVariantMap &map);
	Q_INVOKABLE bool save();

public:
	static QString dataPath();
	bool load();

	QVariantMap &params() { return params_; }
	void setParams(const QVariantMap &map) { params_ = map; }

private:
	QString settingsPath() const;
};

using SettingsPtr = QSharedPointer<Settings>;

inline SettingsPtr GetSettings()
{
	static SettingsPtr settings = nullptr;
	if (settings == nullptr)
		settings = QSharedPointer<Settings>::create();
	return settings;
}

#endif // SETTINGS_H
