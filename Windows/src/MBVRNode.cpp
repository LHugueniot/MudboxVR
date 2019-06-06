#include "MBVRNode.h"

namespace mudbox {


	//-------------------------------------------------------------
	//--					MBVRNode Constructor				 --
	//-------------------------------------------------------------

	MBVRNode::MBVRNode() :	m_eEachTick(this),
							vertexArrObj(QGLBuffer::VertexBuffer),
							vertexIndexObj(QGLBuffer::IndexBuffer),
							vertexColoursObj(QGLBuffer::VertexBuffer),
							WorldSpaceAdjuster (
							100.f, 0.0f, 0.0f, 0.0f,
							0.0f, 100.f, 0.0f, 0.0f,
							0.0f, 0.0f, 100.f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f
							)

	{
		m_eEachTick.Connect(Kernel()->ViewPort()->PostRenderEvent);
		//Usually postrenderEvent

		DEBUG("Initiating VR\n");
		PRINT("Initiating VR");

		Initialized = InitializeMBVR();

		
		//vr::VRInput()->SetActionManifestPath( "C:/Users/lucciano/Documents/UniStuff/MudboxVR/Windows/VRActionSets/mudboxvr_actions.json" );

	}

	//-------------------------------------------------------------
	//--					MBVRNode Destructor				     --
	//-------------------------------------------------------------

	MBVRNode::~MBVRNode()
	{
		Shutdown();
	}

	bool MBVRNode::InitializeMBVR()
	{
		if (!InitVR())
		{
			DEBUG("Initialisation has failed");
			PRINT("Initialisation has failed");

			return false;
		}

		DEBUG("Initiating Compositor\n");
		PRINT("Initiating Compositor");
		if (!InitCompositor())
		{
			DEBUG("Initialisation has failed");
			PRINT("Initialisation has failed");

			return false;
		}

		DEBUG("Initiating OGL\n");
		PRINT("Initiating OGL");
		if (!InitOGL())
		{
			DEBUG("Initialisation has failed");
			PRINT("Initialisation has failed");
		
			return false;
		}

		DEBUG("Initiating VRActions\n");
		PRINT("Initiating VRActions");
		if (!InitVRActions())
		{
			DEBUG("Initialisation has failed");
			PRINT("Initialisation has failed");

			return false;
		}

		DEBUG("Initialisation has succeeded\n");
		PRINT("Initialisation has succeeded");

		return true;
	}

	//-------------------------------------------------------------
	//-- 			Shuts down the VR and any GL dependencies    --
	//-------------------------------------------------------------

	void MBVRNode::Shutdown()
	{
		PRINT("Shutting VR down");
		DEBUG("Shutting VR down");

		if ((vr_pointer != NULL) && (vr_compositor != NULL))
		{
			vr::VR_Shutdown();
			vr_pointer = NULL;
			vr_compositor = NULL;
			PRINT("VR_Shutdown");
			DEBUG("VR_Shutdown");
		}
		else
		{
			PRINT("vr_pointer is null");
			DEBUG("vr_pointer is null");
			PRINT("THE VR NEVER EXISTED");
		}

		PRINT(framecount);
		DEBUG("The framecount was: \n");
		DEBUG(framecount);

		if (Original_pos == NULL)
		{
			DEBUG("delete Original_pos");
			delete Original_pos;
		}

		if (m_leftEyeTexture == NULL)
		{
			DEBUG("delete m_leftEyeTexture");
			delete m_leftEyeTexture;
		}

		if (m_rightEyeTexture == NULL)
		{
			DEBUG("delete m_rightEyeTexture");
			delete m_rightEyeTexture;
		}

	}

	//-----------------------------------------------------------------------------
	//	MudBox Stuff
	//-----------------------------------------------------------------------------

	//-------------------------------------------------------------
	//-- 			Gets called each frameEvent					 --
	//-------------------------------------------------------------

	void MBVRNode::OnEvent(const EventGate &cEvent)
	{
		PRINT("On event");

		if (cEvent == m_eEachTick)
		{
			tick++;
		}

		if (cEvent == frameUpdate)
		{
		}

		if ((vr_pointer != NULL && vr_compositor!=NULL) &&  Initialized == true)
		{
			BREAKPOINT();
			VRContrHandleInputs();
			logMatrix(m_rHand[Right].m_rmat4Pose, "m_rHand[Right].m_rmat4Pose" );
			BREAKPOINT();
			RenderStereoTargets();
		}
		Kernel()->ViewPort()->Redraw();

		PRINT(m_mbvrMeshes.size());

		UpdateMBVRMeshes();

		framecount++;

	}

