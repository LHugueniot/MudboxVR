#include "MBVRNode.h"

namespace mudbox {


	//-------------------------------------------------------------
	//--					MBVRNode Constructor				 --
	//-------------------------------------------------------------

	MBVRNode::MBVRNode() :	m_eEachTick(this),
							vertexArrObj(QGLBuffer::VertexBuffer),
							vertexIndexObj(QGLBuffer::IndexBuffer),
							vertexColoursObj(QGLBuffer::VertexBuffer),
							m_GridVertArray(QGLBuffer::VertexBuffer),
							m_GridColourArray(QGLBuffer::VertexBuffer),
							WorldSpaceAdjuster (
							1.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 1.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f
							)

	{
		//Usually postrenderEvent
		m_eEachTick.Connect(Kernel()->ViewPort()->PostRenderEvent);

		Kernel()->StrokeBeginEvent.Connect(m_VRBrushStrokeBegin);
		
		VRSelectedBrush = Kernel()->Interface()->SelectedBrush.m_cValue;

		//VRSelectedBrush.Name;

		//Kernel()->Interface()->SetSelectedBrush();

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

		//float * f = new float[2]{ 1, 0};
		//glGetFloatv(GL_DEPTH_RANGE, f);
		//DEBUG("First Depth Range Val");
		//DEBUG(f[0]);
		//DEBUG("Second Depth Range Val");
		//DEBUG(f[1]);

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

		if (vr_HMD != NULL)
			delete vr_HMD;

		if (Original_pos == NULL)
			delete Original_pos;

		if (m_leftEyeTexture != NULL)
			delete m_leftEyeTexture;

		if (m_rightEyeTexture != NULL)
			delete m_rightEyeTexture;

		if (m_mbvrMeshes.size() != 0)
		{
			for (unsigned int i = 0; i < m_mbvrMeshes.size(); i++)
			{
				delete m_mbvrMeshes[i];
			}
		}

		if (m_GridVert != NULL)
			delete[] m_GridVert;

		if (m_GridColour != NULL)
			delete[] m_GridColour;

		if (m_RightHand != NULL)
			delete m_RightHand;

		if (CurrentSurfacePoint != NULL)
			delete CurrentSurfacePoint;

		if (VRBrushPicker != NULL)
			delete VRBrushPicker;

		PRINT(framecount);
		DEBUG("The framecount was: \n");
		DEBUG(framecount);

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

		if (cEvent == frameUpdate)
		{
			framecount++;
		}

		if ((vr_pointer != NULL && vr_compositor!=NULL) &&  Initialized == true)
		{
			BREAKPOINT();
			if (cEvent == m_eEachTick)
			{
				
				VRContrHandleInputs(); BREAKPOINT();
				
				RenderStereoTargets(); BREAKPOINT();
				

				UpdateMBVRMeshes(); BREAKPOINT();
			}
			BREAKPOINT();
		}
		BREAKPOINT();

		Kernel()->ViewPort()->Redraw();
		tick++;
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

		//QGLFormat format;
		//format.defaultFormat();

		VRMesh::DefaultMat = CreateInstance<Texture>();
		Color mudColour(0.7, 0.55, 0.25);
		VRMesh::DefaultMat->Create(256, 256, 4, Image::e8integer, mudColour);
		VRMesh::DefaultMat->SetLocation(TexturePool::locationGPU);

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

		PRINT("starting SetupControllerGeometry");
		DEBUG("starting SetupControllerGeometry\n");
		BREAKPOINT();
		if (!SetupControllerGeometry())
			return false;

		PRINT("starting SetupGridGeometry");
		DEBUG("starting SetupGridGeometry\n");
		BREAKPOINT();
		if (!SetupGridGeometry())
			return false;

		return true;

	}

