#include "MBVRMesh.h"

namespace mudbox 
{
	//-----------------------------------------------------------------------------
	// MBVRMesh Implementation
	//-----------------------------------------------------------------------------

	MBVRMesh::MBVRMesh() :
		m_VertexArrayObj(QGLBuffer::VertexBuffer),
		m_VertexNormalObj(QGLBuffer::VertexBuffer),
		m_FaceIndexObj(QGLBuffer::IndexBuffer),
		m_VertexColourObj(QGLBuffer::VertexBuffer),
		m_TCBuffer(QGLBuffer::VertexBuffer)
	{
		AABB = new AxisAlignedBoundingBox(Vector(0, 0, 0), 1000); BREAKPOINT();
	}

	MBVRMesh::~MBVRMesh()
	{
		m_ShaderProgram->removeAllShaders();
		delete AABB;
		AABB = NULL;
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

		if(m_vPos != NULL)
			delete[](m_vPos);

		if (m_vPos != NULL)
			delete[](m_FaceIndex);

		if (m_vPos != NULL)
			delete[](m_vTC);

		if (m_vPos != NULL)
			delete[](m_vColour);



		//Face Indices=======================================================================

		unsigned int nFaceCount = m_Geometry->ActiveLevel()->FaceCount();

		m_NonGenericfaceCount = nFaceCount;
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

		const Vertex * verts= m_Geometry->ActiveLevel()->VertexArray();

		m_vPos = new float[m_nFaceIndices * 3];

		for (unsigned int i=0; i< m_nFaceIndices; i++)
		{
			unsigned int ii = m_FaceIndex[i];

			m_vPos[i * 3    ] = verts[ii].m_vPos.x;
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
		DEBUG(m_VertexArrayObj.size()/3);
		m_VertexArrayObj.release();


		//Vertex Normals=======================================================================

		unsigned int numNormals=m_Geometry->ActiveLevel()->VertexNormalCount();
		m_vNormal = new float[m_nFaceIndices *3];

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
		m_VertexNormalObj.create();

		// Copy data to video memory
		// Normal data
		if (!m_VertexNormalObj.bind())
			return false;
		m_VertexNormalObj.setUsagePattern(QGLBuffer::DynamicDraw);
		m_VertexNormalObj.allocate(m_vNormal, sizeof(GLfloat) * 3 * m_nFaceIndices);
		DEBUG("Total Num Normal:\n");
		DEBUG(m_VertexNormalObj.size()/3);
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

		for (unsigned int i = 0; i < nFaceCount; i++)
		{

			m_vTC[i * m_faceType * 2]	  = m_Geometry->ActiveLevel()->QuadVertexTC(i, 0).m_fU;
			m_vTC[i * m_faceType * 2 + 1] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 0).m_fV;
										    
			m_vTC[i * m_faceType * 2 + 2] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 1).m_fU;
			m_vTC[i * m_faceType * 2 + 3] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 1).m_fV;
										    
