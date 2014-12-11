cmake_minimum_required(VERSION 2.8.7)

project(musicdexer)

if(POLICY CMP0020)
   cmake_policy(SET CMP0020 NEW)
endif(POLICY CMP0020)

set( MD_SRCS
     src/main.cpp 
     src/musicdexer.cpp
     src/customqsqltablemodel.cpp
)
set( MD_HDRS
     src/musicdexer.h
     src/customqsqltablemodel.h
)
set( MD_QRC
     res/musicdexer.qrc
)
set( MD_UI
     src/musicdexer.ui
)

find_package(Qt5Widgets)
find_package(Qt5Sql)
if(Qt5Widgets_FOUND)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    set(MD_RESRCS ${MD_QRC})
    set(Qt4_0R_5_LIB Qt5::Widgets Qt5::Sql)
else()
    message(STATUS "Qt5 not found, trying Qt4.")    
    find_package(Qt4 REQUIRED)
    qt4_add_resources(MD_RESRCS ${MD_QRC}) # creates cpp from resource file
    QT4_WRAP_UI(UI_HEADERS ${MD_UI}) # Creates headers from ui files.
    QT4_WRAP_CPP(MOC_FILES ${MD_HDRS}) # creates cpp moc files from header files with Q_OBJECT macro
    set(Qt4_OR_5_INC ${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
    set(Qt4_0R_5_LIB ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY})
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(Taglib)

If(WIN32)
    set(PF_WIN WIN32)
    if(NOT Taglib_FOUND)
	message(STATUS "Using ../taglib")
        set(TAGLIB_LIBRARIES ${PROJECT_SOURCE_DIR}/../taglib/lib/libtag.dll*)
        set(TAGLIB_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/../taglib/include/taglib)
    endif()
endif()


include_directories(${Qt4_OR_5_INC} ${TAGLIB_INCLUDE_DIRS})

add_executable(musicdexer ${PF_WIN} ${MD_SRCS} ${MD_RESRCS} ${MOC_FILES} ${UI_HEADERS})

target_link_libraries(musicdexer ${Qt4_0R_5_LIB} ${TAGLIB_LIBRARIES})

########################
# Linux Desktop Entry  #
########################

if(UNIX AND NOT APPLE)
    if(NOT DESKTOP_ENTRY)
        set(DESKTOP_ENTRY musicdexer.desktop)
    endif(NOT DESKTOP_ENTRY)

    set(TARGET_DEST bin)
    add_custom_command(OUTPUT ${DESKTOP_ENTRY}
        COMMAND touch ${DESKTOP_ENTRY}
        COMMAND sh ${PROJECT_SOURCE_DIR}/res/data/musicdexer.sh ${CMAKE_INSTALL_PREFIX}/${TARGET_DEST} >${DESKTOP_ENTRY}
        DEPENDS ${PROJECT_SOURCE_DIR}/res/data/musicdexer.sh
        COMMENT "Generating desktop entry file"
    )
    add_custom_target(DESKTOP_ENTRY_FILE ALL
        DEPENDS ${DESKTOP_ENTRY}
    )
endif(UNIX AND NOT APPLE)

#################
#    install    #
#################

if(WIN32)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/../musicdexer-release)
    set(QT_INSTALL_BINS ${_qt5Core_install_prefix}/bin)
    set(QT_INSTALL_PLUGINS ${_qt5Core_install_prefix}/plugins)
    install(
        FILES
        ${PROJECT_SOURCE_DIR}/../taglib/bin/libtag.dll
        ${PROJECT_SOURCE_DIR}/../zlib/zlib1.dll
        ${QT_INSTALL_BINS}/Qt5Core.dll
        ${QT_INSTALL_BINS}/Qt5Gui.dll
        ${QT_INSTALL_BINS}/Qt5Sql.dll
        ${QT_INSTALL_BINS}/Qt5Widgets.dll
        ${QT_INSTALL_BINS}/icudt52.dll
        ${QT_INSTALL_BINS}/icuin52.dll
        ${QT_INSTALL_BINS}/icuuc52.dll
        ${QT_INSTALL_BINS}/libwinpthread-1.dll
        ${QT_INSTALL_BINS}/libgcc_s_dw2-1.dll
        ${QT_INSTALL_BINS}/libstdc++-6.dll
	DESTINATION .
    )
    install(FILES ${QT_INSTALL_PLUGINS}/platforms/qwindows.dll DESTINATION platforms)
    install(FILES ${QT_INSTALL_PLUGINS}/sqldrivers/qsqlite.dll DESTINATION sqldrivers)
    install(TARGETS musicdexer DESTINATION .)
endif()

if(UNIX AND NOT APPLE)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_ENTRY} DESTINATION /usr/share/applications)
  install(TARGETS musicdexer DESTINATION ${TARGET_DEST})
endif()

