
//##########################################################################
//#                                                                        #
//#                           IGITlANDSCAPEVIEWER                          #
//#                                                                        #
//#  This software is from the porject of automatic landscape 3D reconstr- #
//#  -uction and visulization from multiviews.                             #
//#                
//#                                                                        #
//#  AUTHOR: SUIWEI                                                        #
//#  DATE: 04/28/2015                                                      #
//#  ORGANIZATION: INSITITUTE OF AUTOMATION, CHINSE ACADEMY OF SCIENCES    #
//#  EMAIL: wsui@nlpr.ia.ac.cn                                             #
//#                                                                        #
//#  COPYRIGHT:      IGIT/NLPR/CASIA                                       #
//#                                                                        #
//##########################################################################
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//qIGIT_plugins
#include<igitMainAppInterface.h>
#include<ccGLWindow.h>
#include<ccDBRoot.h>

#include "ui_mainwindow.h"
#include "ccPersistentSettings.h"

#include <QtGui/QMainWindow>
#include<qmdiarea.h>
#include<qmdisubwindow.h>
#include <QThread>

#include "PointCloudGenDlg.h"
class QMdiArea;
class ccGLWindow;
class ccHObject;
class QSignalMapper;
class ccCameraParamEditDlg;
class ccOverlayDialog;

class MainWindow : public QMainWindow, public igitMainAppInterface, public Ui::MainWindowClass
{
	Q_OBJECT

protected:
	//default constructor
	MainWindow();
	
	//default deconstructor
	virtual ~MainWindow(){}

public:

	// reTurn the unique instance of this object
	static MainWindow* TheInstance();

	// stacti shortcut to MainWindow:: getActiveGLWindow
	static ccGLWindow* GetActiveGLWindow();

	// return active GL sub-window (if any)
	virtual ccGLWindow * getActiveGLWindow();

	// return a given GL sub window (determined by its title)
	/* param title window title
	*/
	static ccGLWindow* GetGLWindow(const QString & title);

	// return all GL sub windows
	/* glWindows vector to store all pointers of sub windows
	*/
	static void GetGLWindows(std::vector<ccGLWindow*> & glWindows);

	// static shortcut to MainWindow:: refreshAll
	static void RefreshAllGLWindow(bool only2D = false);

	// static shortcut to MainWindow:updateUI
	static void UpdateUI();

	//! Deletes current main window instance
	static void DestroyInstance();

    // inherited from ccMainAppInterface
	virtual void refreshAll(bool only2D = false);
	virtual void updateUI();




	// enables menu entries based on the current selection
	void enableUIItems(dbTreeSelectionInfo& selInfo);


	/*** CCLib "standalone" algorithms ***/
	
	//CCLib algorithms handled by the 'ApplyCCLibAlgortihm' method
	enum CC_LIB_ALGORITHM { CCLIB_ALGO_CURVATURE		= 1, // 曲率计算
							CCLIB_ALGO_SF_GRADIENT		= 2, // scalar field 梯度
							CCLIB_ALGO_ROUGHNESS		= 3, // 
							CCLIB_ALGO_APPROX_DENSITY	= 4, // 密度估计
							CCLIB_ALGO_ACCURATE_DENSITY	= 5, // 准确率估计
							CCLIB_SPHERICAL_NEIGHBOURHOOD_EXTRACTION_TEST = 255,
	};

	//! Applies a standard CCLib algorithm (see CC_LIB_ALGORITHM) on a set of entities
	static bool ApplyCCLibAlgorithm(CC_LIB_ALGORITHM algo,
									ccHObject::Container& entities,
									QWidget* parent = 0,
									void** additionalParameters = 0);

	//! Tries to load several files (and then pushes them into main DB)
	/** \param filenames list of all filenames
		\param fType file type
		\param destWin destination window (0 = active one)
	**/
	// 根据文件名读取数据并添加到data base中
	virtual void addToDB(	const QStringList& filenames,
							QString fileFilter = QString(),
							ccGLWindow* destWin = 0);

	//inherited from ccMainAppInterface
	virtual void addToDB(ccHObject* obj,
							bool updateZoom = false,
							bool autoExpandDBTree = true,
							bool checkDimensions = false );

	// load textured results
	void loadTexturedResults(QString ResultsDir);

	// load cameras automatically
	void loadPMVSCameras(QString ResultsDir);

	// plots
protected slots:

	//echo related
	void echoMouseWheelRotate(float angle);
	void echoCameraDisplaced(float ddx, float ddy); //??????????????????????//
	void echoBaseViewMatRotation(const ccGLMatrixd& rotMat);
	void echoCameraPosChanged(const CCVector3d& P);
	void echoPivotPointChanged(const CCVector3d& P); //????????????????????????
	void echoPixelSizeChanged(float size); //??????????????????????????????

