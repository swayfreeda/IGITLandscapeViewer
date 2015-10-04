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

#ifndef IGIT_MAIN_APP_INTERFACE_HEADER
#define IGIT_MAIN_APP_INTERFACE_HEADER

//Qt
#include <QString>

//qCC_db
#include <ccHObject.h>

class QMainWindow;
class ccGLWindow;
class ccColorScalesManager;

//! Main application interface (for plugins)
class igitMainAppInterface
{
public:

	//! Returns main window
	virtual QMainWindow* getMainWindow(){ return 0;}

	//! Returns active GL sub-window (if any)
	virtual ccGLWindow* getActiveGLWindow() { return 0;}

	//! Returns the unique ID generator
	virtual ccUniqueIDGenerator::Shared getUniqueIDGenerator() {
		ccUniqueIDGenerator::Shared tmp; 
		return tmp;
	}

	//! Adds an entity to main db
	/** \param obj entity
		\param updateZoom updates active GL display zoom to fit the whole scene, including this new entity (addToDisplay must be true)
		\param autoExpandDBTree whether DB tree should automatically be expanded
		\param checkDimensions whether to check entity's dimensions (and potentially asking the user to shift/rescale it) or not
	**/
	virtual void addToDB(	ccHObject* obj,
							bool updateZoom = false,
							bool autoExpandDBTree = true,
							bool checkDimensions = false ) {}

	//! Removes an entity from main db tree
	/** Object is automatically detached from its parent.
		\param obj entity
		\param autoDelete automatically deletes object
	**/
	virtual void removeFromDB(ccHObject* obj, bool autoDelete = true){}

	//! Selects or unselects an entity (in db tree)
	/** \param obj entity
		\param selected whether entity should be selected or not
	**/
	virtual void setSelectedInDB(ccHObject* obj, bool selected) {}

	//! Returns currently selected entities ("read only")
	virtual const ccHObject::Container& getSelectedEntities() const{ 
		ccHObject::Container tmp; 
		return tmp;
	}

	//! Console message level (see dispToConsole)
	enum ConsoleMessageLevel {	STD_CONSOLE_MESSAGE = 0,
								WRN_CONSOLE_MESSAGE = 1,
								ERR_CONSOLE_MESSAGE = 2,
	};

	//! Prints a message to console
	/** \param message message
		\param level message level (standard, warning, error)
	**/
	virtual void dispToConsole(QString message, ConsoleMessageLevel level = STD_CONSOLE_MESSAGE) {}

	//! Forces display of console widget
	virtual void forceConsoleDisplay(){}

	//! Returns DB root (as a ccHObject)
	virtual ccHObject* dbRootObject(){ return 0;}

	//! Forces redraw of all GL windows
	/** \param only2D whether to redraw everything (false) or only the 2D layer (true)
	**/
	virtual void redrawAll(bool only2D = false){}

	//! Redraws all GL windows that have the 'refresh' flag on
	/** See ccGLWindow::toBeRefreshed and ccDrawableObject::prepareDisplayForRefresh.
		\param only2D whether to redraw everything (false) or only the 2D layer (true)
	**/
	virtual void refreshAll(bool only2D = false){}

	//! Enables all GL windows
	virtual void enableAll(){}

	//! Disables all GL windows
	virtual void disableAll(){}

	//! Disables all GL windows but the specified one
	virtual void disableAllBut(ccGLWindow* win){}

	//! Updates UI (menu and properties browser) to reflect current selection state
	/** This method should be called whenever a change is made to any selected entity
	**/
	virtual void updateUI(){}

	//! Freezes/unfreezes UI
	/** \param state freeze state
	**/
	virtual void freezeUI(bool state){}

	//! Returns color scale manager (unique instance)
	virtual ccColorScalesManager* getColorScalesManager(){ return 0;}

	//! Spawns an histogram dialog
	virtual void spawnHistogramDialog(	const std::vector<unsigned>& histoValues,
										double minVal,
										double maxVal,
										QString title,
										QString xAxisLabel ){}

	//other usefull methods
	virtual void setFrontView(){}
	virtual void setBottomView(){}
	virtual void setTopView(){}
	virtual void setBackView(){}
	virtual void setLeftView(){}
	virtual void setRightView(){}
	virtual void toggleActiveWindowCenteredPerspective(){}
	virtual void toggleActiveWindowCustomLight(){}
	virtual void toggleActiveWindowSunLight(){}
	virtual void toggleActiveWindowViewerBasedPerspective(){}
	virtual void zoomOnSelectedEntities(){}

};

#endif //IGIT_MAIN_APP_INTERFACE_HEADER
