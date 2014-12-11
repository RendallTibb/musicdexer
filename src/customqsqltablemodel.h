/* This file is part of Musicdexer.
 * Copyright (C) 2012-2014 Rendall Tibbetts
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CUSTOMQSQLTABLEMODEL_H
#define CUSTOMQSQLTABLEMODEL_H

#include <QSqlTableModel>

class CustomQSqlTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit CustomQSqlTableModel(QObject *parent = 0);
    QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const;
    Qt::ItemFlags  flags(const QModelIndex & index) const;
    void setPathSize(int psize);

signals:

public slots:

private:
    int mpsize;

};

#endif // CUSTOMQSQLTABLEMODEL_H
