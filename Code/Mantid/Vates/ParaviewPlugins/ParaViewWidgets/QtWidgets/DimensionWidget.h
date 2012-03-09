#ifndef DIMENSION_WIDGET_H
#define DIMENSION_WIDGET_H

#include <qgridlayout.h>
#include <qwidget.h>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "WidgetDllOption.h"
#include "GeometryWidget.h"
#include "MantidVatesAPI/DimensionPresenter.h"

//Foward decs
class QLabel;
class QComboBox;
class QLineEdit;
class QCheckBox;
class BinInputWidget;

namespace Mantid
{
  namespace Geometry
  {
    /// Forward decs
    class IMDDimension;
  }
}

/**
class is a Qt (QWidget) concrete version of a DimensionView.

Displays dimension information as commanded by a DimensionPresenter.

- DimensionWidgets are passed a DimensionPresenter, as part of the accept call, but DimensionWidgets do not own it!
- Controlled by a DimensionPresenter
- Has public methods to allow the DimensionPresenter to command changes

*/
class EXPORT_OPT_MANTIDPARVIEW DimensionWidget: public QWidget, public Mantid::VATES::DimensionView
{
Q_OBJECT
public:

  /// Constructor.
  DimensionWidget(Mantid::VATES::BinDisplay binDisplay);

  /// Destructor
  ~DimensionWidget();

  /// Get minimum
  double getMinimum() const;

  /// Get maximum
  double getMaximum() const;


signals:
  void maxSet();
  void minSet();
  void nBinsSet();
private:
  QGridLayout* m_layout;

  //QLineEdit* m_nBinsBox;

  QLineEdit* m_minBox;

  QLineEdit* m_maxBox;

  QCheckBox* m_ckIntegrated;

  QComboBox* m_dimensionCombo;

  //QLabel* m_nBinsLabel;

  QLabel* m_dimensionLabel;

  int m_currentDimensionIndex;

  std::string m_name;

  Mantid::VATES::DimensionPresenter* m_pDimensionPresenter;

  /// Widget for displaying bin information.
  BinInputWidget* m_binWidget;

  /// Helper method to set names in all places required.
  void setDimensionName(const std::string& name);

  private slots:

  /// Handles dimension change events.
  void dimensionSelectedListener();

  void nBinsListener();

  void maxBoxListener();

  void minBoxListener();

  void integratedChanged(bool checkedState);

public:

  //---------------------------------------------------------
  // DimensionView implementations
  //---------------------------------------------------------
  virtual void showAsNotIntegrated(Mantid::Geometry::VecIMDDimension_sptr nonIntegratedDims);
  
  virtual void showAsIntegrated();

  virtual void displayError(std::string message) const;
      
  virtual void accept(Mantid::VATES::DimensionPresenter* pDimensionPresenter);

  virtual void configureStrongly();

  virtual void configureWeakly();

  virtual std::string getVisDimensionName() const;

  virtual unsigned int getNBins() const;

  virtual unsigned int getSelectedIndex() const;

  virtual bool getIsIntegrated() const;
  //---------------------------------------------------------
  // End DimensionView implementations
  //---------------------------------------------------------

};

#endif
