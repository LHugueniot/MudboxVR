#include "MudboxVR.h"

MB_PLUGIN( "MudboxVR", "Test implementation of VR", "Lucien Hugueniot",
"https://github.com/LHugueniot/MudboxVR", MudboxVR::Initializer );

void MudboxVR::Initializer()
{
    Kernel()->Interface()->AddCallbackMenuItem(mudbox::Interface::menuEdit, 
    QString::null, tr("Project to VR"), MudboxVR::printMud);

	Kernel()->Interface()->AddCallbackMenuItem(mudbox::Interface::menuEdit, 
    QString::null, tr("Destroy the VR"),  MudboxVR::printMud);
    
    vr_pointer= NULL;
    EVRInitError eError = VRInitError_None;
    vr_pointer = VR_Init(&eError, VRApplication_Background);
    if (eError != VRInitError_None)
    {
	    vr_pointer = NULL;
	    printf("Unable to init VR runtime: %s \n", 
		VR_GetVRInitErrorAsEnglishDescription(eError));
	    exit(EXIT_FAILURE);
    }
    
}

void MudboxVR::printMud()
{
    std::cout<<"it works?";
}