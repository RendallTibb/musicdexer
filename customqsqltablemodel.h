#ifndef CUSTOMQSQLTABLEMODEL_H
#define CUSTOMQSQLTABLEMODEL_H

#include <QSqlTableModel>


class CustomQSqlTableModel : public QSqlTableModel
{

public:
    CustomQSqlTableModel(QObject* parent = 0);
    QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const;
};

#endif // CUSTOMQSQLTABLEMODEL_H
