QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    extserialport.cpp \
    inputpacket.cpp \
    outputpacket.cpp \
    parameter.cpp \

HEADERS += \
    extserialport.h \
    inputpacket.h \
    outputpacket.h \
    parameter.h \
    vktoolstypes.h

win32:INCLUDEPATH += D:/Development/Qt/vktoolslib
unix:INCLUDEPATH += /home/user/sai/develop/vktoolslib

win32:LIBS+=  D:/Development/Qt/vktoolslib/vktreexml.lib
unix:LIBS += -L/home/user/sai/build-vktoolslib-Desktop_Qt_5_7_1_GCC_64bit-Debug -lvktoolslib


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

