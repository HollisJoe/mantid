#ifndef INSTRUMENTWINDOW_H_
#define INSTRUMENTWINDOW_H_

#include <QPushButton>
#include <QDialog>
#include <QSlider>
#include <QSpinBox>
#include <QTabWidget>
#include <QLineEdit>
#include <QLabel>
#include "Instrument3DWidget.h"
#include "../../MdiSubWindow.h"
#include "BinDialog.h"
#include <string>
#include <vector>
#include "qwt_scale_widget.h"

/*!
  \class  InstrumentWindow
  \brief  This is the main window for the control of display on geometry
  \author Srikanth Nagella
  \date   September 2008
  \version 1.0

  This is a QT widget for the controls and display of instrument geometry

  Copyright &copy; 2007 STFC Rutherford Appleton Laboratories

  This file is part of Mantid.
 	
  Mantid is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  Mantid is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  
  File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>
*/
class InstrumentWindow : public MdiSubWindow
{
	Q_OBJECT
public:
	InstrumentWindow(const QString& label = QString(), ApplicationWindow *app = 0, const QString& name = QString(), Qt::WFlags f = 0);
	~InstrumentWindow();
	void setWorkspaceName(std::string wsName);
public slots:
	void modeSelectButtonClicked();
	void spectraInformation(int);
	void detectorInformation(int value);
	void detectorHighlighted(int detectorId,int spectraid,int count);
	void spectraListInformation(std::vector<int>);
	void detectorListInformation(std::vector<int>);
	void spectraInfoDialog();
	void spectraGroupInfoDialog();
	void changeColormap();
    void sendPlotSpectraSignal();
	void sendPlotSpectraGroupSignal();
	void minValueChanged();
	void maxValueChanged();
	void setViewDirection(const QString&);
signals:
    void plotSpectra(const QString&,int);
	void plotSpectraList(const QString&,std::vector<int>);
private:
	void updateColorMapWidget();
	QLabel*      mInteractionInfo;
	QTabWidget*  mControlsTab;
    QMenu*       mPopupContext; ///< Popup menu for detector picking
	QMenu*       mDetectorGroupPopupContext; ///< Popup menu for detector picking
	QPushButton* mSelectButton; ///< Select the mode Pick/Normal
	QPushButton* mSelectColormap; ///< Select colormap button
	Instrument3DWidget* mInstrumentDisplay; ///< This is the opengl 3d widget for instrument
	int          mSpectraIDSelected; ///< spectra index id
	int          mDetectorIDSelected; ///< detector id
	std::vector<int> mSpectraIDSelectedList;
	std::vector<int> mDetectorIDSelectedList;
	QwtScaleWidget* mColorMapWidget; ///< colormap display widget
	QLineEdit*   mMinValueBox;       ///< Minvalue for the colormap
	QLineEdit*   mMaxValueBox;       ///< Max value for the colormap
	BinDialog*   mBinMapDialog;
};

#endif /*INSTRUMENTWINDOW_H_*/

