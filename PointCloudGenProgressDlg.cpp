
#include"PointCloudGenProgressDlg.h"
#include "BinFilter.h" //without this Shellapi.h will result in errors

#include"texture_generation.h"

#include <qfiledialog.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>
#include <qtimer.h>
#include<Shellapi.h>
#include <tchar.h>

#include<Windows.h>

////typedef void(*Dllfun)(void);

//===============================================makeCongigureFile========================================================//
void makeConfigurationFile(int minImgNum, int maxImgNum, int phreadsNum, int cSize){

	QFile file("nv.ini");
	if(!file.open(QIODevice::WriteOnly)) return;
	QTextStream out(&file);

	out<<"############################################" <<endl;
	out<<"#Default ratio between focal length and max(width, height)"<<endl;
	out<<"#This parameter is very useful for wide angle images"<<endl;
	out<<"#Change this parameter before launching VisualSFM"<<endl;
	out<<"param_default_focal_ratio "<< 1.2<<endl;
	out<<"#Change this to 0 if the focal length from EXIF is too bad"<<endl;
	out<<"param_use_exif_focal_length "<< 1<<endl;
	out<<"#Enable radial distortion (1 to enable, 0 to disable)"<<endl;
	out<<"param_radial_distortion "<< 1 <<endl;

	out<<"#Switch to preemptive matching mode when the number of images is large than"<<endl;
    out<<"#param_preemptive_switch and preemptive matching is enabled" <<endl;
    out<<"#(+subset for command-line or SfM->Pairwise Matching->Use Preemptive Matching)"<<endl;
	out<<"param_preemptive_switch "<< 1000<<endl;
	out<<"#Number of features used in preemptive matching (h in the 3DV2013 paper)"<<endl;
	out<<"param_preemptive_size "<< 100 <<endl;
	out<<"#Threshold for preemptive matching (t_h in the 3DV2013 paper)"<<endl;
	out<<"param_preemptive_min "<< 4<<endl;

	out<<"#Bundle adjustment configuration"<<endl;
	out<<"param_bundle_full_step " << 5<<endl;
    out<<"param_bundle_full_iteration "<< 100<<endl;
    out<<"param_bundle_part_size "<< 20 <<endl;
    out<<"param_bundle_part_iteration "<< 5<<endl;
    out<<"param_bundle_stop_mse "<< 0.25<<endl;
    out<<"param_pba_prefer_gpu "<< 1<<endl;
    out<<"param_pba_use_schur "<< 0 <<endl;

	out<<"#Skip visualization data as much as possible (use 1 to save memory)"<<endl;
    out<<"param_less_visualization_data "<< 0 <<endl;
    out<<"#The number of images that allows thumbnail textures and pixel loading."<<endl;
	out<<"param_visual_data_budget "<<500<<endl;
    out<<"#Enable full resolution rendering for resolution larger than 4096."<<endl;
    out<<"param_enable_huge_texture "<< 0 <<endl;

	out<<"#The number of threads for image undistortion (0 means one-thread per core)"<<endl;
    out<<"param_undistortion_thread_num "<< 0 <<endl;
    out<<"#Parameters for using CMVS/PMVS (by Yasutaka Furukawa)" <<endl;
    out<<"param_cmvs_max_images " <<maxImgNum<<endl;
    out<<"param_pmvs_min_image "<<minImgNum<<endl;
    out<<"param_pmvs_level "<< 1 <<endl;
    out<<"param_pmvs_csize "<< cSize <<endl;
    out<<"param_pmvs_threshold "<< 0.7<<endl;
    out<<"param_pmvs_wsize "<< 7 <<endl;
    out<<"param_pmvs_max_angle "<<10<<endl;
    out<<"#Thread parameter for PMVS: 0 means one-thread per core."<<endl;
    out<<"param_pmvs_max_cpus "<< 0 <<endl;

    out<<"#GPS variance used for GPS-based 3D transformation (beta)"<<endl;
    out<<"param_gps_variance "<< 10 <<endl;
    out<<"param_gps_ignore_exif_altitude  "<<1<<endl;
    out<<"#Do not transform the NVM or PLY. Save the transformation to aln instead."<<endl;
    out<<"param_write_transformation_to_aln "<< 0 <<endl;

    out<<"#Thresholds for triangulating a 3D point"<<endl;
    out<<"param_triangle_angle_min_new "<<4<<endl;
    out<<"param_triangle_angle_min " << 2 <<endl;
    out<<"param_forward_motion_threshold "<< 2 <<endl;
    out<<"#Threshold for adding a point to existing track and making new tracks "<<endl;
    out<<"param_projection_threshold " << 320 <<endl;
    out<<"param_new_track_threshold "<< 320 <<endl;
    out<<"param_merge_track_threshold "<< 32 <<endl;
    out<<"param_retriangulate_threshold "<< 3200 <<endl;
    out<<"#Threshold for removing tracks that have large projection errors"<<endl;
    out<<"param_track_threshold " << 8 <<endl;
    out<<"#Filter unstable points (small triangulation angle)" <<endl;
    out<<"param_remove_unstable_tracks "<< 1 <<endl;

    out<<"#Threshold for the number of projections when adding a new camera" <<endl;
    out<<"param_new_cam_projection_min "<< 20 <<endl;
    out<<"#Threshold for pose estimation "<<endl;
    out<<"param_pose_estimate_threshold "<< 64<<endl;
    out<<"#Adjust pose estimation threshold according to focal lengths"<<endl;
    out<<"param_pose_estimate_adaptive "<< 1 <<endl;
    out<<"#Consider very large focal length as wrong"<<endl;
    out<<"param_pose_estimate_ratio_max  "<< 10 <<endl;
    out<<"param_p3p_inlier_ratio_min "<< 0.4 <<endl;
    out<<"param_pose_inlier_ratio_min "<< 0.2 <<endl;

    out<<"#The minimum inlier matches for a pair to be considered correct"<<endl;
    out<<"param_pair_inlier_num_min "<< 15 <<endl;
    out<<"#Distance threshold in meter for pair-wise image matching (0 to disable)"<<endl;
    out<<"param_pair_gps_threshold "<< 0 <<endl;
    out<<"#Aggressively include all putative matches in reconstruction"<<endl;
    out<<"param_aggressive_triangulation "<< 0 <<endl;
 
    out<<"#The thresholds for selecting a starting pair"<<endl;
    out<<"#Homography threshold : the maximum percent of points on a plane"<<endl;
    out<<"param_init_h_ratio_max "<< 0.4 <<endl;
    out<<"#Inlier count threshold : the minimum number of matches"<<endl;
    out<<"param_init_inlier_min "<< 30 <<endl;
    out<<"#Triangulation angle threshold: minimum average triangulation angle"<<endl;
    out<<"param_init_angle_min "<< 2 <<endl;

    out<<"#Reconstruct one model or multiple?"<<endl;
    out<<"param_search_multiple_models "<< 1 <<endl;
    out<<"#Maximum images shared by two 3D models "<<endl;
    out<<"param_image_reuse_max "<< 10 <<endl;

    out<<"#Save a temporary NVM file when <Reconstruct Sparse> finishes."<<endl;
    out<<"param_model_auto_saving "<< 0 <<endl;
    out<<"#Write the combined PLY file in ASCII or BINARY format"<<endl;
    out<<"param_write_ascii_ply "<< 0 <<endl;
    out<<"#Write camera indices in PLY file"<<endl;
    out<<"param_write_cameras_in_ply "<< 0 <<endl;
    out<<"#Parameters for Poisson Surface Reconstruction"<<endl;
    out<<"param_poisson_arguments --depth "<< 10 <<endl;

    out<<"#Select the tool for feature detection"<<endl;
    out<<"#0: CPU (depending on param_use_vlfeat_or_lowe),"<<endl;
    out<<"#1: GLSL-based SiftGPU, 2 CUDA-based SiftPGU,"<<endl;
    out<<"#3: customized SiftGPU (depending on param_use_siftgpu_customize)"<<endl;
    out<<"param_use_siftgpu "<< 1 <<endl;
    out<<"param_use_siftgpu_customize -fo -1 -v 0 -tc2 7680 -nomc"<<endl;
    out<<"#Use the original binary from Lowe if 0, and VLFeat binary otherwise"<<endl;
    out<<"param_use_vlfeat_or_lowe "<< 0 <<endl;
    out<<"#Options for vlfeat sift (--peak-thresh=3.4 seems to be compatible with Lowe)"<<endl;
    out<<"param_vlfeat_options --peak-thresh=3.4"<<endl;
 
    out<<"#Select the device for feature matching"<<endl; 
    out<<"#0: cpu, 1: glsl, 2+: cuda if compiled in SiftGPU.dll"<<endl;
    out<<"param_use_siftmatchgpu "<< 1 <<endl; 
    out<<"param_cpu_siftmatch_num_thread "<< 2 <<endl;

    out<<"#Feature matching parameters"<<endl;
    out<<"#Max features used in gpu sift matching"<<endl;
    out<<"param_gpu_match_fmax "<< 8192 <<endl;
    out<<"#Max sift matching distance"<<endl;
    out<<"param_sift_max_dist" << 0.7 <<endl;
    out<<"#Max sift matching distance ratio"<<endl;
    out<<"param_sift_max_dist_ratio "<< 0.8 <<endl;
    out<<"#Mutual best match or not"<<endl;
    out<<"param_siftmatch_mbm "<< 1 <<endl;
    out<<"#Filter the stationary point matching"<<endl;
    out<<"param_no_stationary_points "<< 0 <<endl;

    out<<"#Two-view geometry estimation parameters"<<endl;
    out<<"#Threshold for estimating F in ransac"<<endl;
    out<<"param_ransac_f_threshold "<< 2 <<endl;
    out<<"#Threshold for estimating H in ransac"<<endl;
    out<<"param_ransac_h_threshold " << 4 <<endl;
    out<<"#Maximum iterations in ransac"<<endl;
    out<<"param_ransac_max_sample "<< 1000 <<endl;

    out<<"#Disable GPU-based computation when remote desktop is detected"<<endl;
    out<<"#Change this to 0 if VisualSFM incorrectly detects remote desktop"<<endl;
    out<<"param_check_remote_desktop "<<1<<endl;
    out<<"#Write matches asynchronously, which may be faster for NFS."<<endl;
    out<<"param_asynchronous_write "<< 0 <<endl;
    out<<"#Limit the number of worker threads (experimental)"<<endl;
    out<<"param_maximum_cpu_usage "<< 0 <<endl;
    out<<"#Flush stdout for each line of log when set to 1"<<endl;
    out<<"param_stdout_line_flush "<< 0 <<endl;
    out<<"#When set to 1, the program will try to be deterministic by using"<<endl;
    out<<"#fixed seed for randomness, and disable threading in a few places"<<endl;
    out<<"param_deterministic_behaviour " << 0 <<endl;

    out<<"#Configuration version"<<endl;
    out<<"param_config_version "<< 51 <<endl;

}


