#ifndef musicdexer_H
#define musicdexer_H
#include <QStandardItemModel>
#include "ui_musicdexer.h"
#include <QSqlDatabase>
#include "customqsqltablemodel.h"
#include <QSqlRecord>
//#include <QSqlTableModel>

class Musicdexer : public QMainWindow
{
  Q_OBJECT

public:
    Musicdexer();
    virtual ~Musicdexer();

private slots:
//    void dirSelect();
    void Backup();
    void Sync();
    void userSearch();
    void userClear();
    void userDelete();
    void userAddto();
    void userChange();
    void userDirSelect(QModelIndex);
    void dbedit(int, QSqlRecord&);
    void FixScroll();
    
private:
 
    Ui::Musicdexer ui;
    QSqlDatabase MDDB;
    QString musicPath;
    CustomQSqlTableModel *dbmodel;
    int scrollPos;
    // The following is used to update the Browser treeview.
    void populateBrowser();
    void recursDirList(QStandardItem*, QString);
    QStandardItemModel *browseModel;
    QStringList browsePaths;
};

#endif // musicdexer_H
