#ifndef MANTID_MDEVENTS_MDCENTROIDPEAKSTEST_H_
#define MANTID_MDEVENTS_MDCENTROIDPEAKSTEST_H_

#include "MantidMDEvents/CentroidPeaksMD.h"
#include "MantidAPI/AnalysisDataService.h"
#include "MantidAPI/IMDEventWorkspace.h"
#include "MantidDataObjects/PeaksWorkspace.h"
#include "MantidGeometry/MDGeometry/MDHistoDimension.h"
#include "MantidKernel/System.h"
#include "MantidKernel/Timer.h"
#include "MantidMDEvents/MDEventFactory.h"
#include "MantidMDEvents/CentroidPeaksMD.h"
#include "MantidTestHelpers/ComponentCreationHelper.h"
#include <boost/math/distributions/normal.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/math/special_functions/pow.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <cxxtest/TestSuite.h>
#include <iomanip>
#include <iostream>

using Mantid::API::AnalysisDataService;
using Mantid::Geometry::MDHistoDimension;
using namespace Mantid::API;
using namespace Mantid::DataObjects;
using namespace Mantid::Geometry;
using namespace Mantid::MDEvents;
using Mantid::Kernel::V3D;


class CentroidPeaksMDTest : public CxxTest::TestSuite
{
public:

  void test_Init()
  {
    CentroidPeaksMD alg;
    TS_ASSERT_THROWS_NOTHING( alg.initialize() )
    TS_ASSERT( alg.isInitialized() )
  }
  


  //-------------------------------------------------------------------------------
  /** Create the (blank) MDEW */
  static void createMDEW()
  {
    // ---- Start with empty MDEW ----
    FrameworkManager::Instance().exec("CreateMDWorkspace", 14,
        "Dimensions", "3",
        "Extents", "-10,10,-10,10,-10,10",
        "Names", "h,k,l",
        "Units", "-,-,-",
        "SplitInto", "5",
        "MaxRecursionDepth", "2",
        "OutputWorkspace", "CentroidPeaksMDTest_MDEWS");
  }


  //-------------------------------------------------------------------------------
  /** Add a fake "peak"*/
  static void addPeak(size_t num, double x, double y, double z, double radius)
  {
    std::ostringstream mess;
    mess << num << ", " << x << ", " << y << ", " << z << ", " << radius;
    FrameworkManager::Instance().exec("FakeMDEventData", 6,
        "InputWorkspace", "CentroidPeaksMDTest_MDEWS",
        "PeakParams", mess.str().c_str(),
        "RandomSeed", "1234");

  }


  //-------------------------------------------------------------------------------
  /** Run the CentroidPeaksMD with the given peak radius param */
  void doRun( V3D startPos, double PeakRadius, V3D expectedResult, std::string message,
      std::string OutputWorkspace = "CentroidPeaksMDTest_Peaks")
  {
    // Make a fake instrument - doesn't matter, we won't use it really
    Instrument_sptr inst = ComponentCreationHelper::createTestInstrumentCylindrical(5);

    // --- Make a fake PeaksWorkspace in the given coordinate space ---
    PeaksWorkspace_sptr peakWS(new PeaksWorkspace());

    Peak pIn(inst, 1, 1.0, startPos );
    if (CoordinatesToUse == "Q (lab frame)")
      pIn.setQLabFrame(startPos);
    else if (CoordinatesToUse == "Q (sample frame)")
      pIn.setQSampleFrame(startPos);
    else if (CoordinatesToUse == "HKL")
      pIn.setHKL(startPos);
    peakWS->addPeak( pIn );

    TS_ASSERT_EQUALS( peakWS->getPeak(0).getIntensity(), 0.0);
    AnalysisDataService::Instance().addOrReplace("CentroidPeaksMDTest_Peaks", peakWS);

    CentroidPeaksMD alg;
    TS_ASSERT_THROWS_NOTHING( alg.initialize() )
    TS_ASSERT( alg.isInitialized() )
    TS_ASSERT_THROWS_NOTHING( alg.setPropertyValue("InputWorkspace", "CentroidPeaksMDTest_MDEWS" ) );
    TS_ASSERT_THROWS_NOTHING( alg.setPropertyValue("PeaksWorkspace", "CentroidPeaksMDTest_Peaks" ) );
    TS_ASSERT_THROWS_NOTHING( alg.setPropertyValue("CoordinatesToUse", CoordinatesToUse ) );
    TS_ASSERT_THROWS_NOTHING( alg.setPropertyValue("OutputWorkspace", OutputWorkspace ) );
    TS_ASSERT_THROWS_NOTHING( alg.setProperty("PeakRadius", PeakRadius ) );
    TS_ASSERT_THROWS_NOTHING( alg.execute() );
    TS_ASSERT( alg.isExecuted() );

    peakWS = boost::dynamic_pointer_cast<PeaksWorkspace>(
        AnalysisDataService::Instance().retrieve(OutputWorkspace));
    TS_ASSERT(peakWS);
    if (!peakWS) return;

    // Compare the result to the expectation
    V3D result;
    IPeak & p = peakWS->getPeak(0);
    if (CoordinatesToUse == "Q (lab frame)")
      result = p.getQLabFrame();
    else if (CoordinatesToUse == "Q (sample frame)")
    {
      std::cerr << p.getGoniometerMatrix() << std::endl;
      result = p.getQSampleFrame();
    }
    else if (CoordinatesToUse == "HKL")
      result = p.getHKL();

    for (size_t i=0; i<3; i++)
      TSM_ASSERT_DELTA( message, result[i], expectedResult[i], 0.05);

    AnalysisDataService::Instance().remove("CentroidPeaksMDTest_Peaks");
  }

