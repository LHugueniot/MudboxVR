#include "VRViewport.h"

namespace mudbox 
{

	//-----------------------------------------------------------------------------
	// VRViewport Stuff
	//-----------------------------------------------------------------------------

	VRViewport::VRViewport(QString name, QString title)
	{
		m_name = name;
		m_title = title;
	}

	VRViewport::~VRViewport()
	{
	}

	QWidget *VRViewport::CreateWidget(QWidget *parent, QString name)
	{
		qWidgetPtr =new QWidget();
		return qWidgetPtr;
	}


	QWidget *VRViewport::Widget()
	{
		return qWidgetPtr;
	}

	void VRViewport::Start()
	{
	}

	void VRViewport::Stop()
	{
	}

	bool VRViewport::IsEnabled()
	{
		return true;
	}

	QString VRViewport::Name()
	{
		return m_name;
	}

	QString VRViewport::Title()
	{
		return m_title;
	}

}

