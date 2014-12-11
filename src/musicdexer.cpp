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

#include "musicdexer.h"
#include "ui_musicdexer.h"
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QMessageBox>
#include <fileref.h>
#include <tag.h>
#include <QFileInfo>
#include <QScrollBar>
#include <QDirIterator>
#include <QFileDialog>
#include <QSettings>
#include <QProgressDialog>

#if QT_VERSION < 0x050000
#include <QDesktopServices>
#else
#include <QStandardPaths>
#endif


Musicdexer::Musicdexer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Musicdexer)
{
    dbmodelon = false;

    // Setup ui and Populate combo boxes
    ui->setupUi(this);
    QStringList strlFieldsE, strlFields, strlOperators;
    strlFieldsE << "Album" << "Artist" << "Genre" << "Mood" << "Rating" << "Comment";
    strlFields << "Track" << "Title" << "Album" << "Artist" << "Genre" << "Mood" << "Rating" << "Length" << "Size" << "Comment" << "Path" << "Missing";
    strlOperators << "Search" << "LIKE" << "=" << "!" << ">" << "<" << ">=" << "<=";
    ui->cboxField->addItems(strlFields);
    ui->cboxField->setCurrentIndex(2);
    ui->cboxOperator->addItems(strlOperators);
    ui->cboxFieldE->addItems(strlFieldsE);
    ui->cboxFieldE->setCurrentIndex(3);

    // Check/Setup Preferences
    QCoreApplication::setApplicationName("Musicdexer");
    settings = new QSettings("Musicdexer", "Musicdexer");
    musicpath = settings->value("MusicPath").toString();
    QString db_filepath = settings->value("DatabasePath").toString();
    if (!QDir(musicpath).exists() || musicpath.isEmpty()) {
        QMessageBox::information(this, "No saved music path", "Please select the location of your music collection.");
        on_actionDirSelect_triggered();
    }
    QString createquery = "";
    QFileInfo db_file( db_filepath );
    if (!db_file.exists()) { // If db missing, create new db
        QDir pthtool;
        if ( db_filepath.isEmpty() || !(pthtool.mkpath(db_file.absolutePath()) && db_filepath.endsWith(".db")) ) { // If db path unset or invalid, set default path.
            #if QT_VERSION < 0x050000
                db_filepath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);  // First try
            #else
                db_filepath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);  // First try
            #endif
            if (db_filepath.isEmpty()) {
                #if QT_VERSION < 0x050000
                    db_filepath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation); // Second try
                #else
                    db_filepath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation); // Second try
                #endif
            }
            pthtool.mkpath(db_filepath);
            db_filepath = db_filepath + "/musicdexer.db";
            settings->setValue("DatabasePath", db_filepath);
            settings->sync();
        }
        ui->statusBar->showMessage("No database found. Creating a new one at: " + db_filepath, 10000);
        createquery = "CREATE TABLE tracks(id INTEGER PRIMARY KEY AUTOINCREMENT, Track SMALLINT(3), Title VARCHAR(150), Album VARCHAR(100), Artist VARCHAR(150), Genre CHAR(30), Mood CHAR(30), Rating DECIMAL(3,1), Length SMALLINT(5), Size MEDIUMINT(10), Comment VARCHAR(255), Path VARCHAR(255), Missing BOOL NOT NULL DEFAULT '0')";
    } else {
        ui->statusBar->showMessage("Loaded database file: " + db_filepath, 10000);
    }

    // Initialize Database
    MDDB = QSqlDatabase::addDatabase( "QSQLITE" );
    MDDB.setDatabaseName( db_filepath );
    if (!MDDB.open()) {
        ui->statusBar->showMessage("Could not open database or create database file.", 10000);
    }
    QSqlQuery tmpquery(createquery);

    // Setup tableView
    dbmodel = new CustomQSqlTableModel(this);
    dbmodel->setPathSize(musicpath.size());
    dbmodel->setTable("tracks");
    dbmodel->setEditStrategy(QSqlTableModel::OnRowChange);
    if (!dbmodel->select()) {
        ui->statusBar->showMessage("There is a problem with the database structure.", 10000);
    }
    dbmodelon = true;
    ui->tableView->setModel(dbmodel);
    ui->tableView->setEditTriggers(QTableView::DoubleClicked | QTableView::SelectedClicked | QTableView::EditKeyPressed);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->hideColumn(0);
    ui->tableView->horizontalHeader()->resizeSection(1, 35); //Track
    ui->tableView->horizontalHeader()->resizeSection(2, 160); //Title
    ui->tableView->horizontalHeader()->resizeSection(3, 160); //Album
    ui->tableView->horizontalHeader()->resizeSection(4, 100); //Artist
    ui->tableView->horizontalHeader()->resizeSection(5, 50); //Genre
    ui->tableView->horizontalHeader()->resizeSection(6, 100); //Mood
    ui->tableView->horizontalHeader()->resizeSection(7, 40); //Rating
    ui->tableView->horizontalHeader()->resizeSection(8, 45); //Length
    ui->tableView->horizontalHeader()->resizeSection(9, 55); //Size
    ui->tableView->hideColumn(12); // Hide column 'Missing' since Missing files are colored red anyway.
    ui->tableView->horizontalHeader()->setStretchLastSection(true); // Removes white space by allowing last field to stretch with application.
    connect(ui->lieditSearch, SIGNAL(returnPressed()), this, SLOT(on_pbtnSearch_clicked()));
    connect(dbmodel, SIGNAL(beforeUpdate(int,QSqlRecord&)), this, SLOT(dbedit(int,QSqlRecord&)));
    browseModel = new QStandardItemModel(this);
    populateBrowser();
}

