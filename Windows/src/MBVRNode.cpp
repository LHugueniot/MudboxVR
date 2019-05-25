#include "MBVRNode.h"

namespace mudbox {


	//-------------------------------------------------------------
	//--					MBVRNode Constructor				 --
	//-------------------------------------------------------------

	mbvrNode::mbvrNode() :	m_eEachTick(this),
							vertexArrObj(QGLBuffer::VertexBuffer),
							vertexIndexObj(QGLBuffer::IndexBuffer),
							vertexColoursObj(QGLBuffer::VertexBuffer)

	{
		m_eEachTick.Connect(Kernel()->ViewPort()->PostRenderEvent);

		DEBUG("Initiating VR\n");
		PRINT("Initiating VR");

		Initialized = InitializeMBVR();

		
		//vr::VRInput()->SetActionManifestPath( "C:/Users/lucciano/Documents/UniStuff/MudboxVR/Windows/VRActionSets/mudboxvr_actions.json" );

	}

	//-------------------------------------------------------------
	//--					MBVRNode Destructor				     --
	//-------------------------------------------------------------

	mbvrNode::~mbvrNode()
	{
		Shutdown();
	}

	bool mbvrNode::InitializeMBVR()
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

		DEBUG("Initialisation has succeeded\n");
		PRINT("Initialisation has succeeded");

