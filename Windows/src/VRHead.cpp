#include "VRHead.h"

namespace mudbox
{
	VRHead::VRHead(vr::IVRSystem* _vrHeadPtr, float _fNearClip , float _fFarClip) :
		m_vrHeadPtr(_vrHeadPtr),
		m_fNearClip(_fNearClip),
		m_fFarClip(_fFarClip)
	{
		WorldScale = { 1.0f, 0.f, 0.f, 0.f,
					  0.f, 1.0f, 0.f, 0.f,
					  0.f, 0.f, 1.0f, 0.f,
					  0.f, 0.f, 0.f,  1.f};
		std::string Dimensions("m_fNearClip " + std::to_string(m_fNearClip)+ " m_fFarClip " + std::to_string(m_fFarClip) + "\n");
		PRINT(Dimensions);
		DEBUG(Dimensions);
	}


	VRHead::~VRHead()
	{
	}

	bool VRHead::SetupEyes()
	{
		if (!m_vrHeadPtr)
			return false;
		printMud("Starting SetupEyes\n");
		logMud("Starting SetupEyes\n");

		m_mat4ProjectionLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
		m_mat4ProjectionRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
		m_mat4eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
		m_mat4eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);

		return true;
	}

	//Matrix VRHead::GetControllerPose(EHand hand)
	//{
	//
	//}
	//-----------------------------------------------------------------------------
	// Purpose: Gets an HMDMatrixPoseEye with respect to nEye.
	//-----------------------------------------------------------------------------

	Matrix VRHead::GetHMDMatrixPoseEye(vr::Hmd_Eye nEye)
	{
		if (!m_vrHeadPtr)
			return Matrix();

		vr::HmdMatrix34_t matEyeRight = m_vrHeadPtr->GetEyeToHeadTransform(nEye);
		Matrix matrixObj(
			matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
			matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
			matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
			matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f
		);

		return matrixObj.Invert();
	}


	//-----------------------------------------------------------------------------
	// Purpose: Gets a Matrix Projection Eye with respect to nEye.
	//-----------------------------------------------------------------------------

	Matrix VRHead::GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye)
	{

		if (!m_vrHeadPtr)
			return Matrix();

		vr::HmdMatrix44_t mat = m_vrHeadPtr->GetProjectionMatrix(nEye, m_fNearClip, m_fFarClip);
		
		return Matrix(
			mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
			mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
			mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
			mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
		);
	}


	//-----------------------------------------------------------------------------
	// Purpose: Gets a Current View Projection Matrix with respect to nEye,
	//          which may be an Eye_Left or an Eye_Right.
	//-----------------------------------------------------------------------------
	Matrix VRHead::GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye)
	{
		Matrix matMVP;

		if (nEye == vr::Eye_Left)
		{
			matMVP = m_mat4ProjectionLeft * m_mat4eyePosLeft * m_mat4HMDPose;
		}
		else if (nEye == vr::Eye_Right)
		{
			matMVP = m_mat4ProjectionRight * m_mat4eyePosRight *  m_mat4HMDPose;
		}

		return matMVP;
	}

	//-----------------------------------------------------------------------------
	// Purpose: Gets a Current Model View Matrix with respect to nEye,
	//          which may be an Eye_Left or an Eye_Right.
	//-----------------------------------------------------------------------------
	Matrix VRHead::GetCurrentProjectionMatrix(vr::Hmd_Eye nEye)
	{
		Matrix matP;

		if (nEye == vr::Eye_Left)
		{
			matP = m_mat4ProjectionLeft;
		}
		else if (nEye == vr::Eye_Right)
		{
			matP = m_mat4ProjectionRight;
		}

		return matP;
	}

	//-----------------------------------------------------------------------------
	// Purpose: Gets a Current Model View Matrix with respect to nEye,
	//          which may be an Eye_Left or an Eye_Right.
	//-----------------------------------------------------------------------------
	Matrix VRHead::GetCurrentModelViewMatrix(vr::Hmd_Eye nEye)
	{
		return m_mat4HMDPose;
	}

	//-----------------------------------------------------------------------------
	// Purpose: Gets Rotation with respect to nEye.
	//-----------------------------------------------------------------------------

	vr::HmdQuaternion_t VRHead::GetRotation() {
		vr::HmdQuaternion_t q;

		vr::HmdMatrix34_t matrix= m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;

		q.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
		q.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
		q.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
		q.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;
		q.x = copysign(q.x, matrix.m[2][1] - matrix.m[1][2]);
		q.y = copysign(q.y, matrix.m[0][2] - matrix.m[2][0]);
		q.z = copysign(q.z, matrix.m[1][0] - matrix.m[0][1]);
		return q;
	}


	Vector VRHead::GetPosition(vr::Hmd_Eye nEye)
	{

		vr::HmdMatrix34_t matrix = m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;

		Vector Position(matrix.m[0][3], matrix.m[1][3], matrix.m[2][3]);

		return Position;
	}

	//-----------------------------------------------------------------------------
	// Purpose: Updates HMD Matrix
	//-----------------------------------------------------------------------------

	void VRHead::UpdateHMDMatrixPose()
	{
		if (!m_vrHeadPtr)
			return;

		vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

		//m_iValidPoseCount = 0;
		//m_strPoseClasses = "";
		for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
		{
			if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
			{
				//m_iValidPoseCount++;
				m_rmat4DevicePose[nDevice] = ConvertSteamVRMatrixToMatrix(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
				if (m_rDevClassChar[nDevice] == 0)
				{
					switch (m_vrHeadPtr->GetTrackedDeviceClass(nDevice))
					{
					case vr::TrackedDeviceClass_Controller:        m_rDevClassChar[nDevice] = 'C'; break;
					case vr::TrackedDeviceClass_HMD:               m_rDevClassChar[nDevice] = 'H'; break;
					case vr::TrackedDeviceClass_Invalid:           m_rDevClassChar[nDevice] = 'I'; break;
					case vr::TrackedDeviceClass_GenericTracker:    m_rDevClassChar[nDevice] = 'G'; break;
					case vr::TrackedDeviceClass_TrackingReference: m_rDevClassChar[nDevice] = 'T'; break;
					default:                                       m_rDevClassChar[nDevice] = '?'; break;
					}
				}
				//m_strPoseClasses += m_rDevClassChar[nDevice];
			}
		}

		if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
		{
			m_mat4HMDPose = m_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd];
			m_mat4HMDPose._41 *= 100;
			m_mat4HMDPose._42 *= 100;
			m_mat4HMDPose._43 *= 100;
			m_mat4HMDPose.Invert();
			//m_mat4HMDPose = WorldScale * m_mat4HMDPose;
		}
	}

	//----------------------------------------------------MBVR Camera-----------------------------------------

	IMPLEMENT_CLASS(MBVRCamera, Camera, "MBVR Camera");

	MBVRCamera::MBVRCamera(void) : 
		MBVRCamIsInitialised(false),
		m_ForwardsDir(0,0,1), m_UpDir(0, 1, 0), m_RightDir(1, 0, 0),
		m_Yaw(0), m_Pitch(0), m_Roll(0), 
		m_Position(0, 0, 0),
		m_MVP(), 
		m_FOV(0), m_Near(0), m_Far(0), m_AspectRatio(0),
		m_Scale2D(0), m_Angle2D(0),
		m_Position2D(1,1,1),
		m_Orthographic(true),
		m_dirty(false)
	{
	}

	void MBVRCamera::InitVRMB(VRHead *_headPtr, vr::EVREye nEye, Camera * _originalPointer)
	{
		BREAKPOINT();
		if (_originalPointer != NULL)
		{
			m_originalCamera = _originalPointer;
		}
		if (_headPtr != NULL)
		{
			BREAKPOINT();
			m_nEye = nEye;
			BREAKPOINT();
			m_headPtr = _headPtr;
			BREAKPOINT();
			MBVRCamIsInitialised = true;
			BREAKPOINT();
			UpdateFromHMD();
			BREAKPOINT();
			m_FOV = 113;
			BREAKPOINT();
			m_Near = m_headPtr->m_fNearClip;
			BREAKPOINT();
			m_Far = m_headPtr->m_fFarClip;
			BREAKPOINT();
			uint32_t *height = new uint32_t(1);
			BREAKPOINT();
			uint32_t *width= new uint32_t (1);
			BREAKPOINT();

			m_headPtr->m_vrHeadPtr->GetRecommendedRenderTargetSize(width, height);
			BREAKPOINT();

			m_AspectRatio = (*width )/ (*height);
			BREAKPOINT();
			m_Scale2D, m_Angle2D = 1;
			BREAKPOINT();
			m_Position2D.x = m_Position.x;
			BREAKPOINT();
			m_Position2D.y = m_Position.y;
			BREAKPOINT();
			m_Position2D.z = m_Position.z;
			BREAKPOINT();

			m_Orthographic = true;
			BREAKPOINT();
		}
		else
		{
			BREAKPOINT();
			MBVRCamIsInitialised = false;
		}
	}

	void MBVRCamera::UpdateFromHMD(void)
	{
		if (MBVRCamIsInitialised)
		{
			BREAKPOINT();
			mudbox::Matrix Eye_Pose = m_headPtr->GetCurrentModelViewMatrix(m_nEye) * m_headPtr->GetHMDMatrixPoseEye(m_nEye);

			BREAKPOINT();

			m_Position.m_fX = Eye_Pose._41;
			m_Position.m_fY = Eye_Pose._42;
			m_Position.m_fZ = Eye_Pose._43;
			BREAKPOINT();

			//Update Forwards Up Right Directions

			m_ForwardsDir.m_fX = Eye_Pose._31;
			m_ForwardsDir.m_fY = Eye_Pose._32;
			m_ForwardsDir.m_fZ = Eye_Pose._33;
			BREAKPOINT();

			m_UpDir.m_fX = Eye_Pose._21;
			m_UpDir.m_fY = Eye_Pose._22;
			m_UpDir.m_fZ = Eye_Pose._23;
			BREAKPOINT();

			m_RightDir.m_fX = -Eye_Pose._11;
			m_RightDir.m_fY = -Eye_Pose._12;
			m_RightDir.m_fZ = -Eye_Pose._13;
			BREAKPOINT();

			//Update Yaw Pitch Roll

			m_Yaw = atan2(m_ForwardsDir.m_fY, m_ForwardsDir.m_fX);
			BREAKPOINT();
			m_Pitch = -asin(m_ForwardsDir.m_fZ);
			BREAKPOINT();

			float planeRightX = sin(m_Yaw);
			BREAKPOINT();
			float planeRightY = -cos(m_Yaw);
			BREAKPOINT();

			float Roll = asin(m_UpDir.m_fX * planeRightX + m_UpDir.m_fY * planeRightY);
			BREAKPOINT();
			//if (m_UpDir.m_fZ < 0){ float sign = 1; m_Roll = copysign(Roll, sign) * M_PI - Roll; }
			BREAKPOINT();

			m_MVP = m_headPtr->GetCurrentViewProjectionMatrix(m_nEye);
			BREAKPOINT();

			m_dirty = false;
			BREAKPOINT();

		}

	}

	Vector MBVRCamera::Forward(void) const
	{
		BREAKPOINT();
		return m_ForwardsDir;
	}

	Vector  MBVRCamera::Up(void) const
	{
		BREAKPOINT();
		return m_UpDir;
	}

	Vector MBVRCamera::Right(void) const 
	{
		BREAKPOINT();
		return m_RightDir;
	}

	float  MBVRCamera::Yaw(void) const
	{
		BREAKPOINT();
		return m_Yaw;
	}


	float MBVRCamera::Pitch(void) const
	{
		BREAKPOINT();
		return m_Pitch;
	}

	float  MBVRCamera::Roll(void) const
	{
		BREAKPOINT();
		return m_Roll;
	}

	Vector  MBVRCamera::Position(void) const
	{
		BREAKPOINT();
		return m_Position;
	}

	void  MBVRCamera::SetYawPitchRoll( const Vector &vYPR )
	{
		BREAKPOINT();
		m_Yaw = vYPR.x; 
		m_Pitch = vYPR.y; 
		m_Roll = vYPR.z;
	}

	void  MBVRCamera::AddYawPitchRoll( const Vector &vYPR )
	{
		BREAKPOINT();
		m_Yaw += vYPR.x;
		m_Pitch += vYPR.y;
		m_Roll += vYPR.z;
	}

	void MBVRCamera::SetForwardUpRight( const Vector &vForward, const Vector &vUp, const Vector &vRight )
	{
		BREAKPOINT();
		m_ForwardsDir = vForward;
		m_UpDir = vUp;
		m_RightDir = vRight;
	}

	void MBVRCamera::SetTarget( const Vector &vTarget, const Vector &vUpDir)
	{
		BREAKPOINT();

	}

	void  MBVRCamera::SetPosition(const Vector &vPosition)
	{
		BREAKPOINT();
		m_Position = vPosition;
	}

	void  MBVRCamera::MoveForward( float fRange )
	{
		BREAKPOINT();
		m_Position += m_ForwardsDir * fRange;
	}

	void MBVRCamera::MoveUp( float fRange )
	{
		BREAKPOINT();
		m_Position.y += m_UpDir * fRange;
	}

	void  MBVRCamera::MoveRight( float fRange )
	{
		BREAKPOINT();
		m_Position += m_RightDir * fRange;
	}

	void  MBVRCamera::MoveBackward( float fRange )
	{
		BREAKPOINT();
		m_Position += m_ForwardsDir * -fRange;
	}

	void  MBVRCamera::MoveDown( float fRange )
	{
		BREAKPOINT();
		m_Position.y += m_UpDir * -fRange;
	}
	
	void  MBVRCamera::MoveLeft( float fRange)
	{
		BREAKPOINT();
		m_Position += m_RightDir * -fRange;
	}

	Vector  MBVRCamera::Project( const Vector &vWorldPosition ) const
	{
		
		BREAKPOINT();
		return m_originalCamera->Project(vWorldPosition);
	}

	Vector MBVRCamera::UnProject( const Vector &vScreenPosition ) const
	{
		BREAKPOINT();
		return m_originalCamera->UnProject(vScreenPosition);
	}
	
	mudbox::Matrix MBVRCamera::Matrix( bool bOffscreen) const
	{
		BREAKPOINT();
		mudbox::Matrix ModelView = m_headPtr->GetCurrentModelViewMatrix(m_nEye);
		BREAKPOINT();
		mudbox::Matrix Projection = m_headPtr->GetCurrentProjectionMatrix(m_nEye);
		BREAKPOINT();
		mudbox::Matrix MVP= ModelView * Projection;

		logMatrix(MVP, __FUNCTION__);
		BREAKPOINT();
		return (MVP);
	}

	float MBVRCamera::FOV(void) const
	{
		BREAKPOINT();
		return m_FOV;
	}
	
	float MBVRCamera::Near(void) const
	{
		BREAKPOINT();
		return m_Near;
	}
	
	float  MBVRCamera::Far(void) const
	{
		BREAKPOINT();
		return m_Far;
	}
	
	float  MBVRCamera::AspectRatio(void) const
	{
		BREAKPOINT();
		return m_AspectRatio;
	}
	
	void  MBVRCamera::SetFOV( float fFOV	 )
	{
		BREAKPOINT();
		m_FOV = fFOV;
	}
	
	void  MBVRCamera::SetNear( float fNear )
	{
		BREAKPOINT();
		m_Near = fNear;
	}
	
	void  MBVRCamera::SetFar( float fFar )
	{
		BREAKPOINT();
		m_Far = fFar;
	}
	
	void  MBVRCamera::SetAspectRatio( float fAspectRatio )
	{
		BREAKPOINT();
		m_AspectRatio = fAspectRatio;
	}
	
	float  MBVRCamera::Scale2D(void) const
	{
		BREAKPOINT();
		return m_Scale2D;
	}
	
	float MBVRCamera::Angle2D(void) const
	{
		BREAKPOINT();
		return m_Angle2D;
	}
	
	Vector MBVRCamera::Position2D(void) const
	{
		BREAKPOINT();
		return m_Position2D;
	}
	
	void  MBVRCamera::SetScale2D( float fScale2D	)
	{
		BREAKPOINT();
		m_Scale2D = fScale2D;
	}
	
	void  MBVRCamera::SetAngle2D( float fAngle2D	)
	{
		BREAKPOINT();
		m_Angle2D = fAngle2D;
	}
	
	void MBVRCamera::SetPosition2D( const Vector &vPosition2D )
	{
		BREAKPOINT();
		m_Position2D = vPosition2D;
	}
	
	void MBVRCamera::CopyFrom( const Camera &cOther	)
	{
		BREAKPOINT();

	}

	bool MBVRCamera::IsEqualTo( const Camera &cOther ) const
	{
		BREAKPOINT();
		return false;
	}

	bool MBVRCamera::Pick( float fXPosition, float fYPosition, SurfacePoint &sResult )
	{
		BREAKPOINT();
		return m_originalCamera->Pick(fXPosition, fYPosition, sResult);
	}
	
	ScreenSpacePicker * MBVRCamera::GetScreenSpacePicker() const
	{
		BREAKPOINT();
		return m_originalCamera->GetScreenSpacePicker();
	}

	Vector  MBVRCamera::Aim() const
	{
		BREAKPOINT();
		return m_ForwardsDir;
	}

	void MBVRCamera::SetAim( const Vector &v )
	{
		BREAKPOINT();
	}

	bool MBVRCamera::Orthographic() const
	{
		BREAKPOINT();
		return m_Orthographic;
	}

	void MBVRCamera::SetOrthographic( bool isOrtho )
	{
		BREAKPOINT();
		m_Orthographic = isOrtho;
	}

	bool MBVRCamera::IsDirty(void) const
	{
		BREAKPOINT();
		return m_dirty;
	}

	void  MBVRCamera::SetDirty(bool bDirty)
	{
		BREAKPOINT();
		m_dirty = bDirty;
	}

}

