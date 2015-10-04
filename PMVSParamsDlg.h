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
#ifndef CC_PMVS_PARAMS_DLG_H
#define CC_PMVS_PARAMS_DLG_H

#include<qdialog.h>
#include<qwidget.h>

#include"ui_PMVSParamsDlg.h"


class PMVSParamsDlg: public QDialog, public Ui::PMVSParamsDlg{

  Q_OBJECT


public:

	PMVSParamsDlg(QWidget* parent=0);

	uint getMaxImgNum(){return spinBoxMaxImgNum->value();}
	uint getMinImgNum(){return spinBoxMinImgNum->value();}
	uint getPthreads(){ return spinBoxPthreadsNum->value();}
	uint getCisze(){return spinBoxCSize->value();}
	QString getFolderDir(){ return lineEditFolderDir->text();}

public slots:
	void doActionGetFolderDir();
 

};


#endif