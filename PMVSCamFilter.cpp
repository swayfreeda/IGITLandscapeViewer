
#include"PMVSCamFilter.h"
#include"ccCameraSensor.h"
#include"PMVSCamOpenDlg.h"

#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/Dense>


//qt
#include <qdebug.h>
#include<QTableWidgetItem>

// obtains important information from projection matrix decomposition,
/** params
    -input: proj 3*4 projection matrix
	-output: 
	        position -- spatial position of the camera
			axisX, axisY, axisZ  -- the axies of the camera coordinate system
			focal -- the focal of the camera
**/
void decomposePojectionMatix(Eigen::Matrix<float, 3, 4> & proj, 
	                         Eigen::Vector3f & position,
							 Eigen::Vector3f & axisX,
							 Eigen::Vector3f & axisY,
							 Eigen::Vector3f & axisZ,
							 float &focal){
	//get position of the camera
	Eigen::Matrix3f KR;
	for(int i=0; i< 3; i++){
		for(int j=0; j< 3; j++){
			KR(i, j) = proj(i,j);
		}
	}
	Eigen::Vector3f KT;
	for(int i=0;  i< 3; i++) KT(i)= proj(i,3);
	position = -KR.inverse() * KT;

	//get the focal of the camera
	Eigen::Vector3f R0, R1, R2;
	for(int i=0; i<3; i++){
		R0(i) = KR(0, i);
		R1(i) = KR(1, i);
		R2(i) = KR(2, i);
	}
	focal = 0.5* (R0.cross(R2)).norm() + 0.5*(R1.cross(R2)).norm();

	//get the axises of the camera
	for(int i=0; i< 3; i++){
		axisZ(i) = proj(2,i);
	}
	axisZ.normalize();

	axisY = axisZ.cross(R0);
	axisY.normalize();

	axisX = axisY.cross(axisZ);
	axisX.normalize();
}

//=============================================canLoadExtension==================================================//
bool PMVSCamFilter::canLoadExtension(QString upperCaseExt) const{
	return (upperCaseExt == "TXT");
}

//=============================================canSave===========================================================//
bool PMVSCamFilter::canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const
{
	if ( type == CC_TYPES::CAMERA_SENSOR			//only one cloud per file
		||	type == CC_TYPES::HIERARCHY_OBJECT ){	//but we can also save a group (each cloud inside will be saved as a separated file)
		multiple = true;
		exclusive = true;
		return true;
	}
	return false;
}

//=============================================saveToFile========================================================//
CC_FILE_ERROR PMVSCamFilter::saveToFile(ccHObject* entity, QString filename,
	                                          SaveParameters& parameters){
   return CC_FERR_NO_ERROR;
}

