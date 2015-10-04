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

#ifndef CC_HIERARCHY_OBJECT_HEADER
#define CC_HIERARCHY_OBJECT_HEADER

//Local
#include "qCC_db.h"
#include "ccObject.h"
#include "ccDrawableObject.h"
#include "ccBBox.h"

//System
#include <vector>

class QIcon;

//分层数据结构
//! Hierarchical CloudCompare Object
class QCC_DB_LIB_API ccHObject : public ccObject, public ccDrawableObject
	// ccObject提供相应的类ID操作 // ccDrawableObject提供相应的绘制操作
{
public: //construction

	//! Default constructor// 默认构造函数
	/** \param name object name (optional)
	**/
	ccHObject(QString name = QString());

	//! Copy constructor //赋值构造函数
	ccHObject(const ccHObject& object);

	//! Default destructor //析构函数
	virtual ~ccHObject();

	//! Static factory
	/** Warning: objects depending on other structures (such as meshes 
		or polylines that should be linked with point clouds playing the
		role of vertices) are returned 'naked'.
		\param objectType object type
		\param name object name (optional)
		\return instantiated object (if type is valid) or 0
	**/
	//新建物体
	static ccHObject* New(CC_CLASS_ENUM objectType, const char* name = 0);

	//! Static factory (version to be used by external plugin factories)
	/** Two strings are used as keys, one for the plugin name and one for the class name.
		Those strings will typically be saved as metadata of a custom object
	**/
	//新建物体用于插件拓展库 这些名称将被存储为 metadata
	static ccHObject* New(QString pluginId, QString classId, const char* name = 0);

public: //base members access

	//! Returns class ID
	/** \return class unique ID
	**/
	// 返回类的ID 分层物体
	inline virtual CC_CLASS_ENUM getClassID() const { return CC_TYPES::HIERARCHY_OBJECT; }

	//! Returns parent object
	/** \return parent object (NULL if no parent)
	**/
	//返回父亲物体
	inline ccHObject* getParent() const { return m_parent; }

	//! Returns the icon associated to this entity
	/** ccDBRoot will call this method: if an invalid icon is returned
		the default icon for that type will be used instead.
		\return invalid icon by default (to be re-implemented by child class)
	**/
	// 返回物体图标
	virtual QIcon getIcon() const;
	
public: //dependencies management

	//! Dependency flags  //依赖关系标记
	enum DEPENDENCY_FLAGS {	DP_NONE						= 0,	/**< no dependency **/ //没有依赖关系
							DP_NOTIFY_OTHER_ON_DELETE	= 1,	/**< notify 'other' when deleted (will call ccHObject::onDeletionOf) **/ //被删除时通知其它
							DP_NOTIFY_OTHER_ON_UPDATE	= 2,	/**< notify 'other' when its geometry is modified (will call ccHObject::onUpdateOf) **/ //被修改时通知其它
							//DP_NOTIFY_XXX				= 4,  
							DP_DELETE_OTHER				= 8,	/**< delete 'other' before deleting itself **/  //被删除时先删除其它
							DP_PARENT_OF_OTHER			= 24,	/**< same as DP_DELETE_OTHER + declares itself as parent of 'other' **/ //
	};

	//! Adds a new dependence (additive or not)
	/** \param otherObject other object
		\param flags dependency flags (see DEPENDENCY_FLAGS)
		\param additive whether we should 'add' the flag(s) if there's already a dependence with the other object or not
	**/
	// 添加依赖关系
	void addDependency(ccHObject* otherObject, int flags, bool additive = true);

	//! Returns the dependency flags with a given object
	/** \param otherObject other object
	**/
	//给定另一个物体时，返回依赖关系
	int getDependencyFlagsWith(const ccHObject* otherObject);

	//! Removes any dependency flags with a given object
	/** \param otherObject other object
	**/
	// 给定另一个物体时，删除与它的依赖关系
	void removeDependencyWith(ccHObject* otherObject);

	//! Removes a given dependency flag
	/** \param otherObject other object
		\param flag dependency flag to remove (see DEPENDENCY_FLAGS)
	**/
	//删除依赖关系的标记????????????????///
	void removeDependencyFlag(ccHObject* otherObject, DEPENDENCY_FLAGS flag);

public: //children management

	//! Adds a child
	/** \warning by default (i.e. with the DP_PARENT_OF_OTHER flag) the child's parent
		will be automatically replaced by this instance. Moreover the child will be deleted

		\param child child instance
		\param dependencyFlags dependency flags
		\param insertIndex insertion index (if <0, child is simply appended to the children list)
		\return success
	**/
	//添加子物体
	virtual bool addChild(ccHObject* child, int dependencyFlags = DP_PARENT_OF_OTHER, int insertIndex = -1);

	//! Returns the number of children
	/** \return children number
	**/
	//返回子物体的个数
	inline unsigned getChildrenNumber() const { return static_cast<unsigned>(m_children.size()); }

	//! Returns the ith child
	/** \param childPos child position
		\return child object (or NULL if wrong position)
	**/
    //返回子物体
	inline ccHObject* getChild(unsigned childPos) const { return (childPos < getChildrenNumber() ? m_children[childPos] : 0); }

	//! Finds an entity in this object hierarchy
	/** \param uniqueID child unique ID
		\return child (or NULL if not found)
	**/
	//找到子物体，如果存在则返回其指针，如果不存在则返回空指针
	ccHObject* find(unsigned uniqueID);

	//! Finds an entity in this object hierarchy
	/** \param obj name
	    return child (or NULL if not found)
		**/
	ccHObject* find(QString objname);

	//! standard ccHObject container (for children, etc.)
	// 类型定义 物体容器
	typedef std::vector<ccHObject*> Container;

	//! Collects the children corresponding to a certain pattern
	/** \param filteredChildren result container
		\param recursive specifies if the search should be recursive
		\param filter pattern for children selection
		\param strict whether the search is strict on the type (i.e 'isA') or not (i.e. 'isKindOf')
		\return number of collected children
	**/
	// 根据设定的类型对子物体进行筛选
	unsigned filterChildren(Container& filteredChildren,
							bool recursive = false,
							CC_CLASS_ENUM filter = CC_TYPES::OBJECT,
							bool strict = false) const;

	//! Detaches a specific child
	/** This method does not delete the child.
		Removes any dependency between the flag and this object
	**/
	// 删除子物体和其他物体的依赖关系，但是并不删除该物体
	void detachChild(ccHObject* child);
	//! Removes a specific child
	/** \warning This method may delete the child if the DP_PARENT_OF_OTHER
		dependency flag is set for this child (use detachChild if you
		want to avoid deletion).
	**/
	//! Detaches all children
	// 删除所有子物体和其他物体的依赖关系，但是并不删除这些物体
	void detatchAllChildren();

	//删除子物体版本一
	void removeChild(ccHObject* child);
	//! Removes a specific child given its index
	/** \warning This method may delete the child if the DP_PARENT_OF_OTHER
		dependency flag is set for this child (use detachChild if you
		want to avoid deletion).
	**/
	//删除子物体版本二
	void removeChild(int pos);


	//! Removes all children
	//删除所有的子物体
	void removeAllChildren();

	//! Returns child index
	//返回子物体的索引
	int getChildIndex(const ccHObject* aChild) const;

	//! Swaps two children
	//两个子物体互换
	void swapChildren(unsigned firstChildIndex, unsigned secondChildIndex);

	//! Returns index relatively to its parent or -1 if no parent
	//返回相对父亲物体的索引(没有父亲则返回-1)
	int getIndex() const;


	//! Transfer a given child to another parent
	//将一个子物体转移到另一个父亲物体
	void transferChild(ccHObject* child, ccHObject& newParent);

	//! Transfer all children to another parent
	//将所有的子节点转移到另外一个父亲物体
	void transferChildren(ccHObject& newParent, bool forceFatherDependent = false);

	//! Shortcut: returns first child
	//获取第一个子物体
	ccHObject* getFirstChild() const { return (m_children.empty() ? 0 : m_children.front()); }

	//! Shortcut: returns last child
	//获取最后一个子物体
	ccHObject* getLastChild() const { return (m_children.empty() ? 0 : m_children.back()); }

	//! Returns true if the current object is an ancestor of the specified one
	//[判断]是不是祖先物体
	bool isAncestorOf(const ccHObject *anObject) const;

public: //bouding-box

	//! Returns the entity's own bounding-box
	/** Children bboxes are ignored.
		\param withGLFeatures whether to take into account display-only elements (if any)
		\return bounding-box
	**/
	//返回物体自身的边界框
	virtual ccBBox getOwnBB(bool withGLFeatures = false);

	//! Returns the bounding-box of this entity and it's children
	/** \param withGLFeatures whether to take into account display-only elements (if any)
		\param onlyEnabledChildren only consider the 'enabled' children
		\return bounding-box
	**/
	//返回该物体和子物体(开启)的边界框
	virtual ccBBox getBB_recursive(bool withGLFeatures = false, bool onlyEnabledChildren = true);

	//! Returns the bounding-box of this entity and it's children WHEN DISPLAYED
	/** Children's GL transformation is taken into account (if enabled).
		\param relative whether the bounding-box is relative (i.e. in the entity's local coordinate sytem) or absolute (in which case the parent's GL transformation will be taken into account)
		\param display if not null, this method will return the bounding-box of this entity (and its children) in the specified 3D view (i.e. potentially not visible)
		\return bounding-box
	**/
	//返回显示的物体以及其子物体的边界框
	virtual ccBBox getDisplayBB_recursive(bool relative, const ccGenericGLDisplay* display = 0);

	//! Returns best-fit bounding-box (if available)
	/** \warning Only suitable for leaf objects (i.e. without children)
		Therefore children bboxes are always ignored.
		\warning This method is not supported by all entities!
		(returns the axis-aligned bounding-box by default).
		\param[out] trans associated transformation (so that the bounding-box can be displayed in the right position!)
		\return fit bounding-box
	**/
	inline virtual ccBBox getOwnFitBB(ccGLMatrix& trans) { trans.toIdentity(); return getOwnBB(); }

	//! Returns the entity's own global bounding-box
	/** Children bboxes are ignored.
		May differ from the (local) bounding-box if the entity is shifted
		\param[out] minCorner min global bounding-box corner
		\param[out] maxCorner max global bounding-box corner
		\return whether the bounding box is valid or not
	**/
	//返回物体自身的全局边界框 （和局部边界框有什么区别？？？？？？？？？？？？？？？？？？？？）
	virtual bool getGlobalBB(CCVector3d& minCorner, CCVector3d& maxCorner)
	{
		//by default this method returns the local bounding-box!
		ccBBox box = getOwnBB(false);
		minCorner = CCVector3d::fromArray(box.minCorner().u);
		maxCorner = CCVector3d::fromArray(box.maxCorner().u);
		return box.isValid();
	}

	//! Draws the entity (and its children) bounding-box
	//绘制边界框
	virtual void drawBB(const ccColor::Rgb& col);

public: //display

	//Inherited from ccDrawableObject
	//绘制GL环境
	virtual void draw(CC_DRAW_CONTEXT& context);

	//! Returns the absolute transformation (i.e. the actual displayed GL transforamtion) of an entity
	/** \param[out] trans absolute transformation
		\return whether a GL transformation is actually enabled or not
	**/
	//获取绝对的GL变换
	bool getAbsoluteGLTransformation(ccGLMatrix& trans) const;

	//! Returns whether the object is actually displayed (visible) or not
	// [判断] 物体是不是最终显示
	virtual bool isDisplayed() const;

	//! Returns whether the object and all its ancestors are enabled
	// [判断] 物体与其祖先物体是否开启
	virtual bool isBranchEnabled() const;

	/*** RECURSIVE CALL SCRIPTS ***/
	
	//0 parameter
	#define ccHObject_recursive_call0(baseName,recursiveName) \
	inline virtual void recursiveName() \
	{ \
		baseName(); \
		for (Container::iterator it = m_children.begin(); it != m_children.end(); ++it) \
			(*it)->recursiveName(); \
	} \

	//1 parameter
	#define ccHObject_recursive_call1(baseName,param1Type,recursiveName) \
	inline virtual void recursiveName(param1Type p) \
	{ \
		baseName(p); \
		for (Container::iterator it = m_children.begin(); it != m_children.end(); ++it) \
			(*it)->recursiveName(p); \
	} \

	/*****************************/

	//recursive equivalents of some of ccDrawableObject methods
	ccHObject_recursive_call1(setSelected,bool,setSelected_recursive);
	ccHObject_recursive_call1(setDisplay,ccGenericGLDisplay*,setDisplay_recursive);
	ccHObject_recursive_call1(removeFromDisplay,ccGenericGLDisplay*,removeFromDisplay_recursive);
	ccHObject_recursive_call0(prepareDisplayForRefresh,prepareDisplayForRefresh_recursive);
	ccHObject_recursive_call0(refreshDisplay,refreshDisplay_recursive);
	ccHObject_recursive_call0(resetGLTransformationHistory,resetGLTransformationHistory_recursive);
	ccHObject_recursive_call0(toggleActivation,toggleActivation_recursive);
	ccHObject_recursive_call0(toggleVisibility,toggleVisibility_recursive);
	ccHObject_recursive_call0(toggleColors,toggleColors_recursive);
	ccHObject_recursive_call0(toggleNormals,toggleNormals_recursive);
	ccHObject_recursive_call0(toggleSF,toggleSF_recursive);
	ccHObject_recursive_call0(toggleShowName,toggleShowName_recursive);
	ccHObject_recursive_call0(toggleMaterials,toggleMaterials_recursive);

	//! Returns the max 'unique ID' of this entity and its siblings
	//找到最大的 id
	unsigned findMaxUniqueID_recursive() const;

	//! Applies the active OpenGL transformation to the entity (recursive)
	/** The input ccGLMatrix should be left to 0, unless you want to apply
		a pre-transformation.
		\param trans a ccGLMatrix structure (reference to)
	**/
	//回归的施加GL矩阵变换
	void applyGLTransformation_recursive(ccGLMatrix* trans = 0);

	//! Notifies all dependent entities that the geometry of this entity has changed
	//通知所有想依赖的物体，该物体的几何发生改变
	virtual void notifyGeometryUpdate();

	//inherited from ccSerializableObject
	virtual bool isSerializable() const;
	virtual bool toFile(QFile& out) const;
	inline virtual bool fromFile(QFile& in, short dataVersion, int flags) { return fromFile(in,dataVersion,flags,false); }

	//! Custom version of ccSerializableObject::fromFile
	/** This version is used to load only the object own part of a stream (and not its children's)
		\param in input file (already opened)
		\param dataVersion file version
		\param flags deserialization flags (see ccSerializableObject::DeserializationFlags)
		\param omitChildren to omit loading the children's part of the stream
		\return success
	**/
	virtual bool fromFile(QFile& in, short dataVersion, int flags, bool omitChildren);

	//! Returns whether object is shareable or not
	/** If object is father dependent and 'shared', it won't
		be deleted but 'released' instead.
	**/
	virtual bool isShareable() const { return false; }

	//! Behavior when selected
	enum SelectionBehavior { SELECTION_AA_BBOX,
							 SELECTION_FIT_BBOX,
							 SELECTION_IGNORED };

	//! Sets selection behavior (when displayed)
	/** WARNING: SELECTION_FIT_BBOX relies on the
		'ccDrawableObject::getFitBB' method (which
		is not supported by all entities).
	**/
	//设置选择模式
	void setSelectionBehavior(SelectionBehavior mode) { m_selectionBehavior = mode; }

	//! Returns selection behavior
	//返回选择模式
	SelectionBehavior getSelectionBehavior() const { return m_selectionBehavior; }

	//! Returns object unqiue ID used for display
	//返回物体的ID 用于显示
	virtual unsigned getUniqueIDForDisplay() const { return getUniqueID(); }

	//! Returns the transformation 'history' matrix
	//返回上次变换矩阵
	const ccGLMatrix& getGLTransformationHistory() const { return m_glTransHistory; }

	//! Sets the transformation 'history' matrix (handle with care!)
	//设置上次变换矩阵
	void setGLTransformationHistory(const ccGLMatrix& mat) { m_glTransHistory = mat; }

	//! Resets the transformation 'history' matrix
	//重新设置上次变换矩阵
	void resetGLTransformationHistory() { m_glTransHistory.toIdentity(); }

protected:

	//! Sets parent object
	//设置父亲物体
	virtual inline void setParent(ccHObject* anObject) { m_parent = anObject; }

	//! Draws the entity only (not its children)
	//绘制自身（忽略子物体)
	virtual void drawMeOnly(CC_DRAW_CONTEXT& context) { /*does nothing by default*/ }

	//! Applies a GL transformation to the entity
	/** this = rotMat*(this-rotCenter)+(rotCenter+trans)
		\param trans a ccGLMatrix structure
	**/
	//施加GL变换
	virtual void applyGLTransformation(const ccGLMatrix& trans);

	//! Save own object data
	/** Called by 'toFile' (recursive scheme)
		To be overloaded (but still called;) by subclass.
	**/
	//存储到物体
	virtual bool toFile_MeOnly(QFile& out) const;

	//! Loads own object data
	/** Called by 'fromFile' (recursive scheme)
		To be overloaded (but still called;) by subclass.
		\param in input file
		\param dataVersion file version
		\param flags deserialization flags (see ccSerializableObject::DeserializationFlags)
	**/
	virtual bool fromFile_MeOnly(QFile& in, short dataVersion, int flags);

	//! Draws the entity name in 3D
	/** Names is displayed at the center of the bounding box by default.
	**/
	// 在3D空间中绘制物体名字
	virtual void drawNameIn3D(CC_DRAW_CONTEXT& context);

	//! This method is called when another object is deleted
	/** For internal use.
	**/
	//调用函数，彻底删除该物体
	virtual void onDeletionOf(const ccHObject* obj);

	//! This method is called when another object (geometry) is updated
	/** For internal use.
	**/
	//调用函数，当其他物体被更新
	virtual void onUpdateOf(ccHObject* obj) { /*does nothing by default*/ }

	//! Object's parent //父亲物体(只有一个)
	ccHObject* m_parent;

	//! Object's children //子物体（有多个）
	Container m_children;

	//! Selection behavior //选择操作
	SelectionBehavior m_selectionBehavior;

	//! Dependencies map
	/** First parameter: other object
		Second parameter: dependency flags (see DEPENDENCY_FLAGS)
	**/
	//与其它物体的依赖关系
	std::map<ccHObject*,int> m_dependencies;

	//! Cumulative GL transformation
	/** History of all the applied transformations since the creation of the object
		as a single transformation.
	**/
	ccGLMatrix m_glTransHistory;

	//! Flag to safely handle dependencies when the object is being deleted
	//[标记] 安全删除依赖关系，当该物体被删除
	bool m_isDeleting;
};

/*** Helpers ***/

//! Puts all entities inside a container in a group
/** Automatically removes siblings so as to get a valid hierarchy object.
	\param origin origin container
	\param dest destination group
	\param dependencyFlags default dependency link for the children added to the group
**/
//???????????????????????????????????????????????????????????????????????????????????????/ 没看懂
inline void ConvertToGroup(const ccHObject::Container& origin, ccHObject& dest, int dependencyFlags = ccHObject::DP_NONE)
{
	size_t count = origin.size();
	for (size_t i=0; i<count; ++i)
	{
		//we don't take objects that are siblings of others
		bool isSiblingOfAnotherOne = false;
		for (size_t j=0; j<count; ++j)
		{
			if (i != j && origin[j]->isAncestorOf(origin[i]))
			{
				isSiblingOfAnotherOne = true;
				break;
			}
		}

		if (!isSiblingOfAnotherOne)
		{
			dest.addChild(origin[i],dependencyFlags);
		}
	}
}

#endif //CC_HIERARCHY_OBJECT_HEADER
