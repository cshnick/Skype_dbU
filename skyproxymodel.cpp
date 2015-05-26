#include "skyproxymodel.h"
#include "skymodel.h"
#include <QtCore>
#include <QTextDocument>
#include <QFontMetrics>
#include <QRect>
#include <QtGui>

#include "litesql.hpp"
#include "main.hpp"

using namespace litesql;
using namespace SkypeDB;

void SkyDataLoader::process_msg(const QVariantMap &msg){
    chatMessages(msg);
}

void SkyDataLoader::allMessages(const QVariantMap &) {
    try {
        SkypeDB::main db("sqlite3", "database=/home/ilia/.Skype/sc.ryabokon.ilia/main.db");
        // create tables, sequences and indexes
        db.verbose = false;

        //Calculate max id
        SelectQuery q;
        q.result("max(id)");
        q.source(SkypeDB::Messages::table__);
        int count = atoi(db.query(q)[0][0]);

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
                QFontMetrics metrx(fnt);
                int ofs = metrx.boundingRect("H").height();


                QVariantMap m;
                m["Name"] = doc.toPlainText();
                m["Height"] = doc.size().height() + ofs;
                //
                int offset = 9;
                double status_move = i << offset;
                double percentage = status_move / count;
                int status_res = static_cast<int>(percentage * 100) >> offset;
                m["Percent"] = status_res;
                m["Timestamp"] = QDateTime::fromTime_t(message.timestamp).toString();
                m["Author"] = QString::fromStdString(message.author);

                emit send_msg(m);
            }
        }
        emit send_finished();

    } catch (Except e) {
        std::cerr << "Error: " << e << std::endl;
    }
}

void SkyDataLoader::chatMessages(const QVariantMap &) {
    try {
        SkypeDB::main db("sqlite3", "database=/home/ilia/.Skype/sc.ryabokon.ilia/main.db");
        // create tables, sequences and indexes
        db.verbose = true;
        auto dsch = litesql::select<Chats>(db);
        qDebug() << "Chats count" << dsch.count();

        auto chatsDS = litesql::select<Chats>(db);
        int chatsCount = chatsDS.count();
        auto chatsCursor = chatsDS.cursor();

        int progress_counter = 0;
        for (;chatsCursor.rowsLeft();chatsCursor++) {
            Chats chat = *chatsCursor;
            std::string display_name = chat.friendlyname;
            std::string chat_name = chat.name;
            qDebug() << "Friendly name" << chat_name.c_str();

            auto chatMessagesDS = select<Messages>(db, Messages::Chatname == chat_name);
            qDebug() << "chm count " <<  chatMessagesDS.count();
            auto chatMessagesCursor = chatMessagesDS.cursor();


            for (;chatMessagesCursor.rowsLeft();chatMessagesCursor++) {
                Messages message = *chatMessagesCursor;

                QString body = QString::fromStdString(message.body_xml);
                QTextDocument doc(body);
                QFont fnt = doc.defaultFont();
                fnt.setPointSize(10);
                doc.setDefaultFont(fnt);
                doc.adjustSize();
                QFontMetrics metrx(fnt);
                int ofs = metrx.boundingRect("H").height();

                QVariantMap m;
                m["Name"] = doc.toPlainText();
                m["Height"] = doc.size().height() + ofs;
                //
                int offset = 9;
                double status_move = progress_counter << offset;
                double percentage = status_move / chatsCount;
                int status_res = static_cast<int>(percentage * 100) >> offset;
                m["Percent"] = status_res;
                m["Timestamp"] = QDateTime::fromTime_t(message.timestamp).toString();
                m["Author"] = QString::fromStdString(message.author);
                m["Chatname"] = QString::fromStdString(display_name);

                emit send_msg(m);
                progress_counter ++;
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
    QString author = ind.data(Qt::UserRole + 3).toString();

    bool accept = name_data.contains(filterRegExp()) ||
            author.contains(filterRegExp());
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