	bool MBVRNode::SetupVRHead()
	{
		if (vr_pointer != NULL)
		{
			//Kernel()->Scene()->ActiveCamera()->SetNear(20);
			//Kernel()->Scene()->ActiveCamera()->SetFar(2000);

			vr_HMD = new VRHead(vr_pointer,50, 2000);// Kernel()->Scene()->ActiveCamera()->Far()

			
			if (vr_HMD->SetupEyes()) 
			{
				////Matrix CamOriginalPos = Kernel()->Scene()->ActiveCamera()->Transformation()->LocalToWorldMatrix();
				//
				//Vector VRHeadOriginalPos =
				//{
				//	vr_HMD->GetOriginalPosition()._41,
				//	vr_HMD->GetOriginalPosition()._42,
				//	vr_HMD->GetOriginalPosition()._43
				//};
				//
				//logVector(VRHeadOriginalPos, "CamOriginalPos");
				//
				//Matrix VRHeadOriginalMatrix = {
				//		1.0f, 0, 0, VRHeadOriginalPos.x,
				//		0, 1.0f, 0, VRHeadOriginalPos.y,
				//		0, 0, 1.0f, VRHeadOriginalPos.z,
				//		0, 0, 0, 1.0f
				//};
				//
				//Vector CamOriginalPos = Kernel()->Scene()->ActiveCamera()->Position();
				//
				////Matrix CamOriginalMatrix = {
				////	1.0f, 0, 0, CamOriginalPos.x,
				////	0, 1.0f, 0, CamOriginalPos.y,
				////	0, 0, 1.0f, CamOriginalPos.z,
				////	0, 0, 0, 1.0f
				////};
				//WorldSpaceAdjuster = {
				//	WorldSpaceAdjuster._11, 0, 0	,0,
				//	0 ,WorldSpaceAdjuster._22, 0	,0,
				//	0, 0 ,WorldSpaceAdjuster._33	,0,
				//	CamOriginalPos.x - VRHeadOriginalPos.x, CamOriginalPos.y - VRHeadOriginalPos.y, CamOriginalPos.z - VRHeadOriginalPos.z ,1.0f
				//	//CamOriginalPos.x , CamOriginalPos.y, CamOriginalPos.z ,1.0f
				//};
				//
				//logMatrix(WorldSpaceAdjuster, "WorldSpaceAdjuster");

				vr_HMD->SetWorldTransform(WorldSpaceAdjuster);

				//Vector CamOriginalPos = Kernel()->Scene()->ActiveCamera()->Position();
				//
				//Matrix CamOriginalMatrix = {
				//	1.0f, 0, 0, CamOriginalPos.x,
				//	0, 1.0f, 0, CamOriginalPos.y,
				//	0, 0, 1.0f, CamOriginalPos.z,
				//	0, 0, 0, 1.0f
				//};
				//
				////WorldSpaceAdjuster = CamOriginalPos * vr_HMD->GetOriginalPosition() * WorldSpaceAdjuster ;
				//WorldSpaceAdjuster =  vr_HMD->GetOriginalPosition() * CamOriginalMatrix * WorldSpaceAdjuster;

				//vr_HMD->SetWorldTransform(WorldSpaceAdjuster);
				return true;
			}
		}
		return false;
	}

	bool MBVRNode::SetupStereoRenderTargets()
	{
		if (!vr_pointer)
			return false;

		vr_pointer->GetRecommendedRenderTargetSize(&m_nRenderWidth,&m_nRenderHeight);

		m_leftEyeTexture = CreateInstance<Texture>();
		m_rightEyeTexture = CreateInstance<Texture>();

		uint width = m_nRenderWidth;
		uint height = m_nRenderHeight;

		m_leftEyeTexture->Create(m_nRenderWidth, m_nRenderHeight, 4, Image::e8integer, Color(0.5, 0.5, 0.5, 1));
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

		GLDEBUG();
		//-------------------------------Left Eye--------------------------------------

		//Left Eye Render
		m_leftEyeTexture->SetAsRenderTarget();
		GLDEBUG();

		RenderScene(vr::Eye_Left);
		GLDEBUG();

		m_leftEyeTexture->RestoreRenderTarget();

		//Left Eye Submission

		vr::Texture_t leftEyeTexture = { (void*)m_leftEyeTexture->OpenGLName(), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };

		ceError= vr_compositor->Submit(vr::Eye_Left, &leftEyeTexture);

		//-------------------------------Right Eye---------------------------------------

		//Right Eye Render
		m_rightEyeTexture->SetAsRenderTarget();

		RenderScene(vr::Eye_Right);

		m_rightEyeTexture->RestoreRenderTarget();

		//Right Eye Submission

		vr::Texture_t rightEyeTexture = { (void*)m_rightEyeTexture->OpenGLName(), vr::TextureType_OpenGL, vr::ColorSpace_Gamma };

		ceError = vr_compositor->Submit(vr::Eye_Right, &rightEyeTexture);
		BREAKPOINT();
	}

