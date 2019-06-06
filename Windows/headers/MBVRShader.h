#pragma once
#include <mbvr_global.h>


namespace mudbox {

	class MBVRShader
	{
	public:
		MBVRShader();
		~MBVRShader();
		bool InitializeShader(const char *_vertShaderPath, const char *_fragShaderPath);
		bool Activate();
	protected:
		
		QGLShaderProgram * program;
		char m_vertShaderPath;
		char m_fragShaderPath;
	};
}
