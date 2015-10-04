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

#include <ccIncludeGL.h>

//Qt
#include <qapplication.h>
#include <qsplashscreen>
#include <qpixmap>
#include <qmessagebox.h>
#include <qlocale.h>
#include<qtimer.h>
#include<qdatetime.h>

//qCC_db
#include <ccTimer.h>
#include <ccNormalVectors.h>
#include <ccColorScalesManager.h>


//qCC_io
#include <FileIOFilter.h>

#include "mainwindow.h"
#include "ccCommandLineParser.h"



//! QApplication wrapper
class qccApplication : public QApplication
{
public:
	qccApplication( int &argc, char **argv )
		: QApplication( argc, argv ){
		setOrganizationName("CASIA");
		setApplicationName("IGITLandscapeViewer");
	}
};

int main(int argc, char *argv[]){
	//fronts setting
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

	//QT initialiation5
	qccApplication app(argc, argv);
	app.addLibraryPath("./imageformats");

	//Force 'Chinese' local so as to get a consistent behavior everywhere
	QLocale::setDefault(QLocale::Chinese);

	//splash screen
	QSplashScreen* splash = 0;
	QTime splashStartTime;

	bool commandLine = (argc>1 && argv[1][0]=='-');
	if(!commandLine){
		//has openGL??
		if(!QGLFormat::hasOpenGL()){
			QMessageBox::critical(0, "Error", "This application needs OpenGL to run!");
		    return EXIT_FAILURE;
		}	
	    // splash screen
	   splashStartTime.start();
	   QPixmap pixmap(QString::fromUtf8("./images/imLogoV2Qt.png"));
	   splash = new QSplashScreen(pixmap,Qt::WindowStaysOnTopHint);
	   splash->show();
	   QApplication::processEvents();
	}

	//global structures initialization
	ccTimer::Init();
	FileIOFilter::InitInternalFilters(); //load all known I/O filters (plugins will come later!)
	ccNormalVectors::GetUniqueInstance(); //force pre-computed normals array initialization
	ccColorScalesManager::GetUniqueInstance(); //force pre-computed color tables initialization

	int result = 0;
	if (commandLine){
		//command line processing (no GUI)
		result = ccCommandLineParser::Parse(argc,argv);
	}
	else{
		//main window init.
		MainWindow* mainWindow = MainWindow::TheInstance();
		if (!mainWindow){
			QMessageBox::critical(0, "Error", "Failed to initialize the main application window?!");
			return EXIT_FAILURE;
		}
		mainWindow->show();
		
		QApplication::processEvents();
	}

	if(argc >1){
		if(splash){
			splash->close();
		}

		//any additional argument is assumed to be a filename --> we try to load it/them
		QStringList filenames;
		for (int i=1; i<argc; ++i){
			filenames << QString(argv[i]);
		}
		//mainWindow->addToDB(filenames);
	}
	if(splash){
		//we want the splash screen to be visible a minimum amount of time (1000 ms.)
		while (splashStartTime.elapsed() < 1000){
				splash->raise();
				QApplication::processEvents(); //to let the system breath!
		}
		splash->close();
		QApplication::processEvents();

		delete splash;
		splash = 0;
	}
	//let's rock!
	try	{
		result = app.exec();
	}
	catch(...){
		QMessageBox::warning(0, "CC crashed!","Hum, it seems that CC has crashed... Sorry about that :)");
	}

	//release global structures
	////MainWindow::DestroyInstance();
	////FileIOFilter::UnregisterAll();
	return result;

}
