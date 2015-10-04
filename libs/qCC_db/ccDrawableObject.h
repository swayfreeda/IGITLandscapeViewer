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

#ifndef CC_DRAWABLE_OBJECT_HEADER
#define CC_DRAWABLE_OBJECT_HEADER

#include <ccIncludeGL.h>

//Local
#include "qCC_db.h"
#include "ccGLMatrix.h"
#include "ccMaterial.h"

class ccGenericGLDisplay;
class ccScalarField;
class ccColorRampShader;
class ccShader;

//! Display parameters of a 3D entity
struct glDrawParams 
{
	//! Display scalar field (prioritary on colors)
	bool showSF;// 标量场
	//! Display colors
	bool showColors;//颜色
	//! Display normals
	bool showNorms;//法向量
};

//! Display context
struct glDrawContext //显示内容
{
	//! Drawing options (see below)
	uint16_t flags;

	//! GL screen width
	int glW;  //屏幕宽

	//! GL screen height
	int glH; //屏幕高

	//! Corresponding GL window
	ccGenericGLDisplay* _win;//对应的GL 窗口

	//! Current zoom (screen to file rendering mode)
	float renderZoom; //当前的缩放

	//! Default material
	ccMaterial::Shared defaultMat; //默认材料

	//! Default color for mesh (front side)//默认的前面颜色
	ccColor::Rgbaf defaultMeshFrontDiff;

	//! Default color for mesh (back side) //默认的背面颜色
	ccColor::Rgbaf defaultMeshBackDiff;

	//! Default point color
	ccColor::Rgbub pointsDefaultCol;//默认点的颜色

	//! Default text color
	ccColor::Rgbub textDefaultCol; //默认字体颜色

	//! Default label background color //默认背景颜色
	ccColor::Rgbub labelDefaultBkgCol;

	//! Default label marker color //默认标记颜色
	ccColor::Rgbub labelDefaultMarkerCol;

	//! Default bounding-box color//默认的bounding box颜色
	ccColor::Rgbub bbDefaultCol;

	//! Whether to decimate big clouds when updating the 3D view
	bool decimateCloudOnMove;

	//! Minimum level for LOD display
	unsigned char minLODLevel;

	//! Minimum number of points for activating LOD display
	unsigned minLODPointCount;

	//! Current level for LOD display
	unsigned char currentLODLevel;

	//! Start index for current LOD level
	unsigned currentLODStartIndex;

	//! Wheter more points are available or not at the current level
	bool moreLODPointsAvailable;

	//! Wheter higher levels are available or not
	bool higherLODLevelsAvailable;

	//! Whether to decimate big meshes when rotating the camera
	bool decimateMeshOnMove;

	//! Minimum number of triangles for activating LOD display
	unsigned minLODTriangleCount;

	//! Currently displayed color scale (the corresponding scalar field in fact)
	ccScalarField* sfColorScaleToDisplay;
	
	//! Shader for fast dynamic color ramp lookup
	ccColorRampShader* colorRampShader;

	//! Custom rendering shader (OpenGL 3.3+)
	ccShader* customRenderingShader;

	//! Use VBOs for faster display
	bool useVBOs; //定点换缓存数组

	//! Label marker size (radius)//标记大小
	float labelMarkerSize;

	//! Shift for 3D label marker display (around the marker)
	float labelMarkerTextShift; //标记文本偏移

	//! Numerical precision (for displaying text)
	unsigned dispNumberPrecision; //显示数字精度

	//! Label background opacity
	unsigned labelOpacity; //标签不透明

	//! Blending strategy (source)
	GLenum sourceBlend; //源混合

	//! Blending strategy (destination)
	GLenum destBlend;//目标混合

	//Default constructor
	glDrawContext()
		: flags(0)
		, glW(0)
		, glH(0)
		, _win(0)
		, renderZoom(1.0f)
		, defaultMat(new ccMaterial("default"))
		, defaultMeshFrontDiff(ccColor::defaultMeshFrontDiff)
		, defaultMeshBackDiff(ccColor::defaultMeshBackDiff)
		, pointsDefaultCol(ccColor::defaultColor)
		, textDefaultCol(ccColor::defaultColor)
		, labelDefaultBkgCol(ccColor::defaultLabelBkgColor)
		, labelDefaultMarkerCol(ccColor::defaultLabelMarkerColor)
		, bbDefaultCol(ccColor::yellow)
		, decimateCloudOnMove(true)
		, minLODLevel(11)
		, minLODPointCount(10000000)
		, currentLODLevel(0)
		, currentLODStartIndex(0)
		, moreLODPointsAvailable(false)
		, higherLODLevelsAvailable(false)
		, decimateMeshOnMove(true)
		, minLODTriangleCount(2500000)
		, sfColorScaleToDisplay(0)
		, colorRampShader(0)
		, customRenderingShader(0)
		, useVBOs(true)
		, labelMarkerSize(5)
		, labelMarkerTextShift(0)
		, dispNumberPrecision(6)
		, labelOpacity(100)
		, sourceBlend(GL_SRC_ALPHA)
		, destBlend(GL_ONE_MINUS_SRC_ALPHA)
	{}
};
typedef glDrawContext CC_DRAW_CONTEXT;