//=============================================LoadFile==========================================================//
CC_FILE_ERROR PMVSCamFilter::loadFile(QString filename,
									ccHObject& container,
									LoadParameters& parameters){

	QFile file(filename);
	if(!file.exists()) return CC_FERR_READING;

	qint64  fileSize = file.size();
	if( fileSize==0) return CC_FERR_NO_LOAD;
	if(!file.open(QFile::ReadOnly)) return CC_FERR_READING;

	ccCameraSensor* sensor = new ccCameraSensor();

	 PMVSCamOpenDlg dialog;
	if(parameters.alwaysDisplayLoadDialog){  
	   dialog.lineEditFileName->setText(filename);
	}

	//name
	QString sensorname("");
	QStringList words = filename.split(".");
	if(words.size()>0) sensorname = words[0];
	if(words.size()==0){sensorname = QString("Temp %1").arg(container.getChildrenNumber()); }
	
	sensor->setName(filename);
	QTextStream in(&file);

	int lineNum = 0;
	Eigen::Matrix<float, 3, 4> proj;
	while(!in.atEnd()){
		QString line = in.readLine();
	    QStringList values= line.split(" ");
	
		if(line.startsWith("CONTOUR")){
			if(parameters.alwaysDisplayLoadDialog){  
				QTableWidgetItem *item = new QTableWidgetItem();
				item->setText("CONTOUR");
			    dialog.tableWidgetContext->setItem(0, 0, item);
			}
			lineNum++;
			continue;	
		}
		values= line.split(" ");
		for(int i=0; i< 4; i++){
			proj(lineNum-1, i) = values.takeFirst().toFloat();
		}
		values= line.split(" ");
		if(parameters.alwaysDisplayLoadDialog){  
			int count =0;
			while(values.size()>0){
				QTableWidgetItem *item = new QTableWidgetItem();
				item->setText(values.takeFirst());
			    dialog.tableWidgetContext->setItem(lineNum, count, item);
			    count++;
			}
		}
		lineNum ++;
	}

	//get camera information from matrix decompostion
	float focal=0;
	Eigen::Vector3f pos, axisX, axisY, axisZ;
	decomposePojectionMatix(proj, pos, axisX, axisY, axisZ, focal);

	if(parameters.alwaysDisplayLoadDialog){  
	    //x Axis
	    dialog.x1rot->setText(QString("%1").arg(axisX(0)));
	    dialog.x2rot->setText(QString("%1").arg(axisX(1)));
	    dialog.x3rot->setText(QString("%1").arg(axisX(2)));

	    dialog.y1rot->setText(QString("%1").arg(axisY(0)));
	    dialog.y2rot->setText(QString("%1").arg(axisY(1)));
	    dialog.y3rot->setText(QString("%1").arg(axisY(2)));

	    dialog.z1rot->setText(QString("%1").arg(axisZ(0)));
	    dialog.z2rot->setText(QString("%1").arg(axisZ(1)));
	    dialog.z3rot->setText(QString("%1").arg(axisZ(2)));

	    dialog.lineEditFocal->setText(QString("%1").arg(focal));
	    dialog.posXLineEdit->setText(QString("%1").arg(pos(0)));
	    dialog.posYLineEdit->setText(QString("%1").arg(pos(1)));
	    dialog.posZLineEdit->setText(QString("%1").arg(pos(2)));
	}
	
	if(parameters.alwaysDisplayLoadDialog && !dialog.exec()){
	    return CC_FERR_CANCELED_BY_USER; //不在往下执行
	}
	QCoreApplication::processEvents();	

	//}
	
	// initialize the camera
	 ccGLMatrix rot;
	{
		float* mat = rot.data();
		mat[0]  = -axisX(0);
		mat[1]  = -axisX(1);
		mat[2]  = -axisX(2);

		mat[4]  = -axisY(0);
		mat[5]  = -axisY(1);
		mat[6]  = -axisY(2);

		mat[8]  = -axisZ(0);
		mat[9]  = -axisZ(1);
		mat[10] = -axisZ(2);
	}
	
	CCVector3 C(static_cast<PointCoordinateType>(pos(0)),
				static_cast<PointCoordinateType>(pos(1)),
				static_cast<PointCoordinateType>(pos(2)));

	//设置平移向量
	rot.setTranslation(C);
	sensor->setRigidTransformation(rot);

	ccCameraSensor::IntrinsicParameters iParams;
	iParams.focal_pix = static_cast<float>(1); //焦距 // 计算投影矩阵的时候需要格外注意
	iParams.vFOV_rad = static_cast<float>(45 * CC_DEG_TO_RAD); //视场角
	iParams.arrayWidth = 800; //图像宽度
	iParams.arrayHeight = 600;//图像高度
	iParams.pixelSize_mm[0] = static_cast<float>(1); //像素大小
	iParams.pixelSize_mm[1] = static_cast<float>(1);//像素大小
	iParams.zNear_mm = static_cast<float>(0); //近侧裁剪面
	iParams.zFar_mm = static_cast<float>(1); //远侧裁剪面
	iParams.skew = static_cast<float>(0);  //错切稀疏
	sensor->setIntrinsicParameters(iParams);  //设置内参数

	ccIndexedTransformation trans;
	sensor->addPosition(trans,0); // 将当前位置添加到PosBuffer缓冲器中
	sensor->drawFrustrumPlanes(true);

	container.addChild(sensor);

	return CC_FERR_NO_ERROR;

}


