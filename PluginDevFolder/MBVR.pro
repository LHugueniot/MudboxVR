QT       -= gui
TARGET = MudboxVR
TEMPLATE = lib
#CONFIG += c++14

OTHER_FILES+= ../README.md

DEFINES+= MUDBOXVR_LIBRARY

MYUSERNAME = $$system(echo $USER)
message($${MYUSERNAME})

SOURCES += MudboxVR.cpp

HEADERS += MudboxVR.h \
    mbvr_global.h

contains(MYUSERNAME,datlucien) {
        include(HomeBuild/homebuild.pri)
        message("homebuild")

} else {
        include(UniBuild/unibuild.pri)
        message("unibuild")
}
LIBS += -L$${MUDPATH}SDK/lib -lMudboxFramework -lQtCore -lQtGui -lQtOpenGL -lQtXml -lQtNetwork -lCg -lCgGL -shared -lopenvr_api

INCLUDEPATH += .  .. $${MUDPATH}SDK/examples $${MUDPATH}SDK/include $${MUDPATH}SDK/include/QtCore $${MUDPATH}SDK/include/QtGui $${MUDPATH}SDK/include/QtOpenGL

#QMAKE_CXXFLAGS+=-std=c++11

unix {
    libraryFiles.files += $$PWD/libMudboxVR.so
    message($$libraryFiles.files)
    message($$libraryFiles.path)
    INSTALLS+= libraryFiles
}