// Drawing flags (type: short) 
#define CC_DRAW_2D								0x0001 // 二维绘制
#define CC_DRAW_3D								0x0002 // 三维绘制
#define CC_DRAW_FOREGROUND						0x0004 // 前景绘制
#define CC_LIGHT_ENABLED						0x0008 // 灯光开启
#define CC_SKIP_UNSELECTED						0x0010 // 忽略未选择
#define CC_SKIP_SELECTED						0x0020 // 忽略选择
#define CC_SKIP_ALL								0x0030		// = CC_SKIP_UNSELECTED | CC_SKIP_SELECTED
#define CC_DRAW_ENTITY_NAMES					0x0040 //显示物体名字
#define CC_DRAW_POINT_NAMES						0x0080 //显示点的名字
#define CC_DRAW_TRI_NAMES						0x0100 //显示面片名字
#define CC_DRAW_FAST_NAMES_ONLY					0x0200 
#define CC_DRAW_ANY_NAMES						0x03C0		// = CC_DRAW_ENTITY_NAMES | CC_DRAW_POINT_NAMES | CC_DRAW_TRI_NAMES
#define CC_LOD_ACTIVATED						0x0400 //显示LOD
#define CC_VIRTUAL_TRANS_ENABLED				0x0800

// Drawing flags testing macros (see ccDrawableObject)
#define MACRO_Draw2D(context) (context.flags & CC_DRAW_2D)
#define MACRO_Draw3D(context) (context.flags & CC_DRAW_3D)
#define MACRO_DrawPointNames(context) (context.flags & CC_DRAW_POINT_NAMES)
#define MACRO_DrawTriangleNames(context) (context.flags & CC_DRAW_TRI_NAMES)
#define MACRO_DrawEntityNames(context) (context.flags & CC_DRAW_ENTITY_NAMES)
#define MACRO_DrawNames(context) (context.flags & CC_DRAW_ANY_NAMES)
#define MACRO_DrawFastNamesOnly(context) (context.flags & CC_DRAW_FAST_NAMES_ONLY)
#define MACRO_SkipUnselected(context) (context.flags & CC_SKIP_UNSELECTED)
#define MACRO_SkipSelected(context) (context.flags & CC_SKIP_SELECTED)
#define MACRO_LightIsEnabled(context) (context.flags & CC_LIGHT_ENABLED)
#define MACRO_Foreground(context) (context.flags & CC_DRAW_FOREGROUND)
#define MACRO_LODActivated(context) (context.flags & CC_LOD_ACTIVATED)
#define MACRO_VirtualTransEnabled(context) (context.flags & CC_VIRTUAL_TRANS_ENABLED)

//! Generic interface for (3D) drawable entities
class QCC_DB_LIB_API ccDrawableObject
{
public:

	//! Default constructor
	ccDrawableObject();

	//! Copy constructor
	ccDrawableObject(const ccDrawableObject& object);

	//! Draws entity and its children
	//绘制内容
	virtual void draw(CC_DRAW_CONTEXT& context) = 0;

	//! Returns whether entity is visible or not
	//[判断] 是否可视
	inline virtual bool isVisible() const { return m_visible; }

	//! Sets entity visibility
	//[设置] 设置成可视
	inline virtual void setVisible(bool state) { m_visible = state; }

	//! Toggles visibility
	//[切换]可视性
	inline virtual void toggleVisibility() { setVisible(!isVisible()); }

	//! Returns whether visibilty is locked or not
	//[判断] 是否锁定可视性
	inline virtual bool isVisiblityLocked() const { return m_lockedVisibility; }
	//! Locks/unlocks visibilty
	/** If visibility is locked, the user won't be able to modify it
		(via the properties tree for instance).
	**/
	//[设置] 锁定可视性
	inline virtual void lockVisibility(bool state) { m_lockedVisibility = state; }

