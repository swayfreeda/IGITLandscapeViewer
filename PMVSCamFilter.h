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

#ifndef CC_PMVSCAMFILTER_H
#define CC_PMVSCAMFILTER_H

#include "FileIOFilter.h"
#include <qsharedpointer.h>

// ASCII Camera Sensor I/O filter
class QCC_IO_LIB_API PMVSCamFilter: public FileIOFilter
{
 public:
	  //static accessors
	static inline QString GetFileFilter() { return "PMVS Camera (*.txt)"; }
	static inline QString GetDefaultExtension() { return "txt"; }

	//inherited from FileIOFilter
	virtual bool importSupported() const { return true; }
	virtual bool exportSupported() const { return true; }
	virtual CC_FILE_ERROR loadFile(QString filename, ccHObject& container, LoadParameters& parameters);
	virtual CC_FILE_ERROR saveToFile(ccHObject* entity, QString filename, SaveParameters& parameters);
	virtual QStringList getFileFilters(bool onImport) const { return QStringList(GetFileFilter()); }
	virtual QString getDefaultExtension() const { return GetDefaultExtension(); }
	virtual bool canLoadExtension(QString upperCaseExt) const;
	virtual bool canSave(CC_CLASS_ENUM type, bool& multiple, bool& exclusive) const;


	//! Returns associated dialog (creates it if necessary)
	//static QSharedPointer<AsciiOpenDlg> GetOpenDialog();
	//! Returns associated dialog (creates it if necessary)
	//static QSharedPointer<AsciiSaveDlg> GetSaveDialog();


protected:
	//! Internal use only
	CC_FILE_ERROR saveFile(ccHObject* entity, FILE *theFile);

	//! Associated (export) dialog
	//static QSharedPointer<AsciiSaveDlg> s_saveDialog;
	//! Associated (import) dialog
	//static QSharedPointer<AsciiOpenDlg> s_openDialog;
};

#endif


