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
INCLUDEPATH += C:\opencv3\build\include
LIBS += -LC:\opencv3\build\x86\mingw\lib \
    -lopencv_core320.dll \
    -lopencv_highgui320.dll \
    -lopencv_imgproc320.dll \
    -lopencv_imgcodecs320.dll \
    -lopencv_features2d320.dll \
    -lopencv_photo320.dll \
    -lopencv_videoio320.dll \
    -lopencv_flann320.dll \
    -lopencv_calib3d320.dll

INCLUDEPATH += C:\ffmpeg\include
LIBS += -LC:\ffmpeg\lib \
     -lavcodec \
     -lavdevice \
     -lavformat \
     -lavfilter \
     -lswresample \
     -lpostproc \
     -lswscale \
     -lavutil
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


macx|unix {
macx {
QT_CONFIG -= no-pkg-config
CONFIG  += link_pkgconfig
PKGCONFIG += opencv
}
INCLUDEPATH += /Users/bviecelli/opencv3/clang64/include
LIBS += -L/Users/bviecelli/opencv3/clang64/lib \
     -lopencv_imgproc \
     -lopencv_features2d \
     -lopencv_flann \
     -lopencv_objdetect \
     -lopencv_videoio \
     -lopencv_core\
     -lopencv_imgcodecs\
     -lopencv_highgui

INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib \
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