	//Updates entities display target when a gl sub-window is deleted
	/** \param glWindow the window that is going to be delete
	**/
	void prepareWindowDeletion(QObject* glWindow);// 删除子窗口

	//Tries to load (and then adds to main db) several files
	/** \param filenames list of all filenames
	**/
	void addToDBAuto(const QStringList& filenames);

	//! Handles new label
	void handleNewLabel(ccHObject*);

	// update UI
	void updateUIWithSelection();


protected slots:

	//'File->OpenFile'   
	void doActionLoadFile();

	//'File->SaveFile'
	void doActionSaveFile();

	//'Edit->Sensors->Camera->Create'
	void doActionCreateCameraSensor();
	void doActionCreateCameraSensorFromFile();
	//Edit->PointCloudGeneration
	void doActionTextureGeneration();

	// "Menu 3DVeiws"
	void update3DViewsMenu();  // 更新3D视角菜单
	ccGLWindow* new3DView();   // 创建三维视图

	//"Display" menu
	void toggleRotationAboutVertAxis();
	void doActionEditCamera();


	// updateMenus
	void updateMenus(); // 更新菜单
	void on3DViewActivated(QMdiSubWindow*mdiWin); // 激活相关的3D视角


	// "View Tool Bar" 
	// inheritted from igitMainAppInterface
	virtual void freezeUI(bool state);

	virtual void setPivotAlwaysOn(); // 始终显示旋转球
	virtual void setPivotRotationOnly(); //仅在旋转时显示旋转球
	virtual void setPivotOff(); // 始终关闭旋转球
	virtual void setOrthoView();// 正视视角
	virtual void setCenteredPerspectiveView(); // 以物体为中心的透视投影
	virtual void setViewerPerspectiveView();  // 以观察者为中心的透视投影

	virtual void setTopView();
	virtual void setBottomView();
	virtual void setFrontView();
	virtual void setBackView();
	virtual void setLeftView();
	virtual void setRightView();
	virtual void setIsoView1();
	virtual void setIsoView2();

protected:

	//! Connects all QT actions to slots
	void connectActions();

	// set active sub window
	void setActiveSubWindow(QWidget* window);

	//设置正式视角
	void setOrthoView(ccGLWindow *win);
	
	//设置物体为中心的透视投影
	void setCenteredPerspectiveView(ccGLWindow *win);
	
	//设置观察者为中心的透视投影
	void setViewerPerspectiveView(ccGLWindow *win);

	//! Updates the view mode pop-menu based for a given window (or an absence of!)
	virtual void updateViewModePopUpMenu(ccGLWindow* win);
	
	//! Updates the pivot visibility pop-menu based for a given window (or an absence of!)
	virtual void updatePivotVisibilityPopUpMenu(ccGLWindow* win);

	//! CloudCompare MDI area overlay dialogs
	struct ccMDIDialogs{
		//! Constructor with dialog and position
		ccMDIDialogs(ccOverlayDialog* dlg, Qt::Corner pos)
			: dialog(dlg)
			, position(pos){}

		ccOverlayDialog* dialog;
		Qt::Corner position;
	};

	//! Replaces an MDI dialog at its right position
	void placeMDIDialog(ccMDIDialogs& mdiDlg);

	//! Registers a MDI area overlay dialog
	void registerMDIDialog(ccOverlayDialog* dlg, Qt::Corner pos);

	//! Unregisters a MDI area overlay dialog
	void unregisterMDIDialog(ccOverlayDialog* dlg);

	//! Automatically updates all registered MDI dialogs placement
	void updateMDIDialogsPlacement();


public:

	// QMdiArea provides an area where MID windows are displayed
	QMdiArea* m_mdiArea;

	// QSignalMapper is used for widgets with same signals and slots
	QSignalMapper* m_windowMapper;

	
	//View mode Pop menu button
	QToolButton * m_viewModePopupButton;

	//piovt visibility pop-up menu button
	QToolButton * m_pivotVisibilityPopupButton;

	//DB & DB Tree
	ccDBRoot * m_ccRoot;

	// currently selected entities
	ccHObject::Container m_selectedEntities;

	//===============dialogs========================//
	//! Camera params dialog
	ccCameraParamEditDlg* m_cpeDlg; // 相机参数编辑对话框

	//! UI frozen state (see freezeUI)
	bool m_uiFrozen;

	//! Registered MDI area overlay dialogs
	std::vector<ccMDIDialogs> m_mdiDialogs;

	QThread m_thread;

};

#endif // MAINWINDOW_H
