MUDPATH=/usr/autodesk/mudbox2017/

QMAKE_CXXFLAGS+= -fPIC -fvisibility=hidden -fno-strict-aliasing -pthread -m64 -Wno-comment -fno-gnu-keywords -ftemplate-depth-25 -Wno-multichar -Wno-sign-compare -Wno-reorder -c -DUNIX -DFUNCPROTO -D_BOOL -DOSUnix_ -DBits64_ -DNDEBUG -DPCORE_STATIC -DIL_STATIC -fvisibility=hidden -fvisibility-inlines-hidden -Wno-deprecated -D_LANGUAGE_C_PLUS_PLUS -DQ_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED -DQT_TABLET_SUPPORT -DQT_THREAD_SUPPORT -DQT_NO_DEBUG -DMB_USE_QT -O3 -msse3 -mfpmath=sse -fexceptions -D_FILE_OFFSET_BITS=64 -Wno-deprecated -DRedHat8_

unix {
    libraryFiles.path=/usr/autodesk/mudbox2017/SDK/plugins
}

