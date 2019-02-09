//#include <QtGui/QColor>
#include <mbvr_global.h>

using namespace mudbox;
//using namespace vr;

class mbvrNode : public Node
{
public:
    mbvrNode();
    void onEvent( const EventGate &cEvent );
    aevent m_eEachTick;
    static vr::IVRSystem* vr_pointer;
};

class MudboxVR
{
    Q_DECLARE_TR_FUNCTIONS(MudboxVR)

    public:
        MudboxVR();
        ~MudboxVR();
        static void Initializer();
        static void startVR();
        static void endVR();
        static void printMud();
        static mbvrNode* MbvrNodePtr;

};
