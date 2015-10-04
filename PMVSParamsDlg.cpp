#include"PMVSParamsDlg.h"
#include <qfiledialog.h>

PMVSParamsDlg::PMVSParamsDlg(QWidget* parent)
{
  setupUi(this);

  connect(pushButtonFolderDir,             SIGNAL(clicked()),             this,         SLOT(doActionGetFolderDir()));
}

void PMVSParamsDlg::doActionGetFolderDir()
{

	QString dir =  QFileDialog::getSaveFileName(this,"±£´æÂ·¾¶","","*.nvm");

	lineEditFolderDir->setText(dir);
	update();
}