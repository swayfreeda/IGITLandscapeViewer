#include "mainwindow.h"

//qCC includes
#include "ccCommon.h"
#include "ccConsole.h"



//qCC io
#include "BinFilter.h"

//qCC_db
#include <ccGenericPointCloud.h>
#include <ccCameraSensor.h>

//db_tree
#include<ccDBRoot.h>

//dialogs
#include "ccCamSensorProjectionDlg.h"
#include "PointCloudGenDlg.h"
#include "ccCameraParamEditDlg.h"
#include "ccOverlayDialog.h"

//Qt Includes
#include <QtGui>
#include <QMdiArea>
#include <QSignalMapper>
#include <QMdiSubWindow>
#include <QLCDNumber>
#include <QFileDialog>
#include <QActionGroup>
#include <QSettings>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QInputDialog>
#include <QTextStream>
#include <QColorDialog>


//System
#include <string.h>
#include <math.h>
#include <assert.h>
#include <cfloat>
#include <iostream>


//==========================global variables===================================//
//global static pointer (as there shoule only be one instance of MainWidow)
static MainWindow * s_instance=0;

//default 'All files' file filter
static const QString s_allFilesFilter("All (*.*)");

//default file filter separator
static const QString s_fileFilterSeparator(";;");



//=======================================MainWindow===========================================//
MainWindow::MainWindow()
	:m_ccRoot(0)
    ,m_viewModePopupButton(0)
    ,m_pivotVisibilityPopupButton(0)
    ,m_cpeDlg(0){
	
	setupUi(this);

	QSettings settings;
	restoreGeometry(settings.value(ccPS::MainWinGeom()).toByteArray());

	//setWindowTitle(QString("IGITLandscapeViewer v") + ccCommon::GetCCVersion(false));
	setWindowTitle(QString("IGIT地形地物可视化 版本") + ccCommon::GetCCVersion(false));
	
	// console // gloabal variables
	ccConsole::Init(consoleWidget, this, this);

	// db-tree link
	m_ccRoot = new ccDBRoot(dbTreeView, propertiesTreeView, this);
	connect(m_ccRoot, SIGNAL(selectionChanged()), this, SLOT(updateUIWithSelection()));
	
	//MDI Area
	m_mdiArea = new QMdiArea(this);
	// 设置成中心部件
	setCentralWidget(m_mdiArea);
	// 一旦有窗口被激活，更新该激活窗口的菜单 // 一旦有窗口被激活，同时激活该窗口三维视角的
	connect(m_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
	connect(m_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(on3DViewActivated(QMdiSubWindow*)));

	// QSignalMapper
	m_windowMapper = new QSignalMapper(this);
    connect(m_windowMapper, SIGNAL(mapped(QWidget*)),  this,  SLOT(setActiveSubWindow(QWidget*)));

	//advanced widgets not handled by QDesigner
	{  
		//view mode pop-up menu
		{ 
			// create menu
	       m_viewModePopupButton = new QToolButton();
	       QMenu *menu = new QMenu(m_viewModePopupButton);
		   menu->addAction(actionSetOrthoView);
		   menu->addAction(actionSetCenteredPerspectiveView);
		   menu->addAction(actionSetViewerPerspectiveView);

		   // create Popup Button
		   m_viewModePopupButton->setMenu(menu);
		   m_viewModePopupButton->setPopupMode(QToolButton::InstantPopup);
		   m_viewModePopupButton->setToolTip("Set current Veiw Mode");
		   m_viewModePopupButton->setStatusTip(m_viewModePopupButton->toolTip());

		   // insert m_viewModePopButton before actionSetViewTop
		   toolBarView->insertWidget(actionSetViewTop, m_viewModePopupButton);
		   m_viewModePopupButton->setEnabled(false);
		}

		 //pivot center pop-up menu
		{
			// create menu
		   m_pivotVisibilityPopupButton = new QToolButton();
		   QMenu * menu = new QMenu(m_pivotVisibilityPopupButton);
		   menu->addAction(actionSetPivotAlwaysOn);
		   menu->addAction(actionSetPivotRotationOnly);
		   menu->addAction(actionSetPivotOff);

		   m_pivotVisibilityPopupButton->setMenu(menu);
		   m_pivotVisibilityPopupButton->setPopupMode(QToolButton::InstantPopup);
		   m_pivotVisibilityPopupButton->setToolTip("Set Pivot Visibility");
		   m_pivotVisibilityPopupButton->setStatusTip(m_pivotVisibilityPopupButton->toolTip());
		   toolBarView->insertWidget(actionSetViewTop, m_pivotVisibilityPopupButton);
		   m_pivotVisibilityPopupButton->setEnabled(false);
		}

	}

	connectActions();

	// create new 3D view
	new3DView();

	// freeze all widgets
	freezeUI(false);


	updateMenus();

	updateUIWithSelection();

	//maximize
	showMaximized();


	//QMainWindow::statusBar()->showMessage(QString("Ready"));
	//ccConsole::Print("IGITLandscapeVeiwer started!");
	QMainWindow::statusBar()->showMessage(QString("准备完毕！"));
	ccConsole::Print("IGITLandscapeViewer 启动!");

}

//! Connects all QT actions to slots
void MainWindow::connectActions(){
	assert(m_ccRoot);
	assert(m_mdiArea);

	//"File" menu
	connect(actionOpen,               SIGNAL(triggered()),         this,           SLOT(doActionLoadFile()));
	connect(actionSave,               SIGNAL(triggered()),         this,           SLOT(doActionSaveFile()));

	//"Edit" menu
	connect(actionCreateCameraSensor,			SIGNAL(triggered()),	this,		SLOT(doActionCreateCameraSensor()));
	connect(actionCreateCameraSensorFromFile,   SIGNAL(triggered()),    this,       SLOT(doActionCreateCameraSensorFromFile()));
	connect(actionTextureGeneration,         SIGNAL(triggered()),    this,       SLOT(doActionTextureGeneration()));
	

	//"Display"  menu
	connect(actionLockRotationVertAxis,			SIGNAL(triggered()),	this,		SLOT(toggleRotationAboutVertAxis()));
	connect(actionEditCamera,					SIGNAL(triggered()),	this,		SLOT(doActionEditCamera()));



	//"3D Views"
	connect(menu3DViews,              SIGNAL(aboutToShow()),      this,           SLOT(update3DViewsMenu()));
	connect(actionNew3DView,          SIGNAL(triggered()),        this,           SLOT(new3DView()));
	connect(actionClose3DView,        SIGNAL(triggered()),        m_mdiArea,      SLOT(closeActiveSubWindow()));
	connect(actionCloseAll3DViews,    SIGNAL(triggered()),        m_mdiArea,      SLOT(closeAllSubWindows()));
	connect(actionTile3DViews,        SIGNAL(triggered()),        m_mdiArea,      SLOT(tileSubWindows()));
	connect(actionCascade3DViews,     SIGNAL(triggered()),        m_mdiArea,      SLOT(cascadeSubWindows()));
	connect(actionNext3DView,         SIGNAL(triggered()),        m_mdiArea,      SLOT(activateNextSubWindow()));
	connect(actionPrevious3DView,     SIGNAL(triggered()),        m_mdiArea,      SLOT(activatePreviousSubWindow()));

	// View Tool bar
	connect(actionSetPivotAlwaysOn,				SIGNAL(triggered()),	this,		SLOT(setPivotAlwaysOn()));
	connect(actionSetPivotRotationOnly,			SIGNAL(triggered()),	this,		SLOT(setPivotRotationOnly()));
	connect(actionSetPivotOff,					SIGNAL(triggered()),	this,		SLOT(setPivotOff()));
	connect(actionSetOrthoView,					SIGNAL(triggered()),	this,		SLOT(setOrthoView()));
	connect(actionSetCenteredPerspectiveView,	SIGNAL(triggered()),	this,		SLOT(setCenteredPerspectiveView()));
	connect(actionSetViewerPerspectiveView,		SIGNAL(triggered()),	this,		SLOT(setViewerPerspectiveView()));

	connect(actionSetViewTop,					SIGNAL(triggered()),	this,		SLOT(setTopView()));
	connect(actionSetViewBottom,				SIGNAL(triggered()),	this,		SLOT(setBottomView()));
	connect(actionSetViewFront,					SIGNAL(triggered()),	this,		SLOT(setFrontView()));
	connect(actionSetViewBack,					SIGNAL(triggered()),	this,		SLOT(setBackView()));
	connect(actionSetViewLeft,					SIGNAL(triggered()),	this,		SLOT(setLeftView()));
	connect(actionSetViewRight,					SIGNAL(triggered()),	this,		SLOT(setRightView()));
	connect(actionSetViewIso1,					SIGNAL(triggered()),	this,		SLOT(setIsoView1()));
	connect(actionSetViewIso2,					SIGNAL(triggered()),	this,		SLOT(setIsoView2()));

}

//=======================================setActiveSubWindow====================================//
void MainWindow::setActiveSubWindow(QWidget* window){
	if(!window|| !m_mdiArea)return;

	m_mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(window));
}


