//
// Created by ubuntu on 2/19/25.
//

#include "PerformanceTestCase.h"

namespace SpatialFHE {

    PerformanceTestCase::PerformanceTestCase(const std::string& name)
        : name(name), runIter(1) {}

    std::string PerformanceTestCase::getName() const {
        return name;
    }

    void PerformanceTestCase::setRunSize(const std::vector<int>& runSize) {
        this->runSize = runSize;
        runTime.resize(runSize.size());
    }

    std::vector<int> PerformanceTestCase::getRunSize() const {
        return runSize;
    }

    void PerformanceTestCase::setRunIterations(int runIter) {
        this->runIter = runIter;
    }

    int PerformanceTestCase::getRunIterations() const {
        return runIter;
    }

    std::vector<double> PerformanceTestCase::getRunTime() const {
        return runTime;
    }

    void PerformanceTestCase::setTime(int runNum, double time) {
        runTime[runNum] = time;
    }} // SpatialFHE