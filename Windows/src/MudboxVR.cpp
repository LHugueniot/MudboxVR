#include "MudboxVR.h"


namespace mudbox {

	MB_PLUGIN("MudboxVR", "Test implementation of VR", "Lucien Hugueniot",
		"https://github.com/LHugueniot/MudboxVR", MudboxVR::Initializer);

	MBVRNode* MudboxVR::MBVRNodePtr = NULL;
	QGLContext * MBVRNode::MBVRGLContext = NULL;
	vr::IVRSystem * MBVRNode::vr_pointer = NULL;
	vr::IVRCompositor * MBVRNode::vr_compositor = NULL;
	//----------------------------------------------------------------------MudboxVR------------------------------------------------------------------------

	//void OutputGLState()
	//{
	//	glGetBooleanv(GL_ALPHA_TEST, );
	//}

	MudboxVR::MudboxVR()
	{}

	MudboxVR::~MudboxVR()
	{}

	void MudboxVR::Initializer()
	{

		MBVRNode::InitCompositor();

		Kernel()->Interface()->AddCallbackMenuItem(mudbox::Interface::menuEdit,
			QString::null, "Start VR", MudboxVR::startVR);

		Kernel()->RegisterWindowPlugin(new VRViewport(QString("PipelineToolBox"), QString("Pipeline Tool Box")));

		Kernel()->Interface()->AddCallbackMenuItem(mudbox::Interface::menuEdit,
			QString::null, "End VR", MudboxVR::endVR);

	}

	void MudboxVR::startVR()
	{
		printMud( "It's all ready to run the VR application!");
	
	    if (MBVRNodePtr == NULL)
	    {
			MBVRNodePtr = new MBVRNode;
		    printMud("MudboxVR has started.");
	    }
		if (MBVRNodePtr->Initialized == false)
		{
			endVR();
		}

	}

	void MudboxVR::endVR()
	{
		if(MBVRNodePtr!=NULL)
		{

		    logMud(MBVRNodePtr->framecount);
			logMud(MBVRNodePtr->tick);

		    delete MBVRNodePtr;
		    MBVRNodePtr=NULL;
		    
			printMud("MudboxVR has ended.");
		}
	}

}