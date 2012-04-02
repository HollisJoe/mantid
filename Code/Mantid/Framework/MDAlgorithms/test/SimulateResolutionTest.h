#ifndef SIMULATERESOLUTIONTEST_H_
#define SIMULATERESOLUTIONTEST_H_
//
#include <cxxtest/TestSuite.h>
//
#include "MantidMDAlgorithms/SimulateResolution.h"
#include "MantidAPI/CompositeFunction.h"
#include "MantidCurveFitting/Fit.h"
#include "MantidKernel/UnitFactory.h"
#include "MantidKernel/ProgressText.h"
#include "MantidAPI/AnalysisDataService.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidAPI/Algorithm.h"
#include "MantidDataObjects/Workspace2D.h"
#include "MantidDataObjects/TableWorkspace.h"
#include "MantidAPI/TableRow.h"
#include "MantidKernel/Exception.h"
#include "MantidAPI/FunctionFactory.h"
// from SimulateMDDTest
#include "MantidAPI/IMDWorkspace.h"
#include "MantidAPI/IMDIterator.h"
#include "MantidGeometry/Instrument/Detector.h"
#include "MantidGeometry/Instrument.h"
#include "MantidMDEvents/MDEventWorkspace.h"
#include "MantidMDEvents/MDBoxIterator.h"
#include "MantidTestHelpers/MDEventsTestHelper.h"
//
#include "MantidDataObjects/WorkspaceSingleValue.h"
//
#include <iostream>
#include <boost/scoped_ptr.hpp>
//


using namespace Mantid;
//
using namespace Mantid::MDEvents;
using namespace Mantid::Kernel;
using namespace Mantid::API;
using namespace Mantid::CurveFitting;
using namespace Mantid::DataObjects;
using namespace Mantid::MDAlgorithms;
using namespace Mantid::Geometry;
//
typedef Mantid::DataObjects::Workspace2D_sptr WS_type;
typedef Mantid::DataObjects::TableWorkspace_sptr TWS_type;
//
// Class TestSimulateResolution is  simplistic function, like an Sqw function
//
namespace Mantid
{
  namespace MDAlgorithms
  {


    class DLLExport TestSimulateResolution : public SimulateResolution
    {

    public:
      TestSimulateResolution() : SimulateResolution()
      {
        declareParameter("Constant",0.0);
      };
      /// Destructor
      virtual ~TestSimulateResolution() {}

      /// overwrite IFunction base class methods
      std::string name()const{return "TestSimulateResolution";}

      /// TESTING interface expose protected methods
      /// For MC integration return next point in space
      void wrap_getNextPoint(std::vector<double>& ran) const {getNextPoint(ran);};
      void wrap_initRandom() {initRandom();};
      void wrap_initRandom(const bool method) {initRandom(method);};
      void wrap_dMatrix(const double phi, const double beta, Kernel::Matrix<double> & dMat,
        Kernel::Matrix<double> & dinvMat) { dMatrix(phi,beta,dMat,dinvMat);}
      void wrap_bMatrix(const double wi, const double wf, const double x0, const double xa, const double x1,
              const double x2, const double thetam, const double angvel,
              const Kernel::Matrix<double> & sMat, const Kernel::Matrix<double> & dMat,
              Kernel::Matrix<double> & bMat)
             {bMatrix(wi, wf, x0, xa, x1, x2, thetam, angvel, sMat, dMat, bMat);}

      double wrap_sqwConvolutionMC(const Mantid::API::IMDIterator& it, size_t & event, double & error) const
             {return( sqwConvolutionMC(it, event, error) );}

    protected:

      void userSqw(const std::vector<double> & params, const std::vector<double> & qE, std::vector<double> & result) const
      {
        result.push_back(params[0]*qE[0]);
      }
      /// This method must be overridden by the user to define if a sharp or broad model is provided.
      bool userModelIsBroad() const
      {
        return true;
      }
      /// This will be over ridden by the user's getParam function
      void getParams() const
      {
        //
      }
    };

    DECLARE_FUNCTION(TestSimulateResolution)
  }
}


using namespace Mantid;
//
using namespace Mantid::MDEvents;
using namespace Mantid::Kernel;
using namespace Mantid::API;
using namespace Mantid::CurveFitting;
using namespace Mantid::DataObjects;
using namespace Mantid::MDAlgorithms;
using namespace Mantid::Geometry;


class SimulateResolutionTest : public CxxTest::TestSuite
{
private:
  std::string testWrkspc;
  std::string testWrkspc2;
  std::string testWrkspc3;
  IMDEventWorkspace_sptr inMDwrkspc;
  //
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static SimulateResolutionTest *createSuite() { return new SimulateResolutionTest(); }
  static void destroySuite( SimulateResolutionTest *suite ) { delete suite; }

