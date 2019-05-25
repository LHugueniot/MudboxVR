#pragma once
#include "VRHead.h"

namespace mudbox {

	class VRViewport : public WindowPlugin
	{
	public:
		VRViewport(QString name, QString title);
		~VRViewport();
		virtual void Start();
		virtual void Stop();
		virtual bool IsEnabled();
		virtual QWidget *CreateWidget(QWidget *parent, QString name);
		virtual QWidget *Widget();
		virtual QString Name();
		virtual QString Title();
	protected:
		QWidget *qWidgetPtr;
		QString m_name;
		QString m_title;

		uint32_t m_nCompanionWindowWidth;
		uint32_t m_nCompanionWindowHeight;
	};

}



