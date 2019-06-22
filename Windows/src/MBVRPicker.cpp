#include "MBVRPicker.h"

namespace mudbox 
{

	//-------------------------------------------------------------
	//--					VRPicker Functions					 --
	//-------------------------------------------------------------

	void VRPicker::SetMesh( Mesh *m_pMesh )
	{
		m_Mesh = m_pMesh;
	}

	void VRPicker::SetTolerance( float fTolerance )
	{
		m_Tolerance = fTolerance;
	}

	bool VRPicker::Pick(const Vector &vLocalStart, const Vector &vLocalEnd,
						bool bBothSides, SurfacePoint &cResult, float fTarget )
	{

		std::vector<unsigned int> IntersectingFaces;

		unsigned int ClosestFaceIndex =0;

		for (unsigned int i = 0; i < m_Mesh->FaceCount(); i++)
		{
			SurfacePoint TempSurfacePoint;

			if (cResult.Fill(m_Mesh, i, vLocalStart, vLocalEnd, false, true))
			{
				//IntersectingFaces.push_back(i);
				//DEBUG(i);
				return true;
			}
		}
		if (IntersectingFaces.size() == 0)
			return false;
		
		for (auto a : IntersectingFaces)
		{
			DEBUG(a);
		}

		for (unsigned int j = 0; j < IntersectingFaces.size(); j++)
		{
			float ClosestFaceDistance = (FaceCenter(m_Mesh, ClosestFaceIndex) - vLocalStart).Length();

			float CurrentFaceDistance = (FaceCenter(m_Mesh, j) - vLocalStart).Length();

			if (ClosestFaceDistance > CurrentFaceDistance)
			{
				ClosestFaceIndex = IntersectingFaces[j];
			}
		}

		//cResult.Fill(m_Mesh, ClosestFaceIndex, vLocalStart, vLocalEnd, false, true);
		//m_Mesh->QuadAdjacency(cResult.FaceIndex, 0);
		
		return true;

	}

	Vector FaceCenter(Mesh *m_Mesh, unsigned int i)
	{
		Vector Center;
		if (m_Mesh->SideCount() == 3)
		{
			Center = (m_Mesh->TriangleVertexPosition(i, 0) +
					  m_Mesh->TriangleVertexPosition(i, 1) +
					  m_Mesh->TriangleVertexPosition(i, 2)) / 3;
		}
		if (m_Mesh->SideCount() == 4)
		{
			Center = (m_Mesh->QuadVertexPosition(i, 0) +
					  m_Mesh->QuadVertexPosition(i, 1) +
					  m_Mesh->QuadVertexPosition(i, 2) +
					  m_Mesh->QuadVertexPosition(i, 3)) / 3;
		}
		return Center;
	}

}