//================================================c2w=====================================================================//
//convert char to wchar
wchar_t* c2w(const char *str)
{
    int length = strlen(str)+1;
    wchar_t *t = (wchar_t*)malloc(sizeof(wchar_t)*length);
    memset(t,0,length*sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP,0,str,strlen(str),t,length);
    return t;
}

//===============================================ShellExicute==========================================================//
bool shellExecute(const char * file, const char *params){

	   wchar_t * Wfile = c2w(file);
	   wchar_t * Wparams = c2w(params);

	   SHELLEXECUTEINFO ShellInfo;        
       memset(&ShellInfo, 0, sizeof(ShellInfo));
       ShellInfo.cbSize = sizeof(ShellInfo);
       ShellInfo.hwnd = NULL;
       ShellInfo.lpVerb = NULL;
       ShellInfo.lpFile = Wfile; // L"VisualSFM.exe"  L前缀将常量char 转化成 wchar
	   ShellInfo.lpParameters = Wparams;
       ShellInfo.nShow = SW_HIDE;
       ShellInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
     
       if( ShellExecuteEx(&ShellInfo)) {
	     WaitForSingleObject(ShellInfo.hProcess,INFINITE);
		 return true;
	   }
	   else{
		   return false;
	   }

}

//===============================================structureFromMotion======================================================//
bool TextureGenrator::structureFromMotion(){


	int start = 0; 
	int end = 0;
	//start to reconstruction
	
	m_parent->progressBar->setRange(0,0);
	m_parent->m_time->setInterval(10);
	m_parent->m_time->start();
	connect(m_parent->m_time,                        SIGNAL(timeout()),             m_parent,         SLOT(stepOne()));

	//run structrure from motion of VisualSFM.exe
	m_parent->textEditConsole->append("正在进行稀疏重建，请稍候...");
	m_parent->update();
	start = clock();
	
	QString command = " sfm+cmvs imgList.txt TempData";
	if(!shellExecute("VisualSFM.exe", command.toStdString().c_str())){
      m_parent->textEditConsole->append("稀疏重建出现错误");
		 return false;
	}
	end = clock();
	m_parent->textEditConsole->append(QString("稀疏重建完成,用时 %1 s\n").arg((double)(end-start)/CLOCKS_PER_SEC));
	m_parent->m_time->disconnect();
	m_parent->m_time->stop();

	return true;
}