	void MBVRNode::RenderScene(vr::Hmd_Eye nEye)
	{
		GLDEBUG();
		glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);

		//glDisable(GL_BLEND);
		//glDisable(GL_DITHER);
		//glDisable(GL_FOG);
		//
		GLDEBUG();
		glEnable(GL_COLOR_MATERIAL);
		GLDEBUG();
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		GLDEBUG();

		glShadeModel(GL_SMOOTH);
		GLDEBUG();

		glEnable(GL_CULL_FACE);
		GLDEBUG();
		glCullFace(GL_BACK);
		GLDEBUG();
		glEnable(GL_NORMALIZE);
		GLDEBUG();

		glEnable(GL_DEPTH_TEST);
		GLDEBUG();
		glDepthMask(GL_TRUE);
		GLDEBUG();
		glDepthFunc(GL_GREATER);
		GLDEBUG();
		glDepthRange(0.1f, 1.0f);
		GLDEBUG();

		glClearDepth(1.0f);
		GLDEBUG();

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		GLDEBUG();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		GLDEBUG();

		glEnableClientState(GL_VERTEX_ARRAY);
		GLDEBUG();
		glEnableClientState(GL_NORMAL_ARRAY);
		GLDEBUG();
		glEnableClientState(GL_COLOR_ARRAY);
		GLDEBUG();
		glEnableClientState(GL_INDEX_ARRAY);
		GLDEBUG();
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		GLDEBUG();


		if (BWireframe)
			glPolygonMode(GL_FRONT, GL_LINE);
		else
			glPolygonMode(GL_FRONT, GL_FILL);
		GLDEBUG();

		//Draw Grid

		glMatrixMode(GL_PROJECTION);
		GLDEBUG();
		glPushMatrix();
		{
			GLDEBUG();

			auto projection = vr_HMD->GetCurrentProjectionMatrix(nEye);

			glLoadMatrixf(projection);
			GLDEBUG();

			glMatrixMode(GL_MODELVIEW);
			GLDEBUG();

			glPushMatrix();
			{
				GLDEBUG();

				auto ModelView = vr_HMD->GetCurrentModelViewMatrix(nEye);
				GLDEBUG();

				glLoadMatrixf(ModelView);
				GLDEBUG();
				//if(IsIntersecting)

				DrawGrid();
				GLDEBUG();
			}
			glPopMatrix();
			GLDEBUG();

			glEnable(GL_LIGHTING);
			GLDEBUG();


			glDisable(GL_LIGHTING);
			GLDEBUG();

			glMatrixMode(GL_PROJECTION);
			GLDEBUG();
		}
		glPopMatrix();
		GLDEBUG();

		DrawMBVRMesh(nEye);
		GLDEBUG();

		DrawIntersectionPoint(nEye);

		GLDEBUG();

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_INDEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		GLDEBUG();

		glFlush();
		GLDEBUG();
		glFinish();
		GLDEBUG();


		//Kernel()->GLCheckStates("MBVRNode", "RenderScene", __LINE__);
		GLDEBUG();

