#include "mbvr_global.h"

namespace mudbox 
{

	//-------------------------------------------------------------
	//--					Helper Functions					 --
	//-------------------------------------------------------------

	//----------------------Debugging Functions--------------------

	void breakPoint(uint breakPointIndex, std::string funcName)
	{
		std::string msg(funcName + " Breakpoint " + std::to_string(breakPointIndex));
		PRINT(msg);
		DEBUG(msg);
	}

	void printMud(std::string msg)
	{
		Kernel()->Interface()->SetStatus(mudbox::Interface::stWarning, QString(msg.c_str()));
	}

	void logMud(std::string msg)
	{
		Kernel()->Log(QString(msg.c_str()) + "\n");
	}

	void logMatrix(Matrix _matrix, std::string _nameOfMatrix)
	{
		auto _11 = std::to_string(_matrix._11);
		auto _12 = std::to_string(_matrix._12);
		auto _13 = std::to_string(_matrix._13);
		auto _14 = std::to_string(_matrix._14);

		auto _21 = std::to_string(_matrix._21);
		auto _22 = std::to_string(_matrix._22);
		auto _23 = std::to_string(_matrix._23);
		auto _24 = std::to_string(_matrix._24);

		auto _31 = std::to_string(_matrix._31);
		auto _32 = std::to_string(_matrix._32);
		auto _33 = std::to_string(_matrix._33);
		auto _34 = std::to_string(_matrix._34);

		auto _41 = std::to_string(_matrix._41);
		auto _42 = std::to_string(_matrix._42);
		auto _43 = std::to_string(_matrix._43);
		auto _44 = std::to_string(_matrix._44);


		std::string msg = _nameOfMatrix +
			":\n| " + _11 + ", " + _21 + ", " + _31 + ", " + _41 + "|\n" +
			"| " + _12 + ", " + _22 + ", " + _32 + ", " + _42 + "|\n" +
			"| " + _13 + ", " + _23 + ", " + _33 + ", " + _43 + "|\n" +
			"| " + _14 + ", " + _24 + ", " + _34 + ", " + _44 + "|\n";

		Kernel()->Log(QString::fromStdString(msg));
	}

	void GLDebug(uint breakPointIndex, std::string funcName) {
		GLenum err;

		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::string msg;
			switch (err)
			{
			case GL_INVALID_ENUM:
				msg=(funcName + " Breakpoint " + std::to_string(breakPointIndex) + STRING(GL_INVALID_ENUM));
				Kernel()->Log(QString(msg.c_str()) + "\n");
				break;
			case GL_INVALID_VALUE:
				msg = (funcName + " Breakpoint " + std::to_string(breakPointIndex) + STRING(GL_INVALID_VALUE));
				Kernel()->Log(QString(msg.c_str()) + "\n");
				break;
			case GL_INVALID_OPERATION:
				msg = (funcName + " Breakpoint " + std::to_string(breakPointIndex) + STRING(GL_INVALID_OPERATION));
				Kernel()->Log(QString(msg.c_str()) + "\n");
				break;
			case GL_STACK_OVERFLOW:
				msg = (funcName + " Breakpoint " + std::to_string(breakPointIndex) + STRING(GL_STACK_OVERFLOW));
				Kernel()->Log(QString(msg.c_str()) + "\n");
				break;
			case GL_STACK_UNDERFLOW:
				msg = (funcName + " Breakpoint " + std::to_string(breakPointIndex) + STRING(GL_STACK_UNDERFLOW));
				Kernel()->Log(QString(msg.c_str()) + "\n");
				break;
			case GL_OUT_OF_MEMORY:
				msg = (funcName + " Breakpoint " + std::to_string(breakPointIndex) + STRING(GL_OUT_OF_MEMORY));
				Kernel()->Log(QString(msg.c_str()) + "\n");
				break;
			}
		}
	}

	Matrix ConvertSteamVRMatrixToMatrix(const vr::HmdMatrix34_t &matPose)
	{
		Matrix matrixObj(
			matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
			matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
			matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
			matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
		);
		return matrixObj;
	}


	std::string GetTrackedDeviceString(vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL)
	{
		uint32_t unRequiredBufferLen = vr::VRSystem()->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
		if (unRequiredBufferLen == 0)
			return "";

		char *pchBuffer = new char[unRequiredBufferLen];
		unRequiredBufferLen = vr::VRSystem()->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
		std::string sResult = pchBuffer;
		delete[] pchBuffer;
		return sResult;
	}

	std::string getCompositorErrorAsString(vr::EVRCompositorError _cError)
	{
		switch (_cError)
		{
		case vr::VRCompositorError_None:
			return std::string("VRCompositorError_None");
			break;
		case vr::VRCompositorError_RequestFailed:
			return std::string("VRCompositorError_RequestFailed");
			break;
		case vr::VRCompositorError_IncompatibleVersion:
			return std::string("VRCompositorError_IncompatibleVersion");
			break;
		case vr::VRCompositorError_DoNotHaveFocus:
			return std::string("VRCompositorError_DoNotHaveFocus");
			break;
		case vr::VRCompositorError_InvalidTexture:
			return std::string("VRCompositorError_InvalidTexture");
			break;
		case vr::VRCompositorError_IsNotSceneApplication:
			return std::string("VRCompositorError_IsNotSceneApplication");
			break;
		case vr::VRCompositorError_TextureIsOnWrongDevice:
			return std::string("VRCompositorError_TextureIsOnWrongDevice");
			break;
		case vr::VRCompositorError_TextureUsesUnsupportedFormat:
			return std::string("VRCompositorError_TextureUsesUnsupportedFormat");
			break;
		case vr::VRCompositorError_SharedTexturesNotSupported:
			return std::string("VRCompositorError_SharedTexturesNotSupported");
			break;
		case vr::VRCompositorError_IndexOutOfRange:
			return std::string("VRCompositorError_IndexOutOfRange");
			break;
		case vr::VRCompositorError_AlreadySubmitted:
			return std::string("VRCompositorError_AlreadySubmitted");
			break;
		case vr::VRCompositorError_InvalidBounds:
			return std::string("VRCompositorError_InvalidBounds");
			break;
		default:
			break;
		}
		return std::string("Invalid no error");
	}

}
