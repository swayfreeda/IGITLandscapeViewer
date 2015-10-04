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

#ifndef CC_DB_ROOT_HEADER
#define CC_DB_ROOT_HEADER

//Qt
#include <QAbstractItemModel>
#include <QItemSelection>
#include <QPoint>
#include <QTreeView>

//CCLib
#include <CCConst.h>

//qCC_db
#include <ccObject.h>
#include <ccHObject.h>
#include <ccDrawableObject.h>

//System
#include <string.h>
#include <set>

class QStandardItemModel;
class QAction;
class ccPropertiesTreeDelegate;
class ccHObject;
class ccGLWindow;

//! Precise statistics about current selection
struct dbTreeSelectionInfo{
	int selCount;

	int sfCount;// scalar field的个数
	int colorCount; // 颜色的个数
	int normalsCount;//法向量个数
	int octreeCount; //octree个数
	int cloudCount; // 点云个数
	int groupCount; // group 的个数
	int polylineCount; //polyline 的个数
	int meshCount; // mesh 的个数
	int imageCount; // 图像的个数
	int sensorCount; // 传感器的个数
	int gblSensorCount; 
	int cameraSensorCount; // 相机传感器的个数
	int kdTreeCount; // kdtree的个数

	void reset(){
		memset(this,0,sizeof(dbTreeSelectionInfo)); //所有参数重置成0
	}
};

//! Custom QTreeView widget (for advanced selection behavior)
class ccCustomQTreeView : public QTreeView
{
public:

	//! Default constructor
	ccCustomQTreeView(QWidget* parent) : QTreeView(parent) {}

protected:

	//inherited from QTreeView //选择标记
	virtual QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex& index, const QEvent* event=0) const;
};

//data Base Root
//! GUI database tree root
class ccDBRoot : public QAbstractItemModel
{
	Q_OBJECT

public:

	//! Default constructor
	/** \param dbTreeWidget widget for DB tree display //显示物体层次关系
		\param propertiesTreeWidget widget for selected entity's properties tree display //显示属性
		\param parent widget QObject parent
	**/
	ccDBRoot(ccCustomQTreeView* dbTreeWidget, QTreeView* propertiesTreeWidget, QObject* parent = 0);

	//! Destructor
	virtual ~ccDBRoot();

	//! Returns associated root object //返回根节点
	ccHObject* getRootEntity();

	//! Hides properties view   //隐藏属性视图
	void hidePropertiesView();

	//! Updates properties view //更新属性视图
	void updatePropertiesView();

	//! Adds an element to the DB tree  //添加物体到DB Tree
	void addElement(ccHObject* anObject, bool autoExpand = true);

	//! Removes an element from the DB tree  //删除元素
	void removeElement(ccHObject* anObject);

	//! Finds an element in DB     //DB Tree中查找物体
	ccHObject* find(int uniqueID) const;

	//! Returns the number of selected entities in DB tree (optionally with a given type) //返回DBTree中选中的物体的个数
	int countSelectedEntities(CC_CLASS_ENUM filter = CC_TYPES::OBJECT); 

	//! Returns selected entities in DB tree (optionally with a given type and additional information)
	int getSelectedEntities(ccHObject::Container& selEntities,
							CC_CLASS_ENUM filter = CC_TYPES::OBJECT,
							dbTreeSelectionInfo* info = NULL); //返回选择到的物体(给定固定的物体类型)

	//! Expands tree at a given node// 对树进行扩张
	void expandElement(ccHObject* anObject, bool state);

	//! Selects a given entity
	/** If ctrl is pressed by the user at the same time,
		previous selection will be simply updated accordingly.
		\param obj entity to select
		\param forceAdditiveSelection whether to force additive selection (just as if CTRL key is pressed) or not
	**/
	//选择物体
	void selectEntity(ccHObject* obj, bool forceAdditiveSelection = false);

	//! Unselects a given entity //取消选择物体
	void unselectEntity(ccHObject* obj);

	//! Unselects all entities // 取消所有的选择
	void unselectAllEntities();

	//! Unloads all entities //卸载所有的物体
	void unloadAll();

	//inherited from QAbstractItemModel
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual QModelIndex index(int row, int column, const QModelIndex &parentIndex = QModelIndex()) const;
	virtual QModelIndex index(ccHObject* object);
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual Qt::DropActions supportedDropActions() const;
	virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
	virtual QMap<int,QVariant> itemData(const QModelIndex& index) const;
#ifdef CC_QT5
	virtual Qt::DropActions supportedDragActions() const { return Qt::MoveAction; }
#endif

public slots:
	void changeSelection(const QItemSelection & selected, const QItemSelection & deselected);
	void reflectObjectPropChange(ccHObject* obj);//反应物体属性变化
	void redrawCCObject(ccHObject* anObject); // 重新绘制物体
	void redrawCCObjectAndChildren(ccHObject* anObject);//重新绘制物体与子物体
	void updateCCObject(ccHObject* anObject);//更新物体
	void deleteSelectedEntities();//删除选择

	//! Shortcut to selectEntity(ccHObject*)
	void selectEntity(int uniqueID); 

	//! Selects multiple entities at once (shortcut to the other version)
	/** \param entIDs list of the IDs of the entities to select
	**/
	void selectEntities(std::set<int> entIDs);

