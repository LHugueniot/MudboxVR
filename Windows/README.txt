============================================MudBoxVR Readme===============================================

TODO:

-Get Opengl context to work.

BUGS:

-Compositor crashing sometimes, usually after steam VR is closed, potential solution:
https://community.viveport.com/t5/Technical-Support/Error-400-Compositor-not-available-Symptoms-and-Solutions/td-p/279

EXAMPLE CODE:

CVBOMesh::CVBOMesh(void)
    : vertexDataVBO(QGLBuffer::VertexBuffer)
    , normalDataVBO(QGLBuffer::VertexBuffer)
    , textureDataVBO(QGLBuffer::VertexBuffer)
    , indexDataVBO(QGLBuffer::IndexBuffer)
 
{
    pIndexes = NULL;
    pVerts = NULL;
    pNorms = NULL;
    pTexCoords = NULL;
 
    nMaxIndexes = 0;
    nNumIndexes = 0;
    nNumVerts = 0;
}
 
////////////////////////////////////////////////////////////
// Free any dynamically allocated memory. For those C programmers
// coming to C++, it is perfectly valid to delete a NULL pointer.
CVBOMesh::~CVBOMesh(void)
{
    // Just in case these still are allocated when the object is destroyed
    delete [] pIndexes;
    delete [] pVerts;
    delete [] pNorms;
    delete [] pTexCoords;
 
    // Delete buffer objects
    vertexDataVBO.destroy();
    normalDataVBO.destroy();
    textureDataVBO.destroy();
    indexDataVBO.destroy();
}
 
////////////////////////////////////////////////////////////
// Start assembling a mesh. You need to specify a maximum amount
// of indexes that you expect. The EndMesh will clean up any uneeded
// memory. This is far better than shreading your heap with STL containers...
// At least that's my humble opinion.
void CVBOMesh::BeginMesh(GLuint nMaxVerts)
{
    // Just in case this gets called more than once...
    delete [] pIndexes;
    delete [] pVerts;
    delete [] pNorms;
    delete [] pTexCoords;
 
    nMaxIndexes = nMaxVerts;
    nNumIndexes = 0;
    nNumVerts = 0;
 
    // Allocate new blocks
    pIndexes = new GLushort[nMaxIndexes];
    pVerts = new M3DVector3f[nMaxIndexes];
    pNorms = new M3DVector3f[nMaxIndexes];
    pTexCoords = new M3DVector2f[nMaxIndexes];
}
 
/////////////////////////////////////////////////////////////////
// Add a triangle to the mesh. This searches the current list for identical
// (well, almost identical - these are floats you know...) verts. If one is found, it
// is added to the index array. If not, it is added to both the index array and the vertex
// array grows by one as well.
void CVBOMesh::AddTriangle(M3DVector3f verts[3], M3DVector3f vNorms[3], M3DVector2f vTexCoords[3])
{
    const  float e = 0.000001; // How small a difference to equate
 
    // First thing we do is make sure the normals are unit length!
    // It's almost always a good idea to work with pre-normalized normals
    m3dNormalizeVector(vNorms[0]);
    m3dNormalizeVector(vNorms[1]);
    m3dNormalizeVector(vNorms[2]);
 
 
    // Search for match - triangle consists of three verts
    for(GLuint iVertex = 0; iVertex < 3; iVertex++)
    {
        GLuint iMatch = 0;
        for(iMatch = 0; iMatch < nNumVerts; iMatch++)
        {
            // If the vertex positions are the same
            if(m3dCloseEnough(pVerts[iMatch][0], verts[iVertex][0], e) &&
               m3dCloseEnough(pVerts[iMatch][1], verts[iVertex][1], e) &&
               m3dCloseEnough(pVerts[iMatch][2], verts[iVertex][2], e) &&
 
               // AND the Normal is the same...
               m3dCloseEnough(pNorms[iMatch][0], vNorms[iVertex][0], e) &&
               m3dCloseEnough(pNorms[iMatch][1], vNorms[iVertex][1], e) &&
               m3dCloseEnough(pNorms[iMatch][2], vNorms[iVertex][2], e) &&
 
                // And Texture is the same...
                m3dCloseEnough(pTexCoords[iMatch][0], vTexCoords[iVertex][0], e) &&
                m3dCloseEnough(pTexCoords[iMatch][1], vTexCoords[iVertex][1], e))
                {
                    // Then add the index only
                    pIndexes[nNumIndexes] = iMatch;
                    nNumIndexes++;
                    break;
                }
        }
 
        // No match for this vertex, add to end of list
        if(iMatch == nNumVerts)
        {
            memcpy(pVerts[nNumVerts], verts[iVertex], sizeof(M3DVector3f));
            memcpy(pNorms[nNumVerts], vNorms[iVertex], sizeof(M3DVector3f));
            memcpy(pTexCoords[nNumVerts], &vTexCoords[iVertex], sizeof(M3DVector2f));
            pIndexes[nNumIndexes] = nNumVerts;
            nNumIndexes++;
            nNumVerts++;
        }
    }
}
 
 
 