//===============================================densePointCloudReconstruction============================================//
bool TextureGenrator::densePointCloudReconstruction(){

	int start = 0; 
	int end = 0;

	m_parent->textEditConsole->append("正在进行稠密点云重建，请稍候...");
	 //run PMVS for dense reconstruction
	QString path = "TempData" + tr(".nvm.cmvs/00/");
	start = clock();
	// find all the files
	QDir dir;
	dir.setPath(path);
	dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	QStringList filter;
	filter<<"option-*";
	QFileInfoList fileList = dir.entryInfoList(filter);
	int nOpts = fileList.size();
	m_parent->progressBar->setRange(0,nOpts);
	m_parent->progressBar->setValue(0);

	for(int i=0; i< nOpts; i++){
		// option files
		QString optFile = fileList.at(i).filePath();
		QStringList fields= optFile.split("/");

		// commad line arguments
		QString cmdArgu = path +" " + fields.takeLast();
		m_parent->textEditConsole->append( QString("%1 th reconstruction ").arg(i));
		m_parent->textEditConsole->append( dir.currentPath()+ "/"  + optFile);
		if(!shellExecute("pmvs2.exe", cmdArgu.toStdString().c_str()))return false;

		m_parent->progressBar->setValue(i+1);
		QApplication::processEvents();
	} 
	end = clock();
	m_parent->textEditConsole->append(QString("稠密重建完成,用时 %1 s\n").arg((double)(end-start)/CLOCKS_PER_SEC));
	m_parent->textEditConsole->append("重建结果保存在 " + dir.currentPath()+ "/" + path);
	update();

	return true;
}

