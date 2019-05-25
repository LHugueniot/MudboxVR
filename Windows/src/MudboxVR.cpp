#include "MudboxVR.h"


namespace mudbox {

	MB_PLUGIN("MudboxVR", "Test implementation of VR", "Lucien Hugueniot",
		"https://github.com/LHugueniot/MudboxVR", MudboxVR::Initializer);

	mbvrNode* MudboxVR::MbvrNodePtr = NULL;
	vr::IVRSystem * mbvrNode::vr_pointer = NULL;
	vr::IVRCompositor * mbvrNode::vr_compositor = NULL;
	//----------------------------------------------------------------------MudboxVR------------------------------------------------------------------------

	MudboxVR::MudboxVR()
	{}

	MudboxVR::~MudboxVR()
	{}

	void MudboxVR::Initializer()
	{

		mbvrNode::InitCompositor();

		Kernel()->Interface()->AddCallbackMenuItem(mudbox::Interface::menuEdit,
			QString::null, "Start VR", MudboxVR::startVR);

		Kernel()->RegisterWindowPlugin(new VRViewport(QString("PipelineToolBox"), QString("Pipeline Tool Box")));

		Kernel()->Interface()->AddCallbackMenuItem(mudbox::Interface::menuEdit,
			QString::null, "End VR", MudboxVR::endVR);

	}

	void MudboxVR::startVR()
	{
		printMud( "It's all ready to run the VR application!");
	
	    if (MbvrNodePtr == NULL)
	    {
			MbvrNodePtr = new mbvrNode;
		    printMud("MudboxVR has started.");
	    }
		if (MbvrNodePtr->Initialized == false)
		{
			endVR();
		}

	}

	void MudboxVR::endVR()
	{
		if(MbvrNodePtr!=NULL)
		{

		    logMud(MbvrNodePtr->framecount);
			logMud(MbvrNodePtr->tick);

		    delete MbvrNodePtr;
		    MbvrNodePtr=NULL;
		    
			printMud("MudboxVR has ended.");
		}
	}

}