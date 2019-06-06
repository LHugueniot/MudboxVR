#include "MBVRShader.h"

namespace mudbox 
{
	MBVRShader::MBVRShader()
	{
		program = new QGLShaderProgram();
	}

	bool MBVRShader::InitializeShader(const char *_vertShaderPath, const char *_fragShaderPath)
	{
		const char *vsrc =
		"#version 330 core																	"
		"layout(location = 0) in vec3 in_Position;											"
		"layout(location = 3) in vec2 in_TexCoord0;											"
		"																					"
		"// mvpmatrix is the result of multiplying the model, view, and projection matrices	"
		"uniform mat4 mvpmatrix;															"
		"uniform mat4 modelMat;																"
		"																					"
		"// Texture coordinate for the fragment shader										"
		"out vec2 f_TexCoord0;																"
		"out vec3 viewPos;																	"
		"																					"
		"void main(void)																	"
		"{																					"
		"	gl_Position = mvpmatrix * vec4(in_Position, 1.0);								"
		"	viewPos = (modelMat * vec4(in_Position, 1.0)).xyz;								"
		"	f_TexCoord0 = in_TexCoord0;														"
		"};																					";
			
		program->addShaderFromSourceCode(QGLShader::Vertex, vsrc);

		const char *fsrc =
			"#version 330 core																	  "
			"uniform sampler2D texture1;														  "
			"in vec2 f_TexCoord0;																  "
			"in vec3 viewPos;																	  "
			"layout(location = 0) out vec4 fragColor;											  "
			"																					  "
			"uniform vec4 ambientMaterial2, diffuseMaterial2, specularMaterial2;				  "
			"uniform vec4 ambientLight, diffuseLight, specularLight;							  "
			"uniform mat4 viewMat;																  "
			"uniform vec4 lightPosition;														  "
			"uniform float shininess;															  "
			"																					  "
			"void main(void)																	  "
			"{																					  "
			"	vec4 ambientProduct = ambientLight * ambientMaterial2;							  "
			"	vec4 diffuseProduct = diffuseLight * diffuseMaterial2;							  "
			"	vec4 specularProduct = specularLight * specularMaterial2;						  "
			"																					  "
			"	vec3 pos = viewPos;																  "
			"	vec3 nv = cross(dFdx(pos), dFdy(pos));											  "
			"	nv = nv * sign(nv.z);															  "
			"	vec3 L = normalize((viewMat*lightPosition).xyz - pos);							  "
			"	vec3 E = normalize(-pos);														  "
			"	vec3 H = normalize(L + E);														  "
			"	vec3 N = normalize(nv);															  "
			"																					  "
			"	float Kd = max(dot(L, N), 0.0);													  "
			"	vec4 diffuse = Kd * diffuseProduct;												  "
			"																					  "
			"	vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);										  "
			"	if (Kd > 0.0f)																	  "
			"	{																				  "
			"		float Ks = pow(max(dot(H, N), 1.0), shininess);								  "
			"		specular = Ks * specularProduct;											  "
			"	}																				  "
			"	vec4 fragColor_lightning = (ambientProduct + diffuse + specular);				  "
			"	vec4 fragColor_texture = texture2D(texture1, vec2(f_TexCoord0.x, f_TexCoord0.y)); "
			"	fragColor = fragColor_lightning + fragColor_texture;							  "
			"};																					  ";
		program->addShaderFromSourceCode(QGLShader::Fragment, fsrc);
		//shaderProgram.bindAttributeLocation("in_Position", vertexLocationInShader);
		return false;
	}

	bool MBVRShader::Activate()
	{
		//program->bind();
		//
		//int vertexLocation = program->attributeLocation("vertex");
		//int colorLocation = program->attributeLocation("color");
		//int matrixLocation = program->attributeLocation("matrix");
		//
		//QMatrix4x4 pmvMatrix;
		//
		//program->setAttributeArray(vertexLocation, triangleVertices);
		//program->setUniformValue(matrixLocation, pmvMatrix);
		//program->setUniformValue(colorLocation, QColor(1, 1, 1));
		//
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//
		//program->disableAttributeArray(vertexLocation);
		//program->release();
		return false;
	}

	MBVRShader::~MBVRShader()
	{
	}
}