//===============================================textureGeneration()======================================================//
bool TextureGenrator::textureGeneration(){

#if 0
	Dllfun textureGeneration;

    HINSTANCE hdll;
    hdll=LoadLibrary(L"TextureGeneration.dll");
    if(hdll==NULL) {
	  FreeLibrary(hdll);
    }
     textureGeneration=(Dllfun)GetProcAddress(hdll,"textureGeneration");
     if(textureGeneration==NULL) {
	 FreeLibrary(hdll);
	 QMessageBox::warning(this, "警告", "纹理模型生成发生错误");
	 return false;
  }

    textureGeneration();
#endif

  QDir dir;
  if(!dir.exists("TempData.nvm.cmvs/00")) return false;
   runTextureGeneration("./TempData.nvm.cmvs/00", m_parent->getFolderDir(),
	                    m_parent->getPthreads(),
						1024*4,
						m_parent->textEditConsole, m_parent->progressBar);

	return true;
}

//===============================================startReconstruction======================================================//
bool TextureGenrator::startReconstruction(){

	//check the image informattion
	if(!QFile::exists("imgList.txt")){
		QMessageBox::warning(this, "警告", "没有检测到图像信息，重建加载图像");
		return false;
	}

	// output Information
	m_parent->textEditConsole->append("输入参数：");
	m_parent->textEditConsole->append(QString("最大图像个数：%1").arg(m_parent->getMaxImgNum()));
	m_parent->textEditConsole->append(QString("最小图像个数：%1").arg(m_parent->getMinImgNum()));
	m_parent->textEditConsole->append(QString("CPU     个数：%1").arg(m_parent->getPthreads()));
	m_parent->textEditConsole->append(QString("窗 口  大 小：%1").arg(m_parent->getCisze()));
	m_parent->textEditConsole->append(QString("模型文件路径：%1").arg(m_parent->getFolderDir()));
	m_parent->textEditConsole->append("\n");
	m_parent->update();


	 // run SFM
	m_parent->textEditConsole->append("开始稀疏点云重建");
	m_parent->labelIconStart->setEnabled(true);
	if(!structureFromMotion()){
		 QMessageBox::warning(this, "警告", "稀疏点云重建发生错误");
		 return false;
	 }
	m_parent->labelIconSFM->setEnabled(true);
	m_parent->update();

	// run PMVS
	m_parent->textEditConsole->append("开始稠密重建");
	if(!densePointCloudReconstruction()){
		 QMessageBox::warning(this, "警告", "稠密点云重建发生错误");
		 return false;
	}
	m_parent->labelIconPmvs->setEnabled(true);
	m_parent->textEditConsole->append("\n");
	m_parent->update();

	  // run Texture Generation
	  m_parent->textEditConsole->append("开始纹理模型生成");
	  m_parent->update();

	  if(!textureGeneration()){
		 QMessageBox::warning(this, "警告", "纹理模型生成错误");
	  	 return false;
	  }
	  m_parent->labelIconTextureGeneration->setEnabled(true);
	  m_parent->textEditConsole->append("\n");
	  m_parent->update();
	

	//******************************************************************//
	/******************点云进行融合和下采样                          */
	//*******************************************************************//
	m_parent->labelIconCompleted->setEnabled(true);
	m_parent->update();

	return true;
}


