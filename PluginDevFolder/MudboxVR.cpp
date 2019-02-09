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
        Kernel()->Interface()->HUDMessageShow(tr("MudboxVR has started."), mudbox::Interface::HUDmsgFade );
    }
}

void MudboxVR::endVR()
{
    if(MbvrNodePtr!=NULL)
    {
        delete MbvrNodePtr;
        MbvrNodePtr=NULL;
        Kernel()->Interface()->HUDMessageShow(tr("MudboxVR has ended."), mudbox::Interface::HUDmsgFade );
    }
}

void MudboxVR::printMud()
{
    std::cout<<"environment works too";
    std::cout<<"it works?";
}


mbvrNode::mbvrNode() : m_eEachTick(this)
{
    m_eEachTick.Connect(Kernel()->ViewPort()->SceneRenderEvent);

    vr::EVRInitError eError = vr::VRInitError_None;
    vr_pointer = vr::VR_Init(&eError, vr::VRApplication_Background);
    if (eError != vr::VRInitError_None)
    {
        vr_pointer = NULL;
        printf("Unable to init VR runtime: %s \n",
        VR_GetVRInitErrorAsEnglishDescription(eError));
        exit(EXIT_FAILURE);
    }
}


