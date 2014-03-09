#-------------------------------------------------
#
# Project created by QtCreator 2014-01-08T17:10:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = temp-18
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


INCLUDEPATH+=E:\opencv\include\opencv\
                    E:\opencv\include\opencv2\
                    E:\opencv\include

LIBS+=E:\opencv\lib\libopencv_calib3d245.dll.a\
        E:\opencv\lib\libopencv_contrib245.dll.a\
        E:\opencv\lib\libopencv_core245.dll.a\
        E:\opencv\lib\libopencv_features2d245.dll.a\
        E:\opencv\lib\libopencv_flann245.dll.a\
        E:\opencv\lib\libopencv_gpu245.dll.a\
        E:\opencv\lib\libopencv_highgui245.dll.a\
        E:\opencv\lib\libopencv_imgproc245.dll.a\
        E:\opencv\lib\libopencv_legacy245.dll.a\
        E:\opencv\lib\libopencv_ml245.dll.a\
        E:\opencv\lib\libopencv_objdetect245.dll.a\
        E:\opencv\lib\libopencv_video245.dll.a