	//! Selects multiple entities at once
	/** \param entIDs set of the entities to 'select'
		\param incremental whether to 'add' the input set to the selected entities set or to use it as replacement
	**/
	void selectEntities(const ccHObject::Container& entities, bool incremental = false);

protected slots:
	void showContextMenu(const QPoint&); //显示环境菜单
	void expandBranch(); //扩张分支
	void collapseBranch(); //取消分支
	void gatherRecursiveInformation();//收集递归信息
	void sortSiblingsAZ();//对子物体进行排序
	void sortSiblingsZA();
	void sortSiblingsType();//对子物体按照类型排序
	void toggleSelectedEntities(); //切换选择的物体
	void toggleSelectedEntitiesVisibility(); //切换选择物体的可视性
	void toggleSelectedEntitiesColor(); //切换选择物体的颜色
	void toggleSelectedEntitiesNormals(); //切换选择物体的法向量
	void toggleSelectedEntitiesSF(); //切换选择物体的标量场
	void toggleSelectedEntitiesMat(); //切换选择物体的矩阵
	void toggleSelectedEntities3DName(); //切换选择物体的3D名称
	void addEmptyGroup(); //添加空的组
	void alignCameraWithEntityDirect() { alignCameraWithEntity(false); } //????????????????
	void alignCameraWithEntityIndirect() { alignCameraWithEntity(true); } //??????????????????????//
	void enableBubbleViewMode();

signals:
	void selectionChanged(); //选择发生变化

protected:

	//! Aligns the camera with the currently selected entity
	/** \param reverse whether to use the entity's normal (false) or its inverse (true)
	**/
	void alignCameraWithEntity(bool reverse); //将相机与选择的物体进行对齐

	//! Shows properties view for a given element  //显示给定物体的属性
	void showPropertiesView(ccHObject* obj);

	//! Toggles a given property (enable state, visibility, normal, color, SF, etc.) on selected entities
	/** Properties are:
			0 - enable state
			1 - visibility
			2 - normal
			3 - color
			4 - SF
			5 - materials/textures
			6 - 3D name
	**/
	void toggleSelectedEntitiesProperty(unsigned prop); //切换给定的属性的可视性

	//! Entities sorting schemes
	enum SortRules { SORT_A2Z, SORT_Z2A, SORT_BY_TYPE }; //排序规则

	//! Sorts selected entities siblings
	void sortSelectedEntitiesSiblings(SortRules rule); //对选择的兄弟节点进行排序

	//! Expands or collapses hovered item
	void expandOrCollapseHoveredBranch(bool expand); //????????????????????????????????????////

	//! Associated DB root // 树的根节点
	ccHObject* m_treeRoot;   

	//! Associated widget for DB tree // DBTree相关的部件
	QTreeView* m_dbTreeWidget;

	//! Associated widget for selected entity's properties tree
	QTreeView* m_propertiesTreeWidget; // 属性相关的部件

	//! Selected entity's properties data model  //选择的物体的数据模型
	QStandardItemModel* m_propertiesModel;

	//! Selected entity's properties delegate  //选择的物体的属性代表
	ccPropertiesTreeDelegate* m_ccPropDelegate;  //??????????????????????????????????????????????

	//! Context menu action: expand tree branch
	QAction* m_expandBranch; // 扩张树
	//! Context menu action: collapse tree branch
	QAction* m_collapseBranch; // 折叠树
	//! Context menu action: gather (recursive) information on selected entities
	QAction* m_gatherInformation; //搜集信息
	//! Context menu action: sort siblings in alphabetical order
	QAction* m_sortSiblingsAZ; //排序1
	//! Context menu action: sort siblings in reverse alphabetical order
	QAction* m_sortSiblingsZA; //排序2
	//! Context menu action: sort siblings by type
	QAction* m_sortSiblingsType;  //排序3
	//! Context menu action: delete selected entities
	QAction* m_deleteSelectedEntities; // 删除选择物体的属性
	//! Context menu action: enabled/disable selected entities
	QAction* m_toggleSelectedEntities; //切换选择物体的属性
	//! Context menu action: hide/show selected entities
	QAction* m_toggleSelectedEntitiesVisibility; //切换选择物体的可视性
	//! Context menu action: hide/show selected entities color
	QAction* m_toggleSelectedEntitiesColor; // 切换选择物体的颜色
	//! Context menu action: hide/show selected entities normals
	QAction* m_toggleSelectedEntitiesNormals; //切换选择物体的法向量
	//! Context menu action: hide/show selected entities materials/textures
	QAction* m_toggleSelectedEntitiesMat; //切换选择物体的矩阵
	//! Context menu action: hide/show selected entities SF
	QAction* m_toggleSelectedEntitiesSF; //切换选择物体的标量场
	//! Context menu action: hide/show selected entities 3D name
	QAction* m_toggleSelectedEntities3DName;//切换选择物体的3Dname
	//! Context menu action: add empty group
	QAction* m_addEmptyGroup;  //添加空的组
	//! Context menu action: use 3-points labels or planes to orient camera
	QAction* m_alignCameraWithEntity;  //对齐相机物体
	//! Context menu action: reverse of m_alignCameraWithEntity
	QAction* m_alignCameraWithEntityReverse; //
	//! Context menu action: enable bubble-view (on a sensor)
	QAction* m_enableBubbleViewMode;

	//! Last context menu pos
	QPoint m_contextMenuPos;  //菜单位置

};

#endif
