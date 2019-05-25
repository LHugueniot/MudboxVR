#pragma once
#include "VRViewport.h"


namespace mudbox {

	//----------------------------------------------------------------------mbvrNode------------------------------------------------------------------------

	class mbvrNode : public Node
	{
	public:
		mbvrNode();
		~mbvrNode();

		void Shutdown();

		bool InitializeMBVR();

		bool Initialized=false;

	//-----------------------------------------------------------------------------
	// MudBox Stuff
	//-----------------------------------------------------------------------------

	public:

		//eventHandling

		void OnEvent(const EventGate &cEvent);


		aevent m_eEachTick;
		aevent frameUpdate;
		uint framecount=0;
		uint tick=0;

		Vector *Original_pos;


	//-----------------------------------------------------------------------------
	// VR Stuff
	//-----------------------------------------------------------------------------

	public:

		static bool InitVR();
		static bool InitCompositor();

		static vr::IVRSystem* vr_pointer;
		static vr::IVRCompositor *vr_compositor;
		uint32_t m_nRenderWidth;
		uint32_t m_nRenderHeight;
		VRHead *vr_HMD;
		


	//-----------------------------------------------------------------------------
	// OpenGL Stuff
	//-----------------------------------------------------------------------------

	public:

		bool InitOGL();

		bool SetupVRHead();

		bool SetupCameras();

		bool SetupStereoRenderTargets();

		bool CreateFrameBuffer(int _width, int _Height, FrameBufferDescriptor &frameBufferDesc);

		void RenderStereoTargets();

		void RenderScene(vr::Hmd_Eye nEye);

		Texture* m_leftEyeTexture;
		GLuint m_leftEyeID;

		Texture* m_rightEyeTexture;
		GLuint m_rightEyeID;

		Camera * m_originalCamera;

		MBVRCamera * m_leftEyeCamera;
		MBVRCamera * m_rightEyeCamera;

		//----------------------Geometry handling temporary------------------------
	public:

		bool setupGeometry();
		void drawGeom();
		void drawMudBoxGeom(vr::Hmd_Eye nEye);

		uint m_nNumVerts;
		GLshort m_nTotalVerts;

		QGLBuffer vertexArrObj;
		QGLBuffer vertexIndexObj;
		QGLBuffer vertexColoursObj;


		//void RenderScene(vr::Hmd_Eye nEye);


		//std::string m_strPoseClasses;
		//char m_rDevClassChar[vr::k_unMaxTrackedDeviceCount];
		//SDL Stuff

		//GLuint m_unSceneProgramID;
		//GLuint m_unCompanionWindowProgramID;
		//GLuint m_unControllerTransformProgramID;
		//GLuint m_unRenderModelProgramID;
		//
		//GLint m_nSceneMatrixLocation;
		//GLint m_nControllerMatrixLocation;
		//GLint m_nRenderModelMatrixLocation;

		//struct FramebufferDesc
		//{
		//	GLuint m_nDepthBufferId;
		//	GLuint m_nRenderTextureId;
		//	GLuint m_nRenderFramebufferId;
		//	GLuint m_nResolveTextureId;
		//	GLuint m_nResolveFramebufferId;
		//};
		//FramebufferDesc leftEyeDesc;
		//FramebufferDesc rightEyeDesc;
		//
		//bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc);
		//
		//struct Vector2
		//{
		//	float x;
		//	float y;
		//	Vector2(float _x, float _y) {
		//		x = _x;  y = _y;
		//	}
		//};
		//
		//struct VertexDataWindow
		//{
		//	Vector2 position;
		//	Vector2 texCoord;
		//
		//	VertexDataWindow(const Vector2 & pos, const Vector2 tex) : position(pos), texCoord(tex) {	}
		//};

		//GLuint m_glSceneVertBuffer;
		//GLuint m_unSceneVAO;
		//GLuint m_unCompanionWindowVAO;
		//GLuint m_glCompanionWindowIDVertBuffer;
		//GLuint m_glCompanionWindowIDIndexBuffer;
		//unsigned int m_uiCompanionWindowIndexSize;
		//int m_iValidPoseCount;
		//Error Handling
	};

}