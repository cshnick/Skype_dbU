#include "skymodel.h"
#include <QtGui>

static std::vector<QString>g_roles = {"Name",
                                      "Height",
                                      "Timestamp",
                                      "Author",
                                      "Chatname"
                                     };

class SkyModelPrivate {
    friend class SkyModel;
public:
    SkyModelPrivate(SkyModel *p_q) : q(p_q) {}

    int rowCount(const QModelIndex &parent) const {Q_UNUSED(parent); return m_elements.count();}
    QVariant data(const QModelIndex &index, int role) const {
        Q_UNUSED(role)
        int rw = index.row();
        if (rw < 0 || rw > m_elements.count()) {
            return QVariant();
        }
        QVariantMap dta = m_elements.at(index.row());
//        if (role == s_fields.count() + Qt::UserRole + 1) {
//            return dta;
//        }
        return dta.value(g_roles[role - Qt::UserRole]);
    }

    QHash<int, QByteArray> roleNames() const {
        QHash<int, QByteArray> roles;
        for (size_t i = 0; i < g_roles.size(); i++) {
            QString str = g_roles.at(i);
            roles[Qt::UserRole + i] = str.toLocal8Bit();
        }

        return roles;
    }

    void append(const QVariantMap &p_data) {
        q->beginInsertRows(QModelIndex(), m_elements.count(), m_elements.count());
        m_elements.append(p_data);
        q->endInsertRows();
    }

    void insert(int index, const QVariantMap &p_data) {
        q->beginInsertRows(QModelIndex(), index, index);
        m_elements.insert(index, p_data);
        q->endInsertRows();
    }
    void update(int index, const QVariantMap &p_data) {
        if (index < 0 || index >= m_elements.count()) {
            return;
        }
        QVariantMap &mp = m_elements[index];
        for (auto iter = p_data.begin(); iter != p_data.end(); ++iter) {
            mp.insert(iter.key(), iter.value());
        }
    }
    QVariantMap remove(int index) {
        q->beginRemoveRows(QModelIndex(), index, index);
        QVariantMap dct = m_elements.at(index);
        m_elements.removeAt(index);
        q->endRemoveRows();

        return dct;
    }
    void clear() {
        if (!m_elements.count()) {
            return;
        }

        q->beginRemoveRows(QModelIndex(), 0, m_elements.count() - 1);
        m_elements.clear();
        q->endRemoveRows();
    }

private:
    SkyModel *q;
    QList<QVariantMap> m_elements;
};

SkyModel::SkyModel(QObject *parent)
    : QAbstractListModel(parent)
    , p(new SkyModelPrivate(this))
{

}

int SkyModel::rowCount(const QModelIndex &parent) const {
    return p->rowCount(parent);
}
QVariant SkyModel::data(const QModelIndex &index, int role) const {
    return p->data(index, role);
}

void SkyModel::append(const QVariantMap &p_data) {
    p->append(p_data);
}

void SkyModel::insert(int index, const QVariantMap &p_data) {
    p->insert(index, p_data);
}


QHash<int, QByteArray> SkyModel::roleNames() const {
    return p->roleNames();
}

