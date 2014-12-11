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

#ifndef MUSICDEXER_H
#define MUSICDEXER_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QStandardItem>
#include <QStandardItemModel>
#include "customqsqltablemodel.h"
#include <QSettings>

namespace Ui {
class Musicdexer;
}

class Musicdexer : public QMainWindow
{
    Q_OBJECT

public:
    explicit Musicdexer(QWidget *parent = 0);
    ~Musicdexer();

private slots:
    void on_pbtnSearch_clicked();

    void on_pbtnClear_clicked();

    void on_pbtnDelete_clicked();

    void on_pbtnAddto_clicked();

    void on_pbtnChange_clicked();

    void dbedit(int, QSqlRecord&);

    void on_treeView_doubleClicked(const QModelIndex &index);

    void on_actionSync_triggered();

    void on_actionDirSelect_triggered();


private:
    Ui::Musicdexer *ui;
    QString musicpath;
    QSqlDatabase MDDB;
    CustomQSqlTableModel *dbmodel;
    bool dbmodelon;
    QSettings *settings;
    void dirSelect();
    // The following is used to update the browser treeview.
    void populateBrowser();
    void recursDirList(QStandardItem*, QString);
    QStandardItemModel *browseModel;
    QStringList browsePaths;
};

#endif // MUSICDEXER_H
