#pragma once
#include "VRViewport.h"
#include "MBVRMesh.h"
#include "MBVRPicker.h"
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
	// MudBox Methods and data
	//-----------------------------------------------------------------------------

	public:

		//eventHandling

		void OnEvent(const EventGate &cEvent);

		aevent m_eEachTick;
		aevent frameUpdate;
		aevent m_newMeshAdded;

		aevent m_VRBrushStrokeBegin;
		aevent m_VRBrushStrokeEnd;

		uint framecount=0;
		uint tick=0;

	//-----------------------------------------------------------------------------
	// OpenVR Methods and data
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
	// OpenGL Methods and data
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

		bool BWireframe =false;


		//-----------------------------------------------------------------------------
		// Geometry handling Methods and data
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

		//--------------------------Grid stuff--------------------
		bool SetupGridGeometry();

		void DrawGrid();

		float * m_GridVert;
		QGLBuffer m_GridVertArray;
		unsigned int m_GridVertArraySize;

		float * m_GridColour;
		QGLBuffer m_GridColourArray;

		//-----------------------------------------------------------------------------
		// VR controller Methods and data
		//-----------------------------------------------------------------------------
	public:

		bool InitVRActions();

		void VRContrHandleInputs();

		void ProcessVREvent(vr::VREvent_t vr_event);

		bool SetupControllerGeometry();

		void DrawVRController();

		RightHandMesh * m_RightHand;

		//std::vector<RightHandMesh*> m_vrControllerMeshes;

		ControllerInfo_t m_rHand[2];

		//Other Actions

		vr::VRActionHandle_t m_actionRightHandTrigger = vr::k_ulInvalidActionHandle;

		vr::VRActionHandle_t m_actionToggleWireframe = vr::k_ulInvalidActionHandle;
		vr::VRActionHandle_t m_actionHideCubes = vr::k_ulInvalidActionHandle;
		vr::VRActionHandle_t m_actionHideThisController = vr::k_ulInvalidActionHandle;
		vr::VRActionHandle_t m_actionTriggerHaptic = vr::k_ulInvalidActionHandle;
		vr::VRActionHandle_t m_actionAnalongInput = vr::k_ulInvalidActionHandle;

		vr::VRActionSetHandle_t m_actionsetDemo = vr::k_ulInvalidActionSetHandle;

		VRMesh *CurrentPickedMesh = NULL;
		VRPicker *VRBrushPicker;

		void VRBrushStrokeBegin();
		void VRBrushStrokeAction();
		void VRBrushStrokeEnd();

		bool BrushStrokeAction = false;

		bool FloodMesh = false;

		aptr<BrushOperation> VRSelectedBrush;
		//BrushOperation * VRSelectedBrush;
		std::vector<SurfacePoint *> m_SurfacePointBuffer;

		SurfacePoint * CurrentSurfacePoint;

		Vector *Original_pos;

		bool IsIntersecting = true;
		Vector IntersectionPoint;

		void DrawIntersectionPoint(vr::Hmd_Eye nEye);
		void LogBrushStats();

	};

}