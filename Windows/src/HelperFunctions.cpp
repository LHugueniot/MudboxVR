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

	QMatrix4x4 MBtoQMatrix(Matrix _Matrix)
	{
		return QMatrix4x4(
			_Matrix._11, _Matrix._21, _Matrix._31, _Matrix._41,
			_Matrix._12, _Matrix._22, _Matrix._32, _Matrix._42,
			_Matrix._13, _Matrix._23, _Matrix._33, _Matrix._43,
			_Matrix._14, _Matrix._24, _Matrix._34, _Matrix._44);
	}

	QMatrix3x3 Qmat4to3(QMatrix4x4 _Matrix)
	{
		
		QMatrix3x3 retMat;

		retMat.data()[0] = _Matrix.data()[0];
		retMat.data()[1] = _Matrix.data()[1];
		retMat.data()[2] = _Matrix.data()[2];

		retMat.data()[3] = _Matrix.data()[4];
		retMat.data()[4] = _Matrix.data()[5];
		retMat.data()[5] = _Matrix.data()[6];

		retMat.data()[6] = _Matrix.data()[8];
		retMat.data()[7] = _Matrix.data()[9];
		retMat.data()[8] = _Matrix.data()[10];
		//float values[] = { _Matrix.data[0], _Matrix.data[1], _Matrix.data[2],
		//					_Matrix.data[4], _Matrix.data[5], _Matrix.data[6],
		//					_Matrix.data[8], _Matrix.data[9], _Matrix.data[10] };
		//QMatrix3x3 test(values);
		return retMat;
	}
	QMatrix3x3 Inverse(QMatrix3x3 _Matrix)
	{
		//finding determinant
		QMatrix3x3 retMat;

		float determinant = 0;

		qreal mat[3][3] = {
			 _Matrix.data()[0],
			 _Matrix.data()[1],
			 _Matrix.data()[2],

			 _Matrix.data()[3],
			 _Matrix.data()[4],
			 _Matrix.data()[5],

			 _Matrix.data()[6],
			 _Matrix.data()[7],
			 _Matrix.data()[8] };

		for (int i = 0; i < 3; i++)
			determinant = determinant + (mat[0][i] * (mat[1][(i + 1) % 3] * mat[2][(i + 2) % 3] - mat[1][(i + 2) % 3] * mat[2][(i + 1) % 3]));

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++)
				retMat.data()[i + j] =((mat[(j + 1) % 3][(i + 1) % 3] * mat[(j + 2) % 3][(i + 2) % 3]) - (mat[(j + 1) % 3][(i + 2) % 3] * mat[(j + 2) % 3][(i + 1) % 3])) / determinant;
		}
		return retMat;
	}

	//QMatrix4x4 MBtoQMatrix(Matrix _Matrix)
	//{
	//	return QMatrix4x4(
	//		_Matrix._11, _Matrix._12, _Matrix._13, _Matrix._14,
	//		_Matrix._21, _Matrix._22, _Matrix._23, _Matrix._24,
	//		_Matrix._31, _Matrix._32, _Matrix._33, _Matrix._34,
	//		_Matrix._41, _Matrix._42, _Matrix._43, _Matrix._44);
	//}

	std::string ReadFromFile(QString filePath)
	{

		QFile file(filePath);
		logMud(filePath);

		//if (!file.open(QIODevice::ReadOnly)) {
		//
		//	logMud("Cannot open file for reading");
		//	return std::string("Cannot open file for reading");
		//}

		QTextStream in(&file);
		QString OutputString;

		while (!in.atEnd()) {

			OutputString.append(in.readLine());
			DEBUG(in.readLine());
		}

		file.close();
		return (OutputString.toStdString());
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

	std::string GetVRCompositorErrorAsString(vr::EVRCompositorError compError)
	{
		switch (compError)
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

	std::string GetVRInputErrorAsString(vr::EVRInputError inputError)
	{
		switch (inputError)
		{
		case vr::VRInputError_None:
			return std::string("VRInputError_None");
			break;
		case vr::VRInputError_NameNotFound:
			return std::string("VRInputError_NameNotFound");
			break;
		case vr::VRInputError_WrongType:
			return std::string("VRInputError_WrongType");
			break;
		case vr::VRInputError_InvalidHandle:
			return std::string("VRInputError_InvalidHandle");
			break;
		case vr::VRInputError_InvalidParam:
			return std::string("VRInputError_InvalidParam");
			break;
		case vr::VRInputError_NoSteam:
			return std::string("VRInputError_NoSteam");
			break;
		case vr::VRInputError_MaxCapacityReached:
			return std::string("VRInputError_MaxCapacityReached");
			break;
		case vr::VRInputError_IPCError:
			return std::string("VRInputError_IPCError");
			break;
		case vr::VRInputError_NoActiveActionSet:
			return std::string("VRInputError_NoActiveActionSet");
			break;
		case vr::VRInputError_InvalidDevice:
			return std::string("VRInputError_InvalidDevice");
			break;
		case vr::VRInputError_InvalidSkeleton:
			return std::string("VRInputError_InvalidSkeleton");
			break;
		case vr::VRInputError_InvalidBoneCount:
			return std::string("VRInputError_InvalidBoneCount");
			break;
		case vr::VRInputError_InvalidCompressedData:
			return std::string("VRInputError_InvalidCompressedData");
			break;
		case vr::VRInputError_NoData:
			return std::string("VRInputError_NoData");
			break;
		case vr::VRInputError_BufferTooSmall:
			return std::string("VRInputError_BufferTooSmall");
			break;
		case vr::VRInputError_MismatchedActionManifest:
			return std::string("VRInputError_MismatchedActionManifest");
			break;
		case vr::VRInputError_MissingSkeletonData:
			return std::string("VRInputError_MissingSkeletonData");
			break;
		case vr::VRInputError_InvalidBoneIndex:
			return std::string("VRInputError_InvalidBoneIndex");
			break;
		default:
			break;
		}
		return std::string("Invalid no error");
	}

	//---------------------------------------------------------------------------------------------------------------------
	// Purpose: Returns true if the action is active and its state is true
	//---------------------------------------------------------------------------------------------------------------------
	bool GetDigitalActionState(vr::VRActionHandle_t action, vr::VRInputValueHandle_t *pDevicePath)
	{
		vr::InputDigitalActionData_t actionData;
		vr::VRInput()->GetDigitalActionData(action, &actionData, sizeof(actionData), vr::k_ulInvalidInputValueHandle);
		if (pDevicePath)
		{
			*pDevicePath = vr::k_ulInvalidInputValueHandle;
			if (actionData.bActive)
			{
				vr::InputOriginInfo_t originInfo;
				if (vr::VRInputError_None == vr::VRInput()->GetOriginTrackedDeviceInfo(actionData.activeOrigin, &originInfo, sizeof(originInfo)))
				{
					*pDevicePath = originInfo.devicePath;
				}
			}
		}
		return actionData.bActive && actionData.bState;
	}

	//---------------------------------------------------------------------------------------------------------------------
	// Purpose: Returns true if the action is active and had a falling edge
	//---------------------------------------------------------------------------------------------------------------------
	bool GetDigitalActionFallingEdge(vr::VRActionHandle_t action, vr::VRInputValueHandle_t *pDevicePath)
	{
		vr::InputDigitalActionData_t actionData;
		vr::VRInput()->GetDigitalActionData(action, &actionData, sizeof(actionData), vr::k_ulInvalidInputValueHandle);
		if (pDevicePath)
		{
			*pDevicePath = vr::k_ulInvalidInputValueHandle;
			if (actionData.bActive)
			{
				vr::InputOriginInfo_t originInfo;
				if (vr::VRInputError_None == vr::VRInput()->GetOriginTrackedDeviceInfo(actionData.activeOrigin, &originInfo, sizeof(originInfo)))
				{
					*pDevicePath = originInfo.devicePath;
				}
			}
		}
		return actionData.bActive && actionData.bChanged && !actionData.bState;
	}

	//---------------------------------------------------------------------------------------------------------------------
	// Purpose: Returns true if the action is active and had a rising edge
	//---------------------------------------------------------------------------------------------------------------------
	bool GetDigitalActionRisingEdge(vr::VRActionHandle_t action, vr::VRInputValueHandle_t *pDevicePath)
	{
		vr::InputDigitalActionData_t actionData;
		vr::VRInput()->GetDigitalActionData(action, &actionData, sizeof(actionData), vr::k_ulInvalidInputValueHandle);
		if (pDevicePath)
		{
			*pDevicePath = vr::k_ulInvalidInputValueHandle;
			if (actionData.bActive)
			{
				vr::InputOriginInfo_t originInfo;
				if (vr::VRInputError_None == vr::VRInput()->GetOriginTrackedDeviceInfo(actionData.activeOrigin, &originInfo, sizeof(originInfo)))
				{
					*pDevicePath = originInfo.devicePath;
				}
			}
		}
		return actionData.bActive && actionData.bChanged && actionData.bState;
	}

}
