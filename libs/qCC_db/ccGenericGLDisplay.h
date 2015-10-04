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

#ifndef CC_GENERIC_GL_DISPLAY
#define CC_GENERIC_GL_DISPLAY

//Local
#include "qCC_db.h"
#include "ccSerializableObject.h"
#include "ccGLMatrix.h"
#include "ccMaterial.h"

//Qt
#include <QImage>
#include <QString>
#include <QFont>

//CCLib
#include <CCGeom.h>

class QWidget;

//! Standard parameters for GL displays/viewports
//GL显示和视口的参数
class QCC_DB_LIB_API ccViewportParameters : public ccSerializableObject
{
public:
	//! Default constructor
	ccViewportParameters();

	//! Copy constructor
	ccViewportParameters(const ccViewportParameters& params);

	//inherited from ccSerializableObject
	virtual bool isSerializable() const { return true; }
	virtual bool toFile(QFile& out) const;
	virtual bool fromFile(QFile& in, short dataVersion, int flags);

	//! Current pixel size (in 'current unit'/pixel)
	/** This scale is valid eveywhere in ortho. mode 
		or at the focal distance in perspective mode.
		Warning: doesn't take current zoom into account!
	**/
	float pixelSize; //像素大小

	//! Current zoom //当前缩放尺度
	float zoom;

	//! Visualization matrix (rotation only) //可视矩阵(仅旋转）
	ccGLMatrixd viewMat;

	//! Point size //默认点的大小
	float defaultPointSize;

	//! Line width //默认线的宽度
	float defaultLineWidth;

	//! Perspective view state // 透视状态
	bool perspectiveView;

	//! Whether view is centered on displayed scene (true) or on the user eye (false)
	/** Always true for ortho. mode.
	**/
	//判断场景是以物体为中心 还是 以观察者为中心
	bool objectCenteredView;

	//! Theoretical perspective 'zNear' relative position//近侧裁剪平面相对位置
	double zNearCoef;

	//! Actual perspective 'zNear' value //近侧裁剪平面距离
	double zNear;

	//! Actual perspective 'zFar' value //远侧裁剪平面距离
	double zFar;
	
	//! Rotation pivot point (for object-centered view modes)//旋转球中心位置(对以物体为中心的视角模式有效)
	CCVector3d pivotPoint;
	
	//! Camera center (for perspective mode)//相机中心(对透视模式有效)
	CCVector3d cameraCenter;

	//! Camera F.O.V. (field of view - for perspective mode only) //相机视野的角度[0,180](对透视模式有效)
	float fov;

	//! Camera aspect ratio (perspective mode only)//相机平截头体纵横比(仅对透视模式有效)
	float perspectiveAspectRatio;

	//! 3D view aspect ratio (ortho mode only)//正视投影比例
	/** AR = width / height
	**/
	float orthoAspectRatio;

};

//! Generic interface for GL displays
class ccGenericGLDisplay
{
public:

	//! Redraws display immediately
	//重新绘制
	virtual void redraw(bool only2D = false) = 0;

	//! Flags display as 'to be refreshed'
	/** See ccGenericGLDisplay::refresh.
	**/
	virtual void toBeRefreshed() = 0;

	//! Redraws display only if flagged as 'to be refreshed'
	/** See ccGenericGLDisplay::toBeRefreshed. Flag is turned
		to false after a call to this method.
		\param only2D whether to redraw everything (false) or only the 2D layer (true)
	**/
	virtual void refresh(bool only2D = false) = 0;

	//! Invalidates current viewport setup
	/** On next redraw, viewport information will be recomputed.
	**/
	virtual void invalidateViewport() = 0;

	//! Returns the texture ID corresponding to an image
	//获取纹理(对应一副图像)
	virtual unsigned getTextureID(const QImage& image) = 0;
	
	//! Returns the texture ID corresponding to a material
	//获取纹理(d对应材料）
	virtual unsigned getTextureID( ccMaterial::CShared mtl) = 0;

	//! Release texture from context
	//释放纹理
	virtual void releaseTexture(unsigned texID) = 0;

