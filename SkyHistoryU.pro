TEMPLATE = app

QT += qml quick widgets

isEmpty(LITE_SQL_SRC) : LITE_SQL_SRC=/home/ilia/Development/orm/litesql-git
isEmpty(LITE_SQL_CMAKE) : LITE_SQL_CMAKE=/home/ilia/Development/orm/cmake_liteSQL

SOURCES += \
    SkyHistoryMain.cpp \
    skymodel.cpp       \
    skyproxymodel.cpp  \
    $$LITE_SQL_SRC/src/examples/main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =


INCLUDEPATH += $$LITE_SQL_SRC/include \
              $$LITE_SQL_SRC/src/examples
LIBS += \
        -L$$LITE_SQL_CMAKE/src/library -llitesql

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    skymodel.h \
    skyproxymodel.h

CONFIG += c++11
#DEFINES += QT_NO_DEBUG_OUTPUT
