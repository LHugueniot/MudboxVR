//#include <QtGui/QColor>
#include <mbvr_global.h>

using namespace mudbox;
//using namespace vr;

class mbvrNode : public Node
{
public:
    mbvrNode();
    ~mbvrNode();

    void OnEvent( const EventGate &cEvent );
    void Shutdown();

    aevent m_eEachTick;
    static vr::IVRSystem* vr_pointer;
    int framecount=0;
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

        static mbvrNode* MbvrNodePtr;
};

//--------------------------------------------General Functions--------------------------

void printMud(QString msg);