	//-----------------------------------------------------------------------------
	// VR Stuff
	//-----------------------------------------------------------------------------

	bool MBVRNode::InitVR()
	{
		vr::EVRInitError eError = vr::VRInitError_None;

		if (vr_pointer == NULL)
		{
			vr_pointer = vr::VR_Init(&eError, vr::VRApplication_Scene);
			PRINT(VR_GetVRInitErrorAsEnglishDescription(eError));
		}
		if (eError != vr::VRInitError_None)
		{
			vr_pointer = NULL;
			PRINT("Unable to init VR runtime: \n");
			PRINT(VR_GetVRInitErrorAsEnglishDescription(eError));
			DEBUG("Unable to init VR runtime: \n");
			DEBUG(VR_GetVRInitErrorAsEnglishDescription(eError));
			return false;
		}
		
		
		PRINT("VR Initiation worked");
		DEBUG("VR Initiation worked\n");
		return true;
	}

	bool MBVRNode::InitCompositor()
	{
		vr::EVRInitError eError = vr::VRInitError_None;

		BREAKPOINT();

		vr_compositor = (vr::IVRCompositor *)vr::VR_GetGenericInterface(vr::IVRCompositor_Version, &eError);

		BREAKPOINT();
		
		if (eError != vr::VRInitError_None)
		{
			PRINT("Unable to init Compositor: \n");
			PRINT(VR_GetVRInitErrorAsEnglishDescription(eError));
			DEBUG("Unable to init Compositor: \n");
			DEBUG(VR_GetVRInitErrorAsEnglishDescription(eError));
		}

		BREAKPOINT();

		if (vr_compositor ==NULL)
		{
			PRINT("Compositor initialization failed. See log file for details\n");
			DEBUG("Compositor initialization failed.\nError:\n");
			return false;
		}

		PRINT("Compositor Initiation worked");
		DEBUG("Compositor Initiation worked\n");

		return true;
	}

	//-----------------------------------------------------------------------------
	// OpenGL Stuff
	//-----------------------------------------------------------------------------

	bool MBVRNode::InitOGL()
	{

		PRINT("Starting initogl");
		DEBUG("Starting initogl\n");

		QGLFormat format;
		format.defaultFormat();

		//MBVRGLContext = new QGLContext(format);

		PRINT("Starting SetupVRHead");
		DEBUG("Starting SetupVRHead\n");
		if (!SetupVRHead())
			return false;


		PRINT("starting SetupStereoRenderTargets");
		DEBUG("starting SetupStereoRenderTargets\n");

		if (!SetupStereoRenderTargets())
			return false;

		PRINT("starting SetupTestGeometry");
		DEBUG("starting SetupTestGeometry\n");
		BREAKPOINT();
		if (!SetupTestGeometry())
			return false;

		return true;

	}

	bool MBVRNode::SetupVRHead()
	{
		if (vr_pointer != NULL)
		{
			vr_HMD = new VRHead(vr_pointer,5, 500);// Kernel()->Scene()->ActiveCamera()->Far()

			return (vr_HMD->SetupEyes());
		}
		return false;
	}

	bool MBVRNode::SetupStereoRenderTargets()
	{
		if (!vr_pointer)
			return false;

		BREAKPOINT();

		vr_pointer->GetRecommendedRenderTargetSize(&m_nRenderWidth,&m_nRenderHeight);

		
		DEBUG(m_nRenderWidth);
		DEBUG("\n");
		DEBUG(m_nRenderHeight);
		DEBUG("\n");

		BREAKPOINT();

		m_leftEyeTexture = CreateInstance<Texture>();

		m_rightEyeTexture = CreateInstance<Texture>();

		BREAKPOINT();

		uint width = m_nRenderWidth;
		uint height = m_nRenderHeight;

		m_leftEyeTexture->Create(m_nRenderWidth, m_nRenderHeight, 4, Image::e8integer, Color(0.5, 0.5, 0.5, 1));

		BREAKPOINT();

		m_leftEyeTexture->SetLocation(1);

		m_rightEyeTexture->Create(m_nRenderWidth, m_nRenderHeight, 4, Image::e8integer, Color(0.5, 0.5, 0.5, 1));
		m_rightEyeTexture->SetLocation(1);

		return true;
	}

