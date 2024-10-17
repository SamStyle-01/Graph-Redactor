QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 precompile_header

PRECOMPILED_HEADER = \
    pch.h

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CSamField.cpp \
    CSamFrame.cpp \
    CSamMatrix.cpp \
    SamGraph.cpp \
    main.cpp

HEADERS += \
    CSamField.h \
    CSamFrame.h \
    CSamMatrix.h \
    SamGraph.h \
    pch.h

FORMS +=

RC_FILE = icon.rc
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
