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

#ifndef CC_PMVS_CAM_OPEN_DLG_H
#define CC_PMVS_CAM_OPEN_DLG_H

#include"ui_openPMVSCamFileDlg.h"

//local
#include "qCC_io.h"

//Qt
#include <QString>
#include <QDialog>

//system
#include <vector>
#include <algorithm>

//! Dialog for configuration of ASCII files opening sequence
class QCC_IO_LIB_API PMVSCamOpenDlg : public QDialog, public Ui::openPMVSCamFileDialog
{
	Q_OBJECT

public:
	//! Default constructor
	/** \param parent parent widget
	**/
	 PMVSCamOpenDlg(QWidget* parent = 0);

	//! Default destructor
	virtual ~PMVSCamOpenDlg(){}
};

#endif