	//! Returns defaul text display font
	/** Warning: already takes rendering zoom into account!
	**/
	//获取文本显示字体
	virtual QFont getTextDisplayFont() const = 0;

	//! Returns defaul label display font
	/** Warning: already takes rendering zoom into account!
	**/
	//返回默认的标签
	virtual QFont getLabelDisplayFont() const = 0;

	//! Text alignment
	//文本对齐
	enum TextAlign { ALIGN_HLEFT	= 1,
					 ALIGN_HMIDDLE	= 2,
					 ALIGN_HRIGHT	= 4,
					 ALIGN_VTOP		= 8,
					 ALIGN_VMIDDLE	= 16,
					 ALIGN_VBOTTOM	= 32,
					 ALIGN_DEFAULT	= 1 | 8};

	//! Displays a string at a given 2D position
	/** This method should be called solely during 2D pass rendering.
		The coordinates are expressed relatively to the current viewport (y = 0 at the top!).
		\param text string
		\param x horizontal position of string origin
		\param y vertical position of string origin
		\param align alignment position flags 
		\param bkgAlpha background transparency (0 by default)// 透明度
		\param rgbColor text color (optional) //文本颜色
		\param font optional font (otherwise default one will be used) //可选择的字体颜色
	**/
	//在给给定位置显示文本
	virtual void displayText(	QString text,
								int x,
								int y,
								unsigned char align = ALIGN_DEFAULT,
								float bkgAlpha = 0,
								const unsigned char* rgbColor = 0,
								const QFont* font = 0) = 0;

	//! Displays a string at a given 3D position
	/** This method should be called solely during 3D pass rendering (see paintGL).
		\param str string
		\param pos3D 3D position of string origin // 三维位置
		\param rgbColor color (optional: if let to 0, default text rendering color is used)
		\param font font (optional) 
	**/
	//在给定的三维位置显示字符串
	virtual void display3DLabel(const QString& str,
								const CCVector3& pos3D,
								const unsigned char* rgbColor = 0,
								const QFont& font=QFont()) = 0;

	//! Returns whether a given version of OpenGL is supported
	/** \param openGLVersionFlag see QGLFormat::OpenGLVersionFlag
	**/
	//[判断] 是否支持给定的OpenGL版本
	virtual bool supportOpenGLVersion(unsigned openGLVersionFlag) = 0;

	//! Returns current model view matrix (GL_MODELVIEW)
	//返回当前的视图矩阵
	virtual const double* getModelViewMatd() = 0;

	//! Returns current projection matrix (GL_PROJECTION)
	//返回当前的投影矩阵
	virtual const double* getProjectionMatd() = 0;

	//! Returns current viewport (GL_VIEWPORT)
	//返回当前的视口
	virtual void getViewportArray(int vp[/*4*/]) = 0;

	//! Returns viewport parameters (zoom, etc.)
	//返回当前的视口参数
	virtual const ccViewportParameters& getViewportParameters() const = 0;

	//! Makes the associated OpenGL context active
	// 激活相关的OpenGL环境
	virtual void makeContextCurrent() = 0;

	//! Setups a (projective) camera
	/** \param cameraMatrix orientation/position matrix of the camera //相机矩阵(方向/位置)
		\param fov_deg vertical field of view (in degrees). Optional (ignored if 0). //相机视野角度
		\param ar aspect ratio (width/height) //视口的宽高比
		\param viewerBasedPerspective whether the perspective view should be object-centered (false) or camera-centered (true)(物体为中心还是以观察者为中心)
		\param bubbleViewMode set whether bubble-view mode should be enabled or not (in which case viewerBasedPerspective is forced by default)
	**/
	// 设置矩阵参数
	virtual void setupProjectiveViewport(	const ccGLMatrixd& cameraMatrix,
											float fov_deg = 0.0f,
											float ar = 1.0f,
											bool viewerBasedPerspective = true,
											bool bubbleViewMode = false) = 0;

	//! Returns this window as a proper Qt widget
	virtual QWidget* asWidget() { return 0; }

};

#endif //CC_GENERIC_GL_DISPLAY