//========================================TheInstance=========================================//
MainWindow* MainWindow::TheInstance(){
	if(!s_instance){
		s_instance = new MainWindow();
	}
		return s_instance;

}

//==========================================GetActiveGLWindow====================================//
ccGLWindow * MainWindow::GetActiveGLWindow(){
	return TheInstance()->getActiveGLWindow();
}

//==========================================getActiveGLWindow=====================================//
ccGLWindow * MainWindow:: getActiveGLWindow(){
	if(!m_mdiArea){
		return 0;
	}

	// if active sub window existed
	QMdiSubWindow * activeSubWindow = m_mdiArea->activeSubWindow();
    if(activeSubWindow){
		return static_cast<ccGLWindow*>(activeSubWindow->widget());
	}
	// if active sub window does not existed, return the first  sub window
    else{
		QList<QMdiSubWindow*> subWindowList = m_mdiArea->subWindowList();
		if(!subWindowList.empty()){
			return static_cast<ccGLWindow*>(subWindowList[0]->widget());
		}
	}
	return 0;
}

//========================================GetGLWindow============================================//
ccGLWindow* MainWindow:: GetGLWindow(const QString & title){

	// collect the pointers of all sub windows
	QList<QMdiSubWindow*> windows = TheInstance()->m_mdiArea->subWindowList();
	int winNum = windows.size();
	if(winNum ==0){
		return 0;
	}
	// find the sub window with the given title
	for(int i=0; i< winNum; i++){
		ccGLWindow * win = static_cast<ccGLWindow*> (windows.at(i)->widget());
		if(win->windowTitle()== title){
			return win;
		}
	}
	return 0;
}

//==========================================GetGLWindow============================================//
void MainWindow::GetGLWindows(std::vector<ccGLWindow*> & glWindows){
	QList<QMdiSubWindow*> windows = TheInstance()->m_mdiArea->subWindowList();
	int winNum = windows.size();

	if(winNum==0) return;

	glWindows.clear();
	glWindows.reserve(winNum);

	for(int i=0; i<winNum; i++){
		glWindows.push_back(static_cast<ccGLWindow*>(windows.at(i)->widget()));
	}
}

//============================================RefreshAllGLWindow=======================================//
void MainWindow::RefreshAllGLWindow(bool only2D /*false*/){
	TheInstance()->refreshAll(only2D);
}

//=============================================refreshAll===============================================//
void MainWindow::refreshAll(bool only2D ){
	QList<QMdiSubWindow*> windows = m_mdiArea->subWindowList();
	for(int i=0; i<windows.size(); i++){
		static_cast<ccGLWindow*>(windows.at(i)->widget())->refresh(only2D);
	}
}

//=============================================DestroyInstance===========================================//
void MainWindow:: DestroyInstance(){
	if(s_instance){
		delete s_instance;
	}
	s_instance = 0;
}

//==============================================UpdateUI==============================//
void MainWindow::UpdateUI(){
	TheInstance()->updateUI();
}

//==============================================updateUI==============================//
void MainWindow::updateUI(){

	updateUIWithSelection();
	updateMenus();
	if(m_ccRoot){
		m_ccRoot->updatePropertiesView();
	}
}

//=====================================updateUIWithSelection===========================//
void MainWindow::updateUIWithSelection(){
	
	dbTreeSelectionInfo selInfo;
	m_selectedEntities.clear();

	if (m_ccRoot)
		m_ccRoot->getSelectedEntities(m_selectedEntities,CC_TYPES::OBJECT,&selInfo);
	//expandDBTreeWithSelection(m_selectedEntities);
	enableUIItems(selInfo);
}

//====================================a bit complex ===================================//
void MainWindow::enableUIItems(dbTreeSelectionInfo& selInfo){
	
	bool atLeastOneEntity = (selInfo.selCount > 0);
	bool atLeastOneCloud = (selInfo.cloudCount > 0);
	bool atLeastOneMesh = (selInfo.meshCount > 0);

	//bool atLeastOneOctree = (selInfo.octreeCount > 0);
	bool atLeastOneNormal = (selInfo.normalsCount > 0);
	bool atLeastOneColor = (selInfo.colorCount > 0);
	bool atLeastOneSF = (selInfo.sfCount > 0);

	//bool atLeastOneSensor = (selInfo.sensorCount > 0);
	bool atLeastOneCameraSensor = (selInfo.cameraSensorCount > 0);
	bool atLeastOnePolyline = (selInfo.polylineCount > 0);
	bool activeWindow = (getActiveGLWindow() != 0);

	actionSave->setEnabled(atLeastOneEntity);
	
	// == 1
	bool exactlyOneEntity = (selInfo.selCount == 1);
	bool exactlyOneGroup = (selInfo.groupCount == 1);
	bool exactlyOneCloud = (selInfo.cloudCount == 1);
	bool exactlyOneMesh = (selInfo.meshCount == 1);
	bool exactlyOneSF = (selInfo.sfCount == 1);
	bool exactlyOneSensor = (selInfo.sensorCount == 1);
	bool exactlyOneCameraSensor = (selInfo.cameraSensorCount == 1);
	actionCreateCameraSensor->setEnabled(atLeastOneCloud);
	actionCheckPointsInsideFrustrum->setEnabled(exactlyOneCameraSensor);
}

