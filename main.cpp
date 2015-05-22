#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "skyproxymodel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SkyProxyModel *model = new SkyProxyModel();

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("sky_model", model);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
