#include "skyproxymodel.h"
#include "skymodel.h"
#include <QtCore>
#include <QTextDocument>

#include "litesql.hpp"
#include "main.hpp"

using namespace litesql;
using namespace SkypeDB;

SkyProxyModel::SkyProxyModel(QObject *parent)
    :QSortFilterProxyModel(parent)
{
    setSourceModel(new SkyModel);
//    sort(0);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
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
    try {
        SkypeDB::main db("sqlite3", "database=/home/ilia/.Skype/luxa_ryabic/main.db");
        // create tables, sequences and indexes
        db.verbose = true;
        auto ds = select<Messages>(db);
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
            model_impl()->append(m);
        }
    } catch (Except e) {
        std::cerr << "Error: " << e << std::endl;
    }
}
