#include "MantidQtMantidWidgets/MuonFitPropertyBrowser.h"
#include "MantidQtMantidWidgets/PropertyHandler.h"
#include "MantidQtMantidWidgets/SequentialFitDialog.h"
#include "MantidAPI/FunctionFactory.h"

// Suppress a warning coming out of code that isn't ours
#if defined(__INTEL_COMPILER)
  #pragma warning disable 1125
#elif defined(__GNUC__)
  #if (__GNUC__ >= 4 && __GNUC_MINOR__ >= 6 )
    #pragma GCC diagnostic push
  #endif
  #pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif
#include "DoubleEditorFactory.h"
#include "qteditorfactory.h"
#include "StringDialogEditorFactory.h"
#if defined(__INTEL_COMPILER)
  #pragma warning enable 1125
#elif defined(__GNUC__)
  #if (__GNUC__ >= 4 && __GNUC_MINOR__ >= 6 )
    #pragma GCC diagnostic pop
  #endif
#endif

#include "MantidAPI/AlgorithmManager.h"
#include "MantidAPI/FrameworkManager.h"
#include "MantidAPI/CompositeFunction.h"
#include "MantidAPI/Expression.h"
#include "MantidAPI/IBackgroundFunction.h"
#include "MantidAPI/IPeakFunction.h"

#include "qttreepropertybrowser.h"
#include "qtpropertymanager.h"

#include <QSettings>
#include <QMessageBox>


