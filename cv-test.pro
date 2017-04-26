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
INCLUDEPATH += /usr/local/Cellar/opencv3/3.2.0/include
LIBS += -L/usr/local/Cellar/opencv3/3.2.0/lib \
     -lopencv_imgproc \
     -lopencv_features2d \
     -lopencv_flann \
     -lopencv_objdetect \
     -lopencv_videoio \
     -lopencv_core\
     -lopencv_imgcodecs\
     -lopencv_highgui

INCLUDEPATH += /usr/local/Cellar/ffmpeg/3.3/include
LIBS += -L/usr/local/Cellar/ffmpeg/3.3/lib \
     -lavcodec \
     -lavdevice \
     -lavformat \
     -lavfilter \
     -lswresample \
     -lpostproc \
     -lswscale \
     -lavutil
}

SOURCES += main.cpp\
        mainwindow.cpp \
    camframe.cpp \
    dialogeditaddr.cpp

HEADERS  += mainwindow.h \
    camframe.h \
    dialogeditaddr.h

FORMS    += mainwindow.ui \
    dialogeditaddr.ui
