//
// Created by ubuntu on 2/19/25.
//

#include "RelateNGLinesOverlappingPerfTest.h"

#include <tfhe_geos/operation/relateng/TFHERelateNG.h>

#include <iostream>
#include <vector>

#include "PerformanceTestCase.h"
#include "tfhe_geos/geom/TFHEGeometry.h"
#include "tfhe_geos/util/SineStarFactory.h"
using namespace SpatialFHE::geom;

namespace SpatialFHE {
    RelateNGLinesOverlappingPerfTest::RelateNGLinesOverlappingPerfTest(const std::string& name) :
            PerformanceTestCase(name) {
        std::vector<int> run_size({20, 100});
        setRunSize(run_size);
        setRunIterations(N_ITER);
    }

    void RelateNGLinesOverlappingPerfTest::setUp() {
        std::cout << "RelateNG Overlapping Lines perf test" << std::endl;
        std::cout << "SineStar: origin: (" << ORG_X << ", " << ORG_Y << ")  size: " << SIZE << "  # arms: " << N_ARMS
                  << "  arm ratio: " << ARM_RATIO << std::endl;
        std::cout << "# Iterations: " << N_ITER << std::endl;
        std::cout << "# B geoms: " << NUM_CASES << std::endl;
    }

    void RelateNGLinesOverlappingPerfTest::startRun(int npts) {
        TFHECoordinate ori((TFHEDecimal(ORG_X)), TFHEDecimal(ORG_Y));
        std::unique_ptr<TFHEGeometry> sineStar = util::SineStarFactory::create(ori, SIZE, npts, N_ARMS, ARM_RATIO);
        geomA = sineStar->getBoundary();

        int nptsB = npts * B_SIZE_FACTOR / NUM_CASES;
        if (nptsB < 10)
            nptsB = 10;

        createSineStarGridB(NUM_CASES, nptsB);
        // geomB =  createCircleGrid(NUM_CASES, nptsB);

        std::cout << "\n-------  Running with A: line # pts = " << npts << "   B # pts = " << nptsB << "  x "
                  << NUM_CASES << " lines" << std::endl;
    }

    void RelateNGLinesOverlappingPerfTest::run() {
        runIntersectsNG();
    }

    void RelateNGLinesOverlappingPerfTest::runIntersectsOld() {}

    void RelateNGLinesOverlappingPerfTest::runIntersectsOldPrep() {}

    void RelateNGLinesOverlappingPerfTest::runIntersectsNG() {
        for (int i = 0; i < geomB.size(); i++) {
            std::unique_ptr<TFHEGeometry>& b = geomB[i];
            std::cout << i << ".." << std::endl;
            operation::relateng::TFHERelateNG::intersects(geomA.get(), b.get());
        }
        std::cout << std::endl;
    }

    void RelateNGLinesOverlappingPerfTest::runIntersectsNGPrep() {}

    void RelateNGLinesOverlappingPerfTest::createSineStarGridB(int nGeoms, int npts) {
        //         geomB.reserve(nGeoms);
        //         int index = 0;
        //         for (int i = 0; i < GRID_SIZE; i++) {
        //             for (int j = 0; j < GRID_SIZE; j++) {
        //                 double x = GRID_CELL_SIZE/2 + i * GRID_CELL_SIZE;
        //                 double y = GRID_CELL_SIZE/2 + j * GRID_CELL_SIZE;
        // #ifndef DEBUG
        //                 // 没有DEBUG的时候，创建的速度比较慢
        //                 std::cout << "Creating geomB[" << index << "] at (" << x << ", " << y << ")" << std::endl;
        // #endif
        //                 TFHECoordinate coord((TFHEInt32(x, true)), TFHEInt32(y, true));
        //                 auto geom = util::SineStarFactory::create(coord, GRID_CELL_SIZE, npts, N_ARMS, ARM_RATIO);
        //                 geomB[index++] = geom->getBoundary();
        //             }
        //         }

        geomB.resize(nGeoms);

        auto createGeom = [&](int start, int end) {
            for (int i = start; i < end; i++) {
                for (int j = 0; j < GRID_SIZE; j++) {
                    double x = GRID_CELL_SIZE / 2 + i * GRID_CELL_SIZE;
                    double y = GRID_CELL_SIZE / 2 + j * GRID_CELL_SIZE;

                    TFHEInt32::getContext()->setServerKey();
                    TFHECoordinate coord((TFHEDecimal(x )), TFHEDecimal(y ));
                    auto geom = util::SineStarFactory::create(coord, GRID_CELL_SIZE, npts, N_ARMS, ARM_RATIO);
#ifndef DEBUG
                    // 没有DEBUG的时候，创建的速度比较慢
                    std::cout << "Created geomB[" << i*GRID_SIZE+j << "] at (" << x << ", " << y << ")" << std::endl;
#endif
                    geomB[i*GRID_SIZE + j] = geom->getBoundary();
                }
            }
        };

        int numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        int chunkSize = GRID_SIZE / numThreads;

        for (int t = 0; t < numThreads; t++) {
            int start = t * chunkSize;
            int end = (t == numThreads - 1) ? GRID_SIZE : start + chunkSize;
            threads.emplace_back(createGeom, start, end);
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }
}  // namespace SpatialFHE