	//! Returns whether entity is selected or not
	//[判断]是否已经被选择
	inline virtual bool isSelected() const { return m_selected; }

	//! Selects/unselects entity
	// [设置] 是否被选择
	inline virtual void setSelected(bool state) { m_selected = state; }

	//! Returns main OpenGL paramters for this entity
	/** These parameters are deduced from the visiblity states
		of its different features (points, normals, etc.).
		\param params a glDrawParams structure
	**/
	//获取openGL参数
	virtual void getDrawingParameters(glDrawParams& params) const;

	//! Returns whether colors are enabled or not
	//是否存在颜色
	inline virtual bool hasColors() const  { return false; }

	//! Returns whether colors are shown or not
	//是否显示颜色
	inline virtual bool colorsShown() const { return m_colorsDisplayed; }

	//! Sets colors visibility
	//设置颜色可视性
	inline virtual void showColors(bool state) { m_colorsDisplayed = state; }

	//! Toggles colors display state
	//切换显示颜色
	inline virtual void toggleColors() { showColors(!colorsShown()); }

	//! Returns whether normals are enabled or not
	//是否存在法向量
	inline virtual bool hasNormals() const  { return false; }

	//! Returns whether normals are shown or not
	//法向量是否被显示
	inline virtual bool normalsShown() const { return m_normalsDisplayed; }

	//! Sets normals visibility
	//获取法向量可视性
	inline virtual void showNormals(bool state) { m_normalsDisplayed = state; }

	//! Toggles normals display state
	//切换显示法向量
	inline virtual void toggleNormals() { showNormals(!normalsShown()); }

	/*** scalar fields ***/

	//! Returns whether an active scalar field is available or not
	//显示是否已经显示标量场
	inline virtual bool hasDisplayedScalarField() const { return false; }

	//! Returns whether one or more scalar fields are instantiated
	/** WARNING: doesn't mean a scalar field is currently displayed
		(see ccDrawableObject::hasDisplayedScalarField).
	**/
	//是否存在标量场
	inline virtual bool hasScalarFields() const  { return false; }

	//! Sets active scalarfield visibility
	//设置显示标量场
	inline virtual void showSF(bool state) { m_sfDisplayed = state; }

	//! Toggles SF display state
	//切换标量场
	inline virtual void toggleSF() { showSF(!sfShown()); }

	//! Returns whether active scalar field is visible
	//是否显示标量场
	inline virtual bool sfShown() const { return m_sfDisplayed; }

	/*** Mesh materials ***/

	//! Toggles material display state
	//切换材料
	virtual void toggleMaterials() {}; //does nothing by default!

	/*** Name display in 3D ***/

	//! Sets whether name should be displayed in 3D
	//设置 在三维视角中显示名字
	inline virtual void showNameIn3D(bool state) { m_showNameIn3D = state; }

	//! Returns whether name is displayed in 3D or not
	//名字是否被显示
	inline virtual bool nameShownIn3D() const { return m_showNameIn3D; }

	//! Toggles name in 3D display state
	//切换显示名字
	inline virtual void toggleShowName() { showNameIn3D(!nameShownIn3D()); }

	/*** temporary color ***/

	//! Returns whether colors are currently overriden by a temporary (unique) color
	/** See ccDrawableObject::setTempColor.
	**/
	//颜色被覆盖
	inline virtual bool isColorOverriden() const { return m_colorIsOverriden; }

	//! Returns current temporary (unique) color
	//获取临时的颜色状态
	inline virtual const ccColor::Rgb& getTempColor() const { return m_tempColor; }

	//! Sets current temporary (unique)
	/** \param col rgb color
		\param autoActivate auto activates temporary color
	**/
	//设置临时的颜色状态
	virtual void setTempColor(const ccColor::Rgb& col, bool autoActivate = true);

	//! Set temporary color activation state
	//开启临时的颜色状态
	inline virtual void enableTempColor(bool state) { m_colorIsOverriden = state; }

	/*** associated display management ***/

	//! Unlinks entity from a GL display (only if it belongs to it of course)
	//从当前的GL显示中去除该物体
	virtual void removeFromDisplay(const ccGenericGLDisplay* win);

	//! Sets associated GL display
	//设置相关的GL绘制
	virtual void setDisplay(ccGenericGLDisplay* win);

	//! Returns associated GL display
	//返回相关的GL绘制
	inline virtual ccGenericGLDisplay* getDisplay() const { return m_currentDisplay; }

