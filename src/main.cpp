#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

#include "dispatcher.h"
#include "database.h"
#include "settings.h"
#include "log.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

	QGuiApplication app(argc, argv);
	QQmlApplicationEngine engine;
	QQmlApplicationEngine loginEngine;

	Log::create();
	GetSettings()->load();

	if (!GetDatabase()->open())
	{
		qDebug() << "Can't open database!";
		return 1;
	}

	auto loadMain = [&]() {
		const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
		QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
			&app, [url](QObject *obj, const QUrl &objUrl) {
				if (!obj && url == objUrl)
					QCoreApplication::exit(-1);
			}, Qt::QueuedConnection);
		engine.load(url);

		QObject *rootItem = engine.rootObjects().first();
		GetDispatcher()->start(rootItem);
	};

	engine.rootContext()->setContextProperty("historyModel", GetDatabase()->historyModel());
	engine.rootContext()->setContextProperty("contactsModel", GetDatabase()->contactsModel());
	engine.rootContext()->setContextProperty("database", GetDatabase().get());
	engine.rootContext()->setContextProperty("settings", GetSettings().get());
	engine.rootContext()->setContextProperty("dispatcher", GetDispatcher().get());

	loadMain();
	return app.exec();
}
