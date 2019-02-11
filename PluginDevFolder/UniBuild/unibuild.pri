MUDPATH=/opt/autodesk/mudbox2018/

MYLIBS=/home/s4906706/mylibs
OPENVRPATH= $${MYLIBS}/libfiles/openvr/

LIBS += -L$${OPENVRPATH}bin/linux64
INCLUDEPATH += $${OPENVRPATH}headers

QMAKE_CXXFLAGS+= -fPIC -fvisibility=hidden -fno-strict-aliasing -pthread -m64 -Wno-comment -fno-gnu-keywords -Wno-multichar -Wno-sign-compare -Wno-reorder -c -DUNIX -DFUNCPROTO -D_BOOL -DOSUnix_ -DBits64_ -DNDEBUG -DPCORE_STATIC -DIL_STATIC -fvisibility=hidden -fvisibility-inlines-hidden -Wno-deprecated -D_LANGUAGE_C_PLUS_PLUS -DQ_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -DQT_TABLET_SUPPORT -DQT_THREAD_SUPPORT -DQT_NO_DEBUG -DMB_USE_QT -O3 -msse3 -mfpmath=sse -fexceptions -D_FILE_OFFSET_BITS=64 -Wno-deprecated -DRedHat8_

unix {
    libraryFiles.path=../plugins
}
