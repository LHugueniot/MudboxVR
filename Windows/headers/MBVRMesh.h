#pragma once
#include "MBVRShader.h"

namespace mudbox {

	class VRMesh
	{
	public:
		VRMesh() {}
		~VRMesh() {}

		virtual bool SendDataToGPU() = 0;

		virtual bool GeomInitialize(Geometry * _Geometry) = 0;

		virtual void DrawGL(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & ProjMat4x4, QMatrix4x4 & WorldScaleMat4x4) = 0;

		virtual bool CheckIfGeomStillExists() = 0;

		virtual bool InitializeShader(QString _vertShaderName, QString _fragShaderName) = 0;

		virtual Geometry & getGeometry();

		virtual QVector3D getHeadPos(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & WorldScaleMat4x4) = 0;

	};

	class MBVRMesh : public VRMesh
	{
	public:
		MBVRMesh();
		~MBVRMesh();

		bool SendDataToGPU();
		//bool SendIndexedDataToGPU();
		Geometry & getGeometry();
		bool GeomInitialize(Geometry * _Geometry);
		void DrawGL(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & ProjMat4x4, QMatrix4x4 & WorldScaleMat4x4);
		bool CheckIfGeomStillExists();
		bool InitializeShader(QString _vertShaderName, QString _fragShaderName);
		QVector3D getHeadPos(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & WorldScaleMat4x4);

	protected:
		Geometry * m_Geometry;
		QGLShaderProgram * m_ShaderProgram;

		uint m_nNumVerts;
		GLshort m_nTotalVerts;

		float *m_vPos = NULL;
		QGLBuffer m_VertexArrayObj;
		GLuint m_nVertexArrayId;

		unsigned int *m_FaceIndex;
		unsigned int m_faceType;
		unsigned int m_GenericFaceCount;
		unsigned int m_NonGenericfaceCount;
		unsigned int m_nFaceIndices;
		QGLBuffer m_FaceIndexObj;
		GLuint m_nFaceIndexId;

		float *m_vNormal = NULL;
		QGLBuffer m_VertexNormalObj;
		GLuint m_nVertexNormalId;

		float *m_vColour = NULL;
		QGLBuffer m_VertexColourObj;
		GLuint m_nVertexColourId;

		unsigned int m_nTC;
		float *m_vTC = NULL;
		QGLBuffer m_TCBuffer;
		GLuint m_nTCBufferId;
		AxisAlignedBoundingBox * AABB;

	};

}