namespace MantidQt
{
namespace MantidWidgets
{


/**
 * Constructor
 * @param parent :: The parent widget - must be an ApplicationWindow
 * @param mantidui :: The UI form for MantidPlot
*/
MuonFitPropertyBrowser::MuonFitPropertyBrowser(QWidget *parent, QObject* mantidui)
  :FitPropertyBrowser(parent, mantidui)
{}


/**
* Initialise the muon fit property browser.
*/
void MuonFitPropertyBrowser::init()
{
  QWidget* w = new QWidget(this);

  QSettings settings;
  settings.beginGroup("Mantid/FitBrowser");

  /* Create function group */
  QtProperty* functionsGroup = m_groupManager->addProperty("Functions");
  QtProperty* settingsGroup(NULL);
  
  // Seperates the data and the settings into two seperate categories
  settingsGroup = m_groupManager->addProperty("Data");
    
  // Have slightly different names as requested by the muon scientists. 
  m_startX = addDoubleProperty(QString("Start (%1s)" ).arg(QChar(0x03BC))); //(mu); 
  m_endX = addDoubleProperty(QString("End (%1s)" ).arg(QChar(0x03BC)));

  //m_workspace = m_enumManager->addProperty("Workspace");
  m_workspaceIndex = m_intManager->addProperty("Workspace Index");
  m_output = m_stringManager->addProperty("Output");
  m_minimizer = m_enumManager->addProperty("Minimizer");  
  m_minimizers << "Levenberg-Marquardt"
               << "Simplex"
               << "Conjugate gradient (Fletcher-Reeves imp.)"
               << "Conjugate gradient (Polak-Ribiere imp.)"
               << "BFGS";
  m_enumManager->setEnumNames(m_minimizer, m_minimizers);
  m_costFunction = m_enumManager->addProperty("Cost function");
  m_costFunctions << "Least squares"
                  << "Ignore positive peaks";
  m_enumManager->setEnumNames(m_costFunction,m_costFunctions);

  m_plotDiff = m_boolManager->addProperty("Plot Difference");
  bool plotDiff = settings.value("Plot Difference",QVariant(true)).toBool();
  m_boolManager->setValue(m_plotDiff,plotDiff);
  
  settingsGroup->addSubProperty(m_workspace);
  settingsGroup->addSubProperty(m_workspaceIndex);
  settingsGroup->addSubProperty(m_startX);
  settingsGroup->addSubProperty(m_endX);
  
  /* Create editors and assign them to the managers */
  createEditors(w);

  updateDecimals();
  
  m_functionsGroup = m_browser->addProperty(functionsGroup);
  m_settingsGroup = m_browser->addProperty(settingsGroup);

  // Custom settings that are specific and asked for by the muon scientists.
  QtProperty* customSettingsGroup = m_groupManager->addProperty("Settings");
  m_rawData = m_boolManager->addProperty("Fit To Raw Data");
  bool data = settings.value("Fit To Raw Data",QVariant(false)).toBool();
  m_boolManager->setValue(m_rawData,data);
  customSettingsGroup->addSubProperty(m_minimizer);
  customSettingsGroup->addSubProperty(m_plotDiff);
  customSettingsGroup->addSubProperty(m_rawData);
  m_customSettingsGroup = m_browser->addProperty(customSettingsGroup);

  // Initialise the layout.
  initLayout(w);
}


/**
* Set the input workspace name
*/
void MuonFitPropertyBrowser::setWorkspaceName(const QString& wsName)
{
  int i = m_workspaceNames.indexOf(wsName);
  if (i < 0)
  {
    // workspace may not be found because add notification hasn't been processed yet
    populateWorkspaceNames();
    i = m_workspaceNames.indexOf(wsName);
  }
  if (i >= 0)
    m_enumManager->setValue(m_workspace,i);
}


/** Called when a double property changed
 * @param prop :: A pointer to the property 
 */
void MuonFitPropertyBrowser::doubleChanged(QtProperty* prop)
{
  if ( ! m_changeSlotsEnabled ) return;

  double value = m_doubleManager->value(prop);
  if (prop == m_startX )
  {
    // call setWorkspace to change maxX in functions
    setWorkspace(m_compositeFunction);
    getHandler()->setAttribute(QString("Start (%1s)" ).arg(QChar(0x03BC)), value); // (mu)
    emit startXChanged(startX());
    emit xRangeChanged(startX(), endX());
    return;
  }
  else if (prop == m_endX )
  {
    // call setWorkspace to change minX in functions
    setWorkspace(m_compositeFunction);
    getHandler()->setAttribute(QString("End (%1s)" ).arg(QChar(0x03BC)), value); 
    emit endXChanged(endX());
    emit xRangeChanged(startX(), endX());
    return;
  }
  else if(getHandler()->setParameter(prop))
  {
    return;
  }
  else
  {// check if it is a constraint
    MantidQt::MantidWidgets::PropertyHandler* h = getHandler()->findHandler(prop);
    if (!h) return;

    QtProperty* parProp = h->getParameterProperty(prop);
    if (parProp)
    {
      if (prop->propertyName() == "LowerBound")
      {
        double loBound = m_doubleManager->value(prop);
        h->addConstraint(parProp,true,false,loBound,0);
      }
      else if (prop->propertyName() == "UpperBound")
      {
        double upBound = m_doubleManager->value(prop);
        h->addConstraint(parProp,false,true,0,upBound);
      }
    }
    else
    {// it could be an attribute
      h->setAttribute(prop);
    }
  }
}


/**
*Get the registered function names
*/
void MuonFitPropertyBrowser::populateFunctionNames()
{
  const std::vector<std::string> names = Mantid::API::FunctionFactory::Instance().getKeys();
  m_registeredFunctions.clear();
  m_registeredPeaks.clear();
  m_registeredBackgrounds.clear();
  for(size_t i=0;i<names.size();i++)
  {
    std::string fnName = names[i];
    QString qfnName = QString::fromStdString(fnName);
    if (qfnName == "MultiBG") continue;
    
    auto f = Mantid::API::FunctionFactory::Instance().createFitFunction(fnName);
    const std::vector<std::string> categories = f->categories();
    bool muon = false;
    for (size_t j=0; j<categories.size(); ++j)
    {
      if ((categories[j] == "Muon") || (categories[j] == "General") )
        muon = true;
    }
    if (muon == true)
    {
      m_registeredFunctions << qfnName;
    }
    Mantid::API::IPeakFunction* pf = dynamic_cast<Mantid::API::IPeakFunction*>(f.get());
    //Mantid::API::CompositeFunction* cf = dynamic_cast<Mantid::API::CompositeFunction*>(f.get());
    if (pf)
    {
      m_registeredPeaks << qfnName;
    }
    else if (dynamic_cast<Mantid::API::IBackgroundFunction*>(f.get()))
    {
      m_registeredBackgrounds << qfnName;
    }
    else
    {
      m_registeredOther << qfnName;
    }
  }
}


/**
 * Creates an instance of Fit algorithm, sets its properties and launches it.
 */
void MuonFitPropertyBrowser::fit()
{
  std::string wsName = workspaceName();

  if (wsName.empty())
  {
    QMessageBox::critical(this,"Mantid - Error", "Workspace name is not set");
    return;
  }
  try
  {
    m_initialParameters.resize(compositeFunction()->nParams());
    for(size_t i=0;i<compositeFunction()->nParams();i++)
    {
      m_initialParameters[i] = compositeFunction()->getParameter(i);
    }
    m_fitActionUndoFit->setEnabled(true);

    std::string funStr;
    if (m_compositeFunction->name() == "MultiBG")
    {
      //std::string ties;
      //Mantid::API::Expression funExpr;
      //funExpr.parse(m_compositeFunction->asString());
      //funExpr.toList(";");
      //for(size_t i = 0; i < funExpr.size(); ++i)
      //{
      //  const Mantid::API::Expression& e = funExpr[i];
      //  if (e.name() == "=" && e.size() == 2 && e[0].name() == "ties")
      //  {
      //    ties = e[0].name() + "=(" + e[1].str() + ")";
      //  }
      //}
      //funStr = "composite=MultiBG;";
      //for(size_t i=0;i<m_compositeFunction->nFunctions();++i)
      //{
      //  auto f = boost::dynamic_pointer_cast<Mantid::API::IFunction1D>(m_compositeFunction->getFunction(i));
      //  if (!f) continue;
      //  funStr += f->asString();
      //  if (f->getMatrixWorkspace() && !f->getMatrixWorkspace()->getName().empty())
      //  {
      //    funStr += ",Workspace=" + f->getMatrixWorkspace()->getName() + ",WSParam=(WorkspaceIndex="+
      //      boost::lexical_cast<std::string>(f->getWorkspaceIndex()) + ")";
      //  }
      //  funStr += ";";
      //}
      //funStr += ties;
    }
    else if (m_compositeFunction->nFunctions() > 1)
    {
      funStr = m_compositeFunction->asString();
    }
    else
    {
      funStr = (m_compositeFunction->getFunction(0))->asString();
    }

    if ( Mantid::API::AnalysisDataService::Instance().doesExist(wsName+"_NormalisedCovarianceMatrix"))
    {
      Mantid::API::FrameworkManager::Instance().deleteWorkspace(wsName+"_NormalisedCovarianceMatrix");
    }
    if ( Mantid::API::AnalysisDataService::Instance().doesExist(wsName+"_Parameters"))
    {
      Mantid::API::FrameworkManager::Instance().deleteWorkspace(wsName+"_Parameters");
    }
    if ( Mantid::API::AnalysisDataService::Instance().doesExist(wsName+"_Workspace"))
    {
      Mantid::API::FrameworkManager::Instance().deleteWorkspace(wsName+"_Workspace");
    }

    Mantid::API::IAlgorithm_sptr alg = Mantid::API::AlgorithmManager::Instance().create("Fit");
    alg->initialize();
    if (rawData())
      alg->setPropertyValue("InputWorkspace",wsName + "_Raw");
    else
      alg->setPropertyValue("InputWorkspace",wsName);
    alg->setProperty("WorkspaceIndex",workspaceIndex());
    alg->setProperty("StartX",startX());
    alg->setProperty("EndX",endX());
    alg->setPropertyValue("Output",outputName());
    alg->setPropertyValue("Function",funStr);
    alg->setPropertyValue("Minimizer",minimizer());
    alg->setPropertyValue("CostFunction",costFunction());
    observeFinish(alg);
    alg->executeAsync();
  }
  catch(std::exception& e)
  {
    QString msg = "Fit algorithm failed.\n\n"+QString(e.what())+"\n";
    QMessageBox::critical(this,"Mantid - Error", msg);
  }
}


/**
 * Connect to the AnalysisDataServis when shown
 */
void MuonFitPropertyBrowser::showEvent(QShowEvent* e)
{
  (void)e;
  observePostDelete();
  populateWorkspaceNames();
}


/** 
* Enable/disable the Fit button.
*/
void MuonFitPropertyBrowser::setFitEnabled(bool yes)
{
  m_fitActionFit->setEnabled(yes);
  m_fitActionSeqFit->setEnabled(false);
}


/**
* Sets a new workspace
*/
void MuonFitPropertyBrowser::workspaceChange(const QString& wsName)
{
  if (m_guessOutputName)
  {
    if (isWorkspaceAGroup())
    {
      m_stringManager->setValue(m_output,QString::fromStdString(workspaceName()+"_params"));
    }
    else
    {
      m_stringManager->setValue(m_output,QString::fromStdString(workspaceName()));
    }
  }
  if (isWorkspaceAGroup())
  {
    setLogValue();
  }
  else
  {
    //m_groupMember = workspaceName();
    removeLogValue();
  }
  
  // Sets up the peak picker tool for that workspace. Might not work if not on correct tab. i.e loading data on the first tab wouldn't work.
  updatePPTool(wsName);
}


/** Check if the workspace can be used in the fit. The accepted types are
  * MatrixWorkspaces same size and that it isn't the generated raw file.
  * @param ws :: The workspace
  */
bool MuonFitPropertyBrowser::isWorkspaceValid(Mantid::API::Workspace_sptr ws)const
{
  QString workspaceName(QString::fromStdString(ws->name() ) );
  if ( (workspaceName.contains("_Raw") ) || (workspaceName.contains("MuonAnalysis") ) )
    return false;

  if (dynamic_cast<Mantid::API::MatrixWorkspace*>(ws.get()) != 0)
    return true;
  else
    return false;
}


} // MantidQt
} // API
