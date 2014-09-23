/********************************************************************************
** Form generated from reading UI file 'musicdexer.ui'
**
** Created: Sun Dec 4 22:41:17 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MUSICDEXER_H
#define UI_MUSICDEXER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTableView>
#include <QtGui/QToolBar>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Musicdexer
{
public:
    QAction *actionBackup;
    QAction *actionSync;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_Search;
    QPushButton *pbtnSearch;
    QPushButton *pbtnClear;
    QPushButton *pbtnDelete;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents_3;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout_SearchArea;
    QGridLayout *gridLayout_Query;
    QComboBox *cboxField;
    QLineEdit *lieditSearch;
    QComboBox *cboxOperator;
    QSpacerItem *horizontalSpacer_2;
    QTreeView *treeView;
    QGridLayout *gridLayout_Edit;
    QPushButton *pbtnChange;
    QComboBox *cboxFieldE;
    QLineEdit *lieditUpdate;
    QPushButton *pbtnAddto;
    QTableView *tableView;
    QToolBar *toolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Musicdexer)
    {
        if (Musicdexer->objectName().isEmpty())
            Musicdexer->setObjectName(QString::fromUtf8("Musicdexer"));
        Musicdexer->resize(997, 631);
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Musicdexer->sizePolicy().hasHeightForWidth());
        Musicdexer->setSizePolicy(sizePolicy);
        actionBackup = new QAction(Musicdexer);
        actionBackup->setObjectName(QString::fromUtf8("actionBackup"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/iconsandcursors/oxygen/22x22/actions/document-export-table.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionBackup->setIcon(icon);
        actionSync = new QAction(Musicdexer);
        actionSync->setObjectName(QString::fromUtf8("actionSync"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/iconsandcursors/oxygen/22x22/actions/view-refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSync->setIcon(icon1);
        centralwidget = new QWidget(Musicdexer);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        verticalLayout_Search = new QVBoxLayout();
        verticalLayout_Search->setObjectName(QString::fromUtf8("verticalLayout_Search"));
        verticalLayout_Search->setSizeConstraint(QLayout::SetNoConstraint);
        pbtnSearch = new QPushButton(centralwidget);
        pbtnSearch->setObjectName(QString::fromUtf8("pbtnSearch"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pbtnSearch->sizePolicy().hasHeightForWidth());
        pbtnSearch->setSizePolicy(sizePolicy1);

        verticalLayout_Search->addWidget(pbtnSearch);

        pbtnClear = new QPushButton(centralwidget);
        pbtnClear->setObjectName(QString::fromUtf8("pbtnClear"));
        sizePolicy1.setHeightForWidth(pbtnClear->sizePolicy().hasHeightForWidth());
        pbtnClear->setSizePolicy(sizePolicy1);

        verticalLayout_Search->addWidget(pbtnClear);

        pbtnDelete = new QPushButton(centralwidget);
        pbtnDelete->setObjectName(QString::fromUtf8("pbtnDelete"));
        sizePolicy1.setHeightForWidth(pbtnDelete->sizePolicy().hasHeightForWidth());
        pbtnDelete->setSizePolicy(sizePolicy1);

        verticalLayout_Search->addWidget(pbtnDelete);


        horizontalLayout->addLayout(verticalLayout_Search);

        scrollArea = new QScrollArea(centralwidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy2);
        scrollArea->setMinimumSize(QSize(0, 80));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents_3 = new QWidget();
        scrollAreaWidgetContents_3->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_3"));
        scrollAreaWidgetContents_3->setGeometry(QRect(0, 0, 500, 74));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(scrollAreaWidgetContents_3->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents_3->setSizePolicy(sizePolicy3);
        scrollAreaWidgetContents_3->setMinimumSize(QSize(0, 0));
        horizontalLayoutWidget = new QWidget(scrollAreaWidgetContents_3);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 0, 641, 81));
        horizontalLayout_SearchArea = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout_SearchArea->setObjectName(QString::fromUtf8("horizontalLayout_SearchArea"));
        horizontalLayout_SearchArea->setSizeConstraint(QLayout::SetNoConstraint);
        horizontalLayout_SearchArea->setContentsMargins(0, 0, 0, 0);
        gridLayout_Query = new QGridLayout();
        gridLayout_Query->setObjectName(QString::fromUtf8("gridLayout_Query"));
        gridLayout_Query->setSizeConstraint(QLayout::SetNoConstraint);
        cboxField = new QComboBox(horizontalLayoutWidget);
        cboxField->setObjectName(QString::fromUtf8("cboxField"));
        sizePolicy1.setHeightForWidth(cboxField->sizePolicy().hasHeightForWidth());
        cboxField->setSizePolicy(sizePolicy1);
        cboxField->setMinimumSize(QSize(90, 0));

        gridLayout_Query->addWidget(cboxField, 0, 0, 1, 1);

        lieditSearch = new QLineEdit(horizontalLayoutWidget);
        lieditSearch->setObjectName(QString::fromUtf8("lieditSearch"));
        sizePolicy1.setHeightForWidth(lieditSearch->sizePolicy().hasHeightForWidth());
        lieditSearch->setSizePolicy(sizePolicy1);

        gridLayout_Query->addWidget(lieditSearch, 1, 1, 1, 1);

        cboxOperator = new QComboBox(horizontalLayoutWidget);
        cboxOperator->setObjectName(QString::fromUtf8("cboxOperator"));

        gridLayout_Query->addWidget(cboxOperator, 1, 0, 1, 1);


        horizontalLayout_SearchArea->addLayout(gridLayout_Query);

        horizontalSpacer_2 = new QSpacerItem(0, 70, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_SearchArea->addItem(horizontalSpacer_2);

        scrollArea->setWidget(scrollAreaWidgetContents_3);

        horizontalLayout->addWidget(scrollArea);

        treeView = new QTreeView(centralwidget);
        treeView->setObjectName(QString::fromUtf8("treeView"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(treeView->sizePolicy().hasHeightForWidth());
        treeView->setSizePolicy(sizePolicy4);
        treeView->setMinimumSize(QSize(80, 40));
        treeView->setMaximumSize(QSize(200, 80));

        horizontalLayout->addWidget(treeView);

        gridLayout_Edit = new QGridLayout();
        gridLayout_Edit->setObjectName(QString::fromUtf8("gridLayout_Edit"));
        gridLayout_Edit->setSizeConstraint(QLayout::SetNoConstraint);
        pbtnChange = new QPushButton(centralwidget);
        pbtnChange->setObjectName(QString::fromUtf8("pbtnChange"));
        sizePolicy1.setHeightForWidth(pbtnChange->sizePolicy().hasHeightForWidth());
        pbtnChange->setSizePolicy(sizePolicy1);

        gridLayout_Edit->addWidget(pbtnChange, 1, 0, 1, 1);

        cboxFieldE = new QComboBox(centralwidget);
        cboxFieldE->setObjectName(QString::fromUtf8("cboxFieldE"));
        sizePolicy4.setHeightForWidth(cboxFieldE->sizePolicy().hasHeightForWidth());
        cboxFieldE->setSizePolicy(sizePolicy4);
        cboxFieldE->setMinimumSize(QSize(0, 0));

        gridLayout_Edit->addWidget(cboxFieldE, 0, 1, 1, 1);

        lieditUpdate = new QLineEdit(centralwidget);
        lieditUpdate->setObjectName(QString::fromUtf8("lieditUpdate"));
        sizePolicy4.setHeightForWidth(lieditUpdate->sizePolicy().hasHeightForWidth());
        lieditUpdate->setSizePolicy(sizePolicy4);

        gridLayout_Edit->addWidget(lieditUpdate, 1, 1, 1, 1);

        pbtnAddto = new QPushButton(centralwidget);
        pbtnAddto->setObjectName(QString::fromUtf8("pbtnAddto"));

        gridLayout_Edit->addWidget(pbtnAddto, 0, 0, 1, 1);


        horizontalLayout->addLayout(gridLayout_Edit);


        verticalLayout_2->addLayout(horizontalLayout);

        tableView = new QTableView(centralwidget);
        tableView->setObjectName(QString::fromUtf8("tableView"));
        QSizePolicy sizePolicy5(QSizePolicy::Ignored, QSizePolicy::Expanding);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(tableView->sizePolicy().hasHeightForWidth());
        tableView->setSizePolicy(sizePolicy5);
        tableView->setSortingEnabled(true);
        tableView->horizontalHeader()->setCascadingSectionResizes(true);

        verticalLayout_2->addWidget(tableView);

        Musicdexer->setCentralWidget(centralwidget);
        toolBar = new QToolBar(Musicdexer);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        Musicdexer->addToolBar(Qt::TopToolBarArea, toolBar);
        statusBar = new QStatusBar(Musicdexer);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        Musicdexer->setStatusBar(statusBar);

        toolBar->addAction(actionBackup);
        toolBar->addAction(actionSync);

        retranslateUi(Musicdexer);

        QMetaObject::connectSlotsByName(Musicdexer);
    } // setupUi

    void retranslateUi(QMainWindow *Musicdexer)
    {
        Musicdexer->setWindowTitle(QApplication::translate("Musicdexer", "Musicdexer", 0, QApplication::UnicodeUTF8));
        actionBackup->setText(QApplication::translate("Musicdexer", "Backup", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionBackup->setToolTip(QApplication::translate("Musicdexer", "Backup", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionSync->setText(QApplication::translate("Musicdexer", "Update Database", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionSync->setToolTip(QApplication::translate("Musicdexer", "Update Database", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pbtnSearch->setText(QApplication::translate("Musicdexer", "Search", 0, QApplication::UnicodeUTF8));
        pbtnClear->setText(QApplication::translate("Musicdexer", "Clear", 0, QApplication::UnicodeUTF8));
        pbtnDelete->setText(QApplication::translate("Musicdexer", "Delete", 0, QApplication::UnicodeUTF8));
        pbtnChange->setText(QApplication::translate("Musicdexer", "Change", 0, QApplication::UnicodeUTF8));
        pbtnAddto->setText(QApplication::translate("Musicdexer", "Add to", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("Musicdexer", "toolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Musicdexer: public Ui_Musicdexer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MUSICDEXER_H
