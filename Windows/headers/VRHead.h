#pragma once
#include <mbvr_global.h>

namespace mudbox 
{
	class VRHead
	{
	public:
		VRHead(vr::IVRSystem* _vrHeadPtr, float _fNearClip, float _fFarClip);
		~VRHead();
		bool SetupEyes();
		void UpdateHMDMatrixPose();
		//void UpdateMudboxCameras();
		//Camera* GetCamera(vr::Hmd_Eye nEye);

		//Real Time Updates
		Matrix GetHMDMatrixPoseEye(vr::Hmd_Eye nEye);
		Matrix GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye);

		//returns whats saved in VRHead
		Matrix GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye);
		Matrix GetCurrentProjectionMatrix(vr::Hmd_Eye nEye);
		Matrix GetCurrentModelViewMatrix(vr::Hmd_Eye nEye);

		vr::HmdQuaternion_t GetRotation();
		Vector GetPosition(vr::Hmd_Eye nEye);

		vr::IVRSystem* m_vrHeadPtr;

		float m_fNearClip, m_fFarClip;

		Matrix m_mat4HMDPose;
		Matrix m_mat4HMDPoseNonInverted;
		Matrix m_mat4eyePosLeft;
		Matrix m_mat4eyePosRight;

		

		Matrix m_mat4ProjectionCenter;
		Matrix m_mat4ProjectionLeft;
		Matrix m_mat4ProjectionRight;

		vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];

		Matrix m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];

		char m_rDevClassChar[vr::k_unMaxTrackedDeviceCount];
	};

	class  MBVRCamera : public Camera
	{
		DECLARE_CLASS;

	protected:

	public:
		MBVRCamera(void);

		void InitVRMB(VRHead *_headPtr, vr::EVREye nEye);

		void UpdateFromHMD(void);

		/// Returns a normalized vector indicating the camera's direction of view in world space.
		virtual Vector Forward(void) const;

		/// Returns a normalized vector indicating the camera's "up" direction in world space.
		virtual Vector Up(void) const;

		/// Returns a normalized vector indicating the camera's "right" direction in world space.
		virtual Vector Right(void) const;

		/// \brief Returns the \b yaw of the camera in radians.
		///
		/// "Yaw" refers to the amount of rotation around the vertical (Y) axis.
		virtual float Yaw(void) const;

		/// \brief Returns the \b pitch of the current camera in radians.
		///
		/// "Pitch" refers to the amount of rotation around the horizontal axis running
		/// perpendicular to the direction of view.  (i.e., tilting the camera up and down)
		virtual float Pitch(void) const;

		/// \brief Returns the \b roll of the current camera in radians.
		///
		/// "Roll" refers to the amount of rotation around the camera's
		/// direction of view.  (i.e., tilting the camera from side to side)
		virtual float Roll(void) const;

		/// Returns the position of the camera in world space.
		virtual Vector Position(void) const;

		/// \brief Sets rotation of the camera, specified as yaw, pitch and roll values.
		///
		/// All values are in radians.  The position of the camera remains unchanged.
		///
		/// "Yaw" refers to the amount of rotation around the vertical (Y) axis.
		/// "Pitch" refers to the amount of rotation around the horizontal axis running
		/// perpendicular to the direction of view.  (i.e., tilting the camera up and down)
		/// "Roll" refers to the amount of rotation around the camera's
		/// direction of view.  (i.e., tilting the camera from side to side)
		virtual void SetYawPitchRoll(
			const Vector &vYPR		///< [in] Yaw, Pitch and Roll values in radians
		);

		/// \brief Add a rotation to the camera, specified as yaw, pitch and roll values.
		///
		/// All values are in radians.  The position of the camera remains unchanged.
		///
		/// "Yaw" refers to the amount of rotation around the vertical (Y) axis.
		/// "Pitch" refers to the amount of rotation around the horizontal axis running
		/// perpendicular to the direction of view.  (i.e., tilting the camera up and down)
		/// "Roll" refers to the amount of rotation around the camera's
		/// direction of view.  (i.e., tilting the camera from side to side)
		virtual void AddYawPitchRoll(
			const Vector &vYPR		///< [in] Yaw, Pitch and Roll values in radians
		);

		/// \brief Sets the rotation of the camera, specified as three vectors.
		///
		/// The specified vectors do not need to be normalized, but they do need to be non-zero,
		/// and all orthographic to each other.  Failure to do this will result in undefined or
		/// distorted camera views.
		virtual void SetForwardUpRight(
			const Vector &vForward,		///< [in] A vector pointing in the direction that the camera is to be facing
			const Vector &vUp,			///< [in] A vector pointing "up" relative to the camera.
			const Vector &vRight		///< [in] A vector pointing "right" relative to the camera.
		);

		/// \brief Set the rotation of the camera, by telling it what to point at.
		///
		/// The camera stays in its current position, but rotates so that the view direction
		/// points at the specified target.  By default, the new "up" vector will be as close to 
		/// vertical as possible, but you can override this to specify another up vector.
		virtual void SetTarget(
			const Vector &vTarget,						///< [in] A point in world space that the camera should face
			const Vector &vUpDir = Vector(0, 1, 0)	///< [in] The "up" vector for the new rotation.
		);

		/// Set the location of the camera in world space.
		virtual void SetPosition(
			const Vector &vPosition		///< [in] The camera's new position in world space.
		);

		/// Move the camera forward along its view direction. Its rotation remains unchanged.
		virtual void MoveForward(
			float fRange	///< [in] The (world-space) distance to move the camera.
		);

		/// Move the camera up.  Its rotation remains unchanged.
		virtual void MoveUp(
			float fRange	///< [in] The (world-space) distance to move the camera.
		);

		/// Move the camera right.  Its rotation remains unchanged.
		virtual void MoveRight(
			float fRange	///< [in] The (world-space) distance to move the camera.
		);

		/// Move the camera backward along its view direction. Its rotation remains unchanged.
		virtual void MoveBackward(
			float fRange	///< [in] The (world-space) distance to move the camera.
		);

		/// Move the camera down.  Its rotation remains unchanged.
		virtual void MoveDown(
			float fRange	///< [in] The (world-space) distance to move the camera.
		);

		/// Move the camera left.  Its rotation remains unchanged.
		virtual void MoveLeft(
			float fRange	///< [in] The (world-space) distance to move the camera.
		);

		/// \brief Takes a point in world space and returns its position in screen coordinates.
		///
		/// The X and Y values of the vector give the location in screen space.  Screen space
		/// goes from (-1,-1) in the lower-left to (1,1) in the upper right, with (0,0) being
		/// the center of the screen.
		///
		/// The returned Z value represents the distance from the camera to the point.  A value of 0.0 indicates
		/// the point is at the near clip plane, and a value of 1.0 places it at the far clip
		/// plane.  Values outside the range 0 to 1 will not be visible (they would be clipped).
		virtual Vector Project(
			const Vector &vWorldPosition	///< [in] A point in world-space.
		) const;

		/// \brief Takes a point in screen space and returns its position in world space.
		///
		/// Screen space goes from (-1,-1) in the lower-left to (1,1) in the upper right.
		/// The center of the screen is (0,0).  The Z value in your specified screen-space
		/// point should be in the range 0.0 to 1.0.  This will control the depth of the point.
		/// If you specify 0.0, the returned world space point will be on the near clipping plane.  If
		/// you specifiy 1.0, the returned world space point will be on the far clipping plane.
		virtual Vector UnProject(
			const Vector &vScreenPosition	///< [in] The screen-space point to be transformed.
		) const;

		/// Returns the matrix used by the camera to transform world space positions to screen space.
		virtual mudbox::Matrix Matrix(
			bool bOffscreen = false		///< [in] Used internally.  Always set this to \b false.
		) const;

		/// \brief Returns the vertical field of view in radians.
		///
		/// You can calculate the horizontal field of view like this:
		/// \code
		///     hFOV = atan( aspectRatio * tan(vFOV) );
		/// \endcode
		virtual float FOV(void) const;

		/// Returns the near clip plane distance from the camera.
		virtual float Near(void) const;

		/// Returns the far clip plane distance from the camera.
		virtual float Far(void) const;

		/// \brief Returns the current aspect ratio of the viewport.
		///
		/// Aspect ratio is the width of the viewport divided by its height.
		virtual float AspectRatio(void) const;

		/// Sets the vertical field of view of the camera.
		virtual void SetFOV(
			float fFOV			///< [in] New vertical field of view in radians.
		);

		/// Sets the near clip plane for the camera.
		virtual void SetNear(
			float fNear		///< [in] Distance from the camera in world units.
		);

		/// Sets the far clip plane for the camera.
		virtual void SetFar(
			float fFar	///< [in] Distance from the camera in world units.
		);

		/// \brief Sets the aspect ratio for the camera.
		///
		///	The aspect ratio is the width of the viewport divided by the height.
		virtual void SetAspectRatio(
			float fAspectRatio	///< [in] the new aspect ratio for the camers (width/height)
		);

		/// \brief Returns the 2d scale factor.
		///
		/// This is a scale factor that is applied to the scene \a after it is projected
		/// from world space into screen space.  Normally this is 1.0.
		virtual float Scale2D(void) const;

		/// \brief Returns the 2d rotation amount.
		///
		/// This is a rotation that is applied to the scene \a after it is projected
		/// from world space into screen space.  Rotation is around the point 0,0 in screen space.
		/// Positive values rotate clockwise. Returns the angle in radians.
		virtual float Angle2D(void) const;

		/// \brief Returns the 2d offset.
		///
		/// This is an offset that is applied to the scene \a after it is projected
		/// from world space into screen space.
		virtual Vector Position2D(void) const;

		/// \brief Sets the 2d scale factor.
		///
		/// This is a scale factor that is applied to the scene \a after it is projected
		/// from world space into screen space.  Changing this makes the image smaller or
		/// larger without affecting perspective in any way.
		virtual void SetScale2D(
			float fScale2D		///< [in] Amount by which to scale the screen image
		);

		/// \brief Sets the 2d rotation amount.
		///
		/// This is a rotation that is applied to the scene \a after it is projected
		/// from world space into screen space.  Rotation is around the point 0,0 in screen space.
		/// Positive values rotate clockwise.
		virtual void SetAngle2D(
			float fAngle2D		///< [in] Angle, in radians, by which to rotate the screen image
		);

		/// \brief Sets the 2d offset.
		///
		/// This is an offset that is applied to the scene \a after it is projected
		/// from world space into screen space.  Applying a 2d offset lets you move the
		/// screen image around without changing perspective at all.
		virtual void SetPosition2D(
			const Vector &vPosition2D	///< [in] New screen space position for the center of the screen image
		);

		/// Make this camera identical to another one.
		virtual void CopyFrom(
			const Camera &cOther	///< [in] Camera to copy values from
		);


		/// Returns \b true if this camera is equal to the other one.
		virtual bool IsEqualTo(
			const Camera &cOther	///< [in] Camera to compare
		) const;

		/// \brief Returns the point displayed at the given screen location.
		///
		/// Given X and Y coordinates in screen space, this method sees if any geometry
		/// is displayed at those coordinates.  If not, \b false is returned.
		/// If there is geometry there, \b true is returned, and \c sResult is set
		/// to contain information about the found geometry.  SurfacePoint is defined
		/// in mesh.h
		virtual bool Pick(
			float fXPosition,		///< [in] The X coordinate in screen space
			float fYPosition,		///< [in] The Y coordinate in screen space
			SurfacePoint &sResult	///< [out] Information about the picked point
		);

		/// \brief Returns the ScreenSpacePicker used by this camera for picking.
		virtual ScreenSpacePicker *GetScreenSpacePicker() const;

		/// \brief Returns the world-space point around which the camera rolls when you navigate in the 3d view.
		virtual Vector Aim() const;

		/// \brief Sets the point around which the camera will rotate when you navigate in the 3d view.
		///
		/// This point is changed by various navigation actions, such as panning the camera or using the
		/// \b F or \b A keys to reposition the camera.
		virtual void SetAim(
			const Vector &v		///< [in] World-space point around which the camera will rotate
		);

		/// \brief Returns \b true if the camera is orthographic.
		virtual bool Orthographic() const;

		/// \brief Sets the camera to be orthographic or perspective.
		virtual void SetOrthographic(
			bool isOrtho		///< [in] Set \true to make the camera orthographic, \b false to make it perspective
		);

		/// \brief returns if the camera needs refresh
		virtual bool IsDirty(void) const;

		/// \brief sets if the camera needs refresh
		virtual void SetDirty(bool bDirty);

	private:

		VRHead *m_headPtr;
		vr::EVREye m_nEye;
		bool MBVRCamIsInitialised=false;

		Vector m_ForwardsDir;
		Vector m_UpDir;
		Vector m_RightDir;

		float m_Yaw, m_Pitch, m_Roll;

		Vector m_Position;

		mudbox::Matrix m_MVP;

		float m_FOV, m_Near, m_Far, m_AspectRatio;

		float m_Scale2D, m_Angle2D;

		Vector m_Position2D;

		bool m_Orthographic;

		bool m_dirty;
		
	};
}


