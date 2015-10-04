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

#ifndef CC_POINT_CLOUD_GEN_DLG_H
#define CC_POINT_CLOUD_GEN_DLG_H

#include"ui_PointCloudGenDlg.h"

class PointCloudGenDlg: public QDialog, public Ui::PointCloudGenDlg
{
	Q_OBJECT

public:
	PointCloudGenDlg(QWidget* parent=0);
	QString getFolderDir(){ return m_outFolderDir;}

protected:
	void createImgList(QVector<QString>&imgDirs);
	
public slots:

	void doActionLoadImageFiles();
	void startReconstruction();
	void process(QListWidgetItem* item);

private:
	QString m_outFolderDir;
};





#endif