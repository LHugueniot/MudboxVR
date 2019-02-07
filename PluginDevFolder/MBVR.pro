QT       -= gui
TARGET = MudboxVR
TEMPLATE = lib
CONFIG += staticlib



DEFINES+= MUDBOXVR_LIBRARY

MYUSERNAME = $$system(echo $USER)
message($${MYUSERNAME})

SOURCES += MudboxVR.cpp

HEADERS += MudboxVR.h

contains(MYUSERNAME,datlucien) {
        include(HomeBuild/homebuild.pri)
        message("homebuild")

} else {
        include(UniBuild/unibuild.pri)
        message("unibuild")
        #message(MYUSERNAME)
}
LIBS += -L$${MUDPATH}SDK/lib -lMudboxFramework -lQtCore -lQtGui -lQtOpenGL -lQtXml -lQtNetwork -lCg -lCgGL -shared -lopenvr_api

INCLUDEPATH += .  .. $${MUDPATH}SDK/examples $${MUDPATH}SDK/include $${MUDPATH}SDK/include/QtCore $${MUDPATH}SDK/include/QtGui $${MUDPATH}SDK/include/QtOpenGL


unix {
    libraryFiles.files += libMudboxVR.so
    INSTALLS+= libraryFiles
}
