
#include"PMVSCamOpenDlg.h"

 PMVSCamOpenDlg::PMVSCamOpenDlg(QWidget* parent){
	 setupUi(this);

	 connect(pushButtonOK, SIGNAL(clicked()), this, SLOT(accept()));
	 connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
 }



