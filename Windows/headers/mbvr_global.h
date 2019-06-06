#ifndef MBVR_GLOBAL_H
#define MBVR_GLOBAL_H

#endif // MBVR_GLOBAL_H

#define TESTING 1

//-------------------------Mudbox Includes------------------------

#include <math.h>
#include <Mudbox/mudbox.h>
#include <Mudbox/MudBoxGL.h>
#include <QObject>
#include <QTimer>
#include <QtGui/QColor>
#include <QtOpenGL/QtOpenGL>
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLShaderProgram>
#include <QtOpenGL/QGLBuffer>
#include <QGenericMatrix>

//#include <QGLWidget>
//#include <QGLShaderProgram>
//#include <QGLBuffer>

//-------------------------openvr Includes------------------------

#include <openvr.h>

//-------------------------standard lib includes------------------

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>



	//-------------------------------------------------------------
	//----------------------				-----------------------
	//-------------------------------------------------------------

namespace mudbox {

	//-------------------------------------------------------------
	//--					Helper Functions					 --
	//-------------------------------------------------------------

	//----------------------Debugging Functions--------------------

	void breakPoint(uint breakPointIndex, std::string funcName);

	template <typename T>
	void printMud(T msg)
	{
		Kernel()->Interface()->SetStatus(mudbox::Interface::stWarning, QVariant(msg).toString());
		std::cout << msg << "\n";
	}

	void printMud(std::string msg);

	template <typename T>
	void logMud(T msg)
	{
		Kernel()->Log(QVariant(msg).toString() + "\n");
	}	

	void logMud(std::string msg);

	void logMatrix(Matrix _matrix, std::string _nameOfMatrix);

	void GLDebug(uint breakPointIndex, std::string funcName);

#define DEBUG(msg) do if (TESTING == 1){ logMud(msg); }while(0)
#define PRINT(msg) do if (TESTING == 1){ printMud(msg); }while(0)
#define BREAKPOINT() do if (TESTING == 1){  breakPoint(__LINE__, __FUNCTION__); }while(0)
#define GLDEBUG() do if (TESTING == 1){  GLDebug(__LINE__, __FUNCTION__); }while(0)
#define VR_COMPOSITOR_DEBUG(compError) do if (TESTING == 1){ DEBUG(GetVRCompositorErrorAsString(compError)); }while(0)
#define VR_INPUT_DEBUG(inputError) do if (TESTING == 1){ DEBUG(GetVRInputErrorAsString(inputError)); }while(0)
#define STRING(s) #s

	//----------------------VR Functions------------------------

	Matrix ConvertSteamVRMatrixToMatrix(const vr::HmdMatrix34_t &matPose);

	QMatrix4x4 MBtoQMatrix(Matrix _Matrix);

	QMatrix3x3 Qmat4to3(QMatrix4x4 _Matrix);

	QMatrix3x3 Inverse(QMatrix3x3 _Matrix);

	std::string ReadFromFile(QString filePath);

	std::string GetTrackedDeviceString(vr::TrackedDeviceIndex_t unDevice, 
									   vr::TrackedDeviceProperty prop,
									   vr::TrackedPropertyError *peError);

	std::string GetVRCompositorErrorAsString(vr::EVRCompositorError compError);

	std::string GetVRInputErrorAsString(vr::EVRInputError inputError);

	bool GetDigitalActionState(vr::VRActionHandle_t action, vr::VRInputValueHandle_t *pDevicePath = nullptr);

	bool GetDigitalActionFallingEdge(vr::VRActionHandle_t action, vr::VRInputValueHandle_t *pDevicePath = nullptr);

	bool GetDigitalActionRisingEdge(vr::VRActionHandle_t action, vr::VRInputValueHandle_t *pDevicePath = nullptr);

	//World Space Adjustment Matrix

	extern Matrix WorldSpaceAdjuster =
	{
		100.f, 0.0f, 0.0f, 0.0f,
		0.0f, 100.f, 0.0f, 0.0f,
		0.0f, 0.0f, 100.f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	//Controller Info struct and array

	struct ControllerInfo_t
	{
		vr::VRInputValueHandle_t m_source = vr::k_ulInvalidInputValueHandle;
		vr::VRActionHandle_t m_actionPose = vr::k_ulInvalidActionHandle;
		vr::VRActionHandle_t m_actionHaptic = vr::k_ulInvalidActionHandle;
		Matrix m_rmat4Pose;

		std::string m_sRenderModelName;
		bool m_bShowController;
	};

	enum EHand
	{
		Left = 0,
		Right = 1
	};

}