Musicdexer::~Musicdexer()
{
    delete ui;
}


void Musicdexer::on_pbtnSearch_clicked()
{    
    if (!ui->lieditSearch->text().isEmpty()) {
        QString strFilter;
        if (ui->cboxOperator->currentIndex() == 0) {
            strFilter = ui->cboxField->currentText() + " " + "LIKE" + " '%" + ui->lieditSearch->text().replace("'", "''") + "%'";
        }else{
            strFilter = ui->cboxField->currentText() + " " + ui->cboxOperator->currentText() + " '" + ui->lieditSearch->text().replace("'", "''") + "'";
        }
        dbmodel->setFilter(strFilter);
        dbmodel->select();
    }
}

void Musicdexer::on_pbtnClear_clicked()
{
    dbmodel->setFilter("");
    dbmodel->select();
}

void Musicdexer::on_pbtnDelete_clicked()
{
    dbmodel->query().last();
    int numOfRec = dbmodel->query().at() + 1;
    if ( numOfRec == -1 ) {numOfRec++;}
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

void Musicdexer::on_pbtnAddto_clicked()
{
    QString addString = ui->lieditUpdate->text();
    QString updtField = ui->cboxFieldE->currentText();
    if (!addString.isEmpty() || updtField != "Rating") {
        dbmodel->query().last();
        int numOfRec = dbmodel->query().at() + 1;
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
            // Update tags. Do this before db update, otherwise there will be no results from the query to update files.
            if (updtField == "Title" || updtField == "Album" || updtField == "Artist" || updtField == "Genre") {
                addQuery.exec(strCurSearch);
                QString updtPath;
                while (addQuery.next()) {
                    updtPath = addQuery.value(11).toString();
                    TagLib::FileRef u(QFile::encodeName(updtPath).constData());
                    if (!u.isNull()) { // File exists and is tag writable
                        QString addToTag = ";" + addString;
                        if (updtField == "Title") { addToTag = u.tag()->title().toCString() + addToTag; u.tag()->setTitle(addToTag.toStdWString());
                        }else if (updtField == "Album") { addToTag = u.tag()->album().toCString() + addToTag; u.tag()->setAlbum(addToTag.toStdWString());
                        }else if (updtField == "Artist") { addToTag = u.tag()->artist().toCString() + addToTag; u.tag()->setArtist(addToTag.toStdWString());
                        }else if (updtField == "Genre") { addToTag = u.tag()->genre().toCString() + addToTag; u.tag()->setGenre(addToTag.toStdWString());}
                        u.save();
                    }
                }
            }
            // Get the WHERE statement from the query that is used to show the current results and use it for the update.
            // if the whole database is shown, then there is no WHERE statement, so update entire DB.
            int numWhere = strCurSearch.indexOf(" WHERE ");
            if (numWhere == -1) {
                if (strCurSearch.indexOf(" FROM ") != -1) {
                    strCurSearch = "";
                }
            }else{
                numWhere = strCurSearch.size() - numWhere;
            }
            addString.replace("'", "''");
            QString strCustQuery = "UPDATE tracks SET " + updtField + " = COALESCE(" + updtField + ",'') || " + "'" + addString.replace("'", "''") + "'" + strCurSearch.right(numWhere);
            addQuery.exec(strCustQuery);
            if (updtField == ui->cboxField->currentText()) {
                ui->lieditSearch->setText(addString);
            }
            if (numWhere == -1) {
                on_pbtnClear_clicked(); // User didn't search.
            }else if (strCurSearch.indexOf("%' AND Path NOT LIKE '") == -1) {
                on_pbtnSearch_clicked(); // User used Search by Field.
            }else{
                dbmodel->select(); // User used Search by Directory.
            }
        }
    }
}