//////////////////////////////////////////////////////////////////
// Compact the data. This is a nice utility, but you should really
// save the results of the indexing for future use if the model data
// is static (doesn't change).
void CVBOMesh::EndMesh(void)
{
    // Create the buffer objects
    vertexDataVBO.create();
    normalDataVBO.create();
    textureDataVBO.create();
    indexDataVBO.create();
 
    // Copy data to video memory
    // Vertex data
    vertexDataVBO.bind();
    vertexDataVBO.allocate(pVerts, sizeof(GLfloat)*nNumVerts*3);
    vertexDataVBO.setUsagePattern(QGLBuffer::StaticDraw);
 
    // Normal data
    normalDataVBO.bind();
    normalDataVBO.allocate(pNorms, sizeof(GLfloat)*nNumVerts*3);
    normalDataVBO.setUsagePattern(QGLBuffer::StaticDraw);
 
    // Texture coordinates
    textureDataVBO.bind();
    textureDataVBO.allocate(pTexCoords, sizeof(GLfloat)*nNumVerts*2);
    textureDataVBO.setUsagePattern(QGLBuffer::StaticDraw);
 
    // Indexes
    indexDataVBO.bind();
    textureDataVBO.allocate(pIndexes, sizeof(GLushort)*nNumIndexes);
    textureDataVBO.setUsagePattern(QGLBuffer::StaticDraw);
 
    // Free older, larger arrays
    delete [] pIndexes;
    delete [] pVerts;
    delete [] pNorms;
    delete [] pTexCoords;
 
    // Reasign pointers so they are marked as unused
    pIndexes = NULL;
    pVerts = NULL;
    pNorms = NULL;
    pTexCoords = NULL;
}
 
//////////////////////////////////////////////////////////////////////////
// Draw - make sure you call glEnableClientState for these arrays
void CVBOMesh::Draw(void)
{
    // Here's where the data is now
    vertexDataVBO.bind();
    glVertexPointer(3, GL_FLOAT,0,0);
 
    // Normal data
    normalDataVBO.bind();
    glNormalPointer(GL_FLOAT, 0, 0);
 
    // Texture coordinates
    textureDataVBO.bind();
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
 
    // Indexes
    indexDataVBO.bind();
    glDrawElements(GL_TRIANGLES, nNumIndexes, GL_UNSIGNED_SHORT, 0);
}
 
///////////////////////////////////////////////////////////////////////////
// Scale of the vertices. The only way to do this is to map the VBO back
// into client memory, then back again
void CVBOMesh::Scale(GLfloat fScaleValue) 
{
    vertexDataVBO.bind();
    M3DVector3f *pVertexData = (M3DVector3f *)vertexDataVBO.map(QGLBuffer::ReadWrite);
 
    if(pVertexData != NULL)
    {
        for(unsigned int i = 0; i < nNumVerts; i++)
            m3dScaleVector3(pVertexData[i], fScaleValue);
 
        vertexDataVBO.unmap();
    }
}
 
///////////////////////// GLWidget class, both ThunderbirdBody and ThunderbirdGlass are CVBOMesh classes
void GLWidget::DrawThunderBird()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
 
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_CUBE_MAP);
    glActiveTexture(GL_TEXTURE0);
 
    glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D, textureObjects[BODY_TEXTURE]);
        thunderBirdBody.Draw();
    glPopMatrix();
 
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glActiveTexture(GL_TEXTURE0);
 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
    glBindTexture(GL_TEXTURE_2D, textureObjects[GLASS_TEXTURE]);
 
    glTranslatef(0.0f, 0.132f, 0.555f);
 
    glFrontFace(GL_CW);
    thunderBirdGlass.Draw();
    glFrontFace(GL_CCW);
    thunderBirdGlass.Draw();
    glDisable(GL_BLEND);
 
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}