		return true;
	}

	//-----------------------------------------------------------------------------
	//	MudBox Stuff
	//-----------------------------------------------------------------------------

	//-------------------------------------------------------------
	//-- 			Gets called each frameEvent					 --
	//-------------------------------------------------------------

	void mbvrNode::OnEvent(const EventGate &cEvent)
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
			RenderStereoTargets();
		}
		Kernel()->ViewPort()->Redraw();

		framecount++;

	}

	//-----------------------------------------------------------------------------
	// VR Stuff
	//-----------------------------------------------------------------------------

	bool mbvrNode::InitVR()
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
		if (vr_pointer != NULL)
		{
			DEBUG("Setting Action ManifestPath \n");
			//vr::VRInput()->SetActionManifestPath("C:/Users/lucciano/Documents/UniStuff/MudboxVR/Windows/VRActionSets/mudboxvr_actions.json");
		}
		
		PRINT("VR Initiation worked");
		DEBUG("VR Initiation worked\n");
		return true;
	}

	bool mbvrNode::InitCompositor()
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

	bool mbvrNode::InitOGL()
	{
		PRINT("Starting initogl");
		DEBUG("Starting initogl\n");

		PRINT("Starting SetupVRHead");
		DEBUG("Starting SetupVRHead\n");
		if (!SetupVRHead())
			return false;

		PRINT("Starting SetupCameras");
		DEBUG("Starting SetupCameras\n");
		if (!SetupCameras())
			return false;

		PRINT("starting SetupStereoRenderTargets");
		DEBUG("starting SetupStereoRenderTargets\n");

		if (!SetupStereoRenderTargets())
			return false;


		PRINT("starting setupGeometry");
		DEBUG("starting setupGeometry\n");
		BREAKPOINT();
		if (!setupGeometry())
			return false;

		return true;

	}

	bool mbvrNode::SetupVRHead()
	{
		if (vr_pointer != NULL)
		{
			vr_HMD = new VRHead(vr_pointer, 0.1f, Kernel()->Scene()->ActiveCamera()->Far());

			return (vr_HMD->SetupEyes());
		}
		return false;
	}

	bool mbvrNode::SetupCameras()
	{
		if (vr_HMD != NULL)
		{
			m_leftEyeCamera = CreateInstance<MBVRCamera>();
			
			BREAKPOINT();
			Kernel()->Scene()->AddCamera(m_leftEyeCamera);
			BREAKPOINT();


			m_rightEyeCamera = CreateInstance<MBVRCamera>();

			BREAKPOINT();
			Kernel()->Scene()->AddCamera(m_rightEyeCamera);
			BREAKPOINT();

			m_leftEyeCamera->InitVRMB(vr_HMD, vr::Eye_Left);
			BREAKPOINT();
			m_rightEyeCamera->InitVRMB(vr_HMD, vr::Eye_Right);
			BREAKPOINT();
			return true;

		}
		return false;

	}

	bool mbvrNode::SetupStereoRenderTargets()
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

	bool mbvrNode::CreateFrameBuffer(int _width, int _Height, FrameBufferDescriptor &frameBufferDesc)
	{
		return false;
	}

	bool mbvrNode::setupGeometry()
	{
		BREAKPOINT();
		const GLshort nNumVerts = 8;
		
		m_nNumVerts = nNumVerts;
		
		const GLshort numFaces = 2 * 6;
		const GLshort numVertsPerFace = 3;

		const GLshort TotalVerts = numFaces * numVertsPerFace;
		m_nTotalVerts = TotalVerts;
		GLfloat m_TestVert[nNumVerts*3]= {0,0,0,
										0,10,0,
										0,10,10,
										0,0,10,
										10,0,0,
										10,10,0,
										10,10,10,
										10, 0, 10 };

		BREAKPOINT();

		GLfloat m_TestColours[TotalVerts *3];
		for (size_t i = 0; i < TotalVerts; i++) m_TestColours[i] = 0.8f;

		BREAKPOINT();

		GLshort m_TestIndices[TotalVerts]  = {0,3,2 , 2,1,0 ,
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

	void mbvrNode::RenderStereoTargets()
	{
		m_originalCamera = Kernel()->Scene()->ActiveCamera();
		BREAKPOINT();
		vr::EVRCompositorError ceError = vr::VRCompositorError_None;
		BREAKPOINT();
		vr_HMD->UpdateHMDMatrixPose();

		m_leftEyeCamera->UpdateFromHMD();
		m_rightEyeCamera->UpdateFromHMD();

		//vr_HMD->UpdateMudboxCameras();
		BREAKPOINT();

		//-------------------------------Left Eye--------------------------------------

		//Left Eye Render
		m_leftEyeTexture->SetAsRenderTarget();

		BREAKPOINT();

		RenderScene(vr::Eye_Left);

		DEBUG(m_leftEyeTexture->RestoreRenderTarget());

		BREAKPOINT();
		//Left Eye Submission

		vr::Texture_t leftEyeTexture = { (void*)(unsigned int)m_leftEyeTexture->OpenGLName(), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };

		BREAKPOINT();

		ceError= vr_compositor->Submit(vr::Eye_Left, &leftEyeTexture);

		BREAKPOINT();

		//-------------------------------Right Eye---------------------------------------

		//Right Eye Render
		m_rightEyeTexture->SetAsRenderTarget();

		RenderScene(vr::Eye_Right);

		DEBUG(m_rightEyeTexture->RestoreRenderTarget());

		//Right Eye Submission

		vr::Texture_t rightEyeTexture = { (void*)(unsigned int)m_rightEyeTexture->OpenGLName(), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };

		BREAKPOINT();

		ceError = vr_compositor->Submit(vr::Eye_Right, &rightEyeTexture);

		BREAKPOINT();


		BREAKPOINT();


		Kernel()->Scene()->SetActiveCamera(m_originalCamera);

		//VRDEBUG(ceError);

		//glFinish();
		//glClearColor(0, 0, 0, 1);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glFlush();
		//glFinish();
	}

	void mbvrNode::RenderScene(vr::Hmd_Eye nEye)
	{
		glPolygonMode(GL_FRONT, GL_FILL);
		glShadeModel(GL_SMOOTH);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_INDEX_ARRAY);
		BREAKPOINT();


		//-----------------------Projection---------------------

		glPushMatrix();
		{
			glMatrixMode(GL_PROJECTION);
			auto projection = vr_HMD->GetCurrentProjectionMatrix(nEye);
			float *fProj = (float *)malloc(16);

			fProj = projection;

			glLoadMatrixf(fProj);

			logMatrix(projection, "Projection");

			glPushMatrix();
			{
				glMatrixMode(GL_MODELVIEW);
				auto ModelView = vr_HMD->GetCurrentModelViewMatrix(nEye);
				float *fMV = (float *)malloc(16);

				fMV = ModelView;

				glLoadMatrixf(fMV);

				logMatrix(ModelView, "ModelView");

				glScalef(0.5, 0.5, 0.5);

				drawGeom();
				glPopMatrix();

				glMatrixMode(GL_PROJECTION);

			}
			glPopMatrix();
			BREAKPOINT();


		}

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_INDEX_ARRAY);

		drawMudBoxGeom(nEye);
		BREAKPOINT();


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

	void mbvrNode::drawMudBoxGeom(vr::Hmd_Eye nEye)
	{
		//if(nEye==vr::Eye_Left)
		//{
		//	Kernel()->Scene()->SetActiveCamera(m_leftEyeCamera);
		//}
		//if (nEye == vr::Eye_Right)
		//{
		//	Kernel()->Scene()->SetActiveCamera(m_rightEyeCamera);
		//}

		size_t GeomCount = Kernel()->Scene()->GeometryCount();

		BREAKPOINT();

		for (size_t i = 0; i < GeomCount; i++)
		{
			BREAKPOINT();
			Geometry * CurGeom = Kernel()->Scene()->Geometry(i);
			BREAKPOINT();
			Selector * selector= new Selector();
			BREAKPOINT();
			//CurGeom->ActiveLevel()->Renderer()->RenderData();
			BREAKPOINT();
			AxisAlignedBoundingBox AABB(Vector(0,0,0) , 300);

			if(nEye==vr::Eye_Left)
			{
				BREAKPOINT();
				Kernel()->Scene()->SetActiveCamera(m_leftEyeCamera);
				BREAKPOINT();
				CurGeom->ActiveLevel()->Renderer()->Render(NULL, false, m_leftEyeCamera, AABB);
			}
			if (nEye == vr::Eye_Right)
			{
				BREAKPOINT();
				//Kernel()->Scene()->SetActiveCamera(m_rightEyeCamera);
				BREAKPOINT();
				CurGeom->ActiveLevel()->Renderer()->Render(NULL, false, m_rightEyeCamera, AABB);
			}


		}

	}

	void mbvrNode::drawGeom()
	{

		glBegin(GL_TRIANGLES);

		glColor4f(1.f, 0.f, 0.f, 1.f);
		glVertex3f(0, 0, 0);

		glColor4f(0.f, 1.f, 0.f, 1.f);
		glVertex3f(0.5, 0.5, 0);

		glColor4f(0.f, 0.f, 1.f, 1.f);
		glVertex3f(0, 0.5, 0);

		glEnd();

		BREAKPOINT();

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



	//-------------------------------------------------------------
	//-- 			Shuts down the VR and any GL dependencies    --
	//-------------------------------------------------------------
	
	void mbvrNode::Shutdown()
	{
		PRINT("Shutting VR down");
		DEBUG("Shutting VR down");
	
		if ((vr_pointer != NULL) && (vr_compositor !=NULL))
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
	
		if (m_leftEyeTexture==NULL)
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


	//void mbvrNode::RenderScene(vr::Hmd_Eye nEye)
	//{
	//
	//
	//	auto mat = vr_HMD->GetHMDMatrixPoseEye(nEye);
	//	Kernel()->Scene()->ActiveCamera()->SetPosition(Vector(mat._13, mat._23, mat._33));
	//	//Kernel()->Scene()->ActiveCamera()->SetAim(Vector(mat._12, mat._22, mat._23));
	//
	//	auto rotation = vr_HMD->GetRotation();
	//
	//	Kernel()->Scene()->ActiveCamera()->SetYawPitchRoll(Vector(-rotation.x * rotation.w,
	//		-rotation.y * rotation.w,
	//		-rotation.z * rotation.w));
	//	glPolygonMode(GL_FRONT, GL_FILL);
	//	glShadeModel(GL_SMOOTH);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	glEnable(GL_DEPTH_TEST);
	//
	//
	//	//		gluLookAt(mat._13, mat._23, mat._33,
	//	//			mat._12, mat._22, mat._23,
	//	//			mat._11, mat._11, mat._11);
	//
	//	glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
	//
	//	glBegin(GL_TRIANGLES);
	//
	//	glColor4f(1.f, 0.f, 0.f, 1.f);
	//	glVertex3f(0, 0, 0);
	//
	//	glColor4f(0.f, 1.f, 0.f, 1.f);
	//	glVertex3f(.5, .5, 0);
	//
	//	glColor4f(0.f, 0.f, 1.f, 1.f);
	//	glVertex3f(0, 1, 0);
	//
	//	glEnd();
	//
	//
	//}
	//
	//void mbvrNode::RenderStereoTargets()
	//{
	//	MBVRGLDebug();
	//	vr::EVRCompositorError ceError = vr::VRCompositorError_None;
	//
	//	DEBUG("left eye texture size");
	//	DEBUG(m_leftEyeTexture->VideoMemoryUsage());
	//
	//
	//
	//	vr_HMD->UpdateHMDMatrixPose();
	//	//vr::VRCompositor()->ClearLastSubmittedFrame();
	//
	//	breakPoint(1, __FUNCTION__);
	//
	//	//Left Eye
	//
	//	m_leftEyeTexture->SetAsRenderTarget();
	//
	//	breakPoint(2, __FUNCTION__);
	//
	//	RenderScene(vr::Eye_Left);
	//
	//	breakPoint(3, __FUNCTION__);
	//
	//	vr::Texture_t leftEyeTexture = { (void*)(unsigned int)m_leftEyeTexture->OpenGLName(), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	//	DEBUG("left eye texture openglname");
	//	DEBUG(m_leftEyeTexture->OpenGLName());
	//
	//	breakPoint(4, __FUNCTION__);
	//
	//	ceError = vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	//
	//	breakPoint(5, __FUNCTION__);
	//
	//	DEBUG("restoring m_leftEyeTexture render target");
	//	DEBUG(m_leftEyeTexture->RestoreRenderTarget());
	//
	//	//m_leftEyeTexture->RestoreRenderTarget();
	//
	//
	//	breakPoint(6, __FUNCTION__);
	//
	//	if (ceError != vr::VRCompositorError_None)
	//	{
	//		PRINT("Compositor submission failed\n");
	//		DEBUG("Compositor submission failed\n");
	//		DEBUG(getCompositorErrorAsString(ceError));
	//	}
	//
	//	//Right Eye
	//
	//	m_rightEyeTexture->SetAsRenderTarget();
	//	RenderScene(vr::Eye_Right);
	//
	//	vr::Texture_t rightEyeTexture = { (void*)(unsigned int)m_rightEyeTexture->OpenGLName(), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	//	DEBUG("right eye texture openglname");
	//	DEBUG(m_rightEyeTexture->OpenGLName());
	//
	//	ceError = vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
	//
	//	//m_rightEyeTexture->RestoreRenderTarget();
	//
	//
	//	DEBUG("restoring m_leftEyeTexture render target");
	//	DEBUG(m_rightEyeTexture->RestoreRenderTarget());
	//
	//	if (ceError != vr::VRCompositorError_None)
	//	{
	//		PRINT("Compositor submission failed\n");
	//		DEBUG("Compositor submission failed\n");
	//		DEBUG(getCompositorErrorAsString(ceError));
	//	}
	//
	//	glFinish();
	//	glClearColor(0, 0, 0, 1);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	glFlush();
	//	glFinish();
	//}

}