//===================================ApplyCCLibAlgorthim===============================//
//! Applies a standard CCLib algorithm (see CC_LIB_ALGORITHM) on a set of entities
bool MainWindow::ApplyCCLibAlgorithm(CC_LIB_ALGORITHM algo,
									ccHObject::Container& entities,
									QWidget* parent,
									void** additionalParameters){

    return true;
}

//===================================AddToDB========================================//
void MainWindow::addToDB(const QStringList& filenames,
							QString fileFilter,
							ccGLWindow* destWin){
   //to handle same 'shift on load' for multiple files
	CCVector3d loadCoordinatesShift(0,0,0);
	bool loadCoordinatesTransEnabled = false;
	
	FileIOFilter::LoadParameters parameters;
	parameters.alwaysDisplayLoadDialog = false; //显示对话框
	parameters.shiftHandlingMode = ccGlobalShiftManager::DIALOG_IF_NECESSARY;
	parameters.coordinatesShift = &loadCoordinatesShift;
	parameters.coordinatesShiftEnabled = &loadCoordinatesTransEnabled;

	//the same for 'addToDB' (if the first one is not supported, or if the scale remains too big)
	CCVector3d addCoordinatesShift(0,0,0);

	for (int i=0; i<filenames.size(); ++i){
		ccHObject* newGroup = FileIOFilter::LoadFromFile(filenames[i],parameters,fileFilter);
		if (newGroup){
			if (destWin)
				newGroup->setDisplay_recursive(destWin);
			addToDB(newGroup,true,true,false);
		}
	}

	//QMainWindow::statusBar()->showMessage(QString("%1 file(s) loaded").arg(filenames.size()),2000);
	QMainWindow::statusBar()->showMessage(QString("%1 file(s) 加载完成").arg(filenames.size()),2000);
	
}

//==================================addToDB===========================================//
void MainWindow::addToDB(ccHObject* obj, bool updateZoom, 
	                                     bool autoExpandDBTree, 
										 bool checkDimensions){
  
	//let's check that the new entity is not too big nor too far from scene center!
	if (checkDimensions){
		//get entity bounding box
		ccBBox bBox = obj->getBB_recursive();

		CCVector3 center = bBox.getCenter();
		PointCoordinateType diag = bBox.getDiagNorm();

		CCVector3d P = CCVector3d::fromArray(center.u);
		CCVector3d Pshift(0,0,0);
		double scale = 1.0;
		//here we must test that coordinates are not too big whatever the case because OpenGL
		//really doesn't like big ones (even if we work with GLdoubles :( ).
		if (ccGlobalShiftManager::Handle(P,diag,ccGlobalShiftManager::DIALOG_IF_NECESSARY,false,Pshift,&scale))	{

			bool needRescale = (scale != 1.0);
			bool needShift = (Pshift.norm2() > 0);

			if (needRescale || needShift){

				ccGLMatrix mat;
				mat.toIdentity();
				mat.data()[0] = mat.data()[5] = mat.data()[10] = static_cast<float>(scale);
				mat.setTranslation(Pshift);
				obj->applyGLTransformation_recursive(&mat);
				//ccConsole::Warning(QString("Entity '%1' has been translated: (%2,%3,%4) and rescaled of a factor %5 [original position will be restored when saving]").arg(obj->getName()).arg(Pshift.x,0,'f',2).arg(Pshift.y,0,'f',2).arg(Pshift.z,0,'f',2).arg(scale,0,'f',6));
			    ccConsole::Warning(QString("物体 '%1' 被平移: (%2,%3,%4) 被缩放： %5 [存储时将会恢复原始坐标]").arg(obj->getName()).arg(Pshift.x,0,'f',2).arg(Pshift.y,0,'f',2).arg(Pshift.z,0,'f',2).arg(scale,0,'f',6));
			}

			//update 'global shift' and 'global scale' for ALL clouds recursively
			//FIXME: why don't we do that all the time by the way?!
			ccHObject::Container children;
			children.push_back(obj);
			while (!children.empty()){
				ccHObject* child = children.back();
				children.pop_back();

				if (child->isKindOf(CC_TYPES::POINT_CLOUD))	{
					ccGenericPointCloud* pc = ccHObjectCaster::ToGenericPointCloud(child);
					pc->setGlobalShift(pc->getGlobalShift() + Pshift);
					pc->setGlobalScale(pc->getGlobalScale() * scale);
				}

				for (unsigned i=0; i<child->getChildrenNumber(); ++i)
					children.push_back(child->getChild(i));
			}
		}
	}

	//add object to DB root
	if (m_ccRoot){
		//force a 'global zoom' if the DB was emtpy!
		if (!m_ccRoot->getRootEntity() || m_ccRoot->getRootEntity()->getChildrenNumber() == 0)
			updateZoom = true;
		m_ccRoot->addElement(obj,autoExpandDBTree);
	}
	else{
		//ccLog::Warning("[MainWindow::addToDB] Internal error: no associated db?!");
		ccLog::Warning("[MainWindow::addToDB] 内部错误: 没有相关的 db?!");
		assert(false);
	}

	//we can now set destination display (if none already)
	if (!obj->getDisplay()){
		ccGLWindow* activeWin = getActiveGLWindow();
		if (!activeWin){
			//no active GL window?!
			return;
		}
		obj->setDisplay_recursive(activeWin);
	}

	//eventually we update the corresponding display
	assert(obj->getDisplay());
	if (updateZoom){
		static_cast<ccGLWindow*>(obj->getDisplay())->zoomGlobal(); //automatically calls ccGLWindow::redraw
	}
	else{
		obj->prepareDisplayForRefresh();
		refreshAll();
	}

}


//=================================loadTexturedResults================================//
void MainWindow::loadTexturedResults(QString ResultsDir){
     QDir dir(ResultsDir);
	 if(!dir.exists()){
		 return;
	 }
	 dir.setFilter(QDir::Files | QDir::NoSymLinks);
	 QStringList filters; 
     filters<<QString("*.OBJ")<<QString("*.obj");
	 dir.setNameFilters(filters);

	 QFileInfoList nameList = dir.entryInfoList();
	 if(nameList.size()==0) return;


	 //to handle same 'shift on load' for multiple files
	CCVector3d loadCoordinatesShift(0,0,0);
	bool loadCoordinatesTransEnabled = false;
	FileIOFilter::LoadParameters parameters;
	parameters.alwaysDisplayLoadDialog = false; //显示对话框
	parameters.shiftHandlingMode = ccGlobalShiftManager::DIALOG_IF_NECESSARY;
	parameters.coordinatesShift = &loadCoordinatesShift;
	parameters.coordinatesShiftEnabled = &loadCoordinatesTransEnabled;

	 QString currentOpenDlgFilter;
	 currentOpenDlgFilter.clear();
     for (int i = 0; i < nameList.size(); ++i) {
         QFileInfo fileInfo = nameList.at(i);
		 QString fileDir = fileInfo.absoluteFilePath();

		 ccHObject* group = FileIOFilter::LoadFromFile(fileDir,parameters,  currentOpenDlgFilter);
		 if(!group|| group->getChildrenNumber()==0) return;
		 ccHObject * Mesh = group->getFirstChild();
		 if(!Mesh)return;

		 ccHObject * MeshGroup = m_ccRoot->getRootEntity()->find("MeshList");
		 if(!MeshGroup){
		   MeshGroup = new ccHObject();
	       addToDB(MeshGroup);
	       MeshGroup->setName("MeshList");
		}

		 MeshGroup->addChild(Mesh);
		 ccGLWindow* win = getActiveGLWindow();
		 if (win){
			 //setDisplay(win);
             //Mesh->setVisible(true);

			 Mesh->setDisplay_recursive(win);
			addToDB(Mesh,true,true,false);
		 }
		 //addToDB(Mesh);
     }
	 QMainWindow::statusBar()->showMessage(QString("%1 file(s) loaded").arg(nameList.size()),2000);
}

