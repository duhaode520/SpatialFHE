//
// Created by ubuntu on 2/26/25.
//

#ifndef RELATENGPOLYGONOVERLAPPINGPERFTEST_H
#define RELATENGPOLYGONOVERLAPPINGPERFTEST_H

#include <memory>

#include "PerformanceTestCase.h"
#include "tfhe_geos/geom/TFHEGeometry.h"
#include "tfhe_geos/geom/TFHEGeometryFactory.h"

using namespace SpatialFHE::geom;

namespace SpatialFHE {
    class RelateNGPolygonOverlappingPerfTest : public PerformanceTestCase {
    public:
        explicit RelateNGPolygonOverlappingPerfTest(const std::string& name);
        void setUp() override;
        void startRun(int npts) override;
        void run() override;
        void runIntersectsNG();
        void runContainsNG();

    private:
        // Add necessary members and methods for geometry creation and testing
        const static int N_ITER = 1;

        const double ORG_X = 1000;
        const double ORG_Y = ORG_X;
        const double SIZE = 2 * ORG_X;
        const int N_ARMS = 6;
        const double ARM_RATIO = 0.3;

        const int GRID_SIZE = 10;
        const double GRID_CELL_SIZE = SIZE / GRID_SIZE;

        const int NUM_CASES = GRID_SIZE * GRID_SIZE;

        const int B_SIZE_FACTOR = 20;
        TFHEGeometryFactory::Ptr factory = TFHEGeometryFactory::create();

        std::unique_ptr<TFHEGeometry> geomA;
        std::vector<std::unique_ptr<TFHEGeometry>> geomB;

        void createSineStarGridB(int nGeoms, int npts);
    };
};  // namespace SpatialFHE

#endif  // RELATENGPOLYGONOVERLAPPINGPERFTEST_H