		//drawMudBoxGeom(nEye);

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
			mMesh->DrawGL( MBtoQMatrix( vr_HMD->GetCurrentModelViewMatrix(nEye) ) , MBtoQMatrix( vr_HMD->GetCurrentProjectionMatrix(nEye) ), MBtoQMatrix( vr_HMD->m_WorldScale));// *(vr_HMD->GetCurrentModelViewMatrix(nEye))));
		}
		BREAKPOINT();

		if (m_rHand[Right].m_bShowController == true)
		{
			m_RightHand->DrawGL(MBtoQMatrix(vr_HMD->GetCurrentModelViewMatrix(nEye)), MBtoQMatrix(vr_HMD->GetCurrentProjectionMatrix(nEye)), MBtoQMatrix(vr_HMD->m_WorldScale));
		}

		BREAKPOINT();
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

	bool MBVRNode::SetupGridGeometry()
	{
		int Rows = 21;
		int Columns = 21;
		int NumLines = Columns + Rows;
		int PointsPerLine = 2;
		int CoordsPerPoint = 3;

		int LineWidth = 100 ;

		m_GridVert = new float[NumLines * PointsPerLine * CoordsPerPoint];

		m_GridVertArraySize = NumLines * PointsPerLine * CoordsPerPoint;

		for (unsigned int i = 0; i < (unsigned int)(NumLines /2); i++)
		{
			unsigned int iOffset = i * PointsPerLine * CoordsPerPoint * 2;

			m_GridVert[iOffset] = (float)(i * LineWidth) - 10 * LineWidth;
			m_GridVert[iOffset + 1] = 0;
			m_GridVert[iOffset + 2] = (float)(10 * LineWidth);// +100;

			m_GridVert[iOffset + 3] = (float)(i * LineWidth) - 10 * LineWidth;
			m_GridVert[iOffset + 4] = 0;
			m_GridVert[iOffset + 5] = (float)(-10 * LineWidth);// + 100;

			m_GridVert[iOffset + 6] = (float)(-10 * LineWidth);
			m_GridVert[iOffset + 7] = 0;
			m_GridVert[iOffset + 8] = (float)(i * LineWidth) - 10 * LineWidth;

			m_GridVert[iOffset + 9] = (float)(10 * LineWidth);
			m_GridVert[iOffset + 10] = 0;
			m_GridVert[iOffset + 11] = (float)(i * LineWidth) - 10 * LineWidth;
		}

		m_GridVertArray.create();
		// Copy data to video memory
		// Vertex data
		if (!m_GridVertArray.bind())
			return false;
		m_GridVertArray.setUsagePattern(QGLBuffer::DynamicDraw);
		m_GridVertArray.allocate(m_GridVert, sizeof(GLfloat) * 21 * 2 * 3 * 2);
		DEBUG("m_GridVertArray");

		DEBUG(m_GridVertArray.size());
		m_GridVertArray.release();

		m_GridColour = new float[NumLines * PointsPerLine * CoordsPerPoint];

		for (unsigned int j = 0; j < (unsigned int)NumLines; j ++)
		{
			unsigned int jOffset = j * PointsPerLine * CoordsPerPoint;

			if ((j == 21) || (j == 20))
			{
				m_GridColour[jOffset] = 0.6f;
				m_GridColour[jOffset + 1] = 0.49f;
				m_GridColour[jOffset + 2] = 0.25f;

				m_GridColour[jOffset + 3] = 0.6f;
				m_GridColour[jOffset + 4] = 0.49f;
				m_GridColour[jOffset + 5] = 0.25f;
			}
			else
			{
				m_GridColour[jOffset]	  = 0.34f;
				m_GridColour[jOffset + 1] = 0.34f;
				m_GridColour[jOffset + 2] = 0.34f;

				m_GridColour[jOffset + 3] = 0.34f;
				m_GridColour[jOffset + 4] = 0.34f;
				m_GridColour[jOffset + 5] = 0.34f;
			}
		}
		m_GridColourArray.create();
		// Copy data to video memory
		// Vertex data
		if (!m_GridColourArray.bind())
			return false;
		m_GridColourArray.setUsagePattern(QGLBuffer::DynamicDraw);
		m_GridColourArray.allocate(m_GridColour, sizeof(GLfloat) * 21 * 2 * 3 * 2);
		DEBUG("m_GridColourArray");
		DEBUG(m_GridColourArray.size());
		m_GridColourArray.release();

		return true;
	}

	void MBVRNode::DrawGrid()
	{
		m_GridVertArray.bind();
		GLDEBUG();
		glVertexPointer(3, GL_FLOAT, 0, 0);
		GLDEBUG();

		BREAKPOINT();

		// Texture coordinates
		m_GridColourArray.bind();
		GLDEBUG();
		glColorPointer(3, GL_FLOAT, 0, 0);
		GLDEBUG();

		glDrawArrays(GL_LINES, 0, 42 * 2);

		BREAKPOINT();
	}

	//-----------------------------------------------------------------------------
	// Controller handling Stuff
	//-----------------------------------------------------------------------------

	bool MBVRNode::InitVRActions()
	{
		CurrentSurfacePoint = new SurfacePoint();
		VRBrushPicker = new VRPicker();

		vr::EVRInitError eError = vr::VRInitError_None;
		vr::EVRInputError inputError = vr::VRInputError_None;

		if (vr_pointer != NULL)
		{
			DEBUG("Setting Action ManifestPath \n");

			QDir pluginDir(Kernel()->PluginDirectory("MudboxVR"));
			pluginDir.cd("../VRActionSets");
			QString ActionManifestFileName = "mudboxvr_actions.json";
			QFileInfo ActionManifestPath(pluginDir, ActionManifestFileName);
			const char * ConstCharActionManifestPath = ActionManifestPath.filePath().toLocal8Bit().data();

			inputError = vr::VRInput()->SetActionManifestPath(ConstCharActionManifestPath);
			if (inputError != vr::VRInputError_None) { VR_INPUT_DEBUG(inputError); return false; }

			inputError = vr::VRInput()->GetActionHandle("/actions/demo/in/RightHandTrigger", &m_actionRightHandTrigger);
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

			BREAKPOINT();

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

		BWireframe = GetDigitalActionState(m_actionToggleWireframe);

		//BrushStrokeAction = GetDigitalActionState(m_actionRightHandTrigger);

		if (GetDigitalActionState(m_actionRightHandTrigger) && (BrushStrokeAction == false))
		{
			BrushStrokeAction = true;
			HUDPRINT("VRBrushStrokeBegin");
			DEBUG("VRBrushStrokeBegin");
			VRBrushStrokeBegin();
		}
		if (BrushStrokeAction)
		{
			VRBrushStrokeAction();
			HUDPRINT("VRBrushStrokeAction");
			DEBUG("VRBrushStrokeAction");
		}
		if (!GetDigitalActionState(m_actionRightHandTrigger) && (BrushStrokeAction == true))
		{
			VRBrushStrokeEnd();
			BrushStrokeAction = false;
			HUDPRINT("VRBrushStrokeEnd");
			DEBUG("VRBrushStrokeEnd");
		}

		vr::VRInputValueHandle_t ulHapticDevice;
		if (GetDigitalActionRisingEdge(m_actionToggleWireframe, &ulHapticDevice))
		{
			if (ulHapticDevice == m_rHand[Left].m_source)
			{
				//vr::VRInput()->TriggerHapticVibrationAction( m_rHand[Left].m_actionHaptic, 0, 10, 4.f, 100.0f, vr::k_ulInvalidInputValueHandle );
			}
			if (ulHapticDevice == m_rHand[Right].m_source)
			{
				//vr::VRInput()->TriggerHapticVibrationAction(m_rHand[Right].m_actionHaptic, 0, 10, 4.f, 100.0f, vr::k_ulInvalidInputValueHandle);

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

				m_rHand[eHand].m_rmat4Pose._41 *= 500;
				m_rHand[eHand].m_rmat4Pose._42 *= 500;
				m_rHand[eHand].m_rmat4Pose._43 *= 500;
				m_rHand[eHand].m_rmat4Pose = m_rHand[eHand].m_rmat4Pose * WorldSpaceAdjuster;
				//m_rHand[eHand].m_rmat4Pose.Invert();
				DEBUG(eHand);
				m_RightHand->UpdatePosition(m_rHand[eHand].m_rmat4Pose);

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

	bool MBVRNode::SetupControllerGeometry()
	{
		for (EHand eHand = Left; eHand <= Right; ((int&)eHand)++)
		{
			//m_vrControllerMeshes.push_back(new RightHandMesh());

		}
		m_RightHand = new RightHandMesh();

		Matrix IdentityMat;
		IdentityMat.SetIdentity();

		m_RightHand->UpdatePosition(IdentityMat);
		if (!m_RightHand->GeomInitialize(NULL))
			return false;
		return true;
	}

	void MBVRNode::DrawVRController()
	{
	}

	//------------------------------------------------VR Brush stroke stuff-------------------------------------------------------

	void MBVRNode::VRBrushStrokeBegin()
	{
		//m_VRBrushStrokeBegin.Trigger();
		m_SurfacePointBuffer.resize(0);

		BREAKPOINT();
		if (m_mbvrMeshes.size() < 1)
			return;
		CurrentPickedMesh = m_mbvrMeshes[0];

		BREAKPOINT();
		VRBrushPicker->SetMesh(CurrentPickedMesh->getGeometry().ActiveLevel());

		BREAKPOINT();
		VRSelectedBrush->BeginStroke( CurrentPickedMesh->getGeometry().ActiveLevel(),BrushOperation::eModNormal, BrushOperation::eTriggerPrimary);

		LogBrushStats();

		BREAKPOINT();
		FloodMesh = false;
	}

	void MBVRNode::VRBrushStrokeAction()
	{
		BREAKPOINT();
		GLDEBUG();

		auto RightHandPose = m_rHand[Right].m_rmat4Pose;

		BREAKPOINT();

			Matrix MeshWorldToLocalTransform = CurrentPickedMesh->getGeometry().Transformation()->WorldToLocalMatrix();
			//Matrix MeshWorldToLocalTransform = CurrentPickedMesh->Geometry()->Transformation()->LocalToWorldMatrix();

			Vector RightHandPosition( 0, 0, 0);
			RightHandPosition =  RightHandPosition * RightHandPose * MeshWorldToLocalTransform;
			logVector(RightHandPosition, "RightHandPosition");
			//RightHandPosition = RightHandPosition * RightHandPose;

		BREAKPOINT();

			Vector RightHandDirection( 0, 0, -1000);
			RightHandDirection = RightHandDirection * RightHandPose * MeshWorldToLocalTransform;
			logVector(RightHandDirection, "RightHandDirection");
			//RightHandDirection = RightHandDirection * RightHandPose;

		BREAKPOINT();


		if (!VRBrushPicker->Pick(RightHandPosition, RightHandDirection, false, *CurrentSurfacePoint, .5f))
		{
			m_RightHand->UpdateRay( -1000 );
			DEBUG("Out of bounds");
			IsIntersecting = false;
			return;
		}
		DEBUG("In Bounds");
		IsIntersecting = true;
		AxisAlignedBoundingBox AABB(Vector(0, 0, 0), 10.0f);
		VRSelectedBrush->BrushSize();
		BREAKPOINT();


		//IntersectionPoint = CurrentSurfacePoint->WorldPosition();// +(RightHandDirection - RightHandPosition).Normalize() * 0.1f;

		IntersectionPoint = CurrentSurfacePoint->WorldPosition();
		BREAKPOINT();
		m_RightHand->UpdateRay( -(CurrentSurfacePoint->WorldPosition() - (Vector(0, 0, 0) * RightHandPose)).Length());

		DEBUG(CurrentSurfacePoint->FaceIndex());

		VRSelectedBrush->SetBrushStrength(10.f);

		BREAKPOINT();

		VRSelectedBrush->AddPatch(CurrentSurfacePoint, CurrentSurfacePoint->WorldPosition(), 2, 3,5, AABB);

		BREAKPOINT();
		Vector Move(0,0,0);
		
		if (m_SurfacePointBuffer.size() > 1)
		{
			Move = CurrentSurfacePoint->WorldPosition() -
				m_SurfacePointBuffer[m_SurfacePointBuffer.size() - 2]->WorldPosition();
		}

		VRSelectedBrush->SetPatchParameters(CurrentSurfacePoint, 6, 6, Move);
		VRSelectedBrush->
		BREAKPOINT();

		m_SurfacePointBuffer.push_back(CurrentSurfacePoint);
		BREAKPOINT();

		CurrentPickedMesh->UpdateDirtyVertices();
		BREAKPOINT();

		DEBUG("BrushSize");
		DEBUG(VRSelectedBrush->BrushSize());
		DEBUG("BrushStrength");
		DEBUG(VRSelectedBrush->BrushStrength());

		FloodMesh = true;

		BREAKPOINT();
	}

	void MBVRNode::VRBrushStrokeEnd()
	{
		//m_VRBrushStrokeEnd.Trigger();

		LogBrushStats();

		if (FloodMesh)
			VRSelectedBrush->Flood(CurrentPickedMesh->getGeometry().ActiveLevel(), .1, false);

		VRSelectedBrush->EndStroke();
	}

	void MBVRNode::LogBrushStats()
	{

		logVector(CurrentSurfacePoint->LocalPosition(), "LocalPosition");
		logVector(CurrentSurfacePoint->WorldPosition(), "WorldPosition");
		logVector(CurrentSurfacePoint->LocalNormal(), "LocalNormal");
		logVector(CurrentSurfacePoint->WorldNormal(), "WorldNormal");
		logVector(CurrentSurfacePoint->FaceCoordinates(), "FaceCoordinates");

		DEBUG("BrushSize");
		DEBUG(VRSelectedBrush->BrushSize());
		DEBUG("BrushStrength");
		DEBUG(VRSelectedBrush->BrushStrength());
		DEBUG("EffectiveBrushSize");
		DEBUG(VRSelectedBrush->EffectiveBrushSize());
		DEBUG("EffectiveBrushStrength");
		DEBUG(VRSelectedBrush->EffectiveBrushStrength());
		DEBUG("BrushSizeBias");
		DEBUG(VRSelectedBrush->BrushSizeBias());
		DEBUG("BrushStrengthBias");
		DEBUG(VRSelectedBrush->BrushStrengthBias());
		DEBUG("UsesStrokeSmoothing");
		DEBUG(VRSelectedBrush->UsesStrokeSmoothing());
		DEBUG("UsesStampRandomize");
		DEBUG(VRSelectedBrush->UsesStampRandomize());
		DEBUG("IsMaskEnabled");
		DEBUG(VRSelectedBrush->IsMaskEnabled());

		DEBUG("EffectiveBrushSize");
		DEBUG(VRSelectedBrush->EffectiveBrushSize());

		DEBUG("EffectiveBrushSize");
		DEBUG(VRSelectedBrush->EffectiveBrushSize());

		DEBUG("VRSelectedBrush->FalloffData().Size()");
		DEBUG(VRSelectedBrush->FalloffData().Size() );


		DEBUG("VRSelectedBrush->Falloff");
		DEBUG(VRSelectedBrush->Falloff()->Length());

		auto test =VRSelectedBrush->ActiveConfiguration();

		DEBUG(test->ToolTip());
	}


	void MBVRNode::DrawIntersectionPoint(vr::Hmd_Eye nEye)
	{
		if (IsIntersecting)
		{
			glEnable(GL_POINT_SMOOTH);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glMatrixMode(GL_PROJECTION);

			GLDEBUG();
			glPushMatrix();
			{

				auto projection = vr_HMD->GetCurrentProjectionMatrix(nEye);
				GLDEBUG();

				glLoadMatrixf(projection);
				GLDEBUG();

				glMatrixMode(GL_MODELVIEW);
				GLDEBUG();

				glPushMatrix();
				{
					GLDEBUG();
					auto ModelView = vr_HMD->GetCurrentModelViewMatrix(nEye);
					GLDEBUG();
					glLoadMatrixf(ModelView);
					GLDEBUG();

					GLDEBUG();
					glBegin(GL_POINTS);
					GLDEBUG();

					
					glPointSize(10);
					GLDEBUG();
					glColor4f(1, 0, 0, 1);
					GLDEBUG();
					glVertex3f(IntersectionPoint.x, IntersectionPoint.y, IntersectionPoint.z);
					GLDEBUG();

					glEnd();
					GLDEBUG();

				}
				glPopMatrix();
				GLDEBUG();

				glMatrixMode(GL_PROJECTION);
				GLDEBUG();
			}
			glPopMatrix();
			GLDEBUG();
			glDisable(GL_POINT_SMOOTH);
			glDisable(GL_BLEND);
			GLDEBUG();
		}
		

	}
}