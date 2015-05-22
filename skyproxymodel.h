#ifndef SKYPROXYMODEL_H
#define SKYPROXYMODEL_H

#include <QSortFilterProxyModel>

class SkyModel;

class SkyProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SkyProxyModel(QObject *parent = 0);


    Q_INVOKABLE void stringChanged(const QString &p_str);
    Q_INVOKABLE QVariant get(int p_index, int role);
    Q_INVOKABLE QString get_name(int p_index);
    Q_INVOKABLE void refresh();
    //Q_INVOKABLE int indexFromCode(const QString &code);
    //Q_INVOKABLE QStringList parserNames() const;

    Q_INVOKABLE void loadTest();
    Q_INVOKABLE void loadSkypeTest();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    SkyModel *model_impl() {
        return reinterpret_cast<SkyModel*> (this->sourceModel());
    }
    SkyModel *model_impl() const {
        return reinterpret_cast<SkyModel*> (this->sourceModel());
    }
};

#endif // SKYPROXYMODEL_H
