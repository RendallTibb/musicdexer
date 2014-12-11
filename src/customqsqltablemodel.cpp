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

#include "customqsqltablemodel.h"
#include <QColor>

CustomQSqlTableModel::CustomQSqlTableModel(QObject *parent) :
    QSqlTableModel(parent)
{
    mpsize = 0;
}

QVariant CustomQSqlTableModel::data(const QModelIndex& idx, int role) const
{
    QVariant value = QSqlTableModel::data(idx, role);
    if (value.isValid() && role == Qt::DisplayRole) {
        if (idx.column() == 1) { // tracknumber: Add 0 to make single digit tracknumber a 2 digit tracknumber.
            if (value.toInt() == 0) {return "";}
            if (value.toInt() < 10) {return value.toString().prepend("0");}
        } // May want to make a 3 digit track number?
        if (idx.column() == 8) { // length: Convert seconds into a time format.
            if (value.toInt() > 0) {
                int mins = (value.toInt() / 60);
                int secs = (value.toInt() - (mins * 60));
                QString secsStr;
                if (secs > 9) {
                    secsStr = QString::number(secs);
                }else{
                   secsStr = QString::number(secs).prepend("0");
                }
                QString combine = QString::number(mins) + ":" + secsStr;
                return combine;
            }// May want to add hours conversion?
        }
        if (idx.column() == 9) { // Size: Convert bytes to mb or kb.
            if (value.toInt() > 0) {
                int kiloB = (value.toInt() / 1024);
                if (kiloB < 1024) {
                    return QString::number(kiloB) + " kb";
                }
                int MegaB = (kiloB / 1024);
                int MegaBremain = (kiloB % 1024);
                QString strMBremain;
                if (MegaBremain > 9) {
                    strMBremain = QString::number(MegaBremain).left(1);
                }else{
                    strMBremain = QString::number(MegaBremain).prepend("0");
                }
                return QString::number(MegaB) + "." + strMBremain + " MB";
            }// May want to add gb? Unaccounted for wave file :)
        }
        if (idx.column() == 11) { // Path: Chop off music directory. My preference, not everyones?
            QString pathChop = value.toString();
            pathChop.remove(0, mpsize + 2);
            return pathChop;
        }
    }
    if (role == Qt::TextAlignmentRole) {
        if (idx.column() == 1 || idx.column() == 8 || idx.column() == 9) {
            return Qt::AlignRight;
        }
    }
    if (role == Qt::TextColorRole) {
        QVariant MissingColData = idx.sibling(idx.row(), 12).data(); // Get data in column "Missing"
        if (MissingColData.toBool()) { // If file is missing, make it red.
            return qVariantFromValue(QColor(Qt::red));
        }
    }
    return value;
}

Qt::ItemFlags CustomQSqlTableModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags flag = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    int c = index.column();
    if (c == 8 || c == 9 || c == 11 || c == 12) {
        return flag;
    }
    return flag | Qt::ItemIsEditable;
}

void CustomQSqlTableModel::setPathSize(int psize)
{
#ifdef Q_OS_WIN32
    mpsize = psize - 1;
#else
    mpsize = psize - 2;
#endif
}