void Musicdexer::on_pbtnChange_clicked()
{
    dbmodel->query().last();
    int numOfRec = dbmodel->query().at() + 1;
    QString strEmpty;
    if (ui->lieditUpdate->text().isEmpty()) {
        strEmpty = " Deleted.";
    }else{
        strEmpty = " changed to " + ui->lieditUpdate->text() + ".";
    }
    QMessageBox msgUpdate;
    msgUpdate.setText("All " + QString::number(numOfRec) + " songs will have their " + ui->cboxFieldE->currentText() + strEmpty);
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
        QString updtString = ui->lieditUpdate->text();
        QString updtField = ui->cboxFieldE->currentText();
        QString strCurSearch = dbmodel->query().lastQuery();
        // Update tags. Do this before db update; otherwise, there will be no results from the query to update files.
        if (updtField == "Track" || updtField == "Title" || updtField == "Album" || updtField == "Artist" || updtField == "Genre") {
            updtQuery.exec(strCurSearch);
            QString updtPath;
            while (updtQuery.next()) {
                updtPath = updtQuery.value(11).toString();
                TagLib::FileRef u(QFile::encodeName(updtPath).constData());
                if (!u.isNull()) { // File exists and is tag writable
                    if (updtField == "Track") { u.tag()->setTrack(updtString.toInt());
                    }else if (updtField == "Title") { u.tag()->setTitle(updtString.toStdWString());
                    }else if (updtField == "Album") { u.tag()->setAlbum(updtString.toStdWString());
                    }else if (updtField == "Artist") { u.tag()->setArtist(updtString.toStdWString());
                    }else if (updtField == "Genre") { u.tag()->setGenre(updtString.toStdWString());}
                    u.save();
                }
            }
        }
        // Get the WHERE statement from the query used to show the current results to use for the update.
        // if the whole database is shown, then there is no WHERE statement, so update entire DB.
        int numWhere = strCurSearch.indexOf(" WHERE ");
        if (numWhere == -1) { // where is not there.
            if (strCurSearch.indexOf(" FROM ") != -1) { // This should always be there, but check anyways.
                strCurSearch = "";
            }
        }else{
            numWhere = strCurSearch.size() - numWhere;
        }
        if (updtString.isEmpty()) {
            updtString = "NULL";
        }else{
            updtString = "'" + updtString.replace("'", "''") + "'";
        }
        QString strCustQuery = "UPDATE tracks SET " + updtField + " = " + updtString + strCurSearch.right(numWhere);
        updtQuery.exec(strCustQuery);
        // Update finished, now refresh the search so it shows the updated results.
        if (updtField == ui->cboxField->currentText()) {
            ui->lieditSearch->setText(ui->lieditUpdate->text());
        }
        if (numWhere == -1) {
            on_pbtnClear_clicked(); // User didn't search.
        }else if (strCurSearch.indexOf("%' AND Path NOT LIKE '") == -1) {
            on_pbtnSearch_clicked(); // User used Search by Field.
        }else{
            dbmodel->select(); // User used Search by Directory.
        }
    }
}

void Musicdexer::dbedit(int row, QSqlRecord& rec)
{
    // Rating should be 1-10. Anything above 10 is changed to 10. 0 should be changed to NULL.
    if (!rec.value(7).isNull()) {
        float recRating = rec.value(7).toFloat();
        if (recRating < 1) {
            rec.setNull(7);
        }else if (recRating >= 10) {
            rec.setValue(7, 10);
        }
    }

    // Update tag at same time as db if possible, and only if necessary.
    if (!(rec.value(1).isNull() && rec.value(2).isNull() && rec.value(3).isNull() && rec.value(4).isNull() && rec.value(5).isNull())) {
        bool savetag = false;
        TagLib::FileRef f(QFile::encodeName(dbmodel->record(row).value(11).toString()).constData());
        unsigned int recTnum = rec.value(1).toInt();
        QString recTitle = rec.value(2).toString();
        QString recAlbum = rec.value(3).toString();
        QString recArtist = rec.value(4).toString();
        QString recGenre = rec.value(5).toString();
        if (!f.isNull()) {
            if (!rec.value(1).isNull() && f.tag()->track() != recTnum) {
                f.tag()->setTrack(recTnum); savetag=true;
            }
            if (!recTitle.isNull() && f.tag()->title().toCString() != recTitle && !(f.tag()->genre().isNull() && recGenre.isEmpty())) {
                f.tag()->setTitle(recTitle.toStdWString()); savetag=true;
            }
            if (!recAlbum.isNull() && f.tag()->album().toCString() != recAlbum && !(f.tag()->genre().isNull() && recGenre.isEmpty())) {
                f.tag()->setAlbum(recAlbum.toStdWString()); savetag=true;
            }
            if (!recArtist.isNull() && f.tag()->artist().toCString() != recArtist && !(f.tag()->genre().isNull() && recGenre.isEmpty())) {
                f.tag()->setArtist(recArtist.toStdWString()); savetag=true;
            }
            if ( !recGenre.isNull() && f.tag()->genre().toCString() != recGenre && !(f.tag()->genre().isNull() && recGenre.isEmpty())) {
                f.tag()->setGenre(recGenre.toStdWString()); savetag=true;
            }
            if (savetag) {
                if (!f.save()) {
                    ui->statusBar->showMessage("ERROR UPDATING TAG", 10000);
                }else{  // filesize may change due to tag change so update it if need be.
                    QFileInfo fileSize(dbmodel->record(row).value(11).toString());
                    if (dbmodel->record(row).value(9).toInt() != fileSize.size()) {
                        rec.setValue(9, fileSize.size());
                    }
                    ui->statusBar->showMessage("Tag updated", 10000);
                }
            }
        }else{
            ui->statusBar->showMessage("Info: This file does not have a tag but the database has been updated", 10000);
        }
    }
    // Set empty values to null
    for (int v = 2; v < 7; v++) {
        if (!rec.value(v).isNull() && rec.value(v).toString().isEmpty()) {rec.setNull(v);}
    }
}