//=================================loadPMVSCameras====================================//
void MainWindow::loadPMVSCameras(QString ResultsDir){
	
	QDir dir(ResultsDir);
	if(!dir.exists()){
		 return;
	 }
	 dir.setFilter(QDir::Files | QDir::NoSymLinks);
	 QStringList filters; 
     filters<<QString("*.TXT")<<QString("*.txt");
	 dir.setNameFilters(filters);

	 QFileInfoList nameList = dir.entryInfoList();
	 if(nameList.size()==0) return;

	 CCVector3d loadCoordinatesShift(0,0,0);
	bool loadCoordinatesTransEnabled = false;
	FileIOFilter::LoadParameters parameters;
	parameters.alwaysDisplayLoadDialog = false; //显示对话框
	parameters.shiftHandlingMode = ccGlobalShiftManager::DIALOG_IF_NECESSARY;
	parameters.coordinatesShift = &loadCoordinatesShift;
	parameters.coordinatesShiftEnabled = &loadCoordinatesTransEnabled;

	 QString currentOpenDlgFilter;
	 currentOpenDlgFilter.clear();
	for (int i=0; i<nameList.size(); ++i){
	   QFileInfo fileInfo = nameList.at(i);
		QString fileDir = fileInfo.absoluteFilePath();
		ccHObject* group = FileIOFilter::LoadFromFile(fileDir,parameters, currentOpenDlgFilter);
		if(!group|| group->getChildrenNumber()==0) return;
		ccHObject * sensor = group->getFirstChild();
		if(!sensor)return;


		ccHObject * CameraGroup = m_ccRoot->getRootEntity()->find("CameraLists");
			if(!CameraGroup){
			   CameraGroup = new ccHObject();
			   addToDB(CameraGroup);
			   CameraGroup->setName("CameraLists");
			}
			CameraGroup->addChild(sensor);
			ccGLWindow* win = getActiveGLWindow();
			if (win){
				sensor->setDisplay(win);
				sensor->setVisible(true);
			}
			addToDB(sensor);
	}
	//QMainWindow::statusBar()->showMessage(QString("%1 file(s) loaded").arg(filenames.size()),2000);
	QMainWindow::statusBar()->showMessage(QString("%1 file(s) 加载完成").arg(nameList.size()),2000);
}

//===================================echoMouseWheelRotate============================//
void MainWindow::echoMouseWheelRotate(float angle){
	if(checkBoxCameraLink->checkState()!= Qt::Checked) return;

	// find which window sends the signal // dynamic_cast 会进行安全检察
	ccGLWindow* sendingWindow = dynamic_cast<ccGLWindow*>(sender());
	if(!sendingWindow) return;

	QList<QMdiSubWindow*> windows = m_mdiArea->subWindowList();
	for(int i=0; i< windows.size(); i++){
		ccGLWindow *child = static_cast<ccGLWindow*>(windows.at(i)->widget());
		//find the other ccGLwindows, the rotations are performed on them// the sending windows has been
		// rotated, so it should not be processed agained
		if(child!=sendingWindow){
			// can not receive other signals
			child->blockSignals(true);
			//对窗口进行尺度变化
			child->onWheelEvent(angle);
			child->blockSignals(true);
			child->redraw();
		}
	}
}

//===================================echoCameraDisplay==============================//
void MainWindow::echoCameraDisplaced(float ddx, float ddy){
	if(checkBoxCameraLink->checkState()!= Qt::Checked)return;

	// find the window that sends the signal
	ccGLWindow* sendingWindow = dynamic_cast<ccGLWindow*>(sender());
	if(!sendingWindow)return;

	QList<QMdiSubWindow*> windows = m_mdiArea->subWindowList();
	for(int i=0; i<windows.size(); i++){
		ccGLWindow * child = static_cast<ccGLWindow*>(windows.at(i)->widget());
		if(child!= sendingWindow){
			child->blockSignals(true);
			child->moveCamera(ddx,ddy, 0.0f);
			child->blockSignals(false);
			child->redraw();
		}
	}
}

//===================================echoBaseViewMatRotation=========================//
void MainWindow::echoBaseViewMatRotation(const ccGLMatrixd& rotMat){
	if(checkBoxCameraLink->checkState()!= Qt::Checked) return;

	ccGLWindow *sendingWindow = dynamic_cast<ccGLWindow*>(sender());
	if(!sendingWindow)return;

	QList<QMdiSubWindow*> windows = m_mdiArea->subWindowList();
	for(int i=0; i< windows.size(); i++){
		ccGLWindow * child = static_cast<ccGLWindow*>(windows.at(i)->widget());
		if(child!=sendingWindow){
			child->blockSignals(true);
			// 对窗口进行旋转
			child->rotateBaseViewMat(rotMat);
			child->blockSignals(false);
			child->redraw();
		}
	}
}

//===================================echoCameraPosChanged===========================//
void MainWindow::echoCameraPosChanged(const CCVector3d& P){
	if(checkBoxCameraLink->checkState() != Qt::Checked) return;

	ccGLWindow * sendingWindow = dynamic_cast<ccGLWindow*>(sender());
	if(!sendingWindow) return;

	QList<QMdiSubWindow*> windows = m_mdiArea->subWindowList();
	for(int i=0; i< windows.size(); i++){
		ccGLWindow * child = static_cast<ccGLWindow*>(windows.at(i)->widget());
		if(child!=sendingWindow){
			child->blockSignals(true);
			child->setCameraPos(P);
			child->blockSignals(false);
			child->redraw();
		}
	}
}

//===================================echoPivotPointChanged==========================//
void MainWindow::echoPivotPointChanged(const CCVector3d&P){
	if(checkBoxCameraLink->checkState() != Qt::Checked) return;

	ccGLWindow * sendingWindow = dynamic_cast<ccGLWindow*>(sender());
	if(!sendingWindow) return;

	QList<QMdiSubWindow*> windows = m_mdiArea->subWindowList();
	for(int i=0; i< windows.size(); i++){
		ccGLWindow * child = static_cast<ccGLWindow*>(windows.at(i)->widget());
		if(child!= sendingWindow){
			child->blockSignals(true);
		    child->setPivotPoint(P);
			child->blockSignals(false);
			child->redraw();
		}
	}
}