  // create a test data set of 6 MDPoints contributing to 4 MDCells with 1, 2 and 3, 4 points each.
  SimulateResolutionTest()
  {

    testWrkspc="testMDEWrksp";
    testWrkspc2="testMDEWrksp2";
    testWrkspc3="testMDEWrksp3";
    // MDEventWorkspace in 4D with 3x3x3x3=81 boxes and one event of sig=1 err=1 per box
    // Using MDEvent not MDLeanEvent but run and detector pointers are not set
    // arguments are splits on each axis and min/max for each axis
    boost::shared_ptr<Mantid::MDEvents::MDEventWorkspace<Mantid::MDEvents::MDEvent<4>,4> >
    outnew = MDEventsTestHelper::makeMDEWFull<4>(3,0.0,3.,1);
    inMDwrkspc = outnew;
    outnew->refreshCache();
    TS_ASSERT_EQUALS(outnew->getNumDims(),4);
    TS_ASSERT_EQUALS(outnew->getNPoints(),81);

    TS_ASSERT_THROWS_NOTHING( AnalysisDataService::Instance().add(testWrkspc, outnew) );

    // build another workspace with non uniform signal This time with 4x4x4x4=256 boxes
    boost::shared_ptr<Mantid::MDEvents::MDEventWorkspace<Mantid::MDEvents::MDEvent<4>,4> >
    outnew2 = MDEventsTestHelper::makeMDEWFull<4>(4,0.0,4.);
    IMDBox<MDEvent<4>,4> * out = outnew2->getBox();

    // add additional events to make data quadratic in 4th coordinate (energy)
    out->calcVolume();

    double pos[4];
    uint16_t runIndex=1;
    int32_t detectorId;
    float signal;
    float errorsq;
    size_t npt=4;
    ProgressText * prog = NULL;
    errorsq=1.0;
    std::vector<MDEvent<4> > events;
    for(size_t x = 0 ; x<npt ; x++ )
    {
      pos[0]=0.5+static_cast<double>(x);
      for( size_t y = 0 ; y<npt ; y++ )
      {
        pos[1]=0.5+static_cast<double>(y);
        for( size_t z = 0; z<npt ; z++ )
        {
          pos[2]=0.5+static_cast<double>(z);
          for( size_t e = 0; e<npt ; e++ )
          {
            double ep = 0.5+static_cast<double>(e);
            pos[3]=ep;
            signal=static_cast<float>(1.0+0.5*ep+0.1*ep*ep);
            detectorId=static_cast<int32_t>(e+1);
            events.push_back(MDEvent<4> (signal,errorsq,runIndex,detectorId,pos));
          }
        }
      }
    }
    outnew2->addManyEvents(events,prog);
    // need to do this to update the signal values
    outnew2->refreshCache();
    TS_ASSERT_EQUALS(outnew2->getNumDims(),4);
    TS_ASSERT_EQUALS(outnew2->getNPoints(),256);
    IMDIterator* it = outnew2->createIterator();
    TS_ASSERT_EQUALS( it->getDataSize() ,256);
    TS_ASSERT_EQUALS( it->getNumEvents() ,1);
    it->next();
    TS_ASSERT_EQUALS( it->getNumEvents() ,1);


    TS_ASSERT_THROWS_NOTHING( AnalysisDataService::Instance().add(testWrkspc2, outnew2) );

    // build third workspace with non uniform signal and noise
    boost::shared_ptr<Mantid::MDEvents::MDEventWorkspace<Mantid::MDEvents::MDEvent<4>,4> >
    outnew3 = MDEventsTestHelper::makeMDEWFull<4>(4,0.0,4.);
    IMDBox<MDEvent<4>,4> * out3 = outnew3->getBox();

    // add events to make data quadratic in 4th coordinate with noise
    // Make 2 events per box
    out3->calcVolume();

    errorsq=1.0;
    events.clear();
    double noise=0.1;
    for(size_t x = 0 ; x<npt ; x++ )
    {
      pos[0]=0.5+static_cast<double>(x);
      for( size_t y = 0 ; y<npt ; y++ )
      {
        pos[1]=0.5+static_cast<double>(y);
        for( size_t z = 0; z<npt ; z++ )
        {
          pos[2]=0.5+static_cast<double>(z);
          for( size_t e = 0; e<npt ; e++ )
          {
            double ep = 0.5+static_cast<double>(e);
            pos[3]=ep+0.1;
            signal=static_cast<float>(1.0+0.5*ep+0.1*ep*ep+noise*(double(rand())/double(RAND_MAX)-0.5));
            detectorId=static_cast<int32_t>(e+1);
            events.push_back(MDEvent<4> (signal,errorsq,runIndex,detectorId,pos));
            pos[3]=ep-0.1;
            events.push_back(MDEvent<4> (signal,errorsq,runIndex,detectorId,pos));
          }
        }
      }
    }
    outnew3->addManyEvents(events,prog);
    // need to do this to update the signal values
    outnew3->refreshCache();
    TS_ASSERT_EQUALS(outnew3->getNumDims(),4);
    TS_ASSERT_EQUALS(outnew3->getNPoints(),512);
    IMDIterator* it3 = outnew3->createIterator();
    TS_ASSERT_EQUALS( it3->getDataSize() ,256);
    TS_ASSERT_EQUALS( it3->getNumEvents() ,2);
    it->next();
    TS_ASSERT_EQUALS( it3->getNumEvents() ,2);
    TS_ASSERT_EQUALS( it3->getInnerDetectorID(0),1);
    TS_ASSERT_EQUALS( it3->getInnerDetectorID(1),1);


    TS_ASSERT_THROWS_NOTHING( AnalysisDataService::Instance().add(testWrkspc3, outnew3) );
  }