void Musicdexer::populateBrowser()
{
    // Next 3 lines is setting the top level directory of the tree view.
    QString mpathlen = musicpath;
    #ifndef Q_OS_WIN32
        mpathlen.truncate(musicpath.lastIndexOf("/"));
    #endif
    mpathlen.truncate(mpathlen.lastIndexOf("/"));
    mpathlen = QString::number(mpathlen.length() + 2);
    QString holdpath = "";
    QSqlQuery browseQuery;
    browseQuery.setForwardOnly(true);
    browseQuery.exec("SELECT substr(Path, " + mpathlen + ") FROM tracks ORDER BY Path ASC");
    while (browseQuery.next()) {  // browsePaths is populated with Directory paths from all paths in db.
        QString songpath = browseQuery.value(0).toString();
        songpath.truncate(songpath.lastIndexOf("/"));
        if (songpath.compare(holdpath) != 0) {
            browsePaths << songpath;
            holdpath = songpath;
        }
    }
    browsePaths.removeDuplicates();
    browseModel->clear();
    recursDirList(browseModel->invisibleRootItem(), "root");  // Previous lines were preperation for this function, which does the actual populating.
    QStringList browshead; browshead  << "Browse by Folder";
    browseModel->setHorizontalHeaderLabels(browshead);
    browseModel->invisibleRootItem()->sortChildren(0);
    ui->treeView->resizeColumnToContents(0); // need this? Need to test on a db with very deep sub directories.
    ui->treeView->setEditTriggers(QTreeView::NoEditTriggers);
    ui->treeView->setSelectionMode(QTreeView::SingleSelection);
    ui->treeView->setModel(browseModel);
}

void Musicdexer::recursDirList(QStandardItem *parentItem, QString parentPath)	  // Recursive function to populate directories for treeView
{
    if (parentPath != "root") {
        parentPath = parentPath + parentItem->text() + "/";  // Parent directory with a '/' on the end is added to parentPath
    }else{
        parentPath = "";  // Root is a keyword for the top level of database directories(musicpath) which has no parentPath so "".
    }
    QRegExp rx1(QRegExp::escape(parentPath) + "[^/]*");	// Finds all dir on current level. Matches anything after parentPath that doesn't have a '/'
    QRegExp rx2(QRegExp::escape(parentPath) + ".*");	// Finds all dir on levels after current level. Matches anything after parentPath that's left over from rx1.
    QRegExp rx3("/.*");  // Helps remove consecutive sub-directories when trying to get a current directories name. Matches anything past first '/'.
    int recIndex = 0;
    while (recIndex != -1) {	// As long as searches turn up something...
        recIndex = browsePaths.indexOf(rx1);  // Under the last parent directory, find a directory in this one.
        if (recIndex != -1) {  // If directory found...
            QString dirName = browsePaths.at(recIndex).right(browsePaths.at(recIndex).size() - parentPath.size());  // Chop parent path from found directory's full path.
            QStandardItem *recurseItem = new QStandardItem(QString(dirName));  // An item is created with current found directory name.
            parentItem->appendRow(recurseItem);  // Add item as child(subdir) to parent directory.
            browsePaths.removeAt(recIndex);  // Remove found directory from list. Without this, stuck in a loop.
            recursDirList(recurseItem, parentPath);  // Dive down into the found directory(recurseItem), with it's parent's full path(parentPath).
        }else{	// If directory not found...
            recIndex = browsePaths.indexOf(rx2);  // Find any leftovers. Leftovers are directories that contain only directories in them, though further sub-directories may contain music.
            if (recIndex != -1) {  // If leftover found...
                QString dirNameWithSubs = browsePaths.at(recIndex).right(browsePaths.at(recIndex).size() - parentPath.size());  // Chop parent path from found directory's full path.
                QString dirName = dirNameWithSubs.remove(rx3);  // Chop sub-dirs from the end of found directory.
                QStandardItem *recurseItem = new QStandardItem(QString(dirName));  // An item is created with current found directory name.
                parentItem->appendRow(recurseItem);  // Add item as child(subdir) to parent directory.
                recursDirList(recurseItem, parentPath);  // Dive down into the found directory(recurseItem), with it's parent's full path(parentPath).
            }
        }
    }
}

void Musicdexer::on_treeView_doubleClicked(const QModelIndex &index)
{
    QString windir = "/";
    QString selectedDir = "";
    QModelIndex MI = index;
    while (!MI.data().toString().isEmpty()) {
        selectedDir = MI.data().toString() + "/" + selectedDir;
        MI = MI.parent();
    }
    #ifndef Q_OS_WIN32
        windir = "";
    #endif
    QString songsInDirAndSubs = musicpath + windir + selectedDir.remove(0, selectedDir.indexOf("/") + 1) + "%";
    QString minusSubs = songsInDirAndSubs.replace("'", "''") + "/%";
    QString selectDirFilter = "Path LIKE '" + songsInDirAndSubs + "' AND Path NOT LIKE '" + minusSubs + "'";
    QSqlQuery selectDirSql;
    dbmodel->setFilter(selectDirFilter);
}

