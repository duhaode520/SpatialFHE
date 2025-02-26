//
// Created by ubuntu on 2/19/25.
//

#include "PerformanceTestRunner.h"
#include <iostream>
#include <chrono>

namespace SpatialFHE {


    void PerformanceTestRunner::run(PerformanceTestCase* test) {
        runInternal(test);
    }

    void PerformanceTestRunner::runInternal(PerformanceTestCase* test) {
        std::vector<int> runSize = test->getRunSize();
        int runIter = test->getRunIterations();

        test->setUp();
        for (size_t runNum = 0; runNum < runSize.size(); ++runNum) {
            int size = runSize[runNum];
            test->startRun(size);
            for (int iter = 0; iter < runIter; ++iter) {
                auto start = std::chrono::high_resolution_clock::now();
                // Call the test method here
                test->run();
                auto end = std::chrono::high_resolution_clock::now();
                double time = (end - start).count();
                std::cout << "Run " << test->getName() << " Iteration " << iter << " Time " << time / ns_per_ms << std::endl;
                test->setTime(runNum, time / ns_per_ms);
            }
            test->endRun();
        }
        test->tearDown();
    }} // SpatialFHE