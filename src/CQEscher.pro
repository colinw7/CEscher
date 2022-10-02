TEMPLATE = app

TARGET = CQEscher

DEPENDPATH += .

QT += widgets

QMAKE_CXXFLAGS += \
-std=c++17 \
-DCQUTIL_PEN \
-DCQUTIL_BRUSH \
-DCQUTIL_FONT \
-DCQUTIL_IMAGE \
-DCBRUSH_IMAGE 

MOC_DIR = .moc

SOURCES += \
main.cpp \
CQEscher.cpp \
CEscher.cpp \

HEADERS += \
CQEscher.h \
CEscher.h

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
../../CQUtil/include \
../../CImageLib/include \
../../CFont/include \
../../CRenderer/qinclude \
../../CRenderer/include \
../../CFile/include \
../../CMath/include \
../../CStrUtil/include \
../../CUtil/include \
../../COS/include \

unix:LIBS += \
-L../../CQUtil/lib \
-L../../CRenderer/lib \
-L../../CImageLib/lib \
-L../../CFont/lib \
-L../../CConfig/lib \
-L../../CFile/lib \
-L../../CFileUtil/lib \
-L../../CMath/lib \
-L../../CStrUtil/lib \
-L../../CRegExp/lib \
-L../../CUtil/lib \
-L../../COS/lib \
-lCQUtil \
-lCQRenderer \
-lCRenderer \
-lCImageLib \
-lCFont \
-lCConfig \
-lCFile \
-lCFileUtil \
-lCMath \
-lCStrUtil \
-lCRegExp \
-lCUtil \
-lCOS \
-lpng -ljpeg -lfreetype -ltre
