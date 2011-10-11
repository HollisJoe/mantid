#include <iostream>
#include <QApplication>
#include <QSplashScreen>
#include <QMessageBox>
#include <QDir>
#include <QThread>

#include "ApplicationWindow.h"
#include "MantidKernel/Logger.h"
#include "MantidKernel/MantidVersion.h"

#include "Mantid/MantidApplication.h"
#include "qmainwindow.h"
#include "Mantid/SliceViewer/SliceViewer.h"
#include "MantidMDEvents/MDHistoWorkspace.h"
#include "MantidGeometry/MDGeometry/MDHistoDimension.h"

using namespace Mantid;
using namespace Mantid::API;;
using namespace Mantid::MDEvents;
using Mantid::Geometry::MDHistoDimension_sptr;
using Mantid::Geometry::MDHistoDimension;

/** Demo application for quickly testing the SliceViewer GUI.
 *
 * @author Janik Zikovsky
 * @date Oct 3, 2011
 */



/** Creates a fake MDHistoWorkspace
 *
 * @param signal :: signal and error squared in every point
 * @param numDims :: number of dimensions to create. They will range from 0 to max
 * @param numBins :: bins in each dimensions
 * @param max :: max position in each dimension
 * @return the MDHisto
 */
Mantid::MDEvents::MDHistoWorkspace_sptr makeFakeMDHistoWorkspace(double signal, size_t numDims, size_t numBins,
    double max)
{
  Mantid::MDEvents::MDHistoWorkspace * ws = NULL;
  if (numDims ==1)
  {
    ws = new Mantid::MDEvents::MDHistoWorkspace(
        MDHistoDimension_sptr(new MDHistoDimension("x","x","m", 0.0, max, numBins)) );
  }
  else if (numDims == 2)
  {
    ws = new Mantid::MDEvents::MDHistoWorkspace(
        MDHistoDimension_sptr(new MDHistoDimension("x","x","m", 0.0, max, numBins)),
        MDHistoDimension_sptr(new MDHistoDimension("y","y","m", 0.0, max, numBins))  );
  }
  else if (numDims == 3)
  {
    ws = new Mantid::MDEvents::MDHistoWorkspace(
        MDHistoDimension_sptr(new MDHistoDimension("x","x","m", 0.0, max, numBins)),
        MDHistoDimension_sptr(new MDHistoDimension("y","y","m", 0.0, max, numBins)),
        MDHistoDimension_sptr(new MDHistoDimension("z","z","m", 0.0, max, numBins))   );
  }
  else if (numDims == 4)
  {
    ws = new Mantid::MDEvents::MDHistoWorkspace(
        MDHistoDimension_sptr(new MDHistoDimension("x","x","m", 0.0, max, numBins)),
        MDHistoDimension_sptr(new MDHistoDimension("y","y","m", 0.0, max, numBins)),
        MDHistoDimension_sptr(new MDHistoDimension("z","z","m", 0.0, max, numBins)),
        MDHistoDimension_sptr(new MDHistoDimension("t","z","m", 0.0, max, numBins))
        );
  }
  Mantid::MDEvents::MDHistoWorkspace_sptr ws_sptr(ws);
  ws_sptr->setTo(signal, signal);
  return ws_sptr;
}
/** Main application
 *
 * @param argc :: ignored
 * @param argv :: ignored
 * @return return code
 */
int main( int argc, char ** argv )
{
  QApplication app(argc, argv);
  app.setOrganizationName("JanikTech");
  app.setApplicationName("Application Example");
  QMainWindow * mainWin = new QMainWindow();

  QFrame * frame = new QFrame(mainWin);
  mainWin->setCentralWidget(frame);

  QLayout * layout = new QVBoxLayout(frame);
  frame->setLayout(layout);

  // Create a fake workspace
  size_t numBins;

  numBins=100;
  MDHistoWorkspace_sptr ws = makeFakeMDHistoWorkspace(1.0, 3, numBins, double(numBins)/10.0);
  for (size_t x=0;x<numBins; x++)
    for (size_t y=0;y<numBins; y++)
      for (size_t z=0;z<numBins; z++)
      {
        signal_t signal = ( sin(double(x)/5) + sin(double(y)/2) + sin(double(z)/20) ) * 5 + double(x)*0.05 + double(y)*0.05;
        ws->setSignalAt(x+y*numBins+z*numBins*numBins, signal);
      }

//  numBins=50;
//  double mid = 25;
//  MDHistoWorkspace_sptr ws = makeFakeMDHistoWorkspace(1.0, 4, numBins, double(numBins)/10.0);
//  for (size_t x=0;x<numBins; x++)
//    for (size_t y=0;y<numBins; y++)
//      for (size_t z=0;z<numBins; z++)
//        for (size_t t=0;t<numBins; t++)
//        {
//          signal_t signal = 0;
//          double v;
//          v = (double(x)-mid); signal += 25-sqrt(v*v);
//          v = (double(y)-mid); signal += 25-sqrt(v*v);
//          v = (double(z)-mid); signal += 25-sqrt(v*v);
//          v = (double(t)-mid); signal += 25-sqrt(v*v);
//          signal *= 0.1;
//          ws->setSignalAt(x+y*numBins+z*numBins*numBins+t*numBins*numBins*numBins, signal);
//        }


  SliceViewer * slicer = new SliceViewer(frame);
  slicer->setWorkspace(ws);
  slicer->resize(600,600);
  layout->addWidget(slicer);

  mainWin->move(100,100);
  mainWin->resize(700, 700);
  mainWin->show();
  return app.exec();

}
