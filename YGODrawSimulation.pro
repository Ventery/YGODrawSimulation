QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
 # You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_POST_LINK +=

SOURCES += \
    CSqliteOperator.cpp \
    DeckWidget.cpp \
    HistoryManager.cpp \
    SearchWidget.cpp \
    YGOCardPushButton.cpp \
    YGODataManager.cpp \
    YGODrawSimulation.cpp \
    main.cpp \
    sqlite/shell.c \
    sqlite/sqlite3.c

HEADERS += \
    CSqliteOperator.h \
    Common.h \
    DeckWidget.h \
    HistoryManager.h \
    Notifier.h \
    SearchWidget.h \
    Singleton.h \
    YGOCardPushButton.h \
    YGODataManager.h \
    YGODrawSimulation.h \
    noncopyable.h \
    sqlite/sqlite3.h \
    sqlite/sqlite3ext.h

TRANSLATIONS += \
    YGODrawSimulation_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
