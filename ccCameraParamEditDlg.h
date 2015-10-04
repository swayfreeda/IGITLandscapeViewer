//##########################################################################
//#                                                                        #
//#                            CLOUDCOMPARE                                #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
//#                                                                        #
//##########################################################################

#ifndef CC_CAMERA_PARAM_EDIT_DLG_HEADER
#define CC_CAMERA_PARAM_EDIT_DLG_HEADER

//Local
#include "ccOverlayDialog.h"
#include "ccGLWindow.h"

#include <ui_cameraParamDlg.h>

//qCC_db
#include <ccGLMatrix.h>

//system
#include <map>

class QMdiSubWindow;
class ccGLWindow;

//! Dialog to interactively edit the camera pose parameters
class ccCameraParamEditDlg : public ccOverlayDialog, public Ui::CameraParamDlg
{
	Q_OBJECT

public:

	//! Default constructor// [sway] construtor
	ccCameraParamEditDlg(QWidget* parent);

	//! Destructor //[sway] destrutor
	virtual ~ccCameraParamEditDlg();

	//! Makes this dialog frameless
	void makeFrameless();

	//! Returns matrix corresponding to dialog values
	ccGLMatrixd getMatrix();

	//inherited from ccOverlayDialog
	virtual bool start();
	virtual bool linkWith(ccGLWindow* win);

public slots:

	//! Links this dialog with a given sub-window
	void linkWith(QMdiSubWindow* qWin);

	//! Inits dialog values with matrix
	void initWithMatrix(const ccGLMatrixd& mat);

	//! Slots called when the view matrix of the associated window changes
	void updateViewMatrix(const ccGLMatrixd& dummyMat);

	//! Updates dialog values with pivot point
	void updatePivotPoint(const CCVector3d& P);//更新旋转球中心
	//! Updates dialog values with camera center
	void updateCameraCenter(const CCVector3d& P);//更新相机中心
	//! Updates current view mode
	void updateViewMode(); // 更新透视模式
	//! Updates view f.o.v.
	void updateWinFov(float fov_deg);// 更新视角

	//设置8个视角
	void setFrontView();
	void setBottomView();
	void setTopView();
	void setBackView();
	void setLeftView();
	void setRightView();
	void setIso1View();
	void setIso2View();

	//三个角度的变化Slider
	void iThetaValueChanged(int);
	void iPsiValueChanged(int);
	void iPhiValueChanged(int);

	//三个角度的变化 spinBox
	void dThetaValueChanged(double);
	void dPsiValueChanged(double);
	void dPhiValueChanged(double);

	void zNearSliderMoved(int);
	void pivotChanged();
	void cameraCenterChanged();
	void fovChanged(double);

	//选择一个点作为旋转球中心
	void pickPointAsPivot();
	void processPickedItem(int, unsigned, int, int);

protected slots:

	//! Reflects any dialog parameter change
	void reflectParamChange();

	//! Places the camera in a given prefedined orientation
	void setView(CC_VIEW_ORIENTATION orientation);

	//! Pushes current matrix//保存当前的矩阵
	void pushCurrentMatrix();

	//! Reverts to pushed matrix//恢复前一步的矩阵
	void revertToPushedMatrix();

protected:

	//! Inits dialog values with specified window
	void initWith(ccGLWindow* win);

	//! Type of the pushed matrices map structure// 每个窗口对应矩阵，一个相机对话框对应多个窗口
	typedef std::map<ccGLWindow*,ccGLMatrixd> PushedMatricesMapType; 

	//! Type of an element of the pushed matrices map structure
	typedef std::pair<ccGLWindow*,ccGLMatrixd> PushedMatricesMapElement;

	//! Pushed camera matrices (per window)
	PushedMatricesMapType pushedMatrices;
};

#endif