  void testSobolNumbers()
  {
    // set up fitting function
    TestSimulateResolution* fn = new TestSimulateResolution();
    fn->initialize();
    fn->wrap_initRandom();
    std::vector<double> ran;
    //check first two vectors returned by Sobol
    fn->wrap_getNextPoint(ran);
    TS_ASSERT_EQUALS( ran.size(),14 );
    TS_ASSERT_EQUALS( ran.at(0),0.5);
    TS_ASSERT_EQUALS( ran.at(5),0.5);
    TS_ASSERT_EQUALS( ran.at(12),0.5);
    fn->wrap_getNextPoint(ran);
    TS_ASSERT_EQUALS( ran.size(),14 );
    TS_ASSERT_EQUALS( ran.at(0),0.75);
    TS_ASSERT_EQUALS( ran.at(5),0.25);
    TS_ASSERT_EQUALS( ran.at(12),0.25);
    // check the same after reset
    fn->wrap_initRandom(true);
    fn->wrap_getNextPoint(ran);
    TS_ASSERT_EQUALS( ran.size(),14 );
    TS_ASSERT_EQUALS( ran.at(0),0.5);
    TS_ASSERT_EQUALS( ran.at(5),0.5);
    TS_ASSERT_EQUALS( ran.at(12),0.5);
    fn->wrap_getNextPoint(ran);
    TS_ASSERT_EQUALS( ran.size(),14 );
    TS_ASSERT_EQUALS( ran.at(0),0.75);
    TS_ASSERT_EQUALS( ran.at(5),0.25);
    TS_ASSERT_EQUALS( ran.at(12),0.25);
  }

  void testRandomNumbers()
  {
    // set up fitting function
    TestSimulateResolution* fn = new TestSimulateResolution();
    fn->initialize();
    fn->wrap_initRandom(false);
    std::vector<double> ran;
    // check random values are in range 0-1
    fn->wrap_getNextPoint(ran);
    TS_ASSERT_EQUALS( ran.size(),14 );
    TS_ASSERT_DELTA( ran.at(0),0.5,0.5);
    TS_ASSERT_DELTA( ran.at(5),0.5,0.5);
    TS_ASSERT_DELTA( ran.at(12),0.5,0.5);
    std::vector<double> ran0 = ran;
    fn->wrap_getNextPoint(ran);
    TS_ASSERT_EQUALS( ran.size(),14 );
    TS_ASSERT_DELTA( ran.at(0),0.5,0.5);
    TS_ASSERT_DELTA( ran.at(5),0.5,0.5);
    TS_ASSERT_DELTA( ran.at(12),0.5,0.5);
    std::vector<double> ran1 = ran;
    // check values are te same after a reset
    fn->wrap_initRandom(false);
    fn->wrap_getNextPoint(ran);
    TS_ASSERT_EQUALS( ran.size(),14 );
    TS_ASSERT_EQUALS( ran.at(0),ran0.at(0));
    TS_ASSERT_EQUALS( ran.at(3),ran0.at(3));
    TS_ASSERT_EQUALS( ran.at(11),ran0.at(11));
    fn->wrap_getNextPoint(ran);
    TS_ASSERT_EQUALS( ran.size(),14 );
    TS_ASSERT_EQUALS( ran.at(0),ran1.at(0));
    TS_ASSERT_EQUALS( ran.at(3),ran1.at(3));
    TS_ASSERT_EQUALS( ran.at(11),ran1.at(11));
  }

