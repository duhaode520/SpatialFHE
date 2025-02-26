//
// Created by ubuntu on 2/20/25.
//

#include <gtest/gtest.h>

#include "PerformanceTestRunner.h"
#include "RelateNGLinesOverlappingPerfTest.h"
#include "TFHEBaseSuite.h"
using namespace SpatialFHE;

class PerfTestSuite : public TFHEBaseSuite {

};

TEST_F(PerfTestSuite, testRelateNGLinesOverlappingPerfTest) {
    RelateNGLinesOverlappingPerfTest test("RelateNGLinesOverlappingPerfTest");
    PerformanceTestRunner::run(&test);
}