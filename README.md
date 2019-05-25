# MudboxVR

home Mudbox Logs location: "/home/datlucien/Mudbox/2017/data/Logs"
home default plugins path: "/usr/autodesk/mudbox2017/bin/plugins"
home example plugins path: "/usr/autodesk/mudbox2017/SDK/plugins"
home SDK includes path:	   "/usr/autodesk/mudbox2017/SDK/include"

uni Mudbox Logs location: "/home/s4906706/Mudbox/2018/data/Logs"
uni default plugins path:  "/opt/autodesk/mudbox2018/bin/plugins"
uni example plugins path:  "/opt/autodesk/mudbox2018/SDK/plugins"
uni SDK includes path:     "/opt/autodesk/mudbox2018/SDK/include/"

plugin path environement variable: MUDBOX_PLUG_IN_PATH

requires OpenVR v1.0.15

mesh renderer path:
    /usr/autodesk/mudbox2017/SDK/include/Mudbox/renderer.h

windows logs path:

C:\Users\lucciano\Documents\Mudbox\2018-x64\data\Logs

cmake .. -G "Visual Studio 15 2017 Win64" -DCMAKE_PREFIX_PATH=C:\Qt\5.9.1\msvc2017_64\lib\cmake

C:\Users\lucciano\Documents\openvr-master\samples\hellovr_opengl
C:\Users\lucciano\Documents\openvr-master\samples\driver_sample

C:\ExtLibs\SDL2-2.0.9\lib\x64
C:\ExtLibs\GLEW\lib\Release\x64

..\SDL2-2.0.9\include
..\GLEW\include
..\openvr-master\headers

..\SDL2-2.0.9\lib\x64;..\GLEW\lib\Release\x64;..\openvr-master\lib\win64;