QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 -static -static-libgcc
CONFIG += force_debug_info

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/mainwindow.cpp \
    src/main.cpp

HEADERS += \
    src/mainwindow.h

FORMS += \
    src/mainwindow.ui

INCLUDEPATH += /usr/local/include/opencv4
#INCLUDEPATH += /media/hassaku/68937f53-4516-4551-bb46-9542ffecf9a3/usr/local/include/opencv4/


#LIBS    += -L/usr/local/lib \
 #              -lopencv_core
#LIBS    += $(pkg-config --libs opencv4)
#LIBS+=-L/media/hassaku/68937f53-4516-4551-bb46-9542ffecf9a3/usr/local/lib
LIBS+=-lopencv_dnn
LIBS+=-lopencv_gapi
LIBS+=-lopencv_highgui
LIBS+=-lopencv_ml
LIBS+=-lopencv_objdetect
LIBS+=-lopencv_photo
LIBS+=-lopencv_stitching
LIBS+=-lopencv_video
LIBS+=-lopencv_calib3d
LIBS+=-lopencv_features2d
LIBS+=-lopencv_flann
LIBS+=-lopencv_videoio
LIBS+=-lopencv_imgcodecs
LIBS+=-lopencv_imgproc
LIBS+=-lopencv_core


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    bg.jpg \
    haarcascade_frontalface_alt.xml \
    rec.pro.user

