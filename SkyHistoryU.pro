TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    skymodel.cpp \
    skyproxymodel.cpp

RESOURCES += qml.qrc

LITE_SQL_SRC=/home/ilia/Development/orm/litesql-git
LITE_SQL_CMAKE=/home/ilia/Development/orm/cmake_liteSQL

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =


INCLUDEPATH += $$LITE_SQL_SRC/include \
              $$LITE_SQL_SRC/src/examples
LIBS += $$LITE_SQL_CMAKE/src/library/liblitesql_sqlite.a \
        $$LITE_SQL_CMAKE/src/library/liblitesql-util.a \
        $$LITE_SQL_CMAKE/src/library/liblitesql.a

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    skymodel.h \
    skyproxymodel.h

CONFIG += c++11
