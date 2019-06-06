#pragma once
#include "VRViewport.h"
#include "MBVRMesh.h"
#include <unordered_set>
namespace mudbox {

	//----------------------------------------------------------------------MBVRNode------------------------------------------------------------------------

	class MBVRNode : public Node
	{
	public:
		MBVRNode();
		~MBVRNode();

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
		aevent m_newMeshAdded;
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

		bool SetupStereoRenderTargets();

		void RenderStereoTargets();

		void RenderScene(vr::Hmd_Eye nEye);

		static QGLContext * MBVRGLContext;

		Texture* m_leftEyeTexture;
		GLuint m_leftEyeID;

		Texture* m_rightEyeTexture;
		GLuint m_rightEyeID;

		Camera * m_originalCamera;

		bool BWireframe;


		//-----------------------------------------------------------------------------
		// Geometry handling Stuff
		//-----------------------------------------------------------------------------
	public:

		bool SetupTestGeometry();

		void DrawTestGeom();

		void DrawMBVRMesh(vr::Hmd_Eye nEye);

		void UpdateMBVRMeshes();

		uint m_nNumVerts;
		GLshort m_nTotalVerts;

		QGLBuffer vertexArrObj;
		QGLBuffer vertexIndexObj;
		QGLBuffer vertexColoursObj;

		std::vector<VRMesh*> m_mbvrMeshes;

		//World Space Adjustment Matrix

		Matrix WorldSpaceAdjuster;


		//-----------------------------------------------------------------------------
		// VR controller Stuff
		//-----------------------------------------------------------------------------
	public:

		bool InitVRActions();

		void VRContrHandleInputs();

		void ProcessVREvent(vr::VREvent_t vr_event);


		ControllerInfo_t m_rHand[2];

		//Other Actions

		vr::VRActionHandle_t m_actionToggleWireframe = vr::k_ulInvalidActionHandle;
		vr::VRActionHandle_t m_actionHideCubes = vr::k_ulInvalidActionHandle;
		vr::VRActionHandle_t m_actionHideThisController = vr::k_ulInvalidActionHandle;
		vr::VRActionHandle_t m_actionTriggerHaptic = vr::k_ulInvalidActionHandle;
		vr::VRActionHandle_t m_actionAnalongInput = vr::k_ulInvalidActionHandle;

		vr::VRActionSetHandle_t m_actionsetDemo = vr::k_ulInvalidActionSetHandle;

		//enum ActionMode
		//{
		//	None,
		//	Paint,
		//	Transform,
		//	Model
		//};
		//
		//ActionMode CurrentActionMode = None;
	};

}