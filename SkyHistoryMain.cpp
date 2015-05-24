#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTextCodec>

#include "skyproxymodel.h"
#include "main.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

    SkyProxyModel *model = new SkyProxyModel();

    QQmlApplicationEngine engine;
//    SkypeDB::main db("sqlite3", "database=/home/ilia/.Skype/sc.ryabokon.ilia/main.db");

    engine.rootContext()->setContextProperty("sky_model", model);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
