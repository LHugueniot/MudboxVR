#ifndef MBVR_GLOBAL_H
#define MBVR_GLOBAL_H

#endif // MBVR_GLOBAL_H

#define TESTING 1

//-------------------------Mudbox Includes------------------------
#if defined(JAMBUILD)
#include <Mudbox/mudbox.h>
#else
#include <math.h>
#include <Mudbox/mudbox.h>
#include <Mudbox/MudBoxGL.h>
#endif
#include <QObject>
#include <QtGui/qwidget.h>
#include <QTimer>
#include <QtGui/QColor>
#include <QtOpenGL/QtOpenGL>

//-------------------------openvr Includes------------------------

#include <openvr.h>
#include "Matrices.h"

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


	//----------------------VR Functions------------------------

	Matrix ConvertSteamVRMatrixToMatrix(const vr::HmdMatrix34_t &matPose);



	std::string GetTrackedDeviceString(vr::TrackedDeviceIndex_t unDevice, 
									   vr::TrackedDeviceProperty prop,
									   vr::TrackedPropertyError *peError);

	std::string getCompositorErrorAsString(vr::EVRCompositorError _cError);

	

	struct FrameBufferDescriptor
	{
		GLuint m_DepthBufferID;

		GLuint m_RenderTextureID;
		GLuint m_ResolveTextureID;

		GLuint m_RenderFrameBufferID;
		GLuint m_ResolveFrameBufferID;
			
	};


#define DEBUG(msg) do if (TESTING == 1){ logMud(msg); }while(0)
#define PRINT(msg) do if (TESTING == 1){ printMud(msg); }while(0)
#define BREAKPOINT() do if (TESTING == 1){  breakPoint(__LINE__, __FUNCTION__); }while(0)
#define GLDEBUG() do if (TESTING == 1){  GLDebug(__LINE__, __FUNCTION__); }while(0)
#define VRDEBUG(ceError) do if (TESTING == 1){ DEBUG(getCompositorErrorAsString(ceError)); }while(0)
#define STRING(s) #s
}