//===================================================PointCloudProgressDlg==============================================//
PointCloudGenProgressDlg::PointCloudGenProgressDlg(QWidget* parent)
{
  setupUi(this);
  progressBar->setValue(0);
  labelIconStart->setEnabled(false);
  labelIconSFM->setEnabled(false);
  labelIconPmvs->setEnabled(false);
  labelIconCompleted->setEnabled(false);
  labelIconTextureGeneration->setEnabled(false);

  m_time = new QTimer();
  m_TextureGenerator = new TextureGenrator();
  m_TextureGenerator->setParentPtr(this);

  connect(pushButtonFileDir,             SIGNAL(clicked()),             this,         SLOT(doActionGetFileDir()));
  connect(pushButtonPause,               SIGNAL(clicked()),             this,         SLOT(doActionPause()));
  connect(pushButtonStop,                SIGNAL(clicked()),             this,         SLOT(doActionStop()));
 
  //connect(m_TextureGenerator,            SIGNAL(outPutText(QString)),   this,         SLOT(outputText(QString)));
  //connect(m_TextureGenerator,            SIGNAL(processBarStepOn()),    this,         SLOT(stepOne()));
  //connect(m_TextureGenerator,            SIGNAL(updateAll()),           this,         SLOT(updateAll()));
  connect(pushButtonStartReconstruction, SIGNAL(clicked()),             m_TextureGenerator,         SLOT(startReconstruction()),  Qt::QueuedConnection);
  m_TextureGenerator->moveToThread(&m_thread);
  m_thread.start();

  update();
}


//===================================================doActionGetFileDir================================================//
void PointCloudGenProgressDlg::doActionGetFileDir(){
	QString dir =  QFileDialog::getSaveFileName(this,"保存路径","","*.nvm");
	lineEditFileDir->setText(dir);
	update();
}