void Musicdexer::on_actionSync_triggered()     // Music Sync: Synchronize database to music folders.
{

    if (!QFile::exists(musicpath)) {
        ui->statusBar->showMessage("Please set a music collection folder using the folder icon at the top.");
        return;
    }
    ui->statusBar->showMessage("Starting music sync. Updating database with any file changes.", 10000);
    // Initialize variables
    QStringList curfiles; // Holds list of all files currently in music directory.
    int curfindex = 0; // Index used for curfiles.
    int curfsize = 0; // Current number of files in curfiles.
    QSqlQuery syncQuery;
    syncQuery.setForwardOnly(true); // Reduce memory use.
    QSqlQuery updateQuery;

    QProgressDialog prog("Caching files...", "Cancel", 0, 1, this);
    prog.setWindowModality(Qt::WindowModal);
    prog.setMinimumDuration(0);
    prog.show();
    prog.setValue(curfsize);

    // Fill curfiles.
    QDirIterator dirIt(musicpath, QDirIterator::Subdirectories);
    // QDirIterator supports sub-directories but not filtering(mfType) or sort(curfiles).
    QStringList mfType;  // Music file type filtering
    mfType << "mp3" << "ogg" << "wma" << "wav" << "mid" << "midi" << "aac" << "mpc" << "mp4" << "asf" << "umx";
    for ( int i = 0; dirIt.hasNext(); i++ ) {
        dirIt.next();
        if (!dirIt.fileInfo().isDir()) {
            if (mfType.contains(dirIt.fileInfo().suffix(), Qt::CaseInsensitive)) {
                curfiles << dirIt.filePath();
            }
        }
        prog.setValue(i);
    }
    curfiles.sort(); // SQLite sorted the same way. MySQL needs "ORDER BY BINARY Path"
    curfsize = curfiles.size();


    prog.setLabelText(QString("Syncing database with files ( %1 )").arg(curfsize));
    prog.setMaximum(curfsize);
    syncQuery.exec("SELECT id, Path, Missing, Size, Track, Title, Album, Artist, Genre FROM tracks ORDER BY Path");
    // Proceed through each db record, while at the same time, moving through curfiles.
    while (syncQuery.next()) {
        if (prog.wasCanceled()) {
            return;
        }
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
        // Check to see if the file in db exists, if not, mark it missing, otherwise mark it not missing.
        int tmpIndex = curfiles.indexOf(dbPath, curfindex);
        if (tmpIndex == -1) {
            if (!dbmissing) { // Mark missing
                updateQuery.prepare("UPDATE tracks SET Missing = 1 WHERE id = ?");
                updateQuery.addBindValue(dbid);
                updateQuery.exec();
            }
            continue;
        }
        if (dbmissing) { // Mark not missing
            updateQuery.prepare("UPDATE tracks SET Missing = 0 WHERE id = ?");
            updateQuery.addBindValue(dbid);
            updateQuery.exec();
        }
        curfiles.removeAt(tmpIndex); // Remaining files in curfiles are to-be-added later.
        curfsize--;
        prog.setValue(prog.maximum() - curfsize);
        curfindex = tmpIndex;  // Start search at current index to save resources. db and curfiles must be sorted identically.

        QFileInfo curFile(dbPath);
        TagLib::FileRef f(QFile::encodeName(dbPath).constData());
        if (f.isNull()) { // File has no tag. Can't get Length either.
            if (dbSize != curFile.size()) { // Update db of new file size if size changed.
                updateQuery.prepare("UPDATE tracks SET Size = ? WHERE id = ?");
                updateQuery.addBindValue(curFile.size());
                updateQuery.addBindValue(dbid);
                updateQuery.exec();
            }
            continue;
        }

        // Get current tag values.
        int tagTnum = f.tag()->track();
        QString tagTitle = f.tag()->title().toCString();
        QString tagAlbum = f.tag()->album().toCString();
        QString tagArtist = f.tag()->artist().toCString();
        QString tagGenre = f.tag()->genre().toCString();
        int trkLength = f.audioProperties()->length();

        if ((dbTnum == tagTnum) && (dbTitle == tagTitle) && (dbAlbum == tagAlbum) && (dbArtist == tagArtist) && (dbGenre == tagGenre)) {
            // Nothing changed with tag, so only update Size and Length if size changed).
            if (dbSize != curFile.size()) {
                updateQuery.prepare("UPDATE tracks SET Size = ?, Length = ? WHERE id = ?");
                updateQuery.addBindValue(curFile.size());
                updateQuery.addBindValue(trkLength);
                updateQuery.addBindValue(dbid);
                updateQuery.exec();
            }
            continue; // The majority of files should hit this.
        }
        // From here on, tag data in the file has changed. Likely user intent, so update db.

        if (dbSize == curFile.size()) {
            // Size could change from an edited tag, but more likely from a replaced file. Update any missing tag data.
            bool savetag = false;
            if (tagTnum < 1 && dbTnum > 0) { f.tag()->setTrack(dbTnum); savetag = true; }
            if (tagTitle.isEmpty() && !(dbTitle.isEmpty() || dbTitle.isNull())) { f.tag()->setTitle(dbTitle.toStdWString()); savetag = true; }
            if (tagAlbum.isEmpty() && !(dbAlbum.isEmpty() || dbAlbum.isNull())) { f.tag()->setAlbum(dbAlbum.toStdWString()); savetag = true; }
            if (tagArtist.isEmpty() && !(dbArtist.isEmpty() || dbArtist.isNull())) { f.tag()->setArtist(dbArtist.toStdWString()); savetag = true; }
            if (tagGenre.isEmpty() && !(dbGenre.isEmpty() || dbGenre.isNull())) { f.tag()->setGenre(dbGenre.toStdWString()); savetag = true; }
            if (savetag == true) { f.save(); }
        }
        updateQuery.prepare("UPDATE tracks SET Track = ?, Title = ?, Album = ?, Artist = ?, Genre = ?, Size = ?, Length = ? WHERE id = ?");
        updateQuery.addBindValue(tagTnum);
        updateQuery.addBindValue(tagTitle);
        updateQuery.addBindValue(tagAlbum);
        updateQuery.addBindValue(tagArtist);
        updateQuery.addBindValue(tagGenre);
        updateQuery.addBindValue(curFile.size());
        updateQuery.addBindValue(trkLength);
        updateQuery.addBindValue(dbid);
        updateQuery.exec();
    }
    // ### End of syncQuery record loop ###

    prog.setLabelText("Checking for moved files...");
    ui->statusBar->showMessage("Checking for moved files...", 10000);
    // Proceed through each remaining file, see if it is a missing file that just changed location.
    // Goal: Try to skip unnecessary tests for close matches(make less cpu intensive).
    // Files without tags are dealt with first.
    for (int i = 0; i < curfsize; ++i) {
        if (prog.wasCanceled()) {
           return;
        }
        QFileInfo curFile(curfiles.at(i));
        QString likepath = "%/" + curFile.fileName();
        TagLib::FileRef f(QFile::encodeName(curfiles.at(i)).constData());
        if (f.isNull()) {
            // If curFile has no tag, has same size, and same filename as db then missing found.
            syncQuery.prepare("SELECT id FROM tracks WHERE Missing = 1 AND Size = ? AND Path LIKE ? ");
            syncQuery.addBindValue(curFile.size());
            syncQuery.addBindValue(likepath); // %/songname.mp3
            syncQuery.exec();
            if (syncQuery.next()) { // missing found
                updateQuery.prepare("UPDATE tracks SET Missing = 0, Path = ? WHERE id = ?");
                updateQuery.addBindValue(curfiles.at(i));
                updateQuery.addBindValue(syncQuery.value(0).toInt());
                updateQuery.exec();
                curfiles.removeAt(i); i--; // Remaining files in curfiles are to-be-added later.
                curfsize--;
                prog.setValue(prog.maximum() - curfsize);
                continue;
            }
            continue; //Add file in any other case for no tag.
        }

        // Get current tag values.
        int tagTnum = f.tag()->track();
        QString tagTitle = f.tag()->title().toCString();
        QString tagAlbum = f.tag()->album().toCString();
        QString tagArtist = f.tag()->artist().toCString();
        QString tagGenre = f.tag()->genre().toCString();
        int trkLength = f.audioProperties()->length();
        // NOTE update db size, because it may change from tag updates.

        // This should increase performance. 2 less queries to run for new files. 1 more to run for missings found.
        // TODO: Test with this removed under large load of missings found and/or new files.
        syncQuery.prepare("SELECT id FROM tracks WHERE Missing = 1 AND Album = ? OR Title = ? OR Size = ? OR Path LIKE ? LIMIT 1");
        syncQuery.addBindValue(tagAlbum);
        syncQuery.addBindValue(tagTitle);
        syncQuery.addBindValue(curFile.size());
        syncQuery.addBindValue(likepath);
        syncQuery.exec();
        if (!syncQuery.next()) {
            continue;
        }

        // ### ALBUM MATCH ###
        // Usually an Album can only contain one instance of each of the following: Track, Title, Size, filename.
        // So, if a missing file matches the Album, and one of those, then it is found.
        syncQuery.prepare("SELECT id, Track, Title, Length, Size, Path FROM tracks WHERE Missing = 1 AND Album = ? AND ( Track = ? OR Title = ? OR Size = ? OR Path LIKE ? )");
        syncQuery.addBindValue(tagAlbum);
        syncQuery.addBindValue(tagTnum);
        syncQuery.addBindValue(tagTitle);
        syncQuery.addBindValue(curFile.size());
        syncQuery.addBindValue(likepath);
        syncQuery.exec();
        if (syncQuery.next()) {
            // Each record is graded. Missing found only if ONE file is found with a higher grade than the rest.
            int aOldgrade = 0;
            int aGrade;
            int aID = -1;
            QString aPath = syncQuery.value(5).toString();
            aPath.remove(0, aPath.indexOf("/") + 1);
            do {
                aGrade = 0;
                if (syncQuery.value(2).toString() == tagTitle) {
                    if (tagTitle.isEmpty() || tagTitle.isNull()) { aGrade = 1; } else { aGrade = 3; }
                }
                if (aPath == curFile.fileName()) { aGrade = 3; }  // Title and filename about the same, so only count once out of both.
                if (syncQuery.value(1).toInt() == tagTnum) {
                    if (tagTnum == 0) { aGrade += 1; } else { aGrade += 3; }}
                if (syncQuery.value(3).toInt() == trkLength) { aGrade += 2; } // Same length and/or size not enough, so only increase by two.
                if (syncQuery.value(4).toInt() == curFile.size()) { aGrade += 2; }
                if (aGrade >= aOldgrade ) {
                    aID = syncQuery.value(0).toInt();
                    if (aGrade == aOldgrade) { aID = -1; } else { aOldgrade = aGrade; }
                }
            } while (syncQuery.next());
            if (aID > -1) {
                updateQuery.prepare("UPDATE tracks SET Missing = 0, Path = ?, Size = ?, Length = ?, Track = ?, Title = ?, Artist = ?, Genre = ? WHERE id = ?");
                updateQuery.addBindValue(curfiles.at(i));
                updateQuery.addBindValue(curFile.size());
                updateQuery.addBindValue(trkLength);
                updateQuery.addBindValue(tagTnum);
                updateQuery.addBindValue(tagTitle);
                updateQuery.addBindValue(tagArtist);
                updateQuery.addBindValue(tagGenre);
                updateQuery.addBindValue(aID);
                updateQuery.exec();
                curfiles.removeAt(i); i--; // Remaining files in curfiles are to-be-added later.
                curfsize--;
                prog.setValue(prog.maximum() - curfsize);
                continue;
            }
        }
        // From here on. file has tag, but Album doesn't match.
        //  NOTE: Album could be miss-labled/spelled. e.g., Album = 'Moviename - Soundtrack' vs 'Moviename (sound track)'
        //  Could try to add some detection for this and maybe prompt user.

        // ### SIZE MATCH ###
        // Size is a good indicator by itself, but to be safe, try one more of Length, Track, filename, or Title match.
        syncQuery.prepare("SELECT id, Track, Title, Length, Path FROM tracks WHERE Missing = 1 AND Size = ? AND ( Track = ? OR Title = ? OR Length = ? OR Path LIKE ? )");
        syncQuery.addBindValue(curFile.size());
        syncQuery.addBindValue(tagTnum);
        syncQuery.addBindValue(tagTitle);
        syncQuery.addBindValue(trkLength);
        syncQuery.addBindValue(likepath);
        syncQuery.exec();
        if (syncQuery.next()) {
            int sOldgrade = 0;
            int sGrade;
            int sID = -1;
            QString sPath = syncQuery.value(4).toString();
            sPath.remove(0, sPath.indexOf("/") + 1);
            do {
                sGrade = 0;
                if (syncQuery.value(2).toString() == tagTitle) {
                    if (tagTitle.isEmpty() || tagTitle.isNull()) { sGrade=1; } else { sGrade=3; }}
                if (sPath == curFile.fileName()) {
                    if (sGrade > 0) { sGrade+=1; } else { sGrade=3; }}
                if (syncQuery.value(1).toInt() == tagTnum) {
                    if (tagTnum == 0) { sGrade+=1; } else { sGrade+=2; }}
                if (syncQuery.value(3).toInt() == trkLength) { sGrade+=2; }
                if ( sGrade >= sOldgrade ) {
                    sID = syncQuery.value(0).toInt();
                    if (sGrade == sOldgrade) { sID = -1; } else { sOldgrade = sGrade; }
                }
            } while (syncQuery.next());
                if (sID > -1) {
                updateQuery.prepare("UPDATE tracks SET Missing = 0, Path = ?, Length = ?, Track = ?, Title = ?, Album = ?, Artist = ?, Genre = ? WHERE id = ?");
                updateQuery.addBindValue(curfiles.at(i));
                updateQuery.addBindValue(trkLength);
                updateQuery.addBindValue(tagTnum);
                updateQuery.addBindValue(tagTitle);
                updateQuery.addBindValue(tagAlbum);
                updateQuery.addBindValue(tagArtist);
                updateQuery.addBindValue(tagGenre);
                updateQuery.addBindValue(sID);
                updateQuery.exec();
                curfiles.removeAt(i); i--;
                curfsize--;
                prog.setValue(prog.maximum() - curfsize);
                continue;
            }
        }
        // From here on, Album and size don't match.

        // ### TITLE/FILENAME ###
        // Case: Album changed, tag info modified the size.
        // Case: File replaced with new file and Album spelled differently.
        // Title or filname match and 2 or more matches of Length, Track, or Artist.
        syncQuery.prepare("SELECT id, Track, Length, Artist FROM tracks WHERE Missing = 1 AND ( Title = ? OR Path LIKE ? ) AND ( Length = ? OR Track = ? OR Artist = ? ) ");
        syncQuery.addBindValue(tagTitle);
        syncQuery.addBindValue(likepath);
        syncQuery.addBindValue(trkLength);
        syncQuery.addBindValue(tagTnum);
        syncQuery.addBindValue(tagArtist);
        syncQuery.exec();
        if (syncQuery.next()) {
            int tOldgrade = 0;
            int tGrade;
            int tID = -1;
            do { // Missing found if 1 instance of all 3 match. if not then 1 instance of 2 match.
                tGrade = 0;
                if (syncQuery.value(1).toInt() == tagTnum) {
                    if (tagTnum == 0) { tGrade+=1; } else { tGrade+=3; }}
                if (syncQuery.value(2).toInt() == trkLength) { tGrade+=3; }
                if (syncQuery.value(3).toString() == tagArtist) {
                    if (tagArtist.isEmpty() || tagArtist.isNull()) { tGrade+=1; } else { tGrade+=3; }}
                if (tGrade > 4 && tGrade >= tOldgrade ) {
                    tID = syncQuery.value(0).toInt();
                    if (tGrade == tOldgrade) { tID = -1; } else { tOldgrade = tGrade; }
                }
            } while (syncQuery.next());
            if (tID > -1) {
                updateQuery.prepare("UPDATE tracks SET Missing = 0, Path = ?, Size = ?, Length = ?, Track = ?, Title = ?, Album = ?, Artist = ?, Genre = ? WHERE id = ?");
                updateQuery.addBindValue(curfiles.at(i));
                updateQuery.addBindValue(curFile.size());
                updateQuery.addBindValue(trkLength);
                updateQuery.addBindValue(tagTnum);
                updateQuery.addBindValue(tagTitle);
                updateQuery.addBindValue(tagAlbum);
                updateQuery.addBindValue(tagArtist);
                updateQuery.addBindValue(tagGenre);
                updateQuery.addBindValue(tID);
                updateQuery.exec();
                curfiles.removeAt(i); i--;
                curfsize--;
                prog.setValue(prog.maximum() - curfsize);
            }
        }
    // Anything left over will be added as new files
    }

    prog.setLabelText(QString("Adding %1 files").arg(curfsize));
    // ### Add remaining files. ###
    if (curfsize > 0) {
        for (int i = 0; i < curfsize; ++i) {
            if (prog.wasCanceled()) {
               return;
            }
            QFileInfo curFile(curfiles.at(i));
            TagLib::FileRef f(QFile::encodeName(curfiles.at(i)).constData());
            if (!f.isNull()) {
                updateQuery.prepare("INSERT INTO tracks (Path, Size, Track, Title, Album, Artist, Genre, Length) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
                updateQuery.addBindValue(curfiles.at(i));
                updateQuery.addBindValue(curFile.size());
                updateQuery.addBindValue(f.tag()->track());
                updateQuery.addBindValue(f.tag()->title().toCString());
                updateQuery.addBindValue(f.tag()->album().toCString());
                updateQuery.addBindValue(f.tag()->artist().toCString());
                updateQuery.addBindValue(f.tag()->genre().toCString());
                updateQuery.addBindValue(f.audioProperties()->length());
            }else{ // File missing or no tag support
                updateQuery.prepare("INSERT INTO tracks (Path, Size) "
                "VALUES (?, ?)");
                updateQuery.addBindValue(curfiles.at(i));
                updateQuery.addBindValue(curFile.size());
            }
            updateQuery.exec();
            prog.setValue(prog.maximum() - curfsize + i);
        }
        ui->statusBar->showMessage("Sync finished. Added " + QString::number(curfsize) + " files.", 10000);
    }else{
        ui->statusBar->showMessage("Sync finished. No new files.", 10000);
    }
    dbmodel->select();
    populateBrowser();
    prog.setValue(400);
    prog.setLabelText( "DONE" );
}

void Musicdexer::on_actionDirSelect_triggered()
{
    QFileDialog *dialog = new QFileDialog(this, tr("Select a music directory"), QDir::homePath(), tr("All Files"));
    dialog->setFileMode(QFileDialog::Directory); //Bug in my qt version. Supposed to show Directories AND files, but only shows Directories.
    //()dialog.setNameFilter(tr("Music (*.mp3 *.ogg *.wma *.aac *.flac *.m4p *.m4a *.wav *.aiff *.aif *.aifc)"));
    if (dialog->exec()) {
        musicpath = dialog->selectedFiles().takeFirst();
        if(dbmodelon) {
                dbmodel->setPathSize(musicpath.size());
        }
        settings->setValue("MusicPath", musicpath);
        settings->sync();
    }
}
