//#include <QtGui/QColor>
#if defined(JAMBUILD)
#include <Mudbox/mudbox.h>
#else
#include <Mudbox/mudbox.h>
#endif

#include <QObject>
#include <QGLWidget>
#include <QTimer>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <openvr/openvr.h>

using namespace mudbox;
using namespace vr;

class MudboxVR
{
    Q_DECLARE_TR_FUNCTIONS(MudboxVR);

    public:
        MudboxVR();
        ~MudboxVR();
        static void Initializer();
        static void printMud();
        static IVRSystem* vr_pointer;
};