//===================================echoPixelSizeChanged===========================//
void MainWindow::echoPixelSizeChanged(float size){
	if(checkBoxCameraLink->state()!= Qt::Checked) return;

	ccGLWindow* sendingWindow = dynamic_cast<ccGLWindow*>(sender());
	if(!sendingWindow)return;

	QList<QMdiSubWindow*> windows = m_mdiArea->subWindowList();
	for(int i=0; i<windows.size(); i++){
		ccGLWindow* child = static_cast<ccGLWindow*>(windows.at(i)->widget());
		if(child!=sendingWindow){
			child->blockSignals(true);
			// 设置点的大小
			child->setPixelSize(size);
			child->blockSignals(false);
			child->redraw();
		}
	}

}

//===================================prepareWindowDeletion==========================//
void MainWindow::prepareWindowDeletion(QObject* glWindow){
}

//===================================addToDBAuto====================================//
void MainWindow::addToDBAuto(const QStringList& filenames){
}

//====================================handleNewLabel================================//
void MainWindow::handleNewLabel(ccHObject*){
}

//====================================doActionLoadFile=============================//
//QApplication::applicationDirPath()//当前编译exe文件的路径
void MainWindow::doActionLoadFile(){

	//persistent settings
	QSettings settings;
	settings.beginGroup(ccPS::LoadFile());
	QString currentPath = settings.value(ccPS::CurrentPath(),QApplication::applicationDirPath()).toString();
	QString currentOpenDlgFilter = settings.value(ccPS::SelectedInputFilter(), BinFilter::GetFileFilter()).toString();

	// Add all available file I/O filters (with import capabilities)
	QStringList fileFilters;
	fileFilters.append(s_allFilesFilter);
	bool defaultFilterFound = false;
	{
		const FileIOFilter::FilterContainer& filters = FileIOFilter::GetFilters();
		for (size_t i=0; i<filters.size(); ++i){
			if (filters[i]->importSupported()){
				QStringList ff = filters[i]->getFileFilters(true);
				for (int j=0; j<ff.size(); ++j){
					fileFilters.append(ff[j]);
					//is it the (last) default filter?
					if (!defaultFilterFound && currentOpenDlgFilter == ff[j]){
						defaultFilterFound = true;
					}
				}
			}
		}
	}

	//default filter is still valid?
	if (!defaultFilterFound)
		currentOpenDlgFilter = s_allFilesFilter;

	//file choosing dialog
	QStringList selectedFiles = QFileDialog::getOpenFileNames(	this,
																"Open file(s)",
																currentPath,
																fileFilters.join(s_fileFilterSeparator),
																&currentOpenDlgFilter
#ifdef _DEBUG
																,QFileDialog::DontUseNativeDialog
#endif
															);
	if (selectedFiles.isEmpty())
		return;

	//save last loading parameters
	currentPath = QFileInfo(selectedFiles[0]).absolutePath();
	settings.setValue(ccPS::CurrentPath(),currentPath);
	settings.setValue(ccPS::SelectedInputFilter(),currentOpenDlgFilter);
	settings.endGroup();

	if (currentOpenDlgFilter == s_allFilesFilter)
		currentOpenDlgFilter.clear(); //this way FileIOFilter will try to guess the file type automatically!

	//load files
	addToDB(selectedFiles,currentOpenDlgFilter);
}

//====================================doActionSaveFile=============================//
void  MainWindow::doActionSaveFile(){
}

//====================================doActionCreateCameraSensors===================//
void MainWindow::doActionCreateCameraSensor(){
	ccCamSensorProjectionDlg spDlg(this); // 设置相机参数
	if (!spDlg.exec())
		return;

	//We create the corresponding sensor for each input cloud
	ccHObject::Container selectedEntities = m_selectedEntities; //选择到的物体
	size_t selNum = selectedEntities.size(); //选择到的物体的个数

	for (size_t i=0; i<selNum; ++i){

		ccHObject* ent = selectedEntities[i];
		if (ent->isKindOf(CC_TYPES::POINT_CLOUD)){  
			//如果是点云 //将ccHobject 指针转化成 ccGenericPointCloud 指针
			ccGenericPointCloud* cloud = ccHObjectCaster::ToGenericPointCloud(ent);

			//we create a new sensor //创建相机传感器
			ccCameraSensor* sensor = new ccCameraSensor();

			 // 添加孩子传感器
			cloud->addChild(sensor);

			//we init its parameters with the dialog
			spDlg.updateCamSensor(sensor);

			//we try to guess the sensor relative size (dirty)
			ccBBox bb = cloud->getOwnBB();
			double diag = bb.getDiagNorm();
			if (diag < 1.0)
				sensor->setGraphicScale(static_cast<PointCoordinateType>(1.0e-3));
			else if (diag > 10000.0)
				sensor->setGraphicScale(static_cast<PointCoordinateType>(1.0e3));

			//set position
			ccIndexedTransformation trans;
			sensor->addPosition(trans,0); // 将当前位置添加到PosBuffer缓冲器中

			ccGLWindow* win = static_cast<ccGLWindow*>(cloud->getDisplay());

			if (win){
				sensor->setDisplay(win);
				sensor->setVisible(true);
				ccBBox box = cloud->getOwnBB();
				win->updateConstellationCenterAndZoom(&box);
			}
			addToDB(sensor);
		}
	}
	updateUI();
}

