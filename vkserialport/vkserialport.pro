QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    inputpacket.cpp \
    outputpacket.cpp \
    parameter.cpp \
    threadserialport.cpp \

HEADERS += \
    inputpacket.h \
    outputpacket.h \
    parameter.h \
    threadserialport.h \
    vktoolstypes.h

win32:INCLUDEPATH += D:/Development/Qt/vktoolslib
win32:LIBS+=  D:/Development/Qt/vktoolslib/vktreexml.lib


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