	void MBVRNode::RenderStereoTargets()
	{
		BREAKPOINT();
		vr::EVRCompositorError ceError = vr::VRCompositorError_None;
		BREAKPOINT();
		vr_HMD->UpdateHMDMatrixPose();

		//-------------------------------Left Eye--------------------------------------

		//Left Eye Render
		m_leftEyeTexture->SetAsRenderTarget();

		RenderScene(vr::Eye_Left);

		DEBUG(m_leftEyeTexture->RestoreRenderTarget());

		//Left Eye Submission

		vr::Texture_t leftEyeTexture = { (void*)m_leftEyeTexture->OpenGLName(), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };

		ceError= vr_compositor->Submit(vr::Eye_Left, &leftEyeTexture);

		//-------------------------------Right Eye---------------------------------------

		//Right Eye Render
		m_rightEyeTexture->SetAsRenderTarget();

		RenderScene(vr::Eye_Right);

		DEBUG(m_rightEyeTexture->RestoreRenderTarget());

		//Right Eye Submission

		vr::Texture_t rightEyeTexture = { (void*)m_rightEyeTexture->OpenGLName(), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };

		ceError = vr_compositor->Submit(vr::Eye_Right, &rightEyeTexture);
	}

	void MBVRNode::RenderScene(vr::Hmd_Eye nEye)
	{
		glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

		glEnable(GL_COLOR_MATERIAL);
		glShadeModel(GL_SMOOTH);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_INDEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		//glEnable(GL_DEPTH_TEST | GL_CULL_FACE);

		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);

		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glEnable(GL_NORMALIZE);
		DrawMBVRMesh(nEye);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_INDEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glFlush();
		glFinish();

		//drawMudBoxGeom(nEye);
		BREAKPOINT();
  

		BREAKPOINT();
		

		//-----------------------Projection---------------------

		//glPushMatrix();
		//{
		//	glMatrixMode(GL_PROJECTION);
		//	auto projection = vr_HMD->GetCurrentProjectionMatrix(nEye);
		//	//float *fProj = (float *)malloc(16);
		//	//
		//	//fProj = projection;
		//	//
		//	glLoadMatrixf(projection);
		//
		//
		//	glPushMatrix();
		//	{
		//		glMatrixMode(GL_MODELVIEW);
		//		auto ModelView = vr_HMD->GetCurrentModelViewMatrix(nEye);
		//		glLoadMatrixf(ModelView);
		//		glScalef(0.5, 0.5, 0.5);
		//
		//		//DrawTestGeom();
		//
		//	}
		//	glPopMatrix();
		//
		//	glMatrixMode(GL_PROJECTION);
		//
		//
		//
		//}
		//glPopMatrix();



		//-----------------------ModelView---------------------

