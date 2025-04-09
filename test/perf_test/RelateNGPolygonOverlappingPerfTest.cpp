//
// Created by ubuntu on 2/26/25.
//

#include "RelateNGPolygonOverlappingPerfTest.h"

#include <tfhe_geos/operation/relateng/TFHERelateNG.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "PerformanceTestCase.h"
#include "tfhe_geos/geom/TFHEGeometry.h"
#include "tfhe_geos/util/SineStarFactory.h"
using namespace SpatialFHE::geom;

namespace SpatialFHE {

    RelateNGPolygonOverlappingPerfTest::RelateNGPolygonOverlappingPerfTest(const std::string& name) :
            PerformanceTestCase(name) {
        std::vector<int> run_size({20, 100});
        setRunSize(run_size);
        setRunIterations(N_ITER);
    }

    void RelateNGPolygonOverlappingPerfTest::setUp() {
        std::cout << "RelateNG Overlapping Polygons perf test" << std::endl;
        std::cout << "SineStar: origin: (" << ORG_X << ", " << ORG_Y << ")  size: " << SIZE << "  # arms: " << N_ARMS
                  << "  arm ratio: " << ARM_RATIO << std::endl;
        std::cout << "# Iterations: " << N_ITER << std::endl;
        std::cout << "# B geoms: " << NUM_CASES << std::endl;
    }

    void RelateNGPolygonOverlappingPerfTest::startRun(int npts) {
        TFHECoordinate ori((TFHEDecimal(ORG_X)), TFHEDecimal(ORG_Y));
        geomA = util::SineStarFactory::create(ori, SIZE, npts, N_ARMS, ARM_RATIO);

        int nptsB = npts * B_SIZE_FACTOR / NUM_CASES;
        if (nptsB < 10)
            nptsB = 10;

        createSineStarGridB(NUM_CASES, nptsB);
        // geomB =  createCircleGrid(NUM_CASES, nptsB);

        std::cout << "\n-------  Running with A: line # pts = " << npts << "   B # pts = " << nptsB << "  x "
                  << NUM_CASES << " lines" << std::endl;
    }

    void RelateNGPolygonOverlappingPerfTest::run() {
        runIntersectsNG();
    }

    void RelateNGPolygonOverlappingPerfTest::runIntersectsNG() {
        // int numThreads = std::thread::hardware_concurrency() / 4;
        int numThreads = 1;
        std::cout << "Running with " << numThreads << " threads" << std::endl;
        std::vector<std::thread> threads;
        std::vector<double> durations(numThreads);

        auto intersectsTask = [&](int start, int end, int threadIndex) {
            TFHEInt32::getContext()->setServerKey(true);
            auto startTime = std::chrono::high_resolution_clock::now();
            for (int i = start; i < end; i++) {
                std::unique_ptr<TFHEGeometry>& b = geomB[i];
                std::cout << i << ".." << std::endl;
                operation::relateng::TFHERelateNG::intersects(geomA.get(), b.get());
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            durations[threadIndex] = (endTime - startTime).count() / 1e6;
        };

        int chunkSize = geomB.size() / numThreads;
        for (int t = 0; t < numThreads; t++) {
            int start = t * chunkSize;
            int end = (t == numThreads - 1) ? geomB.size() : start + chunkSize;
            threads.emplace_back(intersectsTask, start, end, t);
        }

        for (auto& thread : threads) {
            thread.join();
        }

        for (int t = 0; t < numThreads; t++) {
            std::cout << "Thread " << t << " duration: " << durations[t] << " ms" << std::endl;
        }
        // 输出duration的sum
        double sum = 0;
        for (int t = 0; t < numThreads; t++) {
            sum += durations[t];
        }
        std::cout << "Total duration: " << sum << " ms" << std::endl;
    }

    void RelateNGPolygonOverlappingPerfTest::runContainsNG() {}

    void RelateNGPolygonOverlappingPerfTest::createSineStarGridB(int nGeoms, int npts) {
        geomB.resize(nGeoms);

        auto createGeom = [&](int start, int end) {
            for (int i = start; i < end; i++) {
                for (int j = 0; j < GRID_SIZE; j++) {
                    double x = GRID_CELL_SIZE / 2 + i * GRID_CELL_SIZE;
                    double y = GRID_CELL_SIZE / 2 + j * GRID_CELL_SIZE;

                    TFHEInt32::getContext()->setServerKey();
                    TFHECoordinate coord((TFHEDecimal(x)), TFHEDecimal(y));
                    geomB[i * GRID_SIZE + j] =
                        util::SineStarFactory::create(coord, GRID_CELL_SIZE, npts, N_ARMS, ARM_RATIO);
#ifndef DEBUG
                    // 没有DEBUG的时候，创建的速度比较慢
                    std::cout << "Created geomB[" << i * GRID_SIZE + j << "] at (" << x << ", " << y << ")"
                              << std::endl;
#endif
                }
            }
        };
        int numThreads = std::thread::hardware_concurrency() / 2;
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