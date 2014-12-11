#include "customqsqltablemodel.h"
#include <QtGui/QColor>
#include <QtGui/QTimeEdit>
#include <QDebug>

CustomQSqlTableModel::CustomQSqlTableModel(QObject* parent): QSqlTableModel(parent)
{
}

QVariant CustomQSqlTableModel::data(const QModelIndex& idx, int role) const
{
  QVariant value = QSqlTableModel::data(idx, role);
  if (value.isValid() && role == Qt::DisplayRole) {
    if (idx.column() == 1) { // tracknumber: Add 0 to make single digit tracknumber a 2 digit tracknumber.
      if (value.toInt() == 0) {return "";}
      if (value.toInt() < 10) {return value.toString().prepend("0");}
    }
    if (idx.column() == 8) { // length: Convert seconds int into a time format.
      if (value.toInt() > 0) {
        int mins = (value.toInt() / 60);
	int secs = (value.toInt() - (mins * 60));
	QString secsStr;
	if (secs > 9) {
	  secsStr = QString::number(secs);
	}else{secsStr = QString::number(secs).prepend("0");}	
	QString combine = QString::number(mins) + ":" + secsStr;
	return combine;
      }
    }
    if (idx.column() == 9) { // Size: Convert bytes to mb or kb.
      if (value.toInt() > 0) {  
      int kiloB = (value.toInt() / 1024);
      if (kiloB < 1024) {return QString::number(kiloB) + " kb";}
      int MegaB = (kiloB / 1024);
      int MegaBremain = (kiloB % 1024);
      QString strMBremain;
      if (MegaBremain > 9) {strMBremain = QString::number(MegaBremain).left(1);
      }else{strMBremain = QString::number(MegaBremain).prepend("0");}
      return QString::number(MegaB) + "." + strMBremain + " MB";
      }
    }
    if (idx.column() == 11) { // Path: Chop off music directory. TODO: When making this app public. decide if we should chop music path, and get music path from MD class.
      QString pathChop = value.toString();
      QString MusicPath = "/home/randy/Music/";
      pathChop.remove(0, MusicPath.size());
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
    if (MissingColData.toBool()) {return qVariantFromValue(QColor(Qt::red));} // If file is missing, make it red.
  }
  return value;
}

