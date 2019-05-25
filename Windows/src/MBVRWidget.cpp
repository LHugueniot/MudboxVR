//#include "MBVRWidget.h"
//#include "ui_mainwindow"
//
//
//MBVRWidget::MBVRWidget(QWidget *_parent) : QWidget ( _parent)
//{
//	this->resize(_parent->size());
//}
//
//
//MBVRWidget::~MBVRWidget()
//{
//}
//
//MainWindow::MainWindow(QWidget *parent) :
//	QMainWindow(parent),
//	m_ui(new Ui::MainWindow)
//{
//	m_ui->setupUi(this);
//	m_ui->setupUi(this);
//	m_gl = new GLWindow(this);
//	m_ui->s_mainWindowGridLayout->addWidget(m_gl, 0, 0, 3, 5);
//	connect(m_ui->m_rotating, SIGNAL(clicked(bool)), m_gl, SLOT(rotating(bool)));
//	connect(m_ui->generate, SIGNAL(clicked(bool)), m_gl, SLOT(generateNewGeometry()));
//}
//
//MainWindow::~MainWindow()
//{
//	delete m_ui;
//}