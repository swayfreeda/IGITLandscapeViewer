//##########################################################################
//#                                                                        #
//#                           IGITlANDSCAPEVIEWER                          #
//#                                                                        #
//#  This software is from the porject of automatic landscape 3D reconstr- #
//#  -uction and visulization from multiviews.                             #
//#                
//#                                                                        #
//#  AUTHOR: SUIWEI                                                        #
//#  DATE: 04/28/2015                                                      #
//#  ORGANIZATION: INSITITUTE OF AUTOMATION, CHINSE ACADEMY OF SCIENCES    #
//#  EMAIL: wsui@nlpr.ia.ac.cn                                             #
//#                                                                        #
//#  COPYRIGHT:      IGIT/NLPR/CASIA                                       #
//#                                                                        #
//##########################################################################

#ifndef CC_POINTCLOUD_PROGRESS_DLG_H
#define CC_POINTCLOUD_PROGRESS_DLG_H

#include"ui_PointCloudGenProcessDlg.h"

#include<qdialog.h>
#include<qthread.h>
#include<time.h>
#include<qdir.h>

class PointCloudGenProgressDlg;

/*
class PointCloudGenParams{

public:
	PointCloudGenParams():m_maxImgNum(15),m_minImgNum(3),m_pthreadNum(4),m_cSize(2),m_folderDir("out.nvm") {}

	PointCloudGenParams(int maxImgsNum, int minImgNum, int pthreadNum, int cSize, QString m_folderDir ):
	                       m_maxImgNum(maxImgsNum),m_minImgNum(minImgNum),m_pthreadNum(pthreadNum),m_cSize(cSize),m_folderDir(m_folderDir){}

    PointCloudGenParams(const PointCloudGenParams & params){

		m_maxImgNum = params.m_maxImgNum;
		m_minImgNum = params.m_minImgNum;
		m_pthreadNum = params.m_pthreadNum;
		m_cSize = params.m_cSize;
	    m_folderDir = params.m_folderDir;
	}

	PointCloudGenParams operator=(const PointCloudGenParams & params){
		m_maxImgNum = params.m_maxImgNum;
		m_minImgNum = params.m_minImgNum;
		m_pthreadNum = params.m_pthreadNum;
		m_cSize = params.m_cSize;
	    m_folderDir = params.m_folderDir;

	}
public:
	int m_maxImgNum;
	int m_minImgNum;
	int m_pthreadNum;
	int m_cSize;
	QString m_folderDir;
};
*/
// class PointCloudGenerator
class TextureGenrator:public QDialog
{
	Q_OBJECT
public:
	TextureGenrator(){}
	//TextureGenrator(const PointCloudGenParams & params): m_params(params){}
	//void setParams(const PointCloudGenParams & params){
	//  m_params = params;
	//}
	void setParentPtr(PointCloudGenProgressDlg*ptr){ m_parent = ptr;}
protected:

	 bool structureFromMotion();
	 bool densePointCloudReconstruction();
	 bool textureGeneration();
public slots:
	bool startReconstruction();
	
signals:
	//void outPutText(QString);
	//void processBarStepOn();
	//void updateAll();
private:
	//PointCloudGenParams m_params;
	PointCloudGenProgressDlg * m_parent;
};


// PointCloudGenProcessDlg
class PointCloudGenProgressDlg: public QDialog, public Ui::PointCloudGenProgressDlg
{
	Q_OBJECT
    friend class TextureGenrator;
public:
	PointCloudGenProgressDlg(QWidget* parent=0);

	uint getMaxImgNum(){return spinBoxMaxImgNum->value();}
	uint getMinImgNum(){return spinBoxMinImgNum->value();}
	uint getPthreads(){ return spinBoxPthreadsNum->value();}
	uint getCisze(){return spinBoxCSize->value();}
	QString getFolderDir(){ return lineEditFileDir->text();}
	QString getFullOutPath(){ return getFolderDir();}

public slots:
	void doActionGetFileDir();
	//void doActionStartReconstruction();
	void doActionPause();
	void doActionStop();

	void stepOne();
	//void outputText(QString str);
	//void updateAll();

private:
	QTimer* m_time;
	QString m_fullOutPath;

	TextureGenrator * m_TextureGenerator;
	QThread m_thread;
};

#endif


