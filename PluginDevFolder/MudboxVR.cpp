#include "MudboxVR.h"

MB_PLUGIN( "MudboxVR", "Test implementation of VR", "Lucien Hugueniot",
"https://github.com/LHugueniot/MudboxVR", MudboxVR::Initializer );

mbvrNode* MudboxVR::MbvrNodePtr=NULL;
vr::IVRSystem* mbvrNode::vr_pointer=NULL;

MudboxVR::MudboxVR()
{

}

MudboxVR::~MudboxVR()
{}

void MudboxVR::Initializer()
{
    Kernel()->Interface()->AddCallbackMenuItem(mudbox::Interface::menuEdit, 
    QString::null, tr("Start VR"), MudboxVR::startVR);

	Kernel()->Interface()->AddCallbackMenuItem(mudbox::Interface::menuEdit, 
    QString::null, tr("End VR"),  endVR);
        
}

void MudboxVR::startVR()
{
    if(MbvrNodePtr==NULL)
    {
        MbvrNodePtr=new mbvrNode;

        printMud("MudboxVR has started.");
    }
}

void MudboxVR::endVR()
{
    if(MbvrNodePtr!=NULL)
    {
        printMud(std::to_string(MbvrNodePtr->framecount).c_str());
        delete MbvrNodePtr;
        MbvrNodePtr=NULL;
        //Kernel()->Interface()->HUDMessageShow(tr("MudboxVR has ended."), mudbox::Interface::HUDmsgFade );
        //printMud("MudboxVR has ended.");
    }
}

mbvrNode::mbvrNode() : m_eEachTick(this)
{
    m_eEachTick.Connect(Kernel()->ViewPort()->FrameEvent);

    vr::EVRInitError eError = vr::VRInitError_None;
    vr_pointer = vr::VR_Init(&eError, vr::VRApplication_Background);
    if (eError != vr::VRInitError_None)
    {
        vr_pointer = NULL;
        printf("Unable to init VR runtime: %s \n",
        VR_GetVRInitErrorAsEnglishDescription(eError));

        return;
    }
    printMud("vr initiation worked");
}

mbvrNode::~mbvrNode()
{
    Shutdown();
}

void mbvrNode::Shutdown()
{
    if (vr_pointer != NULL)
    {
        vr::VR_Shutdown();
        vr_pointer = NULL;
        printMud("Shutting VR down");
    }
    printMud("THE VR NEVER EXISTED");
}

void mbvrNode::OnEvent(const EventGate &cEvent)
{
    if(cEvent==m_eEachTick)
    {
        framecount++;
        //printMud(std::to_string(framecount).c_str());
    }
}



//--------------------------------------------General Functions--------------------------


void printMud(QString msg)//,...)
{
//    va_list ap;
//    QString i;

//    va_start(ap, msg);
//      for (i = msg; i >= 0; i = va_arg(ap, char))
//        printf("%d ", i);
//      va_end(ap);
//      putchar('\n');



    Kernel()->Interface()->HUDMessageShow(msg, mudbox::Interface::HUDmsgFade );
}
