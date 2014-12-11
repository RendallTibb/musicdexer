#include "musicdexer.h"
#include "customqsqltablemodel.h"
#include <qstandarditemmodel.h>
//#include <QSqlTableModel>
#include <QSqlQuery>
//#include <QFileDialog>
#include <QFileSystemModel>
#include <QDebug>
//#include <QDirModel>
#include <QFileInfo>
#include <QSqlRecord>
#include <QStringList>
#include <QMessageBox>
#include <qscrollbar.h>
#include <taglib/tag.h>
#include <taglib/fileref.h>
using namespace std;

Musicdexer::Musicdexer()
{
  musicPath = "/home/randy/Music/";
  ui.setupUi(this); // Use GUI described in ui_musicdexer created by uic using musicdexer.ui created by qtdesigner.
  // Populate Combo boxes in gui.
  QStringList strlFields, strlOperators;
  strlFields << "tracknumber" << "title" << "album" << "artist" << "genre" << "mood" << "rating" << "length" << "size" << "mycomment" << "path" << "missing";
  strlOperators << "Search" << "LIKE" << "=" << "!" << ">" << "<" << ">=" << "<=";
  ui.cboxField->addItems(strlFields);
  ui.cboxField->setCurrentIndex(2);
  ui.cboxOperator->addItems(strlOperators);
  ui.cboxFieldE->addItems(strlFields);
  ui.cboxFieldE->setCurrentIndex(9);
  // Ready application to receive button clicks.
  connect(ui.pbtnSearch, SIGNAL(clicked()), this, SLOT(userSearch()));
  connect(ui.lieditSearch, SIGNAL(returnPressed()), this, SLOT(userSearch()));
  connect(ui.pbtnClear, SIGNAL(clicked()), this, SLOT(userClear()));
  connect(ui.pbtnDelete, SIGNAL(clicked()), this, SLOT(userDelete()));
  connect(ui.pbtnAddto, SIGNAL(clicked()), this, SLOT(userAddto()));
  connect(ui.pbtnChange, SIGNAL(clicked()), this, SLOT(userChange()));
  //  connect(ui.actionDir_Select, SIGNAL(triggered()), this, SLOT(dirSelect()));
  connect(ui.actionBackup, SIGNAL(triggered()), this, SLOT(Backup()));
  connect(ui.actionSync, SIGNAL(triggered()), this, SLOT(Sync()));
  // Try to connect to DB and populate tableview and treeview.
  const QString dbhost = "";
  const QString dbname = "";
  const QString dbuser = "";
  const QString dbpass = "";
  MDDB = QSqlDatabase::addDatabase( "QMYSQL" );
  MDDB.setHostName( dbhost );
  MDDB.setUserName( dbuser );
  MDDB.setPassword( dbpass );
  if (!MDDB.open()) {
    ui.statusBar->showMessage("Unable to connect to the SQL server", 10000);
  }else{
    MDDB.setDatabaseName( dbname );
    if (!MDDB.open()) {
      ui.statusBar->showMessage("Could not open database: " + dbname, 10000);
    }else{
// Setup tableView (music database)
      dbmodel = new CustomQSqlTableModel(this);
      dbmodel->setTable("tracks");
      dbmodel->setEditStrategy(QSqlTableModel::OnFieldChange);
      dbmodel->select();
      dbmodel->setHeaderData(0, Qt::Horizontal, tr("id"));
      dbmodel->setHeaderData(1, Qt::Horizontal, tr("Track"));
      dbmodel->setHeaderData(2, Qt::Horizontal, tr("Title"));
      dbmodel->setHeaderData(3, Qt::Horizontal, tr("Album"));
      dbmodel->setHeaderData(4, Qt::Horizontal, tr("Artist"));
      dbmodel->setHeaderData(5, Qt::Horizontal, tr("Genre"));
      dbmodel->setHeaderData(6, Qt::Horizontal, tr("Mood"));
      dbmodel->setHeaderData(7, Qt::Horizontal, tr("Rate"));
      dbmodel->setHeaderData(8, Qt::Horizontal, tr("Length"));
      dbmodel->setHeaderData(9, Qt::Horizontal, tr("Size"));
      dbmodel->setHeaderData(10, Qt::Horizontal, tr("MyComment"));
      dbmodel->setHeaderData(11, Qt::Horizontal, tr("Path"));
      dbmodel->setHeaderData(12, Qt::Horizontal, tr("Missing"));
      ui.tableView->setModel(dbmodel);
      ui.tableView->setEditTriggers(QTableView::DoubleClicked | QTableView::SelectedClicked | QTableView::EditKeyPressed);
      ui.tableView->setAutoScroll(QTableView::PositionAtCenter);
      ui.tableView->verticalHeader()->hide();
      ui.tableView->hideColumn(0);
      ui.tableView->horizontalHeader()->resizeSection(1, 35); //track
      ui.tableView->horizontalHeader()->resizeSection(2, 160); //title
      ui.tableView->horizontalHeader()->resizeSection(3, 160); //album
      ui.tableView->horizontalHeader()->resizeSection(4, 100); //artist
      ui.tableView->horizontalHeader()->resizeSection(5, 50); //genre
      ui.tableView->horizontalHeader()->resizeSection(6, 100); //mood
      ui.tableView->horizontalHeader()->resizeSection(7, 30); //rating
      ui.tableView->horizontalHeader()->resizeSection(8, 45); //length
      ui.tableView->horizontalHeader()->resizeSection(9, 55); //size
      ui.tableView->hideColumn(12); // Hide column 'missing' since missing files are colored red anyway.
      ui.tableView->horizontalHeader()->setStretchLastSection(true); // TODO issue where you can't see the whole path. It wont let you expand the size of the field off the table.
      ui.statusBar->showMessage("Database " + dbname + " opened successfuly", 10000);
      connect(dbmodel, SIGNAL(beforeUpdate(int,QSqlRecord&)), this, SLOT(dbedit(int,QSqlRecord&)));
// Now for the treeview (browser: for displaying music by directory instead of a search. or combine both.)
      browseModel = new QStandardItemModel(this);
      populateBrowser();
      //ui.treeView->setHeaderHidden(true); // Could make more room by removing header but theres enough space with scroll bar and user should know what the box does.

      connect(ui.treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(userDirSelect(QModelIndex)));
// After single data change, view scrolls to top. Prevented by taking scroll pos befor data change and updated on dataChange signal. NOTE: Slower system will loose position? If so, add a timer.
      connect(dbmodel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(FixScroll()));
      
      // P L A Y G R O U N D (I test stuff here)
//      QSqlQuery testQuery;
//      testQuery.prepare("SELECT title FROM tracks WHERE missing = 0 AND CHAR_LENGTH(path) = 78 OR SUBSTRING_INDEX(path, '/', -1) = 'Juno Reactor [Shango 03] - Insects.mp3'");
//      testQuery.addBindValue(1);
//      testQuery.exec();
//      if (testQuery.next()) { qDebug() << testQuery.value(0).toString(); }
//      if (1 < 1) {qDebug() << "Hiya";}
   /* TODO implement Right click remove record, example reimplement of qwidget contextMenuEvent:
   void MyTableView::contextMenuEvent(QContextMenuEvent* event)
   {
   QModelIndex index = indexAt(event->pos());
   if (index.isValid())
   {
   // ...
   }
   }*/
    }
  }
}

