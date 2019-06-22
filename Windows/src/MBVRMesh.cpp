#include "MBVRMesh.h"
#pragma once

namespace mudbox 
{

	

	//-----------------------------------------------------------------------------
	// MBVRMesh Implementation
	//-----------------------------------------------------------------------------

	MBVRMesh::MBVRMesh() : eMeshChange(this),
		m_VertexArrayObj(QGLBuffer::VertexBuffer),
		m_VertexNormalObj(QGLBuffer::VertexBuffer),
		m_FaceIndexObj(QGLBuffer::IndexBuffer),
		m_VertexColourObj(QGLBuffer::VertexBuffer),
		m_TCBuffer(QGLBuffer::VertexBuffer)
	{
	}

	MBVRMesh::~MBVRMesh()
	{
		m_ShaderProgram->removeAllShaders();

		delete m_ShaderProgram;
		m_ShaderProgram = NULL;

		m_VertexArrayObj.destroy();
		m_VertexNormalObj.destroy();
		m_VertexColourObj.destroy();
		m_TCBuffer.destroy();

		delete[](m_vPos);		BREAKPOINT();
		delete[](m_FaceIndex);	BREAKPOINT();
		delete[](m_vTC);		BREAKPOINT();
		delete[](m_vColour);	BREAKPOINT();
		delete[](m_vNormal);	BREAKPOINT();

		(m_vPos) = NULL;
		(m_FaceIndex) = NULL;
		(m_vTC) = NULL;
		(m_vColour) = NULL;
		(m_vNormal) = NULL;
	}

	bool MBVRMesh::SendDataToGPU()
	{
		if (m_Geometry == NULL) return false;

		if (m_vPos != NULL)
			delete[](m_vPos);

		if (m_vPos != NULL)
			delete[](m_FaceIndex);

		if (m_vPos != NULL)
			delete[](m_vTC);

		if (m_vPos != NULL)
			delete[](m_vColour);



		//Face Indices=======================================================================

		unsigned int nFaceCount = m_Geometry->ActiveLevel()->FaceCount();

		m_nFaceIndices = nFaceCount * m_faceType;

		m_FaceIndex = new (unsigned int[m_nFaceIndices]);

		if (m_faceType == 4)
		{

			for (unsigned int i = 0; i < nFaceCount; i++)
			{
				m_FaceIndex[i * m_faceType] = m_Geometry->ActiveLevel()->QuadIndex(i, 0);
				m_FaceIndex[i * m_faceType + 1] = m_Geometry->ActiveLevel()->QuadIndex(i, 1);
				m_FaceIndex[i * m_faceType + 2] = m_Geometry->ActiveLevel()->QuadIndex(i, 2);
				m_FaceIndex[i * m_faceType + 3] = m_Geometry->ActiveLevel()->QuadIndex(i, 3);
			}
		}
		if (m_faceType == 3)
		{

			for (unsigned int i = 0; i < nFaceCount; i++)
			{
				m_FaceIndex[i * m_faceType] = m_Geometry->ActiveLevel()->TriangleIndex(i, 0);
				m_FaceIndex[i * m_faceType + 1] = m_Geometry->ActiveLevel()->TriangleIndex(i, 1);
				m_FaceIndex[i * m_faceType + 2] = m_Geometry->ActiveLevel()->TriangleIndex(i, 2);
			}
		}

		//Vertex Coordinates=======================================================================

		const Vertex * verts = m_Geometry->ActiveLevel()->VertexArray();

		m_vPos = new float[m_nFaceIndices * 3];

		for (unsigned int i = 0; i < m_nFaceIndices; i++)
		{
			unsigned int ii = m_FaceIndex[i];

			m_vPos[i * 3] = verts[ii].m_vPos.x;
			m_vPos[i * 3 + 1] = verts[ii].m_vPos.y;
			m_vPos[i * 3 + 2] = verts[ii].m_vPos.z;
		}
		m_VertexArrayObj.create();
		// Copy data to video memory
		// Vertex data
		if (!m_VertexArrayObj.bind())
			return false;
		m_VertexArrayObj.setUsagePattern(QGLBuffer::DynamicDraw);
		m_VertexArrayObj.allocate(m_vPos, sizeof(GLfloat) * 3 * m_nFaceIndices);
		DEBUG("Total Num Vertices:\n");
		DEBUG(m_VertexArrayObj.size() / 3);
		m_VertexArrayObj.release();


		//Vertex Normals=======================================================================

		m_Geometry->ActiveLevel()->RecalculateNormals();

		unsigned int numNormals = m_Geometry->ActiveLevel()->VertexNormalCount();
		m_vNormal = new float[m_nFaceIndices * 3];

		if (m_faceType == 4)
		{
			for (unsigned int i = 0; i < nFaceCount; i++)
			{
				unsigned int ii = m_FaceIndex[i];

				m_vNormal[i *m_faceType * 3 + 0] = m_Geometry->ActiveLevel()->QuadVertexNormal(i, 0).x;
				m_vNormal[i *m_faceType * 3 + 1] = m_Geometry->ActiveLevel()->QuadVertexNormal(i, 0).y;
				m_vNormal[i *m_faceType * 3 + 2] = m_Geometry->ActiveLevel()->QuadVertexNormal(i, 0).z;

				m_vNormal[i *m_faceType * 3 + 3] = m_Geometry->ActiveLevel()->QuadVertexNormal(i, 1).x;
				m_vNormal[i *m_faceType * 3 + 4] = m_Geometry->ActiveLevel()->QuadVertexNormal(i, 1).y;
				m_vNormal[i *m_faceType * 3 + 5] = m_Geometry->ActiveLevel()->QuadVertexNormal(i, 1).z;

				m_vNormal[i *m_faceType * 3 + 6] = m_Geometry->ActiveLevel()->QuadVertexNormal(i, 2).x;
				m_vNormal[i *m_faceType * 3 + 7] = m_Geometry->ActiveLevel()->QuadVertexNormal(i, 2).y;
				m_vNormal[i *m_faceType * 3 + 8] = m_Geometry->ActiveLevel()->QuadVertexNormal(i, 2).z;

				m_vNormal[i *m_faceType * 3 + 9] = m_Geometry->ActiveLevel()->QuadVertexNormal(i, 3).x;
				m_vNormal[i *m_faceType * 3 + 10] = m_Geometry->ActiveLevel()->QuadVertexNormal(i, 3).y;
				m_vNormal[i *m_faceType * 3 + 11] = m_Geometry->ActiveLevel()->QuadVertexNormal(i, 3).z;

			}
		}
		if (m_faceType == 3)
		{
			for (unsigned int i = 0; i < nFaceCount; i++)
			{
				unsigned int ii = m_FaceIndex[i];

				m_vNormal[i *m_faceType * 3 + 0] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 0).x;
				m_vNormal[i *m_faceType * 3 + 1] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 0).y;
				m_vNormal[i *m_faceType * 3 + 2] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 0).z;

