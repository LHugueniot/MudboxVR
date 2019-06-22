#include "mbvr_global.h"

namespace mudbox {

	class VRPicker : public Picker
	{
	public:
		virtual void SetMesh(
			Mesh *m_pMesh		///< [in] Specify the mesh from which SurfacePoints can be picked
		);

		virtual void SetTolerance(
			float fTolerance	///< [in] The allowed tolerance when the ray misses the surface near an open edge.
		);

		virtual bool Pick(
			const Vector &vLocalStart,	///< [in] Start point of the picking vector, in object space
			const Vector &vLocalEnd,	///< [in] End point of the picking vector, in object space
			bool bBothSides,			///< [in] If \b true, allow the mesh to be picked from either side (front or back of faces)
			SurfacePoint &cResult,		///< [out] The calculated SurfacePoint
			float fTarget = 0.5f		///< [in] This value, ranging from 0 to 1, represents a position on the line between vLocalStart and vLocalEnd.  If there are multiple selection points, the one closes to this value is returned. 
		);

	private:
		Mesh *m_Mesh = NULL;
		float m_Tolerance;

	};

	Vector FaceCenter(Mesh *m_Mesh,unsigned int i);

}