Musicdexer::~Musicdexer()
{
  MDDB.close();
  MDDB.removeDatabase(MDDB.connectionName());
}

void Musicdexer::userSearch()
{
  if (!ui.lieditSearch->text().isEmpty()) {
    QString strFilter;
    if (ui.cboxOperator->currentIndex() == 0) {
      strFilter = ui.cboxField->currentText() + " " + "LIKE" + " '%" + ui.lieditSearch->text().replace("'", "\\'") + "%'";
    }else{
      strFilter = ui.cboxField->currentText() + " " + ui.cboxOperator->currentText() + " '" + ui.lieditSearch->text() + "'";
    }
    qDebug() << strFilter;
    dbmodel->setFilter(strFilter);
  }
}

void Musicdexer::userClear()
{
  dbmodel->setFilter(""); // This 'should' refresh the database but not always?
  dbmodel->select(); // So manualy refresh database.
}

void Musicdexer::userDelete()
{
  int numOfRec = dbmodel->query().size();
  QMessageBox msgDelete;
  msgDelete.setText("Prepared to delete " + QString::number(numOfRec) + " songs from the database.");
  msgDelete.setInformativeText("Are you sure you want to delete them?");
  msgDelete.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgDelete.setDefaultButton(QMessageBox::No);
  int delAnswer = msgDelete.exec();
  if (dbmodel->filter().isEmpty() && (delAnswer == QMessageBox::Yes)) {
    msgDelete.setText("WARNING! This will delete the entire database.");
    msgDelete.setInformativeText("Are you SUPER sure?");
    delAnswer = msgDelete.exec();
  }else if (numOfRec > 100 && (delAnswer == QMessageBox::Yes)) {
    msgDelete.setText("That's " + QString::number(numOfRec) + " songs to be DELETED from the database.");
    msgDelete.setInformativeText("Are you sure?");
    delAnswer = msgDelete.exec();
  }
  if (delAnswer == QMessageBox::Yes) {
    QSqlQuery deleteQuery;
    if (dbmodel->filter().isEmpty()) {
      deleteQuery.exec("DELETE FROM tracks");
      deleteQuery.exec("ALTER TABLE tracks AUTO_INCREMENT = 0");
      dbmodel->select();
    }else{
      deleteQuery.exec("DELETE FROM tracks WHERE " + dbmodel->filter());
      dbmodel->select();
    }
  }
}

void Musicdexer::userAddto()
{
  QString addString = ui.lieditUpdate->text();
  QString updtField = ui.cboxFieldE->currentText();
  if (!addString.isEmpty() || updtField != "tracknumber" || updtField != "rating" || updtField != "length" || updtField != "size" || updtField != "mycomment" || updtField != "path" || updtField != "missing") {
    int numOfRec = dbmodel->query().size();
    QMessageBox msgUpdate;
    msgUpdate.setText("All " + QString::number(numOfRec) + " songs will have '" + addString + "' added to their " + updtField + ".");
    msgUpdate.setInformativeText("Are you sure?");
    msgUpdate.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgUpdate.setDefaultButton(QMessageBox::No);
    int updAnswer = msgUpdate.exec();
    if (numOfRec > 100 && updAnswer == QMessageBox::Yes) {
      msgUpdate.setText(QString::number(numOfRec) + " songs will be changed.");
      msgUpdate.setInformativeText("Are you STILL sure?");
      updAnswer = msgUpdate.exec();
    }
    if (updAnswer == QMessageBox::Yes) {
      QSqlQuery addQuery;
      addQuery.setForwardOnly(true);
      QString strCurSearch = dbmodel->query().lastQuery();
      // Update tags. Do this befor db update; otherwise, there will be no results from the query to update files.
      if (updtField == "title" || updtField == "album" || updtField == "artist" || updtField == "genre") {
        addQuery.exec(strCurSearch);
        QString updtPath;
        while (addQuery.next()) {
	  updtPath = addQuery.value(11).toString();
          TagLib::FileRef u(updtPath.toUtf8());
          if (!u.isNull()) { // File exists and is tag writable
	    QString addToTag = ";" + addString;
            if (updtField == "title") { addToTag = u.tag()->title().toCString() + addToTag; u.tag()->setTitle(addToTag.toStdWString());
	    }else if (updtField == "album") { addToTag = u.tag()->album().toCString() + addToTag; u.tag()->setAlbum(addToTag.toStdWString());
	    }else if (updtField == "artist") { addToTag = u.tag()->artist().toCString() + addToTag; u.tag()->setArtist(addToTag.toStdWString());
	    }else if (updtField == "genre") { addToTag = u.tag()->genre().toCString() + addToTag; u.tag()->setGenre(addToTag.toStdWString());}
	    u.save();
	  }
        }
      }
      // Get the WHERE statment from the query that is used to show the current results and use it for the update.
      // if the whole database is shown, then there is no WHERE statment, so update entire DB.
      int numWhere = strCurSearch.indexOf(" WHERE ");
      if (numWhere == -1) {
        if (strCurSearch.indexOf(" FROM ") != -1) {
	  strCurSearch = "";
        }
      }else{numWhere = strCurSearch.size() - numWhere;}
      addString = "'" + addString + "'";
      QString strCustQuery = "UPDATE tracks SET " + updtField + " = CONCAT_WS(';', " + updtField + ", " + addString + ")" + strCurSearch.right(numWhere);
      addQuery.exec(strCustQuery);
      if (updtField == ui.cboxField->currentText()) {
        ui.lieditSearch->setText(addString);
      }
    if (strCurSearch.indexOf("%' && path NOT LIKE '") == -1) { 
      userSearch(); // User used Search by Field.
    }else{dbmodel->select();} // User used Search by Directory.
    }
  }
}