		//
		//glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		//glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//glBindTexture(GL_TEXTURE_2D, textureObjects[BODY_TEXTURE]);
		//
		//glActiveTexture(GL_TEXTURE1);
		//glEnable(GL_TEXTURE_CUBE_MAP);
		//glActiveTexture(GL_TEXTURE0);
		//
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
		//glBindTexture(GL_TEXTURE_2D, textureObjects[GLASS_TEXTURE]);
	
	}


	//-----------------------------------------------------------------------------
	// Geometry handling Stuff
	//-----------------------------------------------------------------------------

	void MBVRNode::DrawMBVRMesh(vr::Hmd_Eye nEye)
	{
		BREAKPOINT();
		for (auto & mMesh : m_mbvrMeshes)
		{
			mMesh->DrawGL( MBtoQMatrix( vr_HMD->GetCurrentModelViewMatrix(nEye) ) , MBtoQMatrix( vr_HMD->GetCurrentProjectionMatrix(nEye) ), MBtoQMatrix( vr_HMD->WorldScale ));// *(vr_HMD->GetCurrentModelViewMatrix(nEye))));
		}
	}

	bool MBVRNode::SetupTestGeometry()
	{
		BREAKPOINT();
		const GLshort nNumVerts = 8;

		m_nNumVerts = nNumVerts;

		const GLshort numFaces = 2 * 6;
		const GLshort numVertsPerFace = 3;

		const GLshort TotalVerts = numFaces * numVertsPerFace;
		m_nTotalVerts = TotalVerts;
		GLfloat m_TestVert[nNumVerts * 3] = { 0,0,0,
										0,10,0,
										0,10,10,
										0,0,10,
										10,0,0,
										10,10,0,
										10,10,10,
										10, 0, 10 };

		BREAKPOINT();

		GLfloat m_TestColours[TotalVerts * 3];
		for (size_t i = 0; i < TotalVerts; i++) m_TestColours[i] = 0.8f;

		BREAKPOINT();

		GLshort m_TestIndices[TotalVerts] = { 0,3,2 , 2,1,0 ,
									   2,6,1 , 6,5,1 ,
									   1,5,4 , 1,0,4 ,
									   0,3,4 , 3,7,4 ,
									   2,6,3 , 6,7,3 ,
									   4,7,5 , 5,7,6 };

		vertexArrObj.create();
		vertexIndexObj.create();
		vertexColoursObj.create();

		BREAKPOINT();

		// Copy data to video memory
		// Vertex data
		if (!vertexArrObj.bind())
			return false;
		vertexArrObj.setUsagePattern(QGLBuffer::DynamicDraw);
		vertexArrObj.allocate(m_TestVert, sizeof(GLfloat) * 3 * nNumVerts);
		BREAKPOINT();

		// Texture coordinates
		if (!vertexColoursObj.bind())
			return false;
		vertexColoursObj.setUsagePattern(QGLBuffer::StaticDraw);
		vertexColoursObj.allocate(m_TestColours, sizeof(GLfloat) * 3 * TotalVerts);
		BREAKPOINT();

		// Indexes
		if (!vertexIndexObj.bind())
			return false;
		vertexIndexObj.setUsagePattern(QGLBuffer::StaticDraw);
		vertexIndexObj.allocate(m_TestIndices, sizeof(GLushort) * TotalVerts);
		BREAKPOINT();

		// Free older, larger arrays
		//delete[] m_TestVert;
		BREAKPOINT();
		//delete[] m_TestColours;
		BREAKPOINT();
		//delete[] m_TestIndices;
		BREAKPOINT();
		return true;
	}

	void MBVRNode::DrawTestGeom()
	{

		//Simple Triangle at center of scene
		glBegin(GL_TRIANGLES);

		glColor4f(1.f, 0.f, 0.f, 1.f);
		glVertex3f(0, 0, 0);

		glColor4f(0.f, 1.f, 0.f, 1.f);
		glVertex3f(0.5, 0.5, 0);

		glColor4f(0.f, 0.f, 1.f, 1.f);
		glVertex3f(0, 0.5, 0);

		glEnd();

		//Simple Box with data sent on GPU

		// Vertexes
		vertexArrObj.bind();
		GLDEBUG();
		glVertexPointer( 3, GL_FLOAT, 0, 0);
		GLDEBUG();

		BREAKPOINT();
		
		// Texture coordinates
		vertexColoursObj.bind();
		GLDEBUG();
		glColorPointer(3, GL_FLOAT, 0, 0);
		GLDEBUG();
		
		// Indexes
		vertexIndexObj.bind();
		GLDEBUG();
		glDrawElements(GL_TRIANGLES, m_nTotalVerts, GL_UNSIGNED_SHORT, 0);
		GLDEBUG();

	}

	void MBVRNode::UpdateMBVRMeshes()
	{
		unsigned int geomCount = Kernel()->Scene()->GeometryCount();
		if (geomCount != m_mbvrMeshes.size())
		{
			BREAKPOINT();

			if (geomCount > m_mbvrMeshes.size())
			{
				BREAKPOINT();
				for (unsigned int i = 0; i < geomCount; i++)
				{
					BREAKPOINT();
					bool GeomIsRegistered = false;
					for (unsigned int j = 0; j < m_mbvrMeshes.size(); j++)
					{
						BREAKPOINT();
						if (Kernel()->Scene()->Geometry(i) == &m_mbvrMeshes[j]->getGeometry()) GeomIsRegistered = true;
					}
					if (GeomIsRegistered == false)
					{
						
						auto temp = new MBVRMesh(); BREAKPOINT();
						m_mbvrMeshes.push_back(temp);
						temp->GeomInitialize(Kernel()->Scene()->Geometry(i));
						BREAKPOINT();
					}
				}
				BREAKPOINT();

			}
			else
			{
				for (size_t k = 0; k < m_mbvrMeshes.size(); k++)
				{
					BREAKPOINT();
					if (!m_mbvrMeshes[k]->CheckIfGeomStillExists()) delete m_mbvrMeshes[k];
				}
			}
		}
	}

	

	//----------------------------------------------------Controller Code----------------------------------------------

	bool MBVRNode::InitVRActions()
	{

		vr::EVRInitError eError = vr::VRInitError_None;
		vr::EVRInputError inputError = vr::VRInputError_None;

		if (vr_pointer != NULL)
		{
			DEBUG("Setting Action ManifestPath \n");
			inputError = vr::VRInput()->SetActionManifestPath("C:/Users/lucciano/Documents/UniStuff/MudboxVR/Windows/VRActionSets/mudboxvr_actions.json");
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }

			inputError = vr::VRInput()->GetActionHandle("/actions/demo/in/HideCubes", &m_actionToggleWireframe);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }
			inputError = vr::VRInput()->GetActionHandle("/actions/demo/in/HideCubes", &m_actionHideCubes);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }
			inputError = vr::VRInput()->GetActionHandle("/actions/demo/in/HideThisController", &m_actionHideThisController);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }
			inputError = vr::VRInput()->GetActionHandle("/actions/demo/in/TriggerHaptic", &m_actionTriggerHaptic);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }
			inputError = vr::VRInput()->GetActionHandle("/actions/demo/in/AnalogInput", &m_actionAnalongInput);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }

			inputError = vr::VRInput()->GetActionSetHandle("/actions/demo", &m_actionsetDemo);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }

			inputError = vr::VRInput()->GetActionHandle("/actions/demo/out/Haptic_Left", &m_rHand[Left].m_actionHaptic);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }
			inputError = vr::VRInput()->GetInputSourceHandle("/user/hand/left", &m_rHand[Left].m_source);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }
			inputError = vr::VRInput()->GetActionHandle("/actions/demo/in/Hand_Left", &m_rHand[Left].m_actionPose);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }

			inputError = vr::VRInput()->GetActionHandle("/actions/demo/out/Haptic_Right", &m_rHand[Right].m_actionHaptic);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }
			inputError = vr::VRInput()->GetInputSourceHandle("/user/hand/right", &m_rHand[Right].m_source);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }
			inputError = vr::VRInput()->GetActionHandle("/actions/demo/in/Hand_Right", &m_rHand[Right].m_actionPose);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }

			return true;

		}
		return false;
	}

	void MBVRNode::ProcessVREvent(vr::VREvent_t vr_event)
	{
		switch (vr_event.eventType)
		{
			//VR Controller Events

		case vr::VREvent_ButtonPress:
			printf("EVENT (OpenVR) Device : %d attached\n", vr_event.trackedDeviceIndex);

			vr_event.data.controller.button;
			switch (vr_event.data.controller.button)
			{

				//VR Controller Grip Event

			case vr::k_EButton_Grip:
				switch (vr_event.eventType) {
				case vr::VREvent_ButtonPress:
					DEBUG("k_EButton_Grip VREvent_ButtonPress");
					break;
				case vr::VREvent_ButtonUnpress:
					DEBUG("k_EButton_Grip VREvent_ButtonUnpress");
					break;
				}
				break;

				//VR Controller Trigger Event

			case  vr::k_EButton_SteamVR_Trigger:
				switch (vr_event.eventType) {
				case vr::VREvent_ButtonPress:
					DEBUG("k_EButton_SteamVR_Trigger VREvent_ButtonUnpress");
					break;

				case vr::VREvent_ButtonUnpress:
					DEBUG("k_EButton_SteamVR_Trigger VREvent_ButtonUnpress");
					break;
				}
				break;

				//VR Controller Touchpad Event

			case vr::k_EButton_SteamVR_Touchpad:
				switch (vr_event.eventType) {
				case vr::VREvent_ButtonPress:
					DEBUG("k_EButton_SteamVR_Touchpad VREvent_ButtonPress");
					break;

				case vr::VREvent_ButtonUnpress:
					DEBUG("k_EButton_SteamVR_Touchpad VREvent_ButtonUnpress");
					break;

				case vr::VREvent_ButtonTouch:
					DEBUG("k_EButton_SteamVR_Touchpad VREvent_ButtonTouch");
					break;

				case vr::VREvent_ButtonUntouch:
					DEBUG("k_EButton_SteamVR_Touchpad VREvent_ButtonUntouch");
					break;
				}
				break;

				//VR Controller Touchpad Event

			case vr::k_EButton_ApplicationMenu:
				switch (vr_event.eventType) {
				case vr::VREvent_ButtonPress:
					break;

				case vr::VREvent_ButtonUnpress:
					break;
				}
				break;
			}

			break;

			//and so on, can test for any amount of vr events

			//Any other Events

		}
	}

	void MBVRNode::VRContrHandleInputs()
	{

		//Handle Button Events

		vr::VREvent_t event;

		while (vr_pointer->PollNextEvent(&event, sizeof(event)))
		{
			ProcessVREvent(event);
		}

		vr::VRActiveActionSet_t actionSet = { 0 };
		actionSet.ulActionSet = m_actionsetDemo;
		vr::VRInput()->UpdateActionState(&actionSet, sizeof(actionSet), 1);

		BWireframe = !GetDigitalActionState(m_actionToggleWireframe);

		vr::VRInputValueHandle_t ulHapticDevice;
		if (GetDigitalActionRisingEdge(m_actionToggleWireframe, &ulHapticDevice))
		{
			if (ulHapticDevice == m_rHand[Left].m_source)
			{
				vr::VRInput()->TriggerHapticVibrationAction( m_rHand[Left].m_actionHaptic, 0, 1, 4.f, 1.0f, vr::k_ulInvalidInputValueHandle );
			}
			if (ulHapticDevice == m_rHand[Right].m_source)
			{
				vr::VRInput()->TriggerHapticVibrationAction(m_rHand[Right].m_actionHaptic, 0, 1, 4.f, 1.0f, vr::k_ulInvalidInputValueHandle);
			}
		}

		//Get Controller Positions

		m_rHand[Left].m_bShowController = true;
		m_rHand[Right].m_bShowController = true;

		vr::VRInputValueHandle_t ulHideDevice;

		if (GetDigitalActionState(m_actionHideThisController, &ulHideDevice))
		{
			if (ulHideDevice == m_rHand[Left].m_source)
			{
				m_rHand[Left].m_bShowController = false;
			}
			if (ulHideDevice == m_rHand[Right].m_source)
			{
				m_rHand[Right].m_bShowController = false;
			}
		}

		for (EHand eHand = Left; eHand <= Right; ((int&)eHand)++)
		{
			vr::InputPoseActionData_t poseData;
			if (vr::VRInput()->GetPoseActionData(m_rHand[eHand].m_actionPose, vr::TrackingUniverseStanding, 0, &poseData, sizeof(poseData),  vr::k_ulInvalidInputValueHandle) 
				!= vr::VRInputError_None
				|| !poseData.bActive 
				|| !poseData.pose.bPoseIsValid)
			{

				VR_INPUT_DEBUG(vr::VRInput()->GetPoseActionData(m_rHand[eHand].m_actionPose, vr::TrackingUniverseStanding, 0, &poseData, sizeof(poseData), vr::k_ulInvalidInputValueHandle));
				DEBUG(!poseData.bActive);
				DEBUG(!poseData.pose.bPoseIsValid);

				m_rHand[eHand].m_bShowController = false;
				DEBUG("m_rHand[eHand].m_bShowController = false");
			}
			else
			{
				m_rHand[eHand].m_rmat4Pose = ConvertSteamVRMatrixToMatrix(poseData.pose.mDeviceToAbsoluteTracking);
				DEBUG("Updating Position");

				vr::InputOriginInfo_t originInfo;
				if (vr::VRInput()->GetOriginTrackedDeviceInfo(poseData.activeOrigin, &originInfo, sizeof(originInfo)) == vr::VRInputError_None
					&& originInfo.trackedDeviceIndex != vr::k_unTrackedDeviceIndexInvalid)
				{
					//std::string sRenderModelName = GetTrackedDeviceString(originInfo.trackedDeviceIndex, vr::Prop_RenderModelName_String);
					//if (sRenderModelName != m_rHand[eHand].m_sRenderModelName)
					//{
					//	m_rHand[eHand].m_pRenderModel = FindOrLoadRenderModel(sRenderModelName.c_str());
					//	m_rHand[eHand].m_sRenderModelName = sRenderModelName;
					//}
				}
			}
		}

	}
	

}