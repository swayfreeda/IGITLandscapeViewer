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

#include "ccDrawableObject.h"

//Local
#include "ccGenericGLDisplay.h"

/******************************/
/*      ccDrawableObject      */
/******************************/

ccDrawableObject::ccDrawableObject()
	: m_currentDisplay(0)
{
	setVisible(true);//默认可视
	setSelected(false); // 默认未被选中
	lockVisibility(false);//不锁定可视性
	showColors(false);//不显示颜色
	showNormals(false);//不显示法向量
	showSF(false);//不显示标量场
	enableTempColor(false);//未开启临时颜色
	setTempColor(ccColor::white,false);//
	resetGLTransformation();//重置GL变换为单位矩阵
	showNameIn3D(false);//不在三维中显示名字
}

ccDrawableObject::ccDrawableObject(const ccDrawableObject& object)
	: m_visible(object.m_visible)
	, m_selected(object.m_selected)
	, m_lockedVisibility(object.m_lockedVisibility)
	, m_colorsDisplayed(object.m_colorsDisplayed)
	, m_normalsDisplayed(object.m_normalsDisplayed)
	, m_sfDisplayed(object.m_sfDisplayed)
	, m_tempColor(object.m_tempColor)
	, m_colorIsOverriden(object.m_colorIsOverriden)
	, m_glTrans(object.m_glTrans)
	, m_glTransEnabled(object.m_glTransEnabled)
	, m_showNameIn3D(object.m_showNameIn3D)
	, m_currentDisplay(object.m_currentDisplay)
{
}

void ccDrawableObject::redrawDisplay()
{
	if (m_currentDisplay)
		m_currentDisplay->redraw();
}

void ccDrawableObject::refreshDisplay()
{
	if (m_currentDisplay)
		m_currentDisplay->refresh();
}

void ccDrawableObject::prepareDisplayForRefresh()
{
	if (m_currentDisplay)
		m_currentDisplay->toBeRefreshed();
}

//设置显示窗口
void ccDrawableObject::setDisplay(ccGenericGLDisplay* win)
{
	if (win && m_currentDisplay != win)
		win->invalidateViewport();

	m_currentDisplay = win;
}

void ccDrawableObject::removeFromDisplay(const ccGenericGLDisplay* win)
{
	if (m_currentDisplay == win)
		setDisplay(0);
}

void ccDrawableObject::setGLTransformation(const ccGLMatrix& trans)
{
	m_glTrans = trans;
	enableGLTransformation(true);
}

void ccDrawableObject::rotateGL(const ccGLMatrix& rotMat)
{
	m_glTrans = rotMat * m_glTrans;
	enableGLTransformation(true);
}

void ccDrawableObject::translateGL(const CCVector3& trans)
{
	m_glTrans += trans;
	enableGLTransformation(true);
}
//===================================================resetGLTransformation=======================================//
void ccDrawableObject::resetGLTransformation()
{
	enableGLTransformation(false);
	m_glTrans.toIdentity();
}

void ccDrawableObject::setTempColor(const ccColor::Rgb& col, bool autoActivate/*=true*/)
{
	m_tempColor = col;

	if (autoActivate)
		enableTempColor(true);
}

void ccDrawableObject::getDrawingParameters(glDrawParams& params) const
{
	//color override
	if (isColorOverriden())
	{
		params.showColors = true;
		params.showNorms = hasNormals() && normalsShown()/*false*/;
		params.showSF = false;
	}
	else
	{
		params.showNorms = hasNormals() && normalsShown();
		params.showSF = hasDisplayedScalarField() && sfShown();
		//colors are not displayed if scalar field is displayed
		params.showColors = !params.showSF && hasColors() && colorsShown();
	}
}