				m_vNormal[i *m_faceType * 3 + 3] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 1).x;
				m_vNormal[i *m_faceType * 3 + 4] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 1).y;
				m_vNormal[i *m_faceType * 3 + 5] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 1).z;

				m_vNormal[i *m_faceType * 3 + 6] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 2).x;
				m_vNormal[i *m_faceType * 3 + 7] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 2).y;
				m_vNormal[i *m_faceType * 3 + 8] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 2).z;


			}
		}

		m_VertexNormalObj.create();

		// Copy data to video memory
		// Normal data
		if (!m_VertexNormalObj.bind())
			return false;
		m_VertexNormalObj.setUsagePattern(QGLBuffer::DynamicDraw);
		m_VertexNormalObj.allocate(m_vNormal, sizeof(GLfloat) * 3 * m_nFaceIndices);
		DEBUG("Total Num Normal:\n");
		DEBUG(m_VertexNormalObj.size() / 3);
		m_VertexNormalObj.release();

		//Vertex Colours=======================================================================

		m_vColour = new float[m_nFaceIndices * 3];
		for (size_t i = 0; i < m_nFaceIndices * 3; i++)
		{
			m_vColour[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);;
		}
		m_VertexColourObj.create();
		// Copy data to video memory
		// Vertex data
		if (!m_VertexColourObj.bind())
			return false;
		m_VertexColourObj.setUsagePattern(QGLBuffer::DynamicDraw);
		m_VertexColourObj.allocate(m_vColour, sizeof(GLfloat) * m_nFaceIndices * 3);
		//DEBUG(m_VertexColourObj.size());
		m_VertexColourObj.release();

		//Texture Coordinates=======================================================================

		//TC * TextureCoords = &(m_Geometry->ActiveLevel()->m_pTCs[0]);
		unsigned int TCCount = m_Geometry->ActiveLevel()->TCCount();
		DEBUG("TCCount:");
		DEBUG(TCCount);

		m_nTC = TCCount;


		m_vTC = new float[m_nFaceIndices * 2];

		if (m_faceType == 4)
		{
			for (unsigned int i = 0; i < nFaceCount; i++)
			{

				m_vTC[i * m_faceType * 2] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 0).m_fU;
				m_vTC[i * m_faceType * 2 + 1] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 0).m_fV;

				m_vTC[i * m_faceType * 2 + 2] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 1).m_fU;
				m_vTC[i * m_faceType * 2 + 3] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 1).m_fV;

				m_vTC[i * m_faceType * 2 + 4] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 2).m_fU;
				m_vTC[i * m_faceType * 2 + 5] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 2).m_fV;

				m_vTC[i * m_faceType * 2 + 6] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 3).m_fU;
				m_vTC[i * m_faceType * 2 + 7] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 3).m_fV;
			}
		}
		if (m_faceType == 3)
		{
			for (unsigned int i = 0; i < nFaceCount; i++)
			{

				m_vTC[i * m_faceType * 2] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 0).m_fU;
				m_vTC[i * m_faceType * 2 + 1] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 0).m_fV;

				m_vTC[i * m_faceType * 2 + 2] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 1).m_fU;
				m_vTC[i * m_faceType * 2 + 3] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 1).m_fV;

				m_vTC[i * m_faceType * 2 + 4] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 2).m_fU;
				m_vTC[i * m_faceType * 2 + 5] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 2).m_fV;

				m_vTC[i * m_faceType * 2 + 6] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 3).m_fU;
				m_vTC[i * m_faceType * 2 + 7] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 3).m_fV;
			}
		}

		BREAKPOINT();
		m_TCBuffer.create();
		// Copy data to video memory
		// Texture Coord data
		if (!m_TCBuffer.bind())
			return false;
		m_TCBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
		m_TCBuffer.allocate(m_vTC, sizeof(GLfloat) * m_nFaceIndices * 2);
		DEBUG("Total Num Quad TC:\n");
		DEBUG(m_TCBuffer.size());
		m_TCBuffer.release();

		m_nVertexArrayId = m_VertexArrayObj.bufferId();
		m_nVertexNormalId = m_VertexNormalObj.bufferId();
		m_nVertexColourId = m_VertexColourObj.bufferId();
		m_nTCBufferId = m_TCBuffer.bufferId();

		BREAKPOINT();

		if (!InitializeShader("MBVR_vertex.glsl", "MBVR_fragment.glsl"))
			DEBUG("Compiler Init unsuccessfull");

		m_DirtyVertexMachine = new VertexDataProvider();
		BREAKPOINT();

		m_Geometry->ActiveLevel()->Renderer()->AddChild(m_DirtyVertexMachine);
		m_DirtyVertexMachine->Initialize();
		//m_Geometry->ActiveLevel()->Renderer()->Next()->LogAttributes();
		//DEBUG();
		//DEBUG(m_Geometry->ActiveLevel()->Renderer()->FirstChild()->Name().contains("VertexDataProvider"));
		//HUDPRINT( m_Geometry->ActiveLevel()->Renderer()->FirstChild()->Name().contains("VertexDataProvider"));

		BREAKPOINT();

		return true;

	}

	bool MBVRMesh::UpdateDirtyVertices()
	{
		//if (_SurfacePoint.Mesh() == m_Geometry->ActiveLevel())
		//{
		//	if (m_faceType == 4)
		//	{
		//		unsigned int faceIndices[4];
		//		faceIndices[0] = m_Geometry->ActiveLevel()->QuadIndex(_SurfacePoint.FaceIndex(), 0);
		//		faceIndices[1] = m_Geometry->ActiveLevel()->QuadIndex(_SurfacePoint.FaceIndex(), 1);
		//		faceIndices[2] = m_Geometry->ActiveLevel()->QuadIndex(_SurfacePoint.FaceIndex(), 2);
		//		faceIndices[3] = m_Geometry->ActiveLevel()->QuadIndex(_SurfacePoint.FaceIndex(), 3);
		//
		//		float * newVert = new float[m_faceType * 3];
		//
		//		for (unsigned int i = 0; i< m_faceType; i++)
		//		{
		//			unsigned int fOffset = i * 3;
		//
		//			Vector VertexToUpdate = m_Geometry->ActiveLevel()->VertexPosition(faceIndices[i]);
		//
		//			newVert[fOffset] =	VertexToUpdate.x;
		//			newVert[fOffset+1] = VertexToUpdate.y;
		//			newVert[fOffset+2] = VertexToUpdate.z;
		//		}
		//
		//		m_VertexArrayObj.write(faceIndices[0], newVert,4);
		//
		//		delete[] newVert;
		//	}
		//	if (m_faceType == 3)
		//	{
		//		unsigned int faceIndices[4];
		//		faceIndices[0] = m_Geometry->ActiveLevel()->TriangleIndex(_SurfacePoint.FaceIndex(), 0);
		//		faceIndices[1] = m_Geometry->ActiveLevel()->TriangleIndex(_SurfacePoint.FaceIndex(), 1);
		//		faceIndices[2] = m_Geometry->ActiveLevel()->TriangleIndex(_SurfacePoint.FaceIndex(), 2);
		//
		//		float * newVert = new float[m_faceType * 3];
		//
		//		for (unsigned int i = 0; i < m_faceType; i++)
		//		{
		//			unsigned int fOffset = i * 3;
		//
		//			Vector VertexToUpdate = m_Geometry->ActiveLevel()->VertexPosition(faceIndices[i]);
		//
		//			newVert[fOffset] = VertexToUpdate.x;
		//			newVert[fOffset + 1] = VertexToUpdate.y;
		//			newVert[fOffset + 2] = VertexToUpdate.z;
		//		}
		//
		//		m_VertexArrayObj.write(faceIndices[0], newVert, 3);
		//
		//		delete[] newVert;
		//	}
		//	return true;
		//}

		delete[] m_vPos;
		const Vertex * verts = m_Geometry->ActiveLevel()->VertexArray();

		m_vPos = new float[m_nFaceIndices * 3];

		for (unsigned int i = 0; i < m_nFaceIndices; i++)
		{
			unsigned int ii = m_FaceIndex[i];

			m_vPos[i * 3] = verts[ii].m_vPos.x;
			m_vPos[i * 3 + 1] = verts[ii].m_vPos.y;
			m_vPos[i * 3 + 2] = verts[ii].m_vPos.z;
		}

		// Copy data to video memory
		// Vertex data
		if (!m_VertexArrayObj.bind())
			return false;

		m_VertexArrayObj.write(0, m_vPos, sizeof(GLfloat) * 3 * m_nFaceIndices);

		m_VertexArrayObj.release();
		return true;
	}

	void MBVRMesh::UpdatePosition(Matrix _newPos)
	{
		m_Geometry->Transform(_newPos);
	}

	Geometry & MBVRMesh::getGeometry()
	{
		return *m_Geometry;
	}

	QVector3D MBVRMesh::getHeadPos(QMatrix4x4 & ModelMat4x4 ,QMatrix4x4 & WorldScaleMat4x4)
	{
		QMatrix4x4 ScaledModelMat = ModelMat4x4;// *MBtoQMatrix(m_Geometry->Transformation()->LocalToWorldMatrix());//;* WorldScaleMat4x4;

		return QVector3D(ScaledModelMat(0,3), ScaledModelMat(1,3), ScaledModelMat(2,3));
	}

	void MBVRMesh::OnEvent(const EventGate &cEvent)
	{
	}

	bool MBVRMesh::GeomInitialize(Geometry * _Geometry)
	{
		if (_Geometry == NULL)
			return false;
		
		m_Geometry = _Geometry;
		m_faceType = m_Geometry->ActiveLevel()->SideCount();

		eMeshChange.Connect(m_Geometry->ActiveLevel()->Modified);

		if(!SendDataToGPU())
			return false;
		return true;
	}

	void MBVRMesh::DrawGL(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & ProjMat4x4, QMatrix4x4 & WorldScaleMat4x4)
	{
		//Bind shader Progam to use it
		m_ShaderProgram->bind();

		//Activate and bind Texture?

		unsigned int totalNumTextures = m_Geometry->ActiveLevel()->Material()->TextureCount();
		Texture * CurTexture;

		if (totalNumTextures > 0)
		{
			CurTexture = m_Geometry->ActiveLevel()->Material()->Texture(0)->Tile(0);
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, CurTexture->OpenGLName());

			m_ShaderProgram->setUniformValue("PaintTex", 0);
			BREAKPOINT();
		}
		else 
		{
			CurTexture = DefaultMat;
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, CurTexture->OpenGLName());

			m_ShaderProgram->setUniformValue("PaintTex", 0);
		}
		//glBindTexture(GL_TEXTURE_2D, CurrentFrameBuffer);
		//
		//m_ShaderProgram->setUniformValue("FrameBuffer", 0);

		QMatrix4x4 AdjustedModelMat = ModelMat4x4 * MBtoQMatrix(m_Geometry->Transformation()->LocalToWorldMatrix());

		m_ShaderProgram->setUniformValue("ModelMat", AdjustedModelMat);

		QMatrix4x4 InvAdjustedModelMat = AdjustedModelMat;// .inverted().transposed();

		m_ShaderProgram->setUniformValue("InvModelMat", InvAdjustedModelMat);

		//Send MVP matrix to shader
		QMatrix4x4 MeshMVP = ProjMat4x4 * AdjustedModelMat; 

		m_ShaderProgram->setUniformValue("MVP", MeshMVP);
		BREAKPOINT();
		BREAKPOINT();
		m_ShaderProgram->setUniformValue("headPos", getHeadPos(ModelMat4x4, WorldScaleMat4x4));

		m_ShaderProgram->setUniformValue("DrawMode", DrawMode);
		//Bind Vertex, texcoord and index buffers
		m_VertexArrayObj.bind();
		m_ShaderProgram->enableAttributeArray("in_Position");
		m_ShaderProgram->setAttributeBuffer("in_Position", GL_FLOAT, 0, 3, 0);

		m_TCBuffer.bind();
		m_ShaderProgram->enableAttributeArray("in_TexCoords");
		m_ShaderProgram->setAttributeBuffer("in_TexCoords", GL_FLOAT, 0, 2, 0);


		m_VertexNormalObj.bind();
		m_ShaderProgram->enableAttributeArray("in_Normals");
		m_ShaderProgram->setAttributeBuffer("in_Normals", GL_FLOAT, 0, 3, 0);


		//Set attribute buffers to texture locations 
		if (m_faceType == 4)
			glDrawArrays(GL_QUADS, 0, m_nFaceIndices);
		else
			glDrawArrays(GL_TRIANGLES, 0, m_nFaceIndices);

		m_VertexArrayObj.release();
		m_TCBuffer.release();
		m_VertexNormalObj.release();

		m_ShaderProgram->disableAttributeArray("in_Position");
		m_ShaderProgram->disableAttributeArray("in_TexCoords");
		m_ShaderProgram->disableAttributeArray("in_Normals");
		
		m_ShaderProgram->release();

	}

	bool MBVRMesh::CheckIfGeomStillExists()
	{
		unsigned int geomCount = Kernel()->Scene()->GeometryCount();
		bool Found=false;
		for (unsigned int i = 0; i < geomCount; i++)
		{
			if ((Kernel()->Scene()->Geometry(i)) == m_Geometry)
			{
				Found = true;
			}
		}
		return Found;
	}

	bool MBVRMesh::InitializeShader(QString _vertShaderName, QString _fragShaderName)
	{
		m_ShaderProgram = new QGLShaderProgram();

		BREAKPOINT();

		QDir pluginDir(Kernel()->PluginDirectory("MudboxVR"));
		pluginDir.cd("../shaders");
		QFileInfo VertexShaderPath(pluginDir, _vertShaderName);
		QFileInfo FragmentShaderPath(pluginDir, _fragShaderName);
		
		//QFileInfo VertexShaderPath(pluginDir, QString(  "MBVR_vertex.glsl"  ));
		//QFileInfo FragmentShaderPath(pluginDir, QString("MBVR_fragment.glsl"));

		if(!m_ShaderProgram->addShaderFromSourceFile(QGLShader::Vertex, VertexShaderPath.filePath()) )
		{
			GLDEBUG();
			DEBUG(m_ShaderProgram->log());
			Kernel()->Interface()->HUDMessageShow(m_ShaderProgram->log(), Interface::HUDMessageType::HUDmsgPin);
			return false;
		}

		if (!m_ShaderProgram->addShaderFromSourceFile(QGLShader::Fragment, FragmentShaderPath.filePath()) )
		{
			GLDEBUG();
			DEBUG(m_ShaderProgram->log());
			Kernel()->Interface()->HUDMessageShow(m_ShaderProgram->log(), Interface::HUDMessageType::HUDmsgPin);
			return false;
		}

		m_ShaderProgram->bindAttributeLocation("in_Position", m_nVertexArrayId);
		BREAKPOINT();

		if (!m_ShaderProgram->link())
		{
			DEBUG(m_ShaderProgram->log());
			return false;
		}
		BREAKPOINT();
		
		return true;
		//m_ShaderProgram->bindAttributeLocation("in_Normal", m_nVertexNormalId);
		//m_ShaderProgram->bindAttributeLocation("in_TexCoord0", m_nVertexArrayId);
	}

	//=======================================================================================================
	//=======================================================================================================
	// RightHandMesh Implementation
	//=======================================================================================================
	//=======================================================================================================

	RightHandMesh::RightHandMesh() :
		m_VertexArrayObj(QGLBuffer::VertexBuffer),
		m_VertexNormalObj(QGLBuffer::VertexBuffer),
		m_FaceIndexObj(QGLBuffer::IndexBuffer),
		m_VertexColourObj(QGLBuffer::VertexBuffer),
		m_TCBuffer(QGLBuffer::VertexBuffer),
		m_RayColourArrayObj(QGLBuffer::VertexBuffer),
		m_RayVertArrayObj(QGLBuffer::VertexBuffer)
	{
	}

	RightHandMesh::~RightHandMesh() 
	{
		m_ShaderProgram->removeAllShaders();
		delete m_ShaderProgram;
		m_ShaderProgram = NULL;

		delete m_Geometry;
		m_Geometry=NULL;
		delete m_Texture;
		m_Texture = NULL;

		m_VertexArrayObj.destroy();
		m_VertexNormalObj.destroy();
		m_VertexColourObj.destroy();
		m_TCBuffer.destroy();

		m_RayVertArrayObj.destroy();
		m_RayColourArrayObj.destroy();

		delete[](m_vPos);		BREAKPOINT();
		delete[](m_FaceIndex);	BREAKPOINT();
		delete[](m_vTC);		BREAKPOINT();
		delete[](m_vColour);	BREAKPOINT();
		delete[](m_vNormal);	BREAKPOINT();

		delete[](m_rayPos);		BREAKPOINT();
		delete[](m_rayCol);	BREAKPOINT();

		(m_vPos) = NULL;
		(m_FaceIndex) = NULL;
		(m_vTC) = NULL;
		(m_vColour) = NULL;
		(m_vNormal) = NULL;

		(m_rayPos) = NULL;
		(m_rayCol) = NULL;
	}

	bool RightHandMesh::UpdateDirtyVertices()
	{
		return true;
	}

	bool RightHandMesh::SendDataToGPU()
	{

		if (m_Geometry == NULL) return false;

		if (m_vPos != NULL)
			delete[](m_vPos);

		if (m_vPos != NULL)
			delete[](m_FaceIndex);

		if (m_vPos != NULL)
			delete[](m_vTC);

		if (m_vPos != NULL)
			delete[](m_vColour);



		//Face Indices=======================================================================

		unsigned int nFaceCount = m_Geometry->ActiveLevel()->FaceCount();

		//m_NonGenericfaceCount = nFaceCount;
		m_nFaceIndices = nFaceCount * m_faceType;

		m_FaceIndex = new (unsigned int[m_nFaceIndices]);

		if (m_faceType == 4)
		{

			for (unsigned int i = 0; i < nFaceCount; i++)
			{
				m_FaceIndex[i * m_faceType] = m_Geometry->ActiveLevel()->QuadIndex(i, 0);
				m_FaceIndex[i * m_faceType + 1] = m_Geometry->ActiveLevel()->QuadIndex(i, 1);
				m_FaceIndex[i * m_faceType + 2] = m_Geometry->ActiveLevel()->QuadIndex(i, 2);
				m_FaceIndex[i * m_faceType + 3] = m_Geometry->ActiveLevel()->QuadIndex(i, 3);
			}
		}
		if (m_faceType == 3)
		{

			for (unsigned int i = 0; i < nFaceCount; i++)
			{
				m_FaceIndex[i * m_faceType] = m_Geometry->ActiveLevel()->TriangleIndex(i, 0);
				m_FaceIndex[i * m_faceType + 1] = m_Geometry->ActiveLevel()->TriangleIndex(i, 1);
				m_FaceIndex[i * m_faceType + 2] = m_Geometry->ActiveLevel()->TriangleIndex(i, 2);
			}
		}

		//Vertex Coordinates=======================================================================

		const Vertex * verts = m_Geometry->ActiveLevel()->VertexArray();

		m_vPos = new float[m_nFaceIndices * 3];

		for (unsigned int i = 0; i < m_nFaceIndices; i++)
		{
			unsigned int ii = m_FaceIndex[i];

			m_vPos[i * 3] = verts[ii].m_vPos.x;
			m_vPos[i * 3 + 1] = verts[ii].m_vPos.y;
			m_vPos[i * 3 + 2] = verts[ii].m_vPos.z;
		}
		m_VertexArrayObj.create();
		// Copy data to video memory
		// Vertex data
		if (!m_VertexArrayObj.bind())
			return false;
		m_VertexArrayObj.setUsagePattern(QGLBuffer::DynamicDraw);
		m_VertexArrayObj.allocate(m_vPos, sizeof(GLfloat) * 3 * m_nFaceIndices);
		DEBUG("Total Num Vertices:\n");
		DEBUG(m_VertexArrayObj.size() / 3);
		m_VertexArrayObj.release();


		//Vertex Normals=======================================================================

		unsigned int numNormals = m_Geometry->ActiveLevel()->VertexNormalCount();
		m_vNormal = new float[m_nFaceIndices * 3];

		if (m_faceType == 4)
		{
			for (unsigned int i = 0; i < nFaceCount; i++)
			{
				unsigned int ii = m_FaceIndex[i];

				m_vNormal[i *m_faceType * 3 + 0] = m_Geometry->ActiveLevel()->QuadVertexCreaseNormal(i, 0).x;
				m_vNormal[i *m_faceType * 3 + 1] = m_Geometry->ActiveLevel()->QuadVertexCreaseNormal(i, 0).y;
				m_vNormal[i *m_faceType * 3 + 2] = m_Geometry->ActiveLevel()->QuadVertexCreaseNormal(i, 0).z;

				m_vNormal[i *m_faceType * 3 + 3] = m_Geometry->ActiveLevel()->QuadVertexCreaseNormal(i, 1).x;
				m_vNormal[i *m_faceType * 3 + 4] = m_Geometry->ActiveLevel()->QuadVertexCreaseNormal(i, 1).y;
				m_vNormal[i *m_faceType * 3 + 5] = m_Geometry->ActiveLevel()->QuadVertexCreaseNormal(i, 1).z;

				m_vNormal[i *m_faceType * 3 + 6] = m_Geometry->ActiveLevel()->QuadVertexCreaseNormal(i, 2).x;
				m_vNormal[i *m_faceType * 3 + 7] = m_Geometry->ActiveLevel()->QuadVertexCreaseNormal(i, 2).y;
				m_vNormal[i *m_faceType * 3 + 8] = m_Geometry->ActiveLevel()->QuadVertexCreaseNormal(i, 2).z;

				m_vNormal[i *m_faceType * 3 + 9] = m_Geometry->ActiveLevel()->QuadVertexCreaseNormal(i, 3).x;
				m_vNormal[i *m_faceType * 3 + 10] = m_Geometry->ActiveLevel()->QuadVertexCreaseNormal(i, 3).y;
				m_vNormal[i *m_faceType * 3 + 11] = m_Geometry->ActiveLevel()->QuadVertexCreaseNormal(i, 3).z;

			}
		}
		if (m_faceType == 3)
		{
			for (unsigned int i = 0; i < nFaceCount; i++)
			{
				unsigned int ii = m_FaceIndex[i];

				m_vNormal[i *m_faceType * 3 + 0] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 0).x;
				m_vNormal[i *m_faceType * 3 + 1] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 0).y;
				m_vNormal[i *m_faceType * 3 + 2] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 0).z;

				m_vNormal[i *m_faceType * 3 + 3] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 1).x;
				m_vNormal[i *m_faceType * 3 + 4] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 1).y;
				m_vNormal[i *m_faceType * 3 + 5] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 1).z;

				m_vNormal[i *m_faceType * 3 + 6] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 2).x;
				m_vNormal[i *m_faceType * 3 + 7] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 2).y;
				m_vNormal[i *m_faceType * 3 + 8] = m_Geometry->ActiveLevel()->TriangleVertexCreaseNormal(i, 2).z;


			}
		}
		m_VertexNormalObj.create();

		// Copy data to video memory
		// Normal data
		if (!m_VertexNormalObj.bind())
			return false;
		m_VertexNormalObj.setUsagePattern(QGLBuffer::DynamicDraw);
		m_VertexNormalObj.allocate(m_vNormal, sizeof(GLfloat) * 3 * m_nFaceIndices);
		DEBUG("Total Num Normal:\n");
		DEBUG(m_VertexNormalObj.size() / 3);
		m_VertexNormalObj.release();

		//Vertex Colours=======================================================================

		m_vColour = new float[m_nFaceIndices * 3];
		for (size_t i = 0; i < m_nFaceIndices * 3; i++)
		{
			m_vColour[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);;
		}
		m_VertexColourObj.create();
		// Copy data to video memory
		// Vertex data
		if (!m_VertexColourObj.bind())
			return false;
		m_VertexColourObj.setUsagePattern(QGLBuffer::DynamicDraw);
		m_VertexColourObj.allocate(m_vColour, sizeof(GLfloat) * m_nFaceIndices * 3);
		//DEBUG(m_VertexColourObj.size());
		m_VertexColourObj.release();

		//Texture Coordinates=======================================================================

		//TC * TextureCoords = &(m_Geometry->ActiveLevel()->m_pTCs[0]);
		unsigned int TCCount = m_Geometry->ActiveLevel()->TCCount();
		DEBUG("TCCount:");
		DEBUG(TCCount);

		m_nTC = TCCount;

		BREAKPOINT();

		m_vTC = new float[m_nFaceIndices * 2];

		if (m_faceType == 4)
		{
			for (unsigned int i = 0; i < nFaceCount; i++)
			{

				m_vTC[i * m_faceType * 2] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 0).m_fU;
				m_vTC[i * m_faceType * 2 + 1] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 0).m_fV;

				m_vTC[i * m_faceType * 2 + 2] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 1).m_fU;
				m_vTC[i * m_faceType * 2 + 3] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 1).m_fV;

				m_vTC[i * m_faceType * 2 + 4] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 2).m_fU;
				m_vTC[i * m_faceType * 2 + 5] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 2).m_fV;

				m_vTC[i * m_faceType * 2 + 6] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 3).m_fU;
				m_vTC[i * m_faceType * 2 + 7] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 3).m_fV;
			}
		}
		BREAKPOINT();
		if (m_faceType == 3)
		{
			for (unsigned int i = 0; i < nFaceCount; i++)
			{

				m_vTC[i * m_faceType * 2] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 0).m_fU;
				m_vTC[i * m_faceType * 2 + 1] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 0).m_fV;

				m_vTC[i * m_faceType * 2 + 2] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 1).m_fU;
				m_vTC[i * m_faceType * 2 + 3] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 1).m_fV;

				m_vTC[i * m_faceType * 2 + 4] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 2).m_fU;
				m_vTC[i * m_faceType * 2 + 5] = m_Geometry->ActiveLevel()->TriangleVertexTC(i, 2).m_fV;

			}
		}

		BREAKPOINT();
		m_TCBuffer.create();
		// Copy data to video memory
		// Texture Coord data
		if (!m_TCBuffer.bind())
			return false;
		m_TCBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
		m_TCBuffer.allocate(m_vTC, sizeof(GLfloat) * m_nFaceIndices * 2);
		DEBUG("Total Num Quad TC:\n");
		DEBUG(m_TCBuffer.size());
		m_TCBuffer.release();

		m_nVertexArrayId = m_VertexArrayObj.bufferId();
		m_nVertexNormalId = m_VertexNormalObj.bufferId();
		m_nVertexColourId = m_VertexColourObj.bufferId();
		m_nTCBufferId = m_TCBuffer.bufferId();

		InitializeRays();
		BREAKPOINT();

		if (!InitializeShader("MBVR_vertex.glsl", "MBVR_fragment.glsl"))
			DEBUG("Compiler Init unsuccessfull");

		BREAKPOINT();

		return true;

	}

	bool RightHandMesh::InitializeRays()
	{
		LineLength = 1000;

		m_rayPos = new float[2 * 3];

		m_rayPos[0] = 0;
		m_rayPos[1] = 0;
		m_rayPos[2] = 0;
		
		m_rayPos[3] = 0;
		m_rayPos[4] = 0;
		m_rayPos[5] = -1000;

		m_RayVertArrayObj.create();
		if (!m_RayVertArrayObj.bind())
			return false;
		m_RayVertArrayObj.setUsagePattern(QGLBuffer::DynamicDraw);
		m_RayVertArrayObj.allocate(m_rayPos, sizeof(GLfloat) * 2 * 3);

		m_RayVertArrayObj.release();

		m_rayCol = new float[2 * 3];

		m_rayPos[0] = 0;
		m_rayPos[1] = 1;
		m_rayPos[2] = 0;

		m_rayPos[3] = 0;
		m_rayPos[4] = 1;
		m_rayPos[5] = 0;

		m_RayColourArrayObj.create();
		if (!m_RayColourArrayObj.bind())
			return false;
		m_RayColourArrayObj.setUsagePattern(QGLBuffer::DynamicDraw);
		m_RayColourArrayObj.allocate(m_rayCol, sizeof(GLfloat) * 2 * 3);

		m_RayColourArrayObj.release();

		return true;

	}

	void RightHandMesh::DrawRays()
	{
		//UpdateRays();
		m_RayVertArrayObj.bind();
		GLDEBUG();
		glVertexPointer(3, GL_FLOAT, 0, 0);
		GLDEBUG();

		BREAKPOINT();

		m_RayColourArrayObj.bind();

		glColorPointer(3, GL_FLOAT, 0, 0);
		BREAKPOINT();


		BREAKPOINT();
		glDrawArrays(GL_LINES, 0, 2);
		m_RayVertArrayObj.release();
		m_RayColourArrayObj.release();
		BREAKPOINT();
		//glDrawElements(GL_TRIANGLES, m_nTotalVerts, GL_UNSIGNED_SHORT, 0);
	}

	void RightHandMesh::UpdatePosition(Matrix _newPos)
	{
		Matrix SizeUp(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		WorldTransform = SizeUp * _newPos;
		
		//WorldTransform = _newPos * SizeUp;
	}


	void RightHandMesh::UpdateRay(float _LineLength)
	{
		float * newrayLength = new float { _LineLength };
		glEnableClientState(GL_VERTEX_ARRAY);
		GLDEBUG();
		m_RayVertArrayObj.bind();
		GLDEBUG();

		m_RayVertArrayObj.write(5 * sizeof(GLfloat), newrayLength, sizeof(GLfloat));
		GLDEBUG();
		m_RayVertArrayObj.release();
		GLDEBUG();
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	Geometry & RightHandMesh::getGeometry()
	{
		return *m_Geometry;
	}

	bool RightHandMesh::GeomInitialize(Geometry * _Geometry)
	{

		Scene::LoadData LoadDataSettings;
		LoadDataSettings.IncludeGeometries();

		QDir pluginDir(Kernel()->PluginDirectory("MudboxVR"));
		pluginDir.cd("../externalmodels/paintbrushmodel");

		QFileInfo ExternalModelPath(pluginDir, "PaintBrush.obj");

		m_Geometry = Kernel()->Scene()->Import(LoadDataSettings, ExternalModelPath.filePath());

		if (m_Geometry == NULL) {
			DEBUG("Geometry loading has failed using this path:");
			DEBUG(ExternalModelPath.filePath());
			return false;
		}

		DEBUG("Geometry has been found and is loaded");

		m_faceType = m_Geometry->ActiveLevel()->SideCount();

		if (!SendDataToGPU())
			return false;

		DEBUG("Data successfully sent to gpu");

		return true;
	}

	void RightHandMesh::DrawGL(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & ProjMat4x4, QMatrix4x4 & WorldScaleMat4x4)
	{


		//Calculate Relative Matrices
		QMatrix4x4 AdjustedModelMat = ModelMat4x4* MBtoQMatrix(WorldTransform) ;
		QMatrix4x4 InvAdjustedModelMat = AdjustedModelMat;//.inverted().transposed();
		QMatrix4x4 MeshMVP = ProjMat4x4 * AdjustedModelMat;

		//------------------------------------Draw Ray------------------------------

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		{

			glLoadMatrixd(ProjMat4x4.constData());


			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			{

				glLoadMatrixd(AdjustedModelMat.constData());

				DrawRays();

			}
			glPopMatrix();

			glMatrixMode(GL_PROJECTION);

		}
		glPopMatrix();

		//------------------------------------Draw Model------------------------------

				//Bind shader Progam to use it
		m_ShaderProgram->bind();

		//Activate and bind Texture?

		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, m_Texture->OpenGLName());

		m_ShaderProgram->setUniformValue("PaintTex", 0);
		GLDEBUG();

		//glBindTexture(GL_TEXTURE_2D, CurrentFrameBuffer);
		//
		//m_ShaderProgram->setUniformValue("FrameBuffer", 0);
		//GLDEBUG();

		//Pass in Model, View, and Projection matrices to opengl
		m_ShaderProgram->setUniformValue("ModelMat", AdjustedModelMat);
		m_ShaderProgram->setUniformValue("InvModelMat", InvAdjustedModelMat);
		m_ShaderProgram->setUniformValue("MVP", MeshMVP);
		m_ShaderProgram->setUniformValue("headPos", getHeadPos(ModelMat4x4, WorldScaleMat4x4));
		m_ShaderProgram->setUniformValue("DrawMode", DrawMode);

		//Bind Vertex, texcoord and index buffers
		m_VertexArrayObj.bind();
		m_ShaderProgram->enableAttributeArray("in_Position");
		m_ShaderProgram->setAttributeBuffer("in_Position", GL_FLOAT, 0, 3, 0);

		m_TCBuffer.bind();
		m_ShaderProgram->enableAttributeArray("in_TexCoords");
		m_ShaderProgram->setAttributeBuffer("in_TexCoords", GL_FLOAT, 0, 2, 0);


		m_VertexNormalObj.bind();
		m_ShaderProgram->enableAttributeArray("in_Normals");
		m_ShaderProgram->setAttributeBuffer("in_Normals", GL_FLOAT, 0, 3, 0);


		//Set attribute buffers to texture locations 

		if(m_faceType == 4)
			glDrawArrays(GL_QUADS, 0, m_nFaceIndices);
		else
			glDrawArrays(GL_TRIANGLES, 0, m_nFaceIndices);

		m_VertexArrayObj.release();
		m_TCBuffer.release();
		m_VertexNormalObj.release();

		m_ShaderProgram->disableAttributeArray("in_Position");
		m_ShaderProgram->disableAttributeArray("in_TexCoords");
		m_ShaderProgram->disableAttributeArray("in_Normals");

		m_ShaderProgram->release();

		

	}

	bool RightHandMesh::CheckIfGeomStillExists()
	{
		return true;
	}

	bool RightHandMesh::InitializeShader(QString _vertShaderName, QString _fragShaderName)
	{
		//Create Shader Program

		m_ShaderProgram = new QGLShaderProgram();

		BREAKPOINT();

		QDir pluginDir(Kernel()->PluginDirectory("MudboxVR"));
		pluginDir.cd("../shaders");
		QFileInfo VertexShaderPath(pluginDir, _vertShaderName);
		QFileInfo FragmentShaderPath(pluginDir, _fragShaderName);

		//QFileInfo VertexShaderPath(pluginDir, QString(  "MBVR_vertex.glsl"  ));
		//QFileInfo FragmentShaderPath(pluginDir, QString("MBVR_fragment.glsl"));

		if (!m_ShaderProgram->addShaderFromSourceFile(QGLShader::Vertex, VertexShaderPath.filePath()))
		{
			GLDEBUG();
			m_ShaderProgram->log();
			Kernel()->Interface()->HUDMessageShow(m_ShaderProgram->log(), Interface::HUDMessageType::HUDmsgPin);
			return false;
		}

		if (!m_ShaderProgram->addShaderFromSourceFile(QGLShader::Fragment, FragmentShaderPath.filePath()))
		{
			GLDEBUG();
			m_ShaderProgram->log();
			Kernel()->Interface()->HUDMessageShow(m_ShaderProgram->log(), Interface::HUDMessageType::HUDmsgPin);
			return false;
		}

		m_ShaderProgram->bindAttributeLocation("in_Position", m_nVertexArrayId);
		BREAKPOINT();

		if (!m_ShaderProgram->link())
			return false;
		BREAKPOINT();

		//Load Texture

		QDir TexturePluginDir(Kernel()->PluginDirectory("MudboxVR"));
		TexturePluginDir.cd("../externalmodels/paintbrushmodel");

		QFileInfo UVMapTexturePath(TexturePluginDir, "UVMAP.png");
		BREAKPOINT();

		DEBUG(UVMapTexturePath.filePath());

		m_Texture = CreateInstance<Texture>();

		//Create From file
		if( m_Texture->CreateFromFile(UVMapTexturePath.filePath()))
			return false;
		BREAKPOINT();
		
		//Send to GPU
		m_Texture->SetLocation(TexturePool::locationGPU);
		BREAKPOINT();

		return true;
	}

	QVector3D RightHandMesh::getHeadPos(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & WorldScaleMat4x4)
	{
		QMatrix4x4 ScaledModelMat = ModelMat4x4;// *MBtoQMatrix(m_Geometry->Transformation()->LocalToWorldMatrix());//;* WorldScaleMat4x4;

		return QVector3D(ScaledModelMat(0, 3), ScaledModelMat(1, 3), ScaledModelMat(2, 3));
	}


}
