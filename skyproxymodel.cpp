#include "skyproxymodel.h"
#include "skymodel.h"
#include <QtCore>
#include <QTextDocument>

#include "litesql.hpp"
#include "main.hpp"

using namespace litesql;
using namespace SkypeDB;

void SkyDataLoader::process_msg(const QVariantMap &msg){
    try {
        SkypeDB::main db("sqlite3", "database=/home/ilia/.Skype/luxa_ryabic/main.db");
        // create tables, sequences and indexes
        db.verbose = false;
        auto dsch = litesql::select<Chats>(db);
        qDebug() << "Chats count" << dsch.count();

        auto ds = litesql::select<Messages>(db);
        int count = ds.count();
        const int step = 500;
        for (int i = 0; i < count; i+=step) {
            QString expr = QString("OID >= %1 AND OID < %2").arg(i).arg(i + step);
            auto ds = select<Messages>(db, RawExpr(expr.toLocal8Bit().data()));
            for (Messages message: ds.all()) {
                QString body = QString::fromStdString(message.body_xml);
                QTextDocument doc(body);
                QFont fnt = doc.defaultFont();
                fnt.setPointSize(10);
                doc.setDefaultFont(fnt);
                doc.adjustSize();
                QVariantMap m;
                m["Name"] = doc.toPlainText();
                m["Height"] = doc.size().height();
                //
                int offset = 9;
                double status_move = i << offset;
                double percentage = status_move / count;
                int status_res = static_cast<int>(percentage * 100) >> offset;
                m["Percent"] = status_res;

                emit send_msg(m);
            }
        }
        emit send_finished();

    } catch (Except e) {
        std::cerr << "Error: " << e << std::endl;
    }
}

SkyProxyModel::SkyProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{
    setSourceModel(new SkyModel);
//    sort(0);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    SkyDataLoader *skLoader = new SkyDataLoader();
    skLoader->moveToThread(&m_worker);
    connect(&m_worker, &QThread::finished, skLoader, &QObject::deleteLater);
    connect(this, SIGNAL(instigateLoad(QVariantMap)), skLoader, SLOT(process_msg(QVariantMap)));
    connect(skLoader, &SkyDataLoader::send_msg, this, &SkyProxyModel::handleLoadedData);
    connect(skLoader, &SkyDataLoader::send_finished, this, &SkyProxyModel::loadFinished);
    m_worker.start();
}
SkyProxyModel::~SkyProxyModel() {
    m_worker.quit();
    m_worker.wait();
}

bool SkyProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_row)
    Q_UNUSED(source_parent)
    const QModelIndex ind = sourceModel()->index(source_row, 0, source_parent);

    QString name_data = ind.data(Qt::UserRole).toString();

    bool accept = name_data.contains(filterRegExp());
    return accept;
}

bool SkyProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
//    qDebug() << "less than call";

    QString name_data_left = left.data(Qt::UserRole + 1).toString();
    QString name_data_right = right.data(Qt::UserRole + 1).toString();

    return name_data_left < name_data_right;
}

void SkyProxyModel::stringChanged(const QString &p_str)
{
    setFilterFixedString(p_str);
}

QVariant SkyProxyModel::get(int p_index, int role)
{
    return sourceModel()->data(mapToSource(index(p_index, 0)), role);
}
QString SkyProxyModel::get_name(int p_index)
{
    return sourceModel()->data(mapToSource(index(p_index, 0)), Qt::UserRole + 1).toString();
}

void SkyProxyModel::refresh()
{
//    static int i = 0;
//    qDebug() << "===SkyProxyModel::refresh(); call no" << ++i;
//    model_impl()->refresh();
}

//int SkyProxyModel::indexFromCode(const QString &code)
//{
//    int res = mapFromSource(sourceModel()->index(model_impl()->indexFromCode(code), 0)).row();
//    return res;
//}

void SkyProxyModel::loadTest() {
    QStringList l = QString("Psycho,Lever,Mace,Green").split(",");
    for (int i = 0; i < 4; i++) {
        QVariantMap m;
        m["Name"] = l.at(i);
        model_impl()->append(m);
    }
}

void SkyProxyModel::loadSkypeTest() {
    Q_EMIT instigateLoad(QVariantMap());
}

void SkyProxyModel::handleLoadedData(const QVariantMap &msg) {
    model_impl()->append(msg);
    m_progress = msg.value("Percent").toInt();
    emit loadProgressChanged();
}
