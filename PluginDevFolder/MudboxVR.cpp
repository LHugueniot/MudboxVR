//#include "MudboxVR.h"
#include "mudprint.cpp"

#ifndef _MBVR_GLOBAL
#include <mbvr_global.h>
#endif

MB_PLUGIN( "MudboxVR", "Test implementation of VR", "Lucien Hugueniot",
"https://github.com/LHugueniot/MudboxVR", MudboxVR::Initializer );

mbvrNode* MudboxVR::MbvrNodePtr=NULL;


//----------------------------------------------------------------------MudboxVR------------------------------------------------------------------------

MudboxVR::MudboxVR()
{}

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
        MbvrNodePtr->vr_pointer=NULL;
        //printMud("MudboxVR has started.");
    }

    if (vr::VR_IsHmdPresent() && vr::VR_IsRuntimeInstalled())
    {
        printmud( "It's all ready to run the VR application!");
    }
    else
    {
        printmud("HMDs are not present");
    }
}

void MudboxVR::endVR()
{
    if(MbvrNodePtr!=NULL)
    {
        printmud(MbvrNodePtr->framecount);
        MbvrNodePtr->vr_pointer=NULL;
        delete MbvrNodePtr;
        MbvrNodePtr=NULL;
        //Kernel()->Interface()->HUDMessageShow(tr("MudboxVR has ended."), mudbox::Interface::HUDmsgFade );
        //printMud("MudboxVR has ended.");
    }
}

//----------------------------------------------------------------------mbvrNode------------------------------------------------------------------------

mbvrNode::mbvrNode() : m_eEachTick(this)
{
    m_eEachTick.Connect(Kernel()->ViewPort()->FrameEvent);

    vr::EVRInitError eError = vr::VRInitError_None;
    vr_pointer = vr::VR_Init(&eError, vr::VRApplication_Scene);
    if (eError != vr::VRInitError_None)
    {
        vr_pointer = NULL;
        printmud("Unable to init VR runtime: %s \n");
        printmud(VR_GetVRInitErrorAsEnglishDescription(eError));

        return;
    }
    printmud("vr initiation worked");
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
        printmud("Shutting VR down");
    }
    printmud("THE VR NEVER EXISTED");
}

void mbvrNode::OnEvent(const EventGate &cEvent)
{
    if(cEvent==m_eEachTick)
    {
        framecount++;
    }
}