//====================================doActionCreateCameraSensors===================//
void MainWindow::doActionCreateCameraSensorFromFile(){

	//persistent settings
	QSettings settings;
	settings.beginGroup(ccPS::LoadFile());
	QString currentPath = settings.value(ccPS::CurrentPath(),QApplication::applicationDirPath()).toString();
	QString currentOpenDlgFilter = settings.value(ccPS::SelectedInputFilter(), BinFilter::GetFileFilter()).toString();

	// Add all available file I/O filters (with import capabilities)
	QStringList fileFilters;
	fileFilters.append(s_allFilesFilter);
	bool defaultFilterFound = false;
	{
		const FileIOFilter::FilterContainer& filters = FileIOFilter::GetFilters();
		for (size_t i=0; i<filters.size(); ++i){
			if (filters[i]->importSupported()){
				QStringList ff = filters[i]->getFileFilters(true);
				for (int j=0; j<ff.size(); ++j){
					fileFilters.append(ff[j]);
					//is it the (last) default filter?
					if (!defaultFilterFound && currentOpenDlgFilter == ff[j]){
						defaultFilterFound = true;
					}
				}
			}
		}
	}

	//default filter is still valid?
	if (!defaultFilterFound)
		currentOpenDlgFilter = s_allFilesFilter;

	//file choosing dialog
	QStringList selectedFiles = QFileDialog::getOpenFileNames(	this,
																"Open file(s)",
																currentPath,
																fileFilters.join(s_fileFilterSeparator),
																&currentOpenDlgFilter
#ifdef _DEBUG
																,QFileDialog::DontUseNativeDialog
#endif
															);
	if (selectedFiles.isEmpty())
		return;

	//save last loading parameters
	currentPath = QFileInfo(selectedFiles[0]).absolutePath();
	settings.setValue(ccPS::CurrentPath(),currentPath);
	settings.setValue(ccPS::SelectedInputFilter(),currentOpenDlgFilter);
	settings.endGroup();

	if (currentOpenDlgFilter == s_allFilesFilter)
		currentOpenDlgFilter.clear(); //this way FileIOFilter will try to guess the file type automatically!


	 //to handle same 'shift on load' for multiple files
	CCVector3d loadCoordinatesShift(0,0,0);
	bool loadCoordinatesTransEnabled = false;
	FileIOFilter::LoadParameters parameters;
	parameters.alwaysDisplayLoadDialog = false; //显示对话框
	parameters.shiftHandlingMode = ccGlobalShiftManager::DIALOG_IF_NECESSARY;
	parameters.coordinatesShift = &loadCoordinatesShift;
	parameters.coordinatesShiftEnabled = &loadCoordinatesTransEnabled;

	//the same for 'addToDB' (if the first one is not supported, or if the scale remains too big)
	CCVector3d addCoordinatesShift(0,0,0);

	for (int i=0; i<selectedFiles.size(); ++i){
		ccHObject* group = FileIOFilter::LoadFromFile(selectedFiles[i],parameters, currentOpenDlgFilter);
		if(!group|| group->getChildrenNumber()==0) return;
		ccHObject * sensor = group->getFirstChild();
		if(!sensor)return;


		ccHObject * CameraGroup = m_ccRoot->getRootEntity()->find("CameraLists");
			if(!CameraGroup){
			   CameraGroup = new ccHObject();
			   addToDB(CameraGroup);
			   CameraGroup->setName("CameraLists");
			}
			CameraGroup->addChild(sensor);
			ccGLWindow* win = getActiveGLWindow();
			if (win){
				sensor->setDisplay(win);
				sensor->setVisible(true);
			}
			addToDB(sensor);
	}
	//QMainWindow::statusBar()->showMessage(QString("%1 file(s) loaded").arg(filenames.size()),2000);
	QMainWindow::statusBar()->showMessage(QString("%1 file(s) 加载完成").arg(selectedFiles.size()),2000);
}

//====================================doActionPointCloudGeneration==================//
void MainWindow:: doActionTextureGeneration(){

	PointCloudGenDlg pcDlg;
	QString FileFolder;
	if(!pcDlg.exec()){
		if(QFile::exists("imgList.txt")){
	      QFile::remove("imgList.txt");
		}
		FileFolder = pcDlg.getFolderDir();
		QMainWindow::statusBar()->showMessage(FileFolder,2000);
	}
   
	loadTexturedResults(FileFolder);
	loadPMVSCameras("./TempData.nvm.cmvs/00/txt");
}

//====================================update3DViewsMenu============================//
void MainWindow::update3DViewsMenu(){
	menu3DViews->clear();
	menu3DViews->addAction(actionNew3DView);
	menu3DViews->addSeparator();
	menu3DViews->addAction(actionClose3DView);
	menu3DViews->addAction(actionCloseAll3DViews);
	menu3DViews->addSeparator();
	menu3DViews->addAction(actionTile3DViews);
	menu3DViews->addAction(actionCascade3DViews);
	menu3DViews->addSeparator();
	menu3DViews->addAction(actionNext3DView);
	menu3DViews->addAction(actionPrevious3DView);
	 
	QList<QMdiSubWindow*> windows = m_mdiArea->subWindowList();
	if(!windows.isEmpty()){

		//Dynamc Separator
		QAction* seperator = new QAction(this);
		seperator->setSeparator(true);
		menu3DViews->addAction(seperator);
		for(int i=0; i<windows.size(); i++){
			QWidget * child = windows.at(i)->widget();
			QString text = QString("&%1 %2").arg(i+1).arg(child->windowTitle());
			QAction * action = menu3DViews->addAction(text);
			// checkable
			action->setCheckable(true);
			action->setChecked(child == getActiveGLWindow());

			// using singnalmapper to maganize all the signals and slots
			connect(action, SIGNAL(triggered()),  m_windowMapper, SLOT(map()));
			m_windowMapper->setMapping(action, windows.at(i));
	   }
     }
}

//====================================new3DView====================================//
ccGLWindow* MainWindow::new3DView(){


	assert(m_ccRoot && m_mdiArea);
	// already existing window?
	QList<QMdiSubWindow*> subWindowList = m_mdiArea->subWindowList();
	ccGLWindow * otherWin = 0;
	if(!subWindowList.isEmpty()){
		otherWin = static_cast<ccGLWindow*>(subWindowList[0]->widget());
	}

	QGLFormat format = QGLFormat::defaultFormat();
	//format.setStencil(false);
	format.setSwapInterval(0);
	
	ccGLWindow * view3D = new ccGLWindow(this, format, otherWin);

	view3D->setMinimumSize(400, 300);
	view3D->resize(500, 400);
	m_mdiArea->addSubWindow(view3D);//添加子窗口!!!!!!!!!!!!!!!!!!!!!

	//connections for selection
	connect(view3D, SIGNAL(entitySelectionChanged(int)),                m_ccRoot,  SLOT(selectEntity(int)));
	connect(view3D, SIGNAL(entitiesSelectionChanged(std::set<int>)),    m_ccRoot,  SLOT(selectEntities(std::set<int>)));

	//connections for 3DViews
	// display parameters
	connect(view3D,	SIGNAL(mouseWheelRotated(float)),					this,		SLOT(echoMouseWheelRotate(float)));
	connect(view3D,	SIGNAL(cameraDisplaced(float,float)),				this,		SLOT(echoCameraDisplaced(float,float)));
	connect(view3D,	SIGNAL(viewMatRotated(const ccGLMatrixd&)),			this,		SLOT(echoBaseViewMatRotation(const ccGLMatrixd&)));
	connect(view3D,	SIGNAL(cameraPosChanged(const CCVector3d&)),		this,		SLOT(echoCameraPosChanged(const CCVector3d&)));
	connect(view3D,	SIGNAL(pivotPointChanged(const CCVector3d&)),		this,		SLOT(echoPivotPointChanged(const CCVector3d&)));
	connect(view3D,	SIGNAL(pixelSizeChanged(float)),					this,		SLOT(echoPixelSizeChanged(float)));

	connect(view3D,	SIGNAL(destroyed(QObject*)),						this,		SLOT(prepareWindowDeletion(QObject*)));
	connect(view3D,	SIGNAL(filesDropped(const QStringList&)),			this,		SLOT(addToDBAuto(const QStringList&)));
	connect(view3D,	SIGNAL(newLabel(ccHObject*)),						this,		SLOT(handleNewLabel(ccHObject*)));
	//*************************************************************************************//

	view3D->setSceneDB(m_ccRoot->getRootEntity());//?????????????????????
	view3D->setAttribute(Qt::WA_DeleteOnClose);//??????????????????????????
	//更新属性列表
	m_ccRoot->updatePropertiesView();

	//QMainWindow::statusBar()->showMessage(QString("New 3D View"), 2000);
	QMainWindow::statusBar()->showMessage(QString("新建3D视角"), 2000);

	view3D->showMaximized();
	return view3D;
}