void Musicdexer::userChange()
{
  int numOfRec = dbmodel->query().size();
  QString strEmpty;
  if (ui.lieditUpdate->text().isEmpty()) {
        strEmpty = " Deleted.";
  }else{strEmpty = " changed to " + ui.lieditUpdate->text() + ".";}
  QMessageBox msgUpdate;
  msgUpdate.setText("All " + QString::number(numOfRec) + " songs will have their " + ui.cboxFieldE->currentText() + strEmpty);
  msgUpdate.setInformativeText("Are you sure?");
  msgUpdate.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgUpdate.setDefaultButton(QMessageBox::No);
  int updAnswer = msgUpdate.exec();
  if (numOfRec > 100 && updAnswer == QMessageBox::Yes) {
    msgUpdate.setText(QString::number(numOfRec) + " songs will be changed.");
    msgUpdate.setInformativeText("Are you STILL sure?");
    updAnswer = msgUpdate.exec();
  }
  if (updAnswer == QMessageBox::Yes) {
    QSqlQuery updtQuery;
    updtQuery.setForwardOnly(true);
    QString updtString = ui.lieditUpdate->text();
    QString updtField = ui.cboxFieldE->currentText();
    QString strCurSearch = dbmodel->query().lastQuery();
    // Update tags. Do this befor db update; otherwise, there will be no results from the query to update files.
    if (updtField == "tracknumber" || updtField == "title" || updtField == "album" || updtField == "artist" || updtField == "genre") {
      updtQuery.exec(strCurSearch);
      QString updtPath;
      while (updtQuery.next()) {
	updtPath = updtQuery.value(11).toString();
        TagLib::FileRef u(updtPath.toUtf8());
        if (!u.isNull()) { // File exists and is tag writable
          if (updtField == "tracknumber") { u.tag()->setTrack(updtString.toInt());
	  }else if (updtField == "title") { u.tag()->setTitle(updtString.toStdWString());
	  }else if (updtField == "album") { u.tag()->setAlbum(updtString.toStdWString());
	  }else if (updtField == "artist") { u.tag()->setArtist(updtString.toStdWString());
	  }else if (updtField == "genre") { u.tag()->setGenre(updtString.toStdWString());}
	  u.save();
	}
      }
    }
    // Get the WHERE statment from the query used to show the current results to use for the update.
    // if the whole database is shown, then there is no WHERE statment, so update entire DB.
    int numWhere = strCurSearch.indexOf(" WHERE ");
    if (numWhere == -1) { // where is not there.
      if (strCurSearch.indexOf(" FROM ") != -1) { // This should always be there. But just in case, added the if statement.
	strCurSearch = "";
      }
    }else{numWhere = strCurSearch.size() - numWhere;}
    if (updtString.isEmpty()) {updtString = "NULL";}else{updtString = "'" + updtString.replace("'", "\\'") + "'";}
    QString strCustQuery = "UPDATE tracks SET " + updtField + " = " + updtString + strCurSearch.right(numWhere);
    updtQuery.exec(strCustQuery);
    // Update finished, now refresh the search so it shows the updated results.
    if (updtField == ui.cboxField->currentText()) {
      ui.lieditSearch->setText(ui.lieditUpdate->text());
    }
    if (strCurSearch.indexOf("%' && path NOT LIKE '") == -1) { 
      userSearch(); // User used Search by Field.
    }else{dbmodel->select();} // User used Search by Directory.
  }
}

/*
void Musicdexer::dirSelect()
{
  QFileDialog *dialog = new QFileDialog(this, tr("Select a music directory"), QDir::homePath() + "/Music", tr("All Files")); //Did it this way to change the caption.
  dialog->setFileMode(QFileDialog::Directory); //Bug in my qt version. Supposed to show Directories AND files, but only shows Directories.
  //()dialog.setNameFilter(tr("Music (*.mp3 *.ogg *.wma *.aac *.flac *.m4p *.m4a *.wav *.aiff *.aif *.aifc)"));
  if (dialog->exec()) {
    musicPath = dialog->selectedFiles().takeFirst();
  }
}
*/

void Musicdexer::dbedit(int row, QSqlRecord& rec)
{
  // Don't allow edit of length, size, path, missing,
  for (int r=8; r < 12; r++) {
    if (r != 10) { // mycomment can be edited.
      if (!rec.value(r).isNull()) {rec.setValue(r, dbmodel->record(row).value(r));}
    }
  }  
  // Rating should be 1-10. Anything above 10 is changed to 10. 0 should be changed to NULL.
  if (!rec.value(7).isNull()) {
    float recRating = rec.value(7).toFloat();
    if (recRating < 1) {rec.setNull(7);
    }else if (recRating >= 10) {rec.setValue(7, 10);}
  }  
  // Update tag at same time as db if possilbe, and only if necessary.
  if (!(rec.value(1).isNull() && rec.value(2).isNull() && rec.value(3).isNull() && rec.value(4).isNull() && rec.value(5).isNull())) {
    bool savetag = false;
    TagLib::FileRef f(dbmodel->record(row).value(11).toString().toUtf8());
    int recTnum = rec.value(1).toInt();
    QString recTitle = rec.value(2).toString();
    QString recAlbum = rec.value(3).toString();
    QString recArtist = rec.value(4).toString();
    QString recGenre = rec.value(5).toString();
    if (!f.isNull()) {
      if (!rec.value(1).isNull() && f.tag()->track() != recTnum) {f.tag()->setTrack(recTnum); savetag=true;}
      if (!recTitle.isNull() && f.tag()->title().toCString() != recTitle && !(f.tag()->genre().isNull() && recGenre.isEmpty())) {
	f.tag()->setTitle(recTitle.toStdWString()); savetag=true;}
      if (!recAlbum.isNull() && f.tag()->album().toCString() != recAlbum && !(f.tag()->genre().isNull() && recGenre.isEmpty())) {
	f.tag()->setAlbum(recAlbum.toStdWString()); savetag=true;}
      if (!recArtist.isNull() && f.tag()->artist().toCString() != recArtist && !(f.tag()->genre().isNull() && recGenre.isEmpty())) {
	f.tag()->setArtist(recArtist.toStdWString()); savetag=true;}
      if ( !recGenre.isNull() && f.tag()->genre().toCString() != recGenre && !(f.tag()->genre().isNull() && recGenre.isEmpty())) {
	f.tag()->setGenre(recGenre.toStdWString()); savetag=true;}
      if (savetag) {
	if (!f.save()) {ui.statusBar->showMessage("ERROR UPDATING TAG", 10000);}
	else{  // filesize may change due to tag change so update it if need be.
	  QFileInfo fileSize(dbmodel->record(row).value(11).toString());
	  if (dbmodel->record(row).value(9).toInt() != fileSize.size()) {
	    rec.setValue(9, fileSize.size());
	  }
	  ui.statusBar->showMessage("Tag updated", 10000);
	}
      }
    }else{ui.statusBar->showMessage("Info: This file does not have a tag but the database has been updated", 10000);}
  }
  // Set empty values to null in db because null uses up less space? Also, when adding data to empty values, a separator(';') is stilll used.
  // ie, add 'creepy' to an empty value and it will change to ';creepy', Add it to a null value and it will be 'creepy'.
  for (int v = 2; v < 7; v++) {
    if (!rec.value(v).isNull() && rec.value(v).toString().isEmpty()) {rec.setNull(v);}
  }
  scrollPos = ui.tableView->verticalScrollBar()->value();
}

