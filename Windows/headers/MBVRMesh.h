#pragma once
#include <mbvr_global.h>

namespace mudbox {

	class VRMesh //Base class implemetation for VR geometry
	{
	public:
		VRMesh() {}
		~VRMesh() {}

		virtual bool SendDataToGPU() = 0; //Initializes buffers and sends data to gpu buffer

		virtual bool UpdateDirtyVertices() =0; //Called when gpu buffer needs to be updated

		virtual bool GeomInitialize(Geometry * _Geometry) = 0; //Main initializer, 
															   //calls Init shader and Send data to gpu
		virtual void DrawGL(QMatrix4x4 & ModelMat4x4, 
							QMatrix4x4 & ProjMat4x4,
							QMatrix4x4 & WorldScaleMat4x4) = 0; //Main draw function

		virtual bool CheckIfGeomStillExists() = 0;

		virtual bool InitializeShader(QString _vertShaderName, 
									QString _fragShaderName) = 0; //Shader creator and initializer

		virtual Geometry & getGeometry() = 0; //Returns Mudbox geom VRMesh is asociated with

		virtual QVector3D getHeadPos(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & WorldScaleMat4x4) = 0;

		virtual void UpdatePosition(Matrix _newPos) = 0;

		static Texture * DefaultMat; //Texture used by Mesh

		static int DrawMode;

	};


	class MBVRMesh : public VRMesh, public Node
	{
	public:
		MBVRMesh();
		~MBVRMesh();

		bool SendDataToGPU();
		bool UpdateDirtyVertices();
		Geometry & getGeometry();
		void OnEvent(const EventGate &cEvent);
		bool GeomInitialize(Geometry * _Geometry);
		void DrawGL(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & ProjMat4x4, QMatrix4x4 & WorldScaleMat4x4);
		bool CheckIfGeomStillExists();
		bool InitializeShader(QString _vertShaderName, QString _fragShaderName);
		QVector3D getHeadPos(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & WorldScaleMat4x4);
		void UpdatePosition(Matrix _newPos);

	private:

		aevent eMeshChange;

		VertexDataProvider * m_DirtyVertexMachine;
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

		QGLBuffer m_NormalLineBuffer;

	};

	class RightHandMesh : public VRMesh
	{
	public:
		RightHandMesh();
		~RightHandMesh();

		bool SendDataToGPU();
		bool UpdateDirtyVertices();
		Geometry & getGeometry();
		bool GeomInitialize(Geometry * _Geometry);
		void DrawGL(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & ProjMat4x4, QMatrix4x4 & WorldScaleMat4x4);
		bool CheckIfGeomStillExists();
		bool InitializeShader(QString _vertShaderName, QString _fragShaderName);
		QVector3D getHeadPos(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & WorldScaleMat4x4);
		void UpdatePosition(Matrix _newPos);
		void UpdateRay(float _LineLength);

	private:
		bool InitializeRays();
		void DrawRays();

		QGLBuffer m_RayVertArrayObj;
		float * m_rayPos = NULL;

		QGLBuffer m_RayColourArrayObj;
		float * m_rayCol = NULL;

		//Geometry imported by mudbox
		Geometry * m_Geometry;

		Texture * m_Texture;

		//What shader program to use
		QGLShaderProgram * m_ShaderProgram;

		//Face Stuff
		unsigned int *m_FaceIndex;
		unsigned int m_faceType;
		unsigned int m_nFaceCount;
		unsigned int m_nFaceIndices;
		QGLBuffer m_FaceIndexObj;
		GLuint m_nFaceIndexId;

		//Vertice stuff
		uint m_nNumVerts;
		GLshort m_nTotalVerts;

		float *m_vPos = NULL;
		QGLBuffer m_VertexArrayObj;
		GLuint m_nVertexArrayId;

		//Normal Stuff
		float *m_vNormal = NULL;
		QGLBuffer m_VertexNormalObj;
		GLuint m_nVertexNormalId;

		//Colour stuff
		float *m_vColour = NULL;
		QGLBuffer m_VertexColourObj;
		GLuint m_nVertexColourId;

		//TextureCoord stuff
		unsigned int m_nTC;
		float *m_vTC = NULL;
		QGLBuffer m_TCBuffer;
		GLuint m_nTCBufferId;

		Matrix WorldTransform;

		float LineLength;
	};

}



