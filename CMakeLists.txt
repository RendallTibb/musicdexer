project(musicdexer)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(Taglib REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${TAGLIB_INCLUDE_DIRS})

if(UNIX AND NOT APPLE)
  if(NOT DESKTOP_ENTRY)
    set(DESKTOP_ENTRY musicdexer.desktop)
  endif(NOT DESKTOP_ENTRY)

  add_custom_command(OUTPUT ${DESKTOP_ENTRY}
    COMMAND touch ${DESKTOP_ENTRY}
    COMMAND sh ../musicdexer.sh ${CMAKE_INSTALL_PREFIX} >${DESKTOP_ENTRY}
    DEPENDS musicdexer.sh
    COMMENT "Generating desktop entry file"
    )
  add_custom_target(DESKTOP_ENTRY_FILE ALL
    DEPENDS ${DESKTOP_ENTRY}
  )
endif(UNIX AND NOT APPLE)

set(musicdexer_RESRCS res/musicdexer.qrc)
set(musicdexer_SRCS main.cpp musicdexer.cpp customqsqltablemodel.cpp)
qt4_add_resources(musicdexer_SRCS ${musicdexer_RESRCS})
qt4_automoc(${musicdexer_SRCS})
add_executable(musicdexer ${musicdexer_SRCS} ${musicdexer_RESRCS})
target_link_libraries(musicdexer ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY} ${TAGLIB_LIBRARIES})

if(UNIX AND NOT APPLE)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_ENTRY} DESTINATION /usr/share/applications)
endif(UNIX AND NOT APPLE)

install(TARGETS musicdexer DESTINATION bin)