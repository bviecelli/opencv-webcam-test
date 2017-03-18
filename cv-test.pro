#-------------------------------------------------
#
# Project created by QtCreator 2017-03-17T16:41:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cv-test
TEMPLATE = app

win32 {
INCLUDEPATH += C:\opencv\build\include
LIBS += -LC:\opencv\build\x86\vc14\lib \
    -lopencv_core2413 \
    -lopencv_highgui2413 \
    -lopencv_imgproc2413 \
    -lopencv_features2d2413 \
    -lopencv_nonfree2413 \
    -lopencv_flann2413 \
    -lopencv_calib3d2413
}

macx|unix {
macx {
QT_CONFIG -= no-pkg-config
CONFIG  += link_pkgconfig
PKGCONFIG += opencv
}
INCLUDEPATH += /usr/local/Cellar/opencv/2.4.13.2/include
LIBS += -L/usr/local/Cellar/opencv/2.4.13.2 \
     -lopencv_core.2.4 \
     -lopencv_imgproc.2.4 \
     -lopencv_features2d.2.4 \
     -lopencv_highgui.2.4
}

SOURCES += main.cpp\
        mainwindow.cpp \
    camframe.cpp

HEADERS  += mainwindow.h \
    camframe.h

FORMS    += mainwindow.ui