////===================================================doActionStartReconstruction================================================//
//void PointCloudGenProgressDlg::doActionStartReconstruction()
//{
//	//check the image informattion
//	if(!QFile::exists("imgList.txt")){
//		QMessageBox::warning(this, "警告", "没有检测到图像信息，重建加载图像");
//		return;
//	}
//	int start = 0; 
//	int end = 0;
//
//	//get the parameters 
//	int maxImgNum = getMaxImgNum(); 
//	int minImgNum = getMinImgNum();
//	int pthreadNum = getPthreads();
//	int cSize = getCisze();
//	QString folderDir = getFolderDir();
//
//	// make configuration file for VisualSFM
//	makeConfigurationFile(minImgNum, maxImgNum, pthreadNum, cSize);
//
//	//显示参数
//	textEditConsole->append("输入参数：");
//	textEditConsole->append(QString("最大图像个数：%1").arg(maxImgNum));
//	textEditConsole->append(QString("最小图像个数：%1").arg(minImgNum));
//	textEditConsole->append(QString("CPU     个数：%1").arg(pthreadNum));
//	textEditConsole->append(QString("窗 口  大 小：%1").arg(cSize));
//	textEditConsole->append(QString("文件路径：%1").arg(QDir::currentPath()+ "/" + folderDir));
//	textEditConsole->append("\n");
//	update();
//
//	//start to reconstruction
//	textEditConsole->append("开始重建");
//	labelIconStart->setEnabled(true);
//	progressBar->setRange(0,0);
//	m_time->setInterval(10);
//	m_time->start();
//	connect(m_time,                        SIGNAL(timeout()),             this,         SLOT(stepOne()));
//	
//	//run structrure from motion of VisualSFM.exe
//	textEditConsole->append("正在进行稀疏重建，请稍候...");
//	update();
//	start = clock();
//	
//	QString command = " sfm+cmvs imgList.txt TemplateData";
//	if(!shellExecute("VisualSFM.exe", command.toStdString().c_str())){
//      textEditConsole->append("稀疏重建出现错误");
//			return;
//	}
//	end = clock();
//	textEditConsole->append(QString("稀疏重建完成,用时 %1 s\n").arg((double)(end-start)/CLOCKS_PER_SEC));
//	labelIconSFM->setEnabled(true);
//	update();
//	m_time->disconnect();
//	m_time->stop();
//#if 1
//	// run pmvs
//	QString path = folderDir + tr(".cmvs/00/");
//	textEditConsole->append("开始稠密重建");
//	start = clock();
//	// find all the files
//	QDir dir;
//	dir.setPath(path);
//	dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
//	QStringList filter;
//	filter<<"option-*";
//	QFileInfoList fileList = dir.entryInfoList(filter);
//	int nOpts = fileList.size();
//	progressBar->setRange(0,nOpts);
//	progressBar->setValue(0);
//
//	for(int i=0; i< nOpts; i++){
//		// option files
//		QString optFile = fileList.at(i).filePath();
//		QStringList fields= optFile.split("/");
//
//		// commad line arguments
//		QString cmdArgu = path +" " + fields.takeLast();
//		textEditConsole->append( QString("%1 th reconstruction ").arg(i));
//		textEditConsole->append( dir.currentPath()+ "/"  + optFile);
//		if(!shellExecute("pmvs2.exe", cmdArgu.toStdString().c_str()))return;
//
//		progressBar->setValue(i+1);
//		QApplication::processEvents();
//	} 
//	end = clock();
//	textEditConsole->append(QString("稠密重建完成,用时 %1 s\n").arg((double)(end-start)/CLOCKS_PER_SEC));
//	textEditConsole->append("重建结果保存在 " + dir.currentPath()+ "/" + path);
//	labelIconPmvs->setEnabled(true);
//	update();
//
//
//	//******************************************************************//
//	/******************点云进行融合和下采样                          */
//	//*******************************************************************//
//	labelIconCompleted->setEnabled(true);
//	update();
//#endif
//}

//===================================================doActionPause================================================//
void PointCloudGenProgressDlg::doActionPause()
{
	if(m_thread.isRunning())
	{
		//m_thread.wait(10000000000);	
	}
	else{
		//m_thread.start();
	}
}

//===================================================doActionGetFileStop================================================//
void PointCloudGenProgressDlg::doActionStop()
{
	QDir dir;
	QString folderDir = getFolderDir();
	QString dirPath = folderDir + ".cmvs";
	if(dir.exists(dirPath))	{
		dir.remove(dirPath);
	}
	m_thread.terminate();
	close();
}

//===================================================stepOne===========================================================//
void PointCloudGenProgressDlg::stepOne(){

  if(progressBar->value() == progressBar->maximum()){
	  progressBar->setValue(0);
  }
  else{
	  progressBar->setValue(progressBar->value() +1);
  }
  QApplication::processEvents();
}

//================================================outputText==========================================================//
//void PointCloudGenProgressDlg::outputText(QString str){
//	textEditConsole->append(str);
//	update();
//}

//================================================updateALL===========================================================//
//void PointCloudGenProgressDlg::updateAll(){
//	update();
//}