	//! Redraws associated GL display
	//重新绘制相关的显示
	virtual void redrawDisplay();

	//! Sets associated GL display 'refreshable' before global refresh
	/** Only tagged displays will be refreshed when ccGenericGLDisplay::refresh
		is called (see also MainWindow::RefreshAllGLWindow,
		MainWindow::refreshAll and ccDrawableObject::refreshDisplay).
	**/
	//准备更新显示//设置相关的GL显示为 ‘可更新’，只有标记的显示会被更新
	virtual void prepareDisplayForRefresh();

	//! Refreshes associated GL display
	/** See ccGenericGLDisplay::refresh.
	**/
	//更新显示,更新相关的GL显示
	virtual void refreshDisplay();

	/*** Transformation matrix management (for display only) ***/

	//! Associates entity with a GL transformation (rotation + translation)
	/** \warning FOR DISPLAY PURPOSE ONLY (i.e. should only be temporary)
		If the associated GL transformation is enabled (see
		ccDrawableObject::enableGLTransformation), it will
		be applied before displaying this entity.
		However it will not be taken into account by any CCLib algorithm
		(distance computation, etc.) for instance.
		Note: GL transformation is automatically enabled.
	**/
	//对场景进行变化(变化包含平移和旋转)//但是在计算过程中不会被考虑
	virtual void setGLTransformation(const ccGLMatrix& trans);

	//! Enables/disables associated GL transformation
	/** See ccDrawableObject::setGLTransformation.
	**/
	//设置允许变换
	inline virtual void enableGLTransformation(bool state) { m_glTransEnabled = state; }

	//! Returns whether a GL transformation is enabled or not
	//是否允许变换
	inline virtual bool isGLTransEnabled() const { return m_glTransEnabled; }

	//! Retuns associated GL transformation
	/** See ccDrawableObject::setGLTransformation.
	**/
	//获取当前的变换矩阵
	inline virtual const ccGLMatrix& getGLTransformation() const { return m_glTrans; }

	//! Resets associated GL transformation
	/** GL transformation is reset to identity.
		Note: GL transformation is automatically disabled.
		See ccDrawableObject::setGLTransformation.
	**/
	//将相应的GL变换设置成单位矩阵
	virtual void resetGLTransformation();

	//! Mutliplies (left) current GL transformation by a rotation matrix
	/** 'GLtrans = M * GLtrans'
		Note: GL transformation is automatically enabled.
		See ccDrawableObject::setGLTransformation.
	**/
	//对当前的GL进行平移
	virtual void rotateGL(const ccGLMatrix& rotMat);

	//! Translates current GL transformation by a translation vector
	/** 'GLtrans = GLtrans + T'
		Note: GL transformation is automatically enabled.
		See ccDrawableObject::setGLTransformation.
	**/
	//对当前的GL进行平移
	virtual void translateGL(const CCVector3& trans);

protected:

	//! Specifies whether the object is visible or not
	/** Note: this does not influence the children visibility
	**/
	//标记物体是否可见
	bool m_visible;

	//! Specifies whether the object is selected or not
	//物体是否被选中
	bool m_selected;

	//! Specifies whether the visibility can be changed by user or not
	//锁定可视性(能否被用户改变)
	bool m_lockedVisibility;

	/*** OpenGL display parameters ***/

	//! Specifies whether colors should be displayed
	//颜色显示
	bool m_colorsDisplayed;

	//! Specifies whether normals should be displayed
	//法向量显示
	bool m_normalsDisplayed;

	//! Specifies whether scalar field should be displayed
	//标量场显示
	bool m_sfDisplayed;

	//! Temporary (unique) color
	//临时颜色
	ccColor::Rgb m_tempColor;

	//! Temporary (unique) color activation state
	//临时颜色激活状态
	bool m_colorIsOverriden;

	//! Current GL transformation
	/** See ccDrawableObject::setGLTransformation.
	**/
	//变换矩阵
	ccGLMatrix m_glTrans;

	//! Current GL transformation activation state
	/** See ccDrawableObject::setGLTransformation.
	**/
	//是否启动平移
	bool m_glTransEnabled;

	//! Whether name is displayed in 3D or not
	//三维中显示物体名字
	bool m_showNameIn3D;

	//! Currently associated GL display
	//当前相关的GL显示
	ccGenericGLDisplay* m_currentDisplay;
};

#endif //CC_DRAWABLE_OBJECT_HEADER