  void testdMatrix()
  {
    // set up test function
    TestSimulateResolution* fn = new TestSimulateResolution();
    fn->initialize();
    Kernel::Matrix<double> dMat(3,3), dinvMat(3,3);
    // angles from cobalt demo, 1st detector
    const double phi = 0.37538367018968838;
    const double beta =2.618430210304493;
    fn->wrap_dMatrix(phi, beta , dMat, dinvMat);
    // check random values are in range 0-1
    TS_ASSERT_DELTA( dMat[0][0], 0.93036702557838036, 1e-7 );
    TS_ASSERT_DELTA( dMat[2][0], 0.0, 1e-7 );
    TS_ASSERT_DELTA( dMat[0][2], 0.18317619755399642, 1e-7 );
    Kernel::Matrix<double> id = dMat*dinvMat;
    TS_ASSERT_DELTA( id[0][0] , 1.0, 1e-7);
    TS_ASSERT_DELTA( id[1][1] , 1.0, 1e-7);
    TS_ASSERT_DELTA( id[2][2] , 1.0, 1e-7);
    TS_ASSERT_DELTA( id[1][2] , 0.0, 1e-7);
    TS_ASSERT_DELTA( id[0][2] , 0.0, 1e-7);
    //TS_ASSERT_DELTA( id , id0, 1e-7);
  }

  void testbMatrix()
  {
    // set up test function
    TestSimulateResolution* fn = new TestSimulateResolution();
    fn->initialize();
    Kernel::Matrix<double> dMat(3,3), dinvMat(3,3);
    // angles from cobalt demo, 1st detector
    const double phi = 0.37538367018968838;
    const double beta =2.618430210304493;
    fn->wrap_dMatrix(phi, beta , dMat, dinvMat);
    Kernel::Matrix<double> sMat(3,3,true);
    Kernel::Matrix<double> bMat(6,11);
    const double wi=14.687369667050531;
    const double wf=11.027841985547997;
    const double x0=10.1;
    const double xa=8.11;
    const double x1=1.9;
    const double x2=6.034;
    const double thetam=0.55850536063818546;
    const double angvel=3769.9111843077517;
    fn->wrap_bMatrix(wi,wf,x0,xa,x1,x2,thetam,angvel,sMat,dMat,bMat);
    // check selected values with same from tobyfit
    TS_ASSERT_DELTA( bMat[0][0], 13447.663441293, 1e-3 );
    TS_ASSERT_DELTA( bMat[0][3], -13447.663441293, 1e-3 );
    TS_ASSERT_DELTA( bMat[3][0], -2387.196362925, 1e-3 );
    TS_ASSERT_DELTA( bMat[5][0], -0.0, 1e-3 );
    TS_ASSERT_DELTA( bMat[3][1],  0.206156088081, 1e-6 );
    TS_ASSERT_DELTA( bMat[2][2], -1.467269697008, 1e-6 );
    TS_ASSERT_DELTA( bMat[3][3],  15077.02966058, 1e-3 );
    TS_ASSERT_DELTA( bMat[3][4], -0.328109492982, 1e-6 );
    TS_ASSERT_DELTA( bMat[0][5],  0.175706947063, 1e-6 );
    TS_ASSERT_DELTA( bMat[5][6],  1.583161468528, 1e-6 );
  }

  void testSqwConvoutionMC()
  {
  }

  void testWithGenericFit()
  {
    // test GenericFit - note that fit is to cell data but that MDCell
    // returns the sum of point contributions, not average.
    // As the number of points in a cell varies 1 to 4 this must be taken into
    // account if comparing the fit to the cell data.
    Fit alg1;
    TS_ASSERT_THROWS_NOTHING(alg1.initialize());
    TS_ASSERT( alg1.isInitialized() );

    // name of workspace to test against
    std::string wsName = testWrkspc;

    // set up fitting function
    TestSimulateResolution* fn = new TestSimulateResolution();
    fn->initialize();

    alg1.setPropertyValue("Function",fn->asString());

    // Set which spectrum to fit against and initial starting values
    alg1.setPropertyValue("InputWorkspace", wsName);

    // execute fit NOT YET WORKING

  }

  void testGenericFitandCompositeFunctionMD()
  {
    //* Not yet implemented
  }
  void testTidyUp()
  {
    removeWS(testWrkspc);
    removeWS(testWrkspc2);
    removeWS(testWrkspc3);
  }

  TWS_type getTWS(const std::string& name)
  {
    return boost::dynamic_pointer_cast<Mantid::DataObjects::TableWorkspace>(AnalysisDataService::Instance().retrieve(name));
  }
  WS_type getWS(const std::string& name)
  {
    return boost::dynamic_pointer_cast<Mantid::DataObjects::Workspace2D>(AnalysisDataService::Instance().retrieve(name));
  }
  void removeWS(const std::string& name)
  {
    AnalysisDataService::Instance().remove(name);
  }


};

#endif /*SIMULATERESOLUTIONTEST_H_*/
