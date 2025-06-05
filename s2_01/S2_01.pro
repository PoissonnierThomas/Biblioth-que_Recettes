QT += core gui widgets

CONFIG += c++17

TARGET = S2_01
TEMPLATE = app

# Définir les chemins
INCLUDEPATH += .
DEPENDPATH += .

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ingredient.cpp \
    recette.cpp

HEADERS += \
    mainwindow.h \
    ingredient.h \
    recette.h

RESOURCES += \
    ressources.qrc

# Configuration de build
CONFIG += lrelease
CONFIG += embed_translations

TRANSLATIONS += \
    S2_01_en_FR.ts

win32 {
    QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
}

# Rules de déploiement
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
