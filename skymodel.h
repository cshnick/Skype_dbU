#ifndef SKYMODEL_H
#define SKYMODEL_H

#include <QObject>
#include <QAbstractListModel>

class SkyModelPrivate;
class SkyModel : public QAbstractListModel
{
public:
    SkyModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    void append(const QVariantMap &p_data);
    void insert(int index, const QVariantMap &p_data);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    friend class SkyModelPrivate;
    SkyModelPrivate *p;
    void emitDataChanged(const QModelIndex &start, const QModelIndex &end);
};

#endif // SKYMODEL_H