			m_vTC[i * m_faceType * 2 + 4] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 2).m_fU;
			m_vTC[i * m_faceType * 2 + 5] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 2).m_fV;
										    
			m_vTC[i * m_faceType * 2 + 6] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 3).m_fU;
			m_vTC[i * m_faceType * 2 + 7] = m_Geometry->ActiveLevel()->QuadVertexTC(i, 3).m_fV;
		}

		BREAKPOINT();
		m_TCBuffer.create();
		// Copy data to video memory
		// Texture Coord data
		if (!m_TCBuffer.bind())
			return false;
		m_TCBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
		m_TCBuffer.allocate(m_vTC, sizeof(GLfloat) * m_nFaceIndices * 2 );
		DEBUG("Total Num Quad TC:\n");
		DEBUG(m_TCBuffer.size());
		m_TCBuffer.release();

		m_nVertexArrayId = m_VertexArrayObj.bufferId();
		m_nVertexNormalId = m_VertexNormalObj.bufferId();
		m_nVertexColourId = m_VertexColourObj.bufferId();
		m_nTCBufferId = m_TCBuffer.bufferId();

		BREAKPOINT();
		if (!InitializeShader("", ""))
			DEBUG("Compiler Init unsuccessfull");
		
		BREAKPOINT();

		return true;

	}

	Geometry & MBVRMesh::getGeometry()
	{
		return *m_Geometry;
	}

	QVector3D MBVRMesh::getHeadPos(QMatrix4x4 & ModelMat4x4 ,QMatrix4x4 & WorldScaleMat4x4)
	{
		QMatrix4x4 ScaledModelMat = ModelMat4x4 *MBtoQMatrix(m_Geometry->Transformation()->LocalToWorldMatrix());//;* WorldScaleMat4x4;

		return QVector3D(ScaledModelMat(0,3), ScaledModelMat(1,3), ScaledModelMat(2,3));
	}

	bool MBVRMesh::GeomInitialize(Geometry * _Geometry)
	{
		if (_Geometry == NULL)
			return false;
		
		m_Geometry = _Geometry;
		m_faceType = m_Geometry->ActiveLevel()->SideCount();

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



		QMatrix4x4 AdjustedModelMat = ModelMat4x4 * MBtoQMatrix(m_Geometry->Transformation()->LocalToWorldMatrix());// *WorldScaleMat4x4;

		QMatrix3x3 InvAdjustedModelMat = Inverse(Qmat4to3(AdjustedModelMat));// .transposed());
		//QMatrix3x3 InvAdjustedModelMat = Qmat4to3(AdjustedModelMat);

		m_ShaderProgram->setUniformValue("InvModelMat", InvAdjustedModelMat);// Qmat4to3(AdjustedModelMat));

		m_ShaderProgram->setUniformValue("ModelMat", AdjustedModelMat);
		//Send MVP matrix to shader
		QMatrix4x4 MeshMVP = ProjMat4x4 * AdjustedModelMat;
		m_ShaderProgram->setUniformValue("MVP", MeshMVP);
		BREAKPOINT();
		BREAKPOINT();
		m_ShaderProgram->setUniformValue("headPos", getHeadPos(ModelMat4x4, WorldScaleMat4x4));

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
		glDrawArrays(GL_QUADS, 0, m_nFaceIndices);

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
		QFileInfo VertexShaderPath(pluginDir, QString("MBVR_vertex.glsl"));
		QFileInfo FragmentShaderPath(pluginDir, QString("MBVR_fragment.glsl"));

		if(!m_ShaderProgram->addShaderFromSourceFile(QGLShader::Vertex, VertexShaderPath.filePath()) )
		{
			GLDEBUG();
			m_ShaderProgram->log();
			Kernel()->Interface()->HUDMessageShow(m_ShaderProgram->log(), Interface::HUDMessageType::HUDmsgPin);
			return false;
		}

		if (!m_ShaderProgram->addShaderFromSourceFile(QGLShader::Fragment, FragmentShaderPath.filePath()) )
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
		
		return true;
		//m_ShaderProgram->bindAttributeLocation("in_Normal", m_nVertexNormalId);
		//m_ShaderProgram->bindAttributeLocation("in_TexCoord0", m_nVertexArrayId);
	}

	//-----------------------------------------------------------------------------
	// CustomVRMesh Implementation
	//-----------------------------------------------------------------------------

	CustomVRMesh::CustomVRMesh() :
		m_VertexArrayObj(QGLBuffer::VertexBuffer),
		m_VertexNormalObj(QGLBuffer::VertexBuffer),
		m_FaceIndexObj(QGLBuffer::IndexBuffer),
		m_VertexColourObj(QGLBuffer::VertexBuffer),
		m_TCBuffer(QGLBuffer::VertexBuffer)
	{
	}

	CustomVRMesh::~CustomVRMesh() 
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

	bool CustomVRMesh::SendDataToGPU()
	{

		if (m_vPos != NULL)
			delete[](m_vPos);

		if (m_vPos != NULL)
			delete[](m_FaceIndex);

		if (m_vPos != NULL)
			delete[](m_vTC);

		if (m_vPos != NULL)
			delete[](m_vColour);



		//Face Indices=======================================================================

		unsigned int m_nFaceCount =0;
		m_nFaceIndices = 0;

		m_FaceIndex = new (unsigned int[m_nFaceIndices]);

		if (m_faceType == 4)
		{

			for (unsigned int i = 0; i < m_nFaceCount; i++)
			{
			}
		}
		if (m_faceType == 3)
		{

			for (unsigned int i = 0; i < m_nFaceCount; i++)
			{
			}
		}

		//Vertex Coordinates=======================================================================

		m_vPos = new float[m_nFaceIndices * 3];

		for (unsigned int i = 0; i < m_nFaceIndices; i++)
		{
		}
		m_VertexArrayObj.create();
		// Copy data to video memory
		// Vertex data
		if (!m_VertexArrayObj.bind())
			return false;
		m_VertexArrayObj.setUsagePattern(QGLBuffer::DynamicDraw);
		m_VertexArrayObj.allocate(m_vPos, sizeof(GLfloat) * 3 * m_nFaceIndices);
		m_VertexArrayObj.release();


		//Vertex Normals=======================================================================

		m_vNormal = new float[m_nFaceIndices * 3];

		for (unsigned int i = 0; i < m_nFaceCount; i++)
		{
		}
		m_VertexNormalObj.create();

		// Copy data to video memory
		// Normal data
		if (!m_VertexNormalObj.bind())
			return false;
		m_VertexNormalObj.setUsagePattern(QGLBuffer::DynamicDraw);
		m_VertexNormalObj.allocate(m_vNormal, sizeof(GLfloat) * 3 * m_nFaceIndices);
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
		m_VertexColourObj.release();

		//Texture Coordinates=======================================================================

		m_nTC = 0;

		m_vTC = new float[m_nFaceIndices * 2];

		for (unsigned int i = 0; i < m_nFaceCount ; i++)
		{
		}

		BREAKPOINT();
		m_TCBuffer.create();
		// Copy data to video memory
		// Texture Coord data
		if (!m_TCBuffer.bind())
			return false;
		m_TCBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
		m_TCBuffer.allocate(m_vTC, sizeof(GLfloat) * m_nFaceIndices * 2);
		m_TCBuffer.release();

		m_nVertexArrayId = m_VertexArrayObj.bufferId();
		m_nVertexNormalId = m_VertexNormalObj.bufferId();
		m_nVertexColourId = m_VertexColourObj.bufferId();
		m_nTCBufferId = m_TCBuffer.bufferId();

		if (!InitializeShader("", ""))
			DEBUG("Compiler Init unsuccessfull");

		return true;

	}

	Geometry & CustomVRMesh::getGeometry()
	{
		Geometry * null = NULL;
		return *null;
	}

	bool CustomVRMesh::GeomInitialize(Geometry * _Geometry)
	{

	}

	void CustomVRMesh::DrawGL(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & ProjMat4x4, QMatrix4x4 & WorldScaleMat4x4)
	{
		//Bind shader Progam to use it
		m_ShaderProgram->bind();

		//Activate and bind Texture?

		//unsigned int totalNumTextures = m_Geometry->ActiveLevel()->Material()->TextureCount();
		//Texture * CurTexture;
		//
		//if (totalNumTextures > 0)
		//{
		//	CurTexture = m_Geometry->ActiveLevel()->Material()->Texture(0)->Tile(0);
		//	glEnable(GL_TEXTURE_2D);
		//
		//	glBindTexture(GL_TEXTURE_2D, CurTexture->OpenGLName());
		//
		//	m_ShaderProgram->setUniformValue("PaintTex", 0);
		//	BREAKPOINT();
		//}



		QMatrix4x4 AdjustedModelMat = ModelMat4x4 * MBtoQMatrix(WorldTransform);// *WorldScaleMat4x4;

		QMatrix3x3 InvAdjustedModelMat = Inverse(Qmat4to3(AdjustedModelMat));// .transposed());
		//QMatrix3x3 InvAdjustedModelMat = Qmat4to3(AdjustedModelMat);

		m_ShaderProgram->setUniformValue("InvModelMat", InvAdjustedModelMat);// Qmat4to3(AdjustedModelMat));

		m_ShaderProgram->setUniformValue("ModelMat", AdjustedModelMat);
		//Send MVP matrix to shader
		QMatrix4x4 MeshMVP = ProjMat4x4 * AdjustedModelMat;
		m_ShaderProgram->setUniformValue("MVP", MeshMVP);
		BREAKPOINT();
		BREAKPOINT();
		m_ShaderProgram->setUniformValue("headPos", getHeadPos(ModelMat4x4, WorldScaleMat4x4));

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
		glDrawArrays(GL_QUADS, 0, m_nFaceIndices);

		m_VertexArrayObj.release();
		m_TCBuffer.release();
		m_VertexNormalObj.release();

		m_ShaderProgram->disableAttributeArray("in_Position");
		m_ShaderProgram->disableAttributeArray("in_TexCoords");
		m_ShaderProgram->disableAttributeArray("in_Normals");

		m_ShaderProgram->release();

	}

	bool CustomVRMesh::CheckIfGeomStillExists()
	{

	}

	bool CustomVRMesh::InitializeShader(QString _vertShaderName, QString _fragShaderName)
	{

	}

	QVector3D CustomVRMesh::getHeadPos(QMatrix4x4 & ModelMat4x4, QMatrix4x4 & WorldScaleMat4x4)
	{

	}


}
