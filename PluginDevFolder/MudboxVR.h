//#include <QtGui/QColor>
#if defined(JAMBUILD)
#include <Mudbox/mudbox.h>
#else
#include </opt/autodesk/mudbox2018/SDK/include/Mudbox/mudbox.h>
#endif

#include <QObject>
#include <QGLWidget>
#include <QTimer>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace mudbox;

class MudboxVR
{
    Q_DECLARE_TR_FUNCTIONS(MudboxVR);
    public:
        static void Initializer();
        static void printMud();
};