  //-------------------------------------------------------------------------------
  /** Full test using faked-out peak data */
  void do_test_exec()
  {
    // --- Fake workspace with 3 peaks ------
    createMDEW();
    addPeak(1000, 0,0.,0., 1.0);
    addPeak(1000, 2.,3.,4., 0.5);
    addPeak(1000, 6.,6.,6., 2.0);

    MDEventWorkspace3Lean::sptr mdews = AnalysisDataService::Instance().retrieveWS<MDEventWorkspace3Lean>("CentroidPeaksMDTest_MDEWS");
    TS_ASSERT_EQUALS( mdews->getNPoints(), 3000);
    TS_ASSERT_DELTA( mdews->getBox()->getSignal(), 3000.0, 1e-2);

    if (CoordinatesToUse == "HKL")
    {
      doRun(V3D( 0.,0.,0.), 1.0, V3D( 0.,0.,0.), "Start at the center, get the center");

      doRun(V3D( 0.2,0.2,0.2), 1.8, V3D( 0.,0.,0.), "Somewhat off center");
    }

    doRun(V3D( 2.,3.,4.), 1.0, V3D( 2.,3.,4.), "Start at the center, get the center");

    doRun(V3D( 1.5,2.5,3.5), 3.0, V3D( 2.,3.,4.), "Pretty far off");

    doRun(V3D( 1.0,1.5,2.0), 4.0, V3D( 1.0,1.5,2.0), "Include two peaks, get the centroid of the two");

    doRun(V3D( 8.0, 0.0, 1.0), 1.0, V3D( 8.0, 0.0, 1.0), "Include no events, get no change");

    doRun(V3D( 6.,6.,6.), 0.1, V3D( 6.,6.,6.), "Small radius still works");

    AnalysisDataService::Instance().remove("CentroidPeaksMDTest_MDEWS");
  }

  void test_exec_HKL()
  {
    CoordinatesToUse = "HKL";
    do_test_exec();
  }

  void test_exec_QSampleFrame()
  {
    CoordinatesToUse = "Q (sample frame)";
    do_test_exec();
  }

  void test_exec_QLabFrame()
  {
    CoordinatesToUse = "Q (lab frame)";
    do_test_exec();
  }

  void test_exec_HKL_NotInPlace()
  {
    CoordinatesToUse = "HKL";
    createMDEW();
    addPeak(1000, 0,0.,0., 1.0);
    doRun(V3D( 0.,0.,0.), 1.0, V3D( 0.,0.,0.), "Start at the center, get the center",
        "CentroidPeaksMDTest_MDEWS_outputCopy");

  }

private:
  std::string CoordinatesToUse;



};

#endif /* MANTID_MDEVENTS_MDCENTROIDPEAKSTEST_H_ */