//====================================toggleRotationAboutVertAxis=====================//
void MainWindow::toggleRotationAboutVertAxis(){
	ccGLWindow * win = getActiveGLWindow();
	if(win){
		bool wasLocked = win->isVerticalRotationLocked();
		bool isLocked = !wasLocked;

		win->lockVerticalRotation(isLocked);
		actionLockRotationVertAxis->blockSignals(true);
		actionLockRotationVertAxis->setCheckable(isLocked);
		actionLockRotationVertAxis->blockSignals(false);

		// 屏幕上显示文本信息
		if(isLocked){
			win->displayNewMessage(QString("[ROTATION LOCKED]"),ccGLWindow::UPPER_CENTER_MESSAGE,false,24*3600,ccGLWindow::ROTAION_LOCK_MESSAGE);
		}
		else{
			win->displayNewMessage(QString(),ccGLWindow::UPPER_CENTER_MESSAGE,false,0,ccGLWindow::ROTAION_LOCK_MESSAGE);
		}
		win->redraw(true);//仅仅显示2D
	}
}

//====================================doActionEditCamera==============================//
void MainWindow::doActionEditCamera(){

	//current active MDI area
	QMdiSubWindow * qWin = m_mdiArea->activeSubWindow();
	if(!qWin) return;

	if(!m_cpeDlg){
		m_cpeDlg = new ccCameraParamEditDlg(qWin);
		// 当有窗口激活时，自动的将对话框链接到激活窗口上
		SIGNAL(subWindowActivated(QMdiSubWindow*)), m_cpeDlg, SLOT(linkWith(QMdiSubWindow*));
		registerMDIDialog(m_cpeDlg, Qt::BottomLeftCorner);
	}
	//与当前活动窗口建立联系 // 很重要
	m_cpeDlg->linkWith(qWin);
	m_cpeDlg->start();

	updateMDIDialogsPlacement();
}

//====================================updateMenus===================================//
void MainWindow::updateMenus(){
	ccGLWindow* win = getActiveGLWindow();
	bool hasMdiChild = (win!=0);
	bool hasSelectedEntities = (m_ccRoot && m_ccRoot->countSelectedEntities()>0);

	// General Menu
	menuEdit->setEnabled(true);
	menuTools->setEnabled(true);

	//3D Views Menu
	actionClose3DView->setEnabled(hasMdiChild);
	actionCloseAll3DViews->setEnabled(hasMdiChild);
	actionTile3DViews->setEnabled(hasMdiChild);
	actionCascade3DViews->setEnabled(hasMdiChild);
	actionNext3DView->setEnabled(hasMdiChild);
	actionPrevious3DView->setEnabled(hasMdiChild);

	//View Menu
	toolBarView->setEnabled(hasMdiChild);
}

//====================================on3DViewActive===============================//
void MainWindow::on3DViewActivated(QMdiSubWindow*mdiWin){
	
	if(mdiWin==0)return;
	ccGLWindow * win = static_cast<ccGLWindow*>(mdiWin->widget());
	
	if(win==0) return;
	updateViewModePopUpMenu(win);
	updatePivotVisibilityPopUpMenu(win);

	actionLockRotationVertAxis->blockSignals(true);
	actionLockRotationVertAxis->setChecked(win->isVerticalRotationLocked());
	actionLockRotationVertAxis->blockSignals(false);
}

//====================================freezeUI=====================================//
void MainWindow::freezeUI(bool state){

	toolBarMainTools->setDisabled(state);
	DockableDBTree->setDisabled(state);
	menubar->setDisabled(state);

	menuEdit->setDisabled(state);
	menuTools->setDisabled(state);

	if(state==false) updateMenus();
	m_uiFrozen = state;
}

//=====================================setPivotAlwaysOn=============================//
void MainWindow::setPivotAlwaysOn(){
	
	ccGLWindow* win = getActiveGLWindow();
	if(win){
		win->setPivotVisibility(ccGLWindow::PIVOT_ALWAYS_SHOW);
		win->redraw();

		//update pop-up menu  'top'  icon
		if(m_pivotVisibilityPopupButton){
			m_pivotVisibilityPopupButton->setIcon(actionSetPivotAlwaysOn->icon());
		}
	}
}

//=====================================setPivotRotationOnly==========================//
void MainWindow::setPivotRotationOnly(){
	ccGLWindow *win = getActiveGLWindow();
	if(win){
		win->setPivotVisibility(ccGLWindow::PIVOT_SHOW_ON_MOVE);
		win->redraw();

		//update pop-up menu 'top' icon
		if(m_pivotVisibilityPopupButton){
			m_pivotVisibilityPopupButton->setIcon(actionSetPivotRotationOnly->icon());
		}
	}
}

//=====================================setPivotOff===================================//
void MainWindow::setPivotOff(){
	ccGLWindow *win = getActiveGLWindow();
	if(win){
		win->setPivotVisibility(ccGLWindow::PIVOT_HIDE);
		win->redraw();

		//update pop-up menu 'top' icon
		if(m_pivotVisibilityPopupButton){
			m_pivotVisibilityPopupButton->setIcon(actionSetPivotOff->icon());
		}
	}
}

//======================================setOrthoView=================================//
void MainWindow::setOrthoView(){
	setOrthoView(getActiveGLWindow());
}

//=======================================setOrthoView=================================//
void MainWindow::setOrthoView(ccGLWindow *win){
	if(win){
		win->setPerspectiveState(false, true);
		win->redraw();

		//update pop-up menu 'top' icon
		if(m_viewModePopupButton){
			m_viewModePopupButton->setIcon(actionSetOrthoView->icon());
		}
		if(m_pivotVisibilityPopupButton){
			m_pivotVisibilityPopupButton->setEnabled(true);
		}
	}
}

//=======================================setCenteredPerspectiveView====================//
void MainWindow::setCenteredPerspectiveView(){
	setCenteredPerspectiveView(getActiveGLWindow());
}

//========================================setCenteredPerspectiveView====================//
void MainWindow::setCenteredPerspectiveView(ccGLWindow *win){
	if(win){
		win->setPerspectiveState(true, true);
		win->redraw();

			//update pop-up menu 'top' icon
		if(m_viewModePopupButton){
			m_viewModePopupButton->setIcon(actionSetCenteredPerspectiveView->icon());
		}
		if(m_pivotVisibilityPopupButton){
			m_pivotVisibilityPopupButton->setEnabled(true);
		}
	}
}

//=======================================setViewerPerspectiveView======================//
void MainWindow::setViewerPerspectiveView(){
	setViewerPerspectiveView(getActiveGLWindow());
}