void Musicdexer::FixScroll()
{
  ui.tableView->verticalScrollBar()->setValue(scrollPos);
}

void Musicdexer::userDirSelect(QModelIndex MI)
{
  QString selectedDir;
  while (!MI.data().toString().isEmpty()) {
    selectedDir = MI.data().toString() + "/" + selectedDir;
    MI = MI.parent();
  }
  QString songsInDirAndSubs = musicPath + selectedDir + "%";
  QString minusSubs = songsInDirAndSubs + "/%";
  QString selectDirFilter = "path LIKE '" + songsInDirAndSubs + "' && path NOT LIKE '" + minusSubs + "'";
  QSqlQuery selectDirSql;
  dbmodel->setFilter(selectDirFilter);
}

void Musicdexer::populateBrowser()
{
  QSqlQuery browseQuery;
  browseQuery.setForwardOnly(true);
  browseQuery.exec("SELECT DISTINCT SUBSTRING_INDEX( LEFT(path, LENGTH(path) - LOCATE('/', REVERSE(path) ) ), '" + musicPath + "', -1) FROM tracks");
  while (browseQuery.next()) {browsePaths << browseQuery.value(0).toString();}  	   // browsePaths is populated with Directory paths from all paths in db.
  browsePaths.removeAt(browsePaths.indexOf(musicPath.left(musicPath.size() - 1)));  // SQL query returns root path(musicPath). Not needed, so removed.
  browseModel->clear();
  recursDirList(browseModel->invisibleRootItem(), "root");  //  Previouse lines were preporation for this function, which does the actual populating.
  QStringList browshead; browshead  << "Browse by Directory";
  browseModel->setHorizontalHeaderLabels(browshead);
  browseModel->invisibleRootItem()->sortChildren(0);
  ui.treeView->resizeColumnToContents(0); //need this? Need to test on a db with very deep sub directories.
  ui.treeView->setEditTriggers(QTreeView::NoEditTriggers);
  ui.treeView->setSelectionMode(QTreeView::SingleSelection);
  ui.treeView->setModel(browseModel);
}

void Musicdexer::recursDirList(QStandardItem *parentItem, QString parentPath)	  // Recursive function to populate directories for treeView
{
  if (parentPath != "root") {parentPath = parentPath + parentItem->text() + "/";}   // Parent directory with a '/' on the end is added to parentPath
  else{parentPath = "";}	  // Root is a keyword for the top level of database directories(musicPath) which has no parentPath so "".
  QRegExp rx1(QRegExp::escape(parentPath) + "[^/]*");	// Finds all dir on current level. Matches anything after parentPath that doesn't have a '/'
  QRegExp rx2(QRegExp::escape(parentPath) + ".*");	// Finds all dir on levels after current level. Matches anything after parentPath that's left over from rx1. 
  QRegExp rx3("/.*");			// Helps remove consecutive subdirectorys when trying to get a current directories name. Matches anything past first '/'.
  int recIndex = 0;
  while (recIndex != -1) {  			// As long as searchs turn up something...
    recIndex = browsePaths.indexOf(rx1);	// Under the last parent directory, find a directory in this one.    
    if (recIndex != -1) {                    // If directory found...
      QString dirName = browsePaths.at(recIndex).right(browsePaths.at(recIndex).size() - parentPath.size());  // Chop parent path from found directory's full path.
      QStandardItem *recurseItem = new QStandardItem(QString(dirName));  // An item is created with current found directory name.
      parentItem->appendRow(recurseItem);	  // Add item as child(subdir) to parent directory.
      browsePaths.removeAt(recIndex);	  // Remove found directory from list. Without this, stuck in a loop.
      recursDirList(recurseItem, parentPath);	  // Dive down into the found directory(recurseItem), with it's parent's full path(parentPath).      
    }else{					// If directory not found...
      recIndex = browsePaths.indexOf(rx2);   // Find any leftovers. Lefterovers are directories that contain only directories in them, though further sub-directorys may contain music.
      if (recIndex != -1) {                  // If leftover found...
	QString dirNameWithSubs = browsePaths.at(recIndex).right(browsePaths.at(recIndex).size() - parentPath.size()); // Chop parent path from found directory's full path. 
	QString dirName = dirNameWithSubs.remove(rx3); // Chob sub-dirs from the end of found directory.
	QStandardItem *recurseItem = new QStandardItem(QString(dirName));  // An item is created with current found direcotry name.
        parentItem->appendRow(recurseItem);	  // Add item as child(subdir) to parent directory.
	recursDirList(recurseItem, parentPath);    // Dive down into the found directory(recurseItem), with it's parent's full path(parentPath).
      }
    }
  }
}

void Musicdexer::Backup()
{ // TODO this is a temporary backup solution. needs more safty and add a way to backup table or option to recreate structure along with restore.
  QMessageBox msgBackup;
  msgBackup.setText("Backup Failed :(");
  msgBackup.setStandardButtons(QMessageBox::Ok);
  msgBackup.setDefaultButton(QMessageBox::Ok);
  QSqlQuery backupQuery;
  if (!( QFile::exists("/tmp/tracks.dat") && !QFile::remove("/tmp/tracks.dat") )) {
    QFile::remove("/home/randy/Music/tracks.dat");
    if (backupQuery.exec("SELECT * INTO OUTFILE '/tmp/tracks.dat' FROM tracks")
    && QFile::copy("/tmp/tracks.dat","/home/randy/Music/tracks.dat")) {
      msgBackup.setText("Backup Successfull! :)");
    }else{msgBackup.setInformativeText("Could not save to file '/tmp/tracks.dat' or couldn't move it after save.");}
  }else{msgBackup.setInformativeText("Could not remove '/tmp/tracks.dat'.");}
  msgBackup.exec();
}

