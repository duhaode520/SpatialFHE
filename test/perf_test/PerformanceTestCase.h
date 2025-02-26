//
// Created by ubuntu on 2/19/25.
//

#ifndef PERFORMANCETESTCASE_H
#define PERFORMANCETESTCASE_H

#include <string>
#include <vector>

namespace SpatialFHE {

    class PerformanceTestCase {
    public:
        explicit PerformanceTestCase(const std::string& name);
        virtual ~PerformanceTestCase() = default;

        std::string getName() const;
        void setRunSize(const std::vector<int>& runSize);
        std::vector<int> getRunSize() const;
        void setRunIterations(int runIter);
        int getRunIterations() const;
        [[nodiscard]] std::vector<double> getRunTime() const;
        void setTime(int runNum, double time);

        virtual void setUp() = 0;
        virtual void startRun(int size) = 0;
        virtual void run() = 0;
        virtual void endRun() {}

        virtual void tearDown() {}

    private:
        std::string name;
        std::vector<int> runSize;
        int runIter;
        std::vector<double> runTime;
    };

}  // namespace SpatialFHE

#endif  // PERFORMANCETESTCASE_H
