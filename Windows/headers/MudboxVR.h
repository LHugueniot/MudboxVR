#pragma once
#include "MBVRNode.h"

namespace mudbox {


	class MudboxVR
	{
		//Q_DECLARE_TR_FUNCTIONS(MudboxVR)

	public:
		MudboxVR();
		~MudboxVR();

		static void Initializer();
		static void startVR();
		static void endVR();

		static MBVRNode* MBVRNodePtr;
	};

}