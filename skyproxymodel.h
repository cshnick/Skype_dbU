#ifndef SKYPROXYMODEL_H
#define SKYPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QThread>

class SkyModel;

class SkyDataLoader : public QObject
{
    Q_OBJECT

public:
    Q_SLOT void process_msg(const QVariantMap &msg);
    Q_SIGNAL void send_msg(const QVariantMap &msg);
    Q_SIGNAL void send_finished();
};

class SkyProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int loadProgress READ loadProgress WRITE setLoadProgress NOTIFY loadProgressChanged)

public:
    explicit SkyProxyModel(QObject *parent = 0);
    ~SkyProxyModel();

    int loadProgress() const {return m_progress;}
    void setLoadProgress(int val) {m_progress = val;}

    Q_INVOKABLE void stringChanged(const QString &p_str);
    Q_INVOKABLE QVariant get(int p_index, int role);
    Q_INVOKABLE QString get_name(int p_index);
    Q_INVOKABLE void refresh();
    //Q_INVOKABLE int indexFromCode(const QString &code);
    //Q_INVOKABLE QStringList parserNames() const;

    Q_INVOKABLE void loadTest();
    Q_INVOKABLE void loadSkypeTest();

    Q_SIGNAL void instigateLoad(const QVariantMap &msg);
    Q_SIGNAL void loadProgressChanged();
    Q_SIGNAL void loadFinished();
    Q_SLOT void handleLoadedData(const QVariantMap &msg);

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
    QThread m_worker;
    int m_progress = 0;
};

#endif // SKYPROXYMODEL_H