//=======================================setViewerPerspectiveView======================//
void MainWindow::setViewerPerspectiveView(ccGLWindow *win){
	if (win){
		win->setPerspectiveState(true,false);
		win->redraw();

		//update pop-up menu 'top' icon
		if (m_viewModePopupButton)
			m_viewModePopupButton->setIcon(actionSetViewerPerspectiveView->icon());
		if (m_pivotVisibilityPopupButton)
			m_pivotVisibilityPopupButton->setEnabled(false);
	}
}

//=======================================updateViewModePopUpMenu=======================//
//win->getPerspectiveState()
void MainWindow::updateViewModePopUpMenu(ccGLWindow* win){
	
	if(!m_viewModePopupButton) return;

	// update the view mode pop-up 'top' icon
	if(win){
		bool objectCentered = true;
		
		//窗口透视的状态
		bool perspectiveEnabled = win->getPerspectiveState(objectCentered);

		QAction* currentModeAction = 0;
		if(!perspectiveEnabled){
			currentModeAction = actionSetOrthoView;
		}
		else if (objectCentered){
			currentModeAction = actionSetCenteredPerspectiveView;
		}
		else{
			currentModeAction = actionSetViewerPerspectiveView;
		}

		assert(currentModeAction);
		m_viewModePopupButton->setIcon(currentModeAction->icon());
		m_viewModePopupButton->setEnabled(true);
	}
	else{
		m_viewModePopupButton->setIcon(QIcon());
		m_viewModePopupButton->setEnabled(false);
	}

}

//=======================================updatePivotVisibilityPopUpMenu==================//
//win->getPivotVisibility();
//win->getPerspectiveState();
void MainWindow::updatePivotVisibilityPopUpMenu(ccGLWindow* win){
	if(!m_pivotVisibilityPopupButton) return;

	//update the pivot visibility pop-up 'top' icon
	if(win){
		// find which action is acitvied
         QAction* visibilityAction = 0;
		 switch(win->getPivotVisibility()){
		     case ccGLWindow::PIVOT_HIDE:
			      visibilityAction = actionSetPivotOff;
			       break;
		     case ccGLWindow::PIVOT_SHOW_ON_MOVE:
			      visibilityAction = actionSetPivotRotationOnly;
			      break;
			 case ccGLWindow::PIVOT_ALWAYS_SHOW:
				  visibilityAction = actionSetPivotAlwaysOn;
				  break;
			 default:
				 assert(false);
		 }
		 if(visibilityAction){
			 m_pivotVisibilityPopupButton->setIcon(visibilityAction->icon());
		 }

		 // whether the action should be enabled
		 //picot is not available in viwer-based perspective!
		 bool objectCentered = true;
		 win->getPerspectiveState(objectCentered);
		 m_pivotVisibilityPopupButton->setEnabled(objectCentered);	 
	}
	else{
		m_pivotVisibilityPopupButton->setIcon(QIcon());
		m_pivotVisibilityPopupButton->setEnabled(false);
	}
}

//=======================================palceMDIDialog=================================//
void MainWindow::placeMDIDialog(ccMDIDialogs& mdiDlg){
	if(!mdiDlg.dialog || !mdiDlg.dialog->isVisible() || !m_mdiArea) return;

	int dx =0, dy =0;
	switch(mdiDlg.position){
	case Qt::TopLeftCorner:
		dx = 5;
		dy = 5;
		break;
	case Qt::TopRightCorner:
		dx = std::max(5, m_mdiArea->width() - mdiDlg.dialog->width() -5);
		dy = 5;
		break;
	case Qt::BottomLeftCorner:
		dx = 5;
		dy = std::max(5, m_mdiArea->height() - mdiDlg.dialog->height() -5);
		break;
	case Qt::BottomRightCorner:
		dx = std::max(5,m_mdiArea->width() - mdiDlg.dialog->width() - 5);
		dy = std::max(5,m_mdiArea->height() - mdiDlg.dialog->height() - 5);
		break;
	}
	// widget show
	mdiDlg.dialog->move(m_mdiArea->mapToGlobal(QPoint(dx, dy)));
	//After this call the widget will be visually in front of any overlapping sibling widgets.
	mdiDlg.dialog->raise();
}

//=======================================registerMDIDialog===============================//
void MainWindow::registerMDIDialog(ccOverlayDialog* dlg, Qt::Corner pos){
	// check whether the dialog existed
	for(size_t i=0; i< m_mdiDialogs.size();  i++){
		if(m_mdiDialogs[i].dialog ==dlg){
			//update the position of this dialog
			m_mdiDialogs[i].position = pos;
			return;
		}
	}
	// otherwise create a new dialog
	m_mdiDialogs.push_back(ccMDIDialogs(dlg, pos));
}

//=======================================unregisterMDIDialog=============================//
void MainWindow::unregisterMDIDialog(ccOverlayDialog* dlg){
	if(dlg){
		std::vector<ccMDIDialogs>::iterator it = m_mdiDialogs.begin();
		while (it!= m_mdiDialogs.end()){
			if(it->dialog == dlg)return;
		}
		if(it!= m_mdiDialogs.end())m_mdiDialogs.erase(it);
	}
	dlg->disconnect();
	dlg->stop(false);
	dlg->deleteLater();
}

//=======================================updateMDIDialogPlacement=========================//
void MainWindow::updateMDIDialogsPlacement(){
	for(size_t i=0; i< m_mdiDialogs.size(); i++){
		placeMDIDialog(m_mdiDialogs[i]);
	}
}

//====================================setTopView===================================//
void MainWindow::setTopView(){
	ccGLWindow* win = getActiveGLWindow();
	if(win){
		win->setView(CC_FRONT_VIEW);
	}
}

//====================================setBottomView===============================//
void MainWindow::setBottomView(){
	ccGLWindow* win = getActiveGLWindow();
	if(win){
		win->setView(CC_BOTTOM_VIEW);
	}
}

//====================================setFrontView==================================//
void MainWindow::setFrontView(){
	ccGLWindow * win = getActiveGLWindow();
	if(win){
		win->setView(CC_FRONT_VIEW);
	}
}

//====================================setBackView====================================//
void MainWindow::setBackView(){
	ccGLWindow * win = getActiveGLWindow();
	if(win){
		win->setView(CC_BACK_VIEW);
	}
}

//=====================================setLeftView====================================//
void MainWindow::setLeftView(){
	ccGLWindow* win = getActiveGLWindow();
	if(win){
		win->setView(CC_LEFT_VIEW);
	}
}

//=====================================setRightView===================================//
void MainWindow::setRightView(){
	ccGLWindow *win = getActiveGLWindow();
	if(win){
		win->setView(CC_RIGHT_VIEW);
	}
}

//=====================================setIsoView1====================================//
void MainWindow::setIsoView1(){
	ccGLWindow* win = getActiveGLWindow();
	if(win){
		win->setView(CC_ISO_VIEW_1);
	}
}

//=====================================setIsoView2====================================//
void MainWindow::setIsoView2(){
	ccGLWindow * win = getActiveGLWindow();
	if(win){
		win->setView(CC_ISO_VIEW_2);
	}
}
