#-------------------------------------------------
#
# Project created by QtCreator 2014-11-08T14:42:57
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = musicdexer
TEMPLATE = app

SOURCES += src/customqsqltablemodel.cpp \
           src/main.cpp \
           src/musicdexer.cpp

HEADERS  += src/customqsqltablemodel.h \
            src/musicdexer.h

FORMS    += src/musicdexer.ui

RESOURCES += res/musicdexer.qrc

CONFIG(release, debug|release) {
    LFILES += -llibtag
} else {
    LFILES += -llibtagd
}

win32 {
    INCLUDEPATH += $$PWD/../taglib/include/taglib
    DEPENDPATH += $$PWD/../taglib/include/taglib
    LIBS += -L$$PWD/../taglib/lib/ $$LFILES
    installdlls.path = $$PWD/../musicdexer-release
    installdlls.files = \
        $$PWD/../taglib/bin/libtag.dll \
        $$PWD/../zlib/zlib1.dll \
        $$[QT_INSTALL_BINS]/Qt5Core.dll \
        $$[QT_INSTALL_BINS]/Qt5Gui.dll \
        $$[QT_INSTALL_BINS]/Qt5Sql.dll \
        $$[QT_INSTALL_BINS]/Qt5Widgets.dll \
        $$[QT_INSTALL_BINS]/icudt52.dll \
        $$[QT_INSTALL_BINS]/icuin52.dll \
        $$[QT_INSTALL_BINS]/icuuc52.dll \
        $$[QT_INSTALL_BINS]/libwinpthread-1.dll \
        $$[QT_INSTALL_BINS]/libgcc_s_dw2-1.dll \
        $$[QT_INSTALL_BINS]/libstd~1.dll # libstdc++-6.dll
# Used DOS name above since Qt INSTALLS doesn't quote paths with special characters.
    pluginplatforms.path = $$PWD/../musicdexer-release/platforms
    pluginplatforms.files = $$[QT_INSTALL_PLUGINS]/platforms/qwindows.dll
    pluginsqlite.path = $$PWD/../musicdexer-release/sqldrivers
    pluginsqlite.files = $$[QT_INSTALL_PLUGINS]/sqldrivers/qsqlite.dll
    target.path = $$PWD/../musicdexer-release
    INSTALLS += target installdlls pluginplatforms pluginsqlite
}

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += taglib
    menu.CONFIG = no_check_exist
    menu.extra = touch $${TARGET}.desktop; sh res/data/musicdexer.sh /usr/local/bin >$${TARGET}.desktop
    menu.path = /usr/share/applications/
    menu.files = $${TARGET}.desktop
    QMAKE_CLEAN += $${TARGET}.desktop
    target.path = /usr/local/bin
    INSTALLS += target menu
}