//shaderProgram.bindAttributeLocation("inPosition", vertexLocationInShader);
//shaderProgram.link();
//
////setup ortho projection
//shaderProgram.bind();
//QMatrix4x4 m;
//shaderProgram.setUniformValue("modelviewMatrix", m);
//m.ortho(0, 1, 0, 1, 0, -1);
//shaderProgram.setUniformValue("projectionMatrix", m);
//shaderProgram.release();
//
//
//
//
//
//
//
//
//class GLWidgetOpenGL4Example : public QGLWidget {
//public:
//	GLWidgetOpenGL4Example(const QGLFormat& fmt) :QGLWidget(fmt), vaoId(0), elementBuffer(0) {}
//
//protected:
//	virtual void paintGL();
//	virtual void initializeGL();
//
//	virtual void resizeGL(int width, int height) { glViewport(0, 0, width, height); }
//private:
//	const int        vertexLocationInShader = 0;
//	GLuint           vaoId;
//	GLuint           elementBuffer;
//	QGLShaderProgram shaderProgram;
//};
//
//void GLWidgetOpenGL4Example::paintGL() {
//	glBindVertexArray(vaoId);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
//	shaderProgram.bind();
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//	shaderProgram.release();
//	glBindVertexArray(0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//}
//
//void GLWidgetOpenGL4Example::initializeGL() {
//	qInfo() << "Widget OpenGl: " << format().majorVersion() << "." << format().minorVersion();
//	qInfo() << "Context valid: " << context()->isValid();
//	qInfo() << "Really used OpenGl: " << context()->format().majorVersion() << "." << context()->format().minorVersion();
//	qInfo() << "OpenGl information: VENDOR:       " << (const char*)glGetString(GL_VENDOR);
//	qInfo() << "                    RENDERDER:    " << (const char*)glGetString(GL_RENDERER);
//	qInfo() << "                    VERSION:      " << (const char*)glGetString(GL_VERSION);
//	qInfo() << "                    GLSL VERSION: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
//
//	glClearColor(0.0, 1.0, 0.0, 1.0);
//
//
//	//Faces:
//	vector<GLuint> f = {
//			0, 1, 2,
//			0, 2, 3
//	};
//	//Vertices:
//	vector<GLfloat> v = {
//			0.1, 0.1, 0.0,  // 0
//			0.1, 0.9, 0.0,  // 1
//			0.9, 0.9, 0.0,  // 2
//			0.9, 0.1, 0.0   // 3
//	};
//
//	qInfo() << "VAO: generate Vertex Array";
//	glGenVertexArrays(1, &vaoId);
//	glBindVertexArray(vaoId);
//
//	{
//		// buffer for faces
//		glGenBuffers(1, &elementBuffer);
//		qInfo("VAO %d: new elements (faces) buffer %d:  %d faces", vaoId, elementBuffer, f.size() / 3);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * f.size(), &(f[0]), GL_STATIC_DRAW);
//	}
//
//	// buffer for vertex positions
//	{
//		GLuint buffer;
//		glGenBuffers(1, &buffer);
//		qInfo("VAO %d: new position / vertex buffer %d: %d vertices", vaoId, buffer, v.size() / 3);
//		glBindBuffer(GL_ARRAY_BUFFER, buffer);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * v.size(), &(v[0]), GL_STATIC_DRAW);
//		glVertexAttribPointer(vertexLocationInShader, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glEnableVertexAttribArray(vertexLocationInShader);
//	}
//
//	glBindVertexArray(0);
//
//	const char *vsrc =
//		"uniform mat4 projectionMatrix;                      \n"
//		"uniform mat4 modelviewMatrix;                       \n"
//		"                                                    \n"
//		"layout(location = 0) in vec3 inPosition;            \n"
//		"void main(void)                                     \n"
//		"{                                                   \n"
//		"    gl_Position = projectionMatrix *                \n"
//		"                   modelviewMatrix *                \n"
//		"                        inPosition;                 \n"
//		"}                                                   \n";
//
//	const char *fsrc =
//		"layout(location = 0, index = 0) out vec4 fragColor; \n"
//		"void main(void)                                     \n"
//		"{                                                   \n"
//		"    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);        \n"
//		"}                                                   \n";
//
//	shaderProgram.addShaderFromSourceCode(QGLShader::Vertex, vsrc);
//	shaderProgram.addShaderFromSourceFile(QGLShader::Fragment, fsrc);
//	shaderProgram.bindAttributeLocation("inPosition", vertexLocationInShader);
//	shaderProgram.link();
//
//	//setup ortho projection
//	shaderProgram.bind();
//	QMatrix4x4 m;
//	shaderProgram.setUniformValue("modelviewMatrix", m);
//	m.ortho(0, 1, 0, 1, 0, -1);
//	shaderProgram.setUniformValue("projectionMatrix", m);
//	shaderProgram.release();
//}
//
//int main(int argc, char** argv) {
//	// Read command lines arguments.
//	QApplication application(argc, argv);
//
//	//Important: Setup the Core profile (and not the compatability profile, OpenGL 2.1)
//	QGLFormat fmt = QGLFormat::defaultFormat();
//	fmt.setVersion(4, 1);
//	fmt.setProfile(QGLFormat::CoreProfile);
//
//	// Instantiate the viewer.
//	GLWidgetOpenGL4Example glwidgetExample(fmt);
//
//	glwidgetExample.setWindowTitle("QGLWidget OpenGL 4 Example");
//
//	// Make the viewer window visible on screen.
//	glwidgetExample.show();
//
//	// Run main loop.
//	return application.exec();
//}