void Musicdexer::Sync()
{
  // Music Sync: Syncronize database to music folders. TODO: Status bar does not get updated durring sync, only the last message shows. Probably because the main window is busy until sync's compleation, which is good to prevent users from messing with the sync process.
  ui.statusBar->showMessage("Starting music sync. Updating database with any file changes.", 10000);
  qDebug() << "Starting music sync. Updating database with any file changes.";
  // Initialize variables
  QStringList tagfType;
  int cfIndex = 0; // Index used for curFiles
  QSqlQuery syncQuery;
  syncQuery.setForwardOnly(true); // Allows only moving forward through query results, reducing resource use.
  QSqlQuery updateQuery; // Doesn't need .setForwardOnly since it won't be used for queries.
  
  // Prepare list(curFiles) with all current music files in music folder(s)
  QDirIterator dirIt(musicPath, QDirIterator::Subdirectories); // QDirIterator supports subdirectories
  QStringList curFiles; // QDirIterator does NOT support filetype/directory filtering and sort. I add my own filtering and QStringList has sort.
  QStringList mfType;  // Music filetypes
  mfType << "mp3" << "ogg" << "wma" << "wav" << "mid" << "midi" << "aac" << "mpc" << "mp4" << "asf" << "umx";
  while (dirIt.hasNext()) {
    dirIt.next();
    if (!dirIt.fileInfo().isDir()) {
      if (mfType.contains(dirIt.fileInfo().suffix(), Qt::CaseInsensitive)) {
        curFiles << dirIt.filePath();

      }
    }
  }
  curFiles.sort(); // SQL sorted the same way with "ORDER BY BINARY path" 


  syncQuery.exec("SELECT id, path, missing, size, tracknumber, title, album, artist, genre, length FROM tracks ORDER BY BINARY path");
  // Proceed through each record. Update any changes to db or tags.
  // Compair DB's files to curFile. Check for missing. Files left over will be added later.
  while (syncQuery.next()) {
    // Get current record values.
    int dbid = syncQuery.value(0).toInt();
    QString dbPath = syncQuery.value(1).toString();
    bool dbmissing = syncQuery.value(2).toBool();
    int dbSize = syncQuery.value(3).toInt();
    int dbTnum = syncQuery.value(4).toInt();
    QString dbTitle = syncQuery.value(5).toString();
    QString dbAlbum = syncQuery.value(6).toString();
    QString dbArtist = syncQuery.value(7).toString();
    QString dbGenre = syncQuery.value(8).toString();
    int dbLength = syncQuery.value(9).toInt();

    // Check to see if DB file exists, if not, mark it missing, otherwises mark it not missing.
    // Search for location of dbPath in curFiles, starting from cfIndex.
    int tmpIndex = curFiles.indexOf(dbPath, cfIndex);
    if (tmpIndex == -1) {
      if (!dbmissing) { // Mark missing
        updateQuery.prepare("UPDATE tracks SET missing = 1 WHERE id = ?");
        updateQuery.addBindValue(dbid);
        updateQuery.exec();
      }
      continue;
    }
    if (dbmissing) { // Mark not missing
      updateQuery.prepare("UPDATE tracks SET missing = 0 WHERE id = ?");
      updateQuery.addBindValue(dbid);
      updateQuery.exec();
    }
    curFiles.removeAt(tmpIndex); // Remaining files in curFiles are to-be-added later.
    cfIndex = tmpIndex;  // Start search at current index to save resources. db and curFiles must be sorted identically.

    QFileInfo curFile(dbPath); //info about current db file
    // Need tag manipulation for the rest of the loop.
    TagLib::FileRef f(dbPath.toUtf8());
    if (f.isNull()) { // File has no tag.
      if (dbSize != curFile.size()) { // Update db of new file size.
	updateQuery.prepare("UPDATE tracks SET size = ? WHERE id = ?");
	updateQuery.addBindValue(curFile.size());
	updateQuery.addBindValue(dbid);
      }
      continue;
    }
    // From here on, file has a tag.
    
    int tagTnum = f.tag()->track();
    QString tagTitle = f.tag()->title().toCString();
    QString tagAlbum = f.tag()->album().toCString();
    QString tagArtist = f.tag()->artist().toCString();
    QString tagGenre = f.tag()->genre().toCString();
    int trkLength = f.audioProperties()->length();
    //NOTE update DB size after any tag modifications incase it changes filesize.
    
    // Size: If Size and Path match, nothing changed except mabye tags with a set size like ID3v1. Probably intentional.
    if (dbSize == curFile.size()) {
      if (!f.tag()->isEmpty()) {
        updateQuery.prepare("UPDATE tracks SET tracknumber = ?, title = ?, album = ?, artist = ?, genre = ? WHERE id = ?");
        updateQuery.addBindValue(tagTnum);
        updateQuery.addBindValue(tagTitle);
        updateQuery.addBindValue(tagAlbum);
        updateQuery.addBindValue(tagArtist);
        updateQuery.addBindValue(tagGenre);
        updateQuery.addBindValue(dbid);
        updateQuery.exec();
      } //TODO else if (all db entries not empty) {write tag. update dbsize} Tag accidentaly erased?
      continue;
    }
    // From here on, Size does not match.

    bool mTnum = (dbTnum == tagTnum);
    bool mTitle = (dbTitle == tagTitle);
    bool mAlbum = (dbAlbum == tagAlbum);
    bool mArtist = (dbArtist == tagArtist);
    bool mGenre = (dbGenre == tagGenre);
    bool mLength = (dbLength == trkLength);

    /* Cases: Automatic handling of updates is not always to the users liking, Either settings would have to be
       set ahead of time, or the user would have to be prompted with choices durring the sync.  Here are some scenarios:
       - New File: reasons: Replace bad,corrupted file OR new filetype OR better quality.
           How to detect? length MAY change... UpdateTAG or DB depending on if new info better than old.
           Currently updating tag if db has more info.
           Probably acceptable to prompt user in this case as it shouldn't happen too often.
       - Misslabled(update tag). An incorrectly named audio file was swapped with the correct audio file.
           ie. There is the file "01. Hard times" and file "02. Harder times". The audio data in these files
           are actully supposed to be switched around so the user renames "01. Hard times" to "02. Harder times"
           and vice-versa. The database will see the size, length, tracknumber, and title change.
           It's possible the user got the tag info correct and named the file wrong, in which case the 
           database should be updated and not the tag. Going to assume the first case and update the tag.
           Could also prompt user in this case.
       - Tag Modified(update db): Most likely modified by the user intentionally outside of musicdexer.
           Or a music program used by the user found more up-to-date tag info and updated it automatically.
       The next if statments are an attempt to handle these cases. */
    
    // Handles some of case: 'New File' and 'Tag Modified'.
    if (mTnum && mTitle && mAlbum && mArtist && mGenre) {continue;} // Only audio data changed/edited. do nothing.
    if ( (tagTitle.isEmpty() || tagTitle.isNull()) && !(dbTitle.isEmpty() || dbTitle.isNull()) ) {  // These shouldn't be empty in combination, Assume incomplete/bad tag.
      bool savetag = false;
      // Write DB to tags where needed.
      if (!mTnum && dbTnum > 0) { f.tag()->setTrack(dbTnum); savetag = true; }
      if (!mTitle && !(dbTitle.isEmpty() || dbTitle.isNull())) { f.tag()->setTitle(dbTitle.toStdWString()); savetag = true; }
      if (!mAlbum && !(dbAlbum.isEmpty() || dbAlbum.isNull())) { f.tag()->setAlbum(dbAlbum.toStdWString()); savetag = true; }
      if (!mArtist && !(dbArtist.isEmpty() || dbArtist.isNull())) { f.tag()->setArtist(dbArtist.toStdWString()); savetag = true; }
      if (!mGenre && !(dbGenre.isEmpty() || dbGenre.isNull())) { f.tag()->setGenre(dbGenre.toStdWString()); savetag = true; }
      if (savetag == true) { f.save(); }
      if (dbSize != curFile.size()) {
	updateQuery.prepare("UPDATE tracks SET size = ? WHERE id = ?");
	updateQuery.addBindValue(curFile.size());
	updateQuery.addBindValue(dbid);
      } 
      continue;
    }

    // Case: Misslabled. Tracknumber and title dont match, but the rest do. If they are not empty in db or tag, then update tag.
    if (!mTnum && !mTitle && !mLength && mAlbum && mArtist && mGenre && tagTnum > 0 && dbTnum > 0 && !tagTitle.isEmpty() && !dbTitle.isEmpty() && !tagTitle.isNull() && !dbTitle.isNull()) {
      f.tag()->setTrack(dbTnum);
      f.tag()->setTitle(dbTitle.toStdWString());
      f.save();
      updateQuery.prepare("UPDATE tracks SET size = ?, length = ? WHERE id = ?");
      updateQuery.addBindValue(curFile.size());
      updateQuery.addBindValue(trkLength);
      updateQuery.addBindValue(dbid);
      continue;
    } 
    
    // Case: New File. Will choose based on what tag has more info.
    if (!mLength) {
      QString dbVs = dbTitle + dbAlbum + dbArtist + dbGenre;
      QString tagVs = tagTitle + tagAlbum + tagArtist + tagGenre;
      if (dbVs.size() > tagVs.size()) {
	f.tag()->setTrack(dbTnum);
	f.tag()->setTitle(dbTitle.toStdWString());
	f.tag()->setAlbum(dbAlbum.toStdWString());
	f.tag()->setArtist(dbArtist.toStdWString());
	f.tag()->setGenre(dbGenre.toStdWString());
        updateQuery.prepare("UPDATE tracks SET size = ?, length = ? WHERE id = ?");
        updateQuery.addBindValue(curFile.size());
        updateQuery.addBindValue(trkLength);
        updateQuery.addBindValue(dbid);
        continue;
      }
    }
    
    //update db in all other cases. Modified Tag.
    updateQuery.prepare("UPDATE tracks SET tracknumber = ?, title = ?, album = ?, artist = ?, genre = ?, size = ?, length = ? WHERE id = ?");
    updateQuery.addBindValue(tagTnum);
    updateQuery.addBindValue(tagTitle);
    updateQuery.addBindValue(tagAlbum);
    updateQuery.addBindValue(tagArtist);
    updateQuery.addBindValue(tagGenre);
    updateQuery.addBindValue(curFile.size());
    updateQuery.addBindValue(trkLength);
    updateQuery.addBindValue(dbid);
  }
  // ### End of syncQuery record loop ###
  

  //Music Sync: Add new files
  ui.statusBar->showMessage("Adding new files.", 10000);
  qDebug() << "Adding new files.";
  // Proceed through each remaining file, see if it is a missing file, if not, add it to the db.
  // Goal: Try to skip unnecesary tests for close matches(make less cpu intensive).
  // Files without tags are dealt with first.
  for (int i = 0; i < curFiles.size(); ++i) {
    QFileInfo curFile(curFiles.at(i));
    TagLib::FileRef f(curFiles.at(i).toUtf8());
    if (f.isNull()) { // Missing found if curFile has no tag, same size, and same filename or filname matches the title in db.
      // In a very rare case, a problem is caused when a file comming in is exact same song as a missing file but from another album.
        // ie. The song's original album and a greatest hits release. Aspects of the song could change but might be the same.
      // Could add match-parent-directory to be more strict, but some people pile all their music in one fulder.
      // TODO Improve filename/title matching by stripping elements from filename like a track number or artist.
      syncQuery.prepare("SELECT id FROM tracks WHERE missing = 1 AND ( title = ? OR SUBSTRING_INDEX(path, '/', -1) = ? )"); // "SUBS..-1)" grabs filename from path.
      syncQuery.addBindValue(curFile.baseName()); // songname
      syncQuery.addBindValue(curFile.fileName()); // songname.mp3
      syncQuery.exec();
      if (syncQuery.next()) { //missing found
	updateQuery.prepare("UPDATE tracks SET missing = 0, path = ? WHERE id = ?");
	updateQuery.addBindValue(curFiles.at(i));
	updateQuery.addBindValue(syncQuery.value(0).toInt());
	updateQuery.exec();
        curFiles.removeAt(i); i--; // Remaining files in curFiles are to-be-added later.
	continue;
      }
      continue; //Add file in any other case for no tag.
    }
    // From here on, file has a tag,
    int tagTnum = f.tag()->track();
    QString tagTitle = f.tag()->title().toCString();
    QString tagAlbum = f.tag()->album().toCString();
    QString tagArtist = f.tag()->artist().toCString();
    QString tagGenre = f.tag()->genre().toCString();
    int trkLength = f.audioProperties()->length();
    //NOTE update DB size, because it may change from tag updates.
    // The next line of variables are used for futher query when a query returns more than one result.
    int dbId; int dbTnum; QString dbTitle; int dbLength; QString dbFname; QString dbArtist;

    
    // ### ALBUM ###
    // Usually an album can only contain one instance of each of the following: tracknumber, title, length, size, filename.
    // So, if a missing file matches one of those, plus the album matches it, then it is found.
    syncQuery.prepare("SELECT id, tracknumber, title, length, size, SUBSTRING_INDEX(path, '/', -1) FROM tracks WHERE missing = 1 AND album = ? AND ( tracknumber = ? OR title = ? OR length = ? OR size = ? OR SUBSTRING_INDEX(path, '/', -1) = ? )");
    syncQuery.addBindValue(tagAlbum);
    syncQuery.addBindValue(tagTnum);
    syncQuery.addBindValue(tagTitle);
    syncQuery.addBindValue(trkLength);
    syncQuery.addBindValue(curFile.size());
    syncQuery.addBindValue(curFile.fileName());
    syncQuery.exec();
    if (syncQuery.next()) {
      if (syncQuery.size() == 1) {
	updateQuery.prepare("UPDATE tracks SET missing = 0, path = ?, size = ?, length = ?, tracknumber = ?, title = ?, artist = ?, genre = ? WHERE id = ?");
	updateQuery.addBindValue(curFiles.at(i));
	updateQuery.addBindValue(curFile.size());
	updateQuery.addBindValue(trkLength);
	updateQuery.addBindValue(tagTnum);
	updateQuery.addBindValue(tagTitle);
	updateQuery.addBindValue(tagArtist);
	updateQuery.addBindValue(tagGenre);
	updateQuery.addBindValue(syncQuery.value(0).toInt());
	updateQuery.exec();
        curFiles.removeAt(i); i--; // Remaining files in curFiles are to-be-added later.
        continue;
      }
      // Multiple results found, Test each one to see if curFile is a missing found.
      // Each file is graded. Missing found only if ONE file is found with a higher grade than the rest.
      // In cases where fields match but are empty/null, they are not worth as mutch.
      int aOldgrade = 0;
      int aGrade;
      int aID = -1;
      do {
	  aGrade = 0;
          if (syncQuery.value(2).toString() == tagTitle) {
	    if (tagTitle.isEmpty() || tagTitle.isNull()) {aGrade=1;}else{aGrade=3;}}
          if (syncQuery.value(5).toString() == curFile.fileName()) {aGrade=3;}  //Title and filename about the same, so only count once out of both.
          if (syncQuery.value(1).toInt() == tagTnum) {
	    if (tagTnum == 0) {aGrade+=1;}else{aGrade+=3;}}
          if (syncQuery.value(3).toInt() == trkLength) {aGrade+=2;} // Same length and/or size not enough, so only increase by two.
	  if (syncQuery.value(4).toInt() == curFile.size()) {aGrade+=2;}
          if (aGrade > 4 && aGrade >= aOldgrade ) {
	    aID = syncQuery.value(0).toInt();
	    if (aGrade == aOldgrade) {aID == -1;}else{aOldgrade = aGrade;}
	  }
      } while (syncQuery.next());
      if (aID > -1) {
	updateQuery.prepare("UPDATE tracks SET missing = 0, path = ?, size = ?, length = ?, tracknumber = ?, title = ?, artist = ?, genre = ? WHERE id = ?");
	updateQuery.addBindValue(curFiles.at(i));
	updateQuery.addBindValue(curFile.size());
	updateQuery.addBindValue(trkLength);
	updateQuery.addBindValue(tagTnum);
	updateQuery.addBindValue(tagTitle);
	updateQuery.addBindValue(tagArtist);
	updateQuery.addBindValue(tagGenre);
	updateQuery.addBindValue(aID);
	updateQuery.exec();
        curFiles.removeAt(i); i--; // Remaining files in curFiles are to-be-added later.
      }
      continue; // Add File
    }
    // From here on. file has tag, but album doesn't match.

    // TODO Album could be mis-labled/spelled. Try to detect this and prompt user.
      // ie. Album = 'Halo - Soundtrack' vs 'Halo (sound track)'
      // Would cost resources but could parse album to words only and search album for words.
      // Though it might hit multiple albums or compleatly miss one when it should hit.
    
    // ### SIZE ###
    // Case: Album changed? if size match. and, length or tracknumber match.
    syncQuery.prepare("SELECT id, tracknumber, title, length, SUBSTRING_INDEX(path, '/', -1) FROM tracks WHERE missing = 1 AND size = ? AND ( tracknumber = ? OR title = ? OR length = ? OR SUBSTRING_INDEX(path, '/', -1) = ? )");
    syncQuery.addBindValue(curFile.size());
    syncQuery.addBindValue(tagTnum);
    syncQuery.addBindValue(tagTitle);
    syncQuery.addBindValue(trkLength);
    syncQuery.addBindValue(curFile.size());
    syncQuery.exec();
    if (syncQuery.next()) {
      if (syncQuery.size() == 1) {
	updateQuery.prepare("UPDATE tracks SET missing = 0, path = ?, length = ?, tracknumber = ?, title = ?, album = ?, artist = ?, genre = ? WHERE id = ?");
	updateQuery.addBindValue(curFiles.at(i));
	updateQuery.addBindValue(trkLength);
	updateQuery.addBindValue(tagTnum);
	updateQuery.addBindValue(tagTitle);
        updateQuery.addBindValue(tagAlbum);
	updateQuery.addBindValue(tagArtist);
	updateQuery.addBindValue(tagGenre);
	updateQuery.addBindValue(syncQuery.value(0).toInt());
	updateQuery.exec();
        curFiles.removeAt(i); i--; // Remaining files in curFiles are to-be-added later.
	continue;
      }
      // Multiple results found. Size match, album doesn't, and at least one other thing does.
      int sOldgrade = 0;
      int sGrade;
      int sID = -1;
      do {
        sGrade = 0;
        if (syncQuery.value(2).toString() == tagTitle) {
	  if (tagTitle.isEmpty() || tagTitle.isNull()) {sGrade=1;}else{sGrade=3;}}
        if (syncQuery.value(4).toString() == curFile.fileName()) {
	  if (sGrade > 0) {sGrade+=1;}else{sGrade=3;}}
        if (syncQuery.value(1).toInt() == tagTnum) {
	  if (tagTnum == 0) {sGrade+=1;}else{sGrade+=2;}}
        if (syncQuery.value(3).toInt() == trkLength) {sGrade+=2;}
        if (sGrade > 4 && sGrade >= sOldgrade ) {
	  sID = syncQuery.value(0).toInt();
	  if (sGrade == sOldgrade) {sID == -1;}else{sOldgrade = sGrade;}
	}
      } while (syncQuery.next());
      if (sID > -1) {
	updateQuery.prepare("UPDATE tracks SET missing = 0, path = ?, length = ?, tracknumber = ?, title = ?, album = ?, artist = ?, genre = ? WHERE id = ?");
	updateQuery.addBindValue(curFiles.at(i));
	updateQuery.addBindValue(trkLength);
	updateQuery.addBindValue(tagTnum);
	updateQuery.addBindValue(tagTitle);
	updateQuery.addBindValue(tagAlbum);
	updateQuery.addBindValue(tagArtist);
	updateQuery.addBindValue(tagGenre);
	updateQuery.addBindValue(sID);
	updateQuery.exec();
        curFiles.removeAt(i); i--; // Remaining files in curFiles are to-be-added later.
      }
      continue; // Add file
    }
    // From here on, album and size don't match.

    // ### TITLE/FILENAME ###
    // Case: Album changed, tag info modified the size.
    // Case: File replaced with new file and album spelled differently.
    // Query a filname or title match. if one result, check if length, tracknumber, or artist matches as well.
    // if multiple results, check for one result of 2 or more matches of length, tracknumber, or artist.
    syncQuery.prepare("SELECT id, tracknumber, length, artist FROM tracks WHERE missing = 1 AND ( title = ? OR SUBSTRING_INDEX(path, '/', -1) = ? ) AND ( length = ? OR tracknumber = ? OR artist = ? ) ");
    syncQuery.addBindValue(tagTitle);
    syncQuery.addBindValue(curFile.fileName());
    syncQuery.addBindValue(trkLength);
    syncQuery.addBindValue(tagTnum);
    syncQuery.addBindValue(tagArtist);
    syncQuery.exec();
    if (syncQuery.next()) {
      if (syncQuery.size() == 1) {
	updateQuery.prepare("UPDATE tracks SET missing = 0, path = ?, size = ?. length = ?, tracknumber = ?, title = ?, album = ?, artist = ?, genre = ? WHERE id = ?");
	updateQuery.addBindValue(curFiles.at(i));
	updateQuery.addBindValue(curFile.size());
	updateQuery.addBindValue(trkLength);
	updateQuery.addBindValue(tagTnum);
	updateQuery.addBindValue(tagTitle);
        updateQuery.addBindValue(tagAlbum);
	updateQuery.addBindValue(tagArtist);
	updateQuery.addBindValue(tagGenre);
	updateQuery.addBindValue(syncQuery.value(0).toInt());
	updateQuery.exec();
        curFiles.removeAt(i); i--; // Remaining files in curFiles are to-be-added later.
	continue;
      }
      // Multiple results found. Title or Filename match, Size and Album don't, and at least two other thing do.
      int tOldgrade = 0;
      int tGrade;
      int tID = -1;
      do { // Missing found if 1 instance of all 3 match. if not then 1 instance of 2 match.
        tGrade = 0;
        if (syncQuery.value(1).toInt() == tagTnum) {
	  if (tagTnum == 0) {tGrade+=1;}else{tGrade+=3;}}
        if (syncQuery.value(2).toInt() == trkLength) {tGrade+=3;}
        if (syncQuery.value(3).toString() == tagArtist) {
          if (tagArtist.isEmpty() || tagArtist.isNull()) {tGrade+=1;}else{tGrade+=3;}}
        if (tGrade > 4 && tGrade >= tOldgrade ) {
	  tID = syncQuery.value(0).toInt();
	  if (tGrade == tOldgrade) {tID == -1;}else{tOldgrade = tGrade;}
	}
      } while (syncQuery.next());
      if (tID > -1) {
	updateQuery.prepare("UPDATE tracks SET missing = 0, path = ?, size = ?, length = ?, tracknumber = ?, title = ?, album = ?, artist = ?, genre = ? WHERE id = ?");
	updateQuery.addBindValue(curFiles.at(i));
	updateQuery.addBindValue(curFile.size());
	updateQuery.addBindValue(trkLength);
	updateQuery.addBindValue(tagTnum);
	updateQuery.addBindValue(tagTitle);
	updateQuery.addBindValue(tagAlbum);
	updateQuery.addBindValue(tagArtist);
	updateQuery.addBindValue(tagGenre);
	updateQuery.addBindValue(tID);
	updateQuery.exec();
        curFiles.removeAt(i); i--; // Remaining files in curFiles are to-be-added later.
      }
    }
    // Anything left over will be added.
  }
  
  
  // ### Add remaining files. ###
  if (!curFiles.isEmpty()) {
    syncQuery.prepare("INSERT INTO tracks (path, size, tracknumber, title, album, artist, genre, length) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    for (int i = 0; i < curFiles.size(); ++i) {
      QFileInfo curFile(curFiles.at(i));
      TagLib::FileRef f(curFiles.at(i).toUtf8());
      syncQuery.addBindValue(curFiles.at(i));
      syncQuery.addBindValue(curFile.size());
      if (!f.isNull()) { // File missing or no tag support
        syncQuery.addBindValue(f.tag()->track());
        syncQuery.addBindValue(f.tag()->title().toCString());
        syncQuery.addBindValue(f.tag()->album().toCString());
        syncQuery.addBindValue(f.tag()->artist().toCString());
        syncQuery.addBindValue(f.tag()->genre().toCString());
        syncQuery.addBindValue(f.audioProperties()->length());
      }
      syncQuery.exec();
    }
    ui.statusBar->showMessage("Sync finished. Added " + QString::number(curFiles.size()) + " files.", 10000); 
  }else{ui.statusBar->showMessage("Sync finished. No new files.", 10000);}
  dbmodel->select();
  populateBrowser();
}


#include "musicdexer.moc"


/*
//Default Table creation
QSqlQuery query;
query.exec("CREATE TABLE tracks(id MEDIUMINT UNSIGNED PRIMARY KEY AUTO_INCREMENT, tracknumber SMALLINT(3) UNSIGNED, title VARCHAR(150), album VARCHAR(100), artist VARCHAR(150), genre CHAR(30), mood CHAR(30), rating DECIMAL(3,1) UNSIGNED, length SMALLINT(5) UNSIGNED, size MEDIUMINT(10) UNSIGNED, mycomment VARCHAR(255), path VARCHAR(255), missing BOOL NOT NULL DEFAULT '0')");
*/