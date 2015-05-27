#include "skyproxymodel.h"
#include "skymodel.h"
#include <QtCore>
#include <QTextDocument>
#include <QFontMetrics>
#include <QRect>
#include <QtGui>

#include "litesql.hpp"
//#include "main.hpp"

using namespace litesql;
using namespace SkypeDB;

QString SkyProxyModel::s_dbPath = "/home/ilia/.Skype/luxa_ryabic/main.db";

SkyDataLoader::SkyDataLoader()
{
    QString dbPath = QString("database=%1").arg(SkyProxyModel::s_dbPath);
    m_db.reset(new SkypeDB::main("sqlite3", dbPath.toStdString()));
    m_db->verbose = true;
}

void SkyDataLoader::process_msg(const QVariantMap &msg){
    allMessages(msg);
}

void SkyDataLoader::calcMessagesFromToId(int from, int to) {
    QHash<QString, QString>chats_hash;
    auto cursor = select<Chats>(*m_db).cursor();
    for (;cursor.rowsLeft();cursor++) {
        Chats ch = *cursor;
        chats_hash[QString::fromStdString(ch.name)] = QString::fromStdString(ch.friendlyname);
    }

    const int step = 500;
    for (int i = from; i >= to; i-=step) {
        QString expr = QString("OID > %1 AND OID <= %2").arg(std::max(i - step, to)).arg(i);
        auto ds = select<Messages>(*m_db, RawExpr(expr.toLocal8Bit().data())).orderBy(Messages::Timestamp, false);
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
            double status_move = (from - i) << offset;
            double percentage = status_move / from;
            int status_res = static_cast<int>(percentage * 100) >> offset;
            m["Percent"] = status_res;
            m["Timestamp"] = QDateTime::fromTime_t(message.timestamp).toString();
            m["Author"] = QString::fromStdString(message.author);
            m["Chatname"] = chats_hash.value(QString::fromStdString(message.chatname));

            emit send_msg(m);
        }
    }
    QVariantMap m_finished;
    m_finished["maxId"] = from;
    emit send_finished(m_finished);
}

void SkyDataLoader::allMessages(const QVariantMap &) {
    try {
        //Calculate max id
        SelectQuery q;
        q.result("max(id)");
        q.source(SkypeDB::Messages::table__);
        int count = atoi(m_db->query(q)[0][0]);
        calcMessagesFromToId(count, 0);
        Q_EMIT can_start_watcher();
    } catch (Except e) {
        std::cerr << "Error: " << e << std::endl;
    }
}

void SkyDataLoader::chatMessages(const QVariantMap &) {
    try {
        ;
    } catch (Except e) {
        std::cerr << "Error: " << e << std::endl;
    }
}

void SkyDataLoader::MessagesDataSources(const QVariantMap &) {
}

void SkyDataLoader::processNewMsg(const QVariantMap &msg) {
    qDebug() << "Process new msg";
    int maxId = msg.value("maxId").toInt();
    if (maxId <= 0) {
        return;
    }
    //Calculate max id
    SelectQuery q;
    q.result("max(id)");
    q.source(SkypeDB::Messages::table__);
    int maxIdDb = atoi(m_db->query(q)[0][0]);
    if (maxId == maxIdDb) { //Nothing new
        return;
    }
    qDebug() << "Maybe really new msg";
    calcMessagesFromToId(maxIdDb, maxId);
    QVariantMap m;
    m["max_id"] = maxIdDb;
    emit send_finished(m);
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
    connect(skLoader, &SkyDataLoader::can_start_watcher, this, &SkyProxyModel::startWatcher);
    connect(skLoader, &SkyDataLoader::send_finished, this, &SkyProxyModel::processLoadFinished);
    connect(this, &SkyProxyModel::checkNewMessages, skLoader, &SkyDataLoader::processNewMsg);
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

void SkyProxyModel::startWatcher() {
    m_watcher.addPath(s_dbPath);
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &SkyProxyModel::processChangedFile);
}

void SkyProxyModel::processChangedFile(const QString &) {
    //TODO implement for different accounts
    qDebug() << "Notify changed";
    QVariantMap m;
    m["maxId"] = m_maxId;
    emit checkNewMessages(m);
}

void SkyProxyModel::processLoadFinished(const QVariantMap &msg) {
    if (m_maxId == -1 || msg.contains("maxId")) {
        if (m_mutex.tryLock()) {
            m_maxId = msg.value("maxId").toInt();
            m_mutex.unlock();
        }
    }
}
