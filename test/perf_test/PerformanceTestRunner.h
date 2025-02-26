//
// Created by ubuntu on 2/19/25.
//

#ifndef PERFORMANCETESTRUNNER_H
#define PERFORMANCETESTRUNNER_H
#include "PerformanceTestCase.h"
#include <vector>
#include <string>

namespace SpatialFHE {


class PerformanceTestRunner {
public:
    static void run(PerformanceTestCase* test);

private:
    static constexpr int ns_per_ms = 1000000;
    PerformanceTestRunner() = default;
    static void runInternal(PerformanceTestCase* test);
};


} // SpatialFHE

#endif //PERFORMANCETESTRUNNER_H
