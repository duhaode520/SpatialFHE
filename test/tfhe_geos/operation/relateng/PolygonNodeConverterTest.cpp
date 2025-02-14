//
// Created by ubuntu on 2/10/25.
//
#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <vector>

// geos
#include <tfhe_geos/algorithm/TFHEPolygonNodeTopology.h>
#include <tfhe_geos/geom/TFHEGeometry.h>
#include <tfhe_geos/geom/enums.h>
#include <tfhe_geos/io/WKTReader.h>
#include <tfhe_geos/operation/relateng/TFHEPolygonNodeConverter.h>

// utility header (if needed)

using namespace SpatialFHE::geom;
using namespace SpatialFHE::operation::relateng;
using SpatialFHE::TFHEInt32;
using SpatialFHE::algorithm::TFHEPolygonNodeTopology;

// The fixture for testing PolygonNodeConverter.
// It mimics the functionalities of the original TUT test fixture.
class PolygonNodeConverterTest : public ::testing::Test {
    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }

protected:
    // Helper function to create a new NodeSection. For shells the ringId is 0 and for holes it is 1.
    TFHENodeSection* section(
        int ringId,
        TFHEInt32&& v0x,
        TFHEInt32&& v0y,
        TFHEInt32&& nx,
        TFHEInt32&& ny,
        TFHEInt32&& v1x,
        TFHEInt32&& v1y) {
        return new TFHENodeSection(
            true,
            Dimension::A,
            1,
            ringId,
            nullptr,
            false,
            new TFHECoordinate(v0x, v0y),
            TFHECoordinate(nx, ny),
            new TFHECoordinate(v1x, v1y));
    }

    TFHENodeSection* sectionShell(int v0x, int v0y, int nx, int ny, int v1x, int v1y) {
        return section(0, TFHEInt32(v0x), TFHEInt32(v0y), TFHEInt32(nx), TFHEInt32(ny), TFHEInt32(v1x), TFHEInt32(v1y));
    }

    TFHENodeSection* sectionHole(int v0x, int v0y, int nx, int ny, int v1x, int v1y) {
        return section(1, TFHEInt32(v0x), TFHEInt32(v0y), TFHEInt32(nx), TFHEInt32(ny), TFHEInt32(v1x), TFHEInt32(v1y));
    }

    // Converts a vector of unique_ptr<NodeSection> to a vector of bare pointers.
    std::vector<const TFHENodeSection*> toPtrVector(const std::vector<std::unique_ptr<TFHENodeSection>>& input) {
        std::vector<const TFHENodeSection*> vec;
        for (const auto& ptr : input) {
            vec.emplace_back(ptr.get());
        }
        return vec;
    }

    // Compares two sets of NodeSection pointers.
    bool checkSectionsEqual(std::vector<const TFHENodeSection*>& ns1, std::vector<const TFHENodeSection*>& ns2) {
        if (ns1.size() != ns2.size())
            return false;
        sort(ns1);
        sort(ns2);
        for (std::size_t i = 0; i < ns1.size(); i++) {
            if (ns1[i]->compareTo(ns2[i]) != 0) {
                std::cout << "Mismatch at index " << i << std::endl;
                return false;
            }
        }
        return true;
    }

    // Sorts the vector using the compareTo method.
    void sort(std::vector<const TFHENodeSection*>& ns) {
        auto comparator = [](const TFHENodeSection* a, const TFHENodeSection* b) { return a->compareTo(b) < 0; };
        std::sort(ns.begin(), ns.end(), comparator);
    }

    // Frees the memory allocated for NodeSection and its Coordinates.
    void freeNodeSections(std::vector<const TFHENodeSection*>& ns) {
        for (auto section : ns) {
            delete section->getVertex(0);
            delete section->getVertex(1);
            delete section;
        }
    }

    // Checks conversion by calling PolygonNodeConverter::convert, converting the result to a vector of raw pointers,
    // comparing them with expected values, and freeing allocated nodes.
    void checkConversion(std::vector<const TFHENodeSection*>& input, std::vector<const TFHENodeSection*>& expected) {
        auto actualUnique = TFHEPolygonNodeConverter::convert(input);
        std::vector<const TFHENodeSection*> actual = toPtrVector(actualUnique);
        bool isEqual = checkSectionsEqual(actual, expected);
        freeNodeSections(input);
        freeNodeSections(expected);
        ASSERT_TRUE(isEqual);
    }
};

// Test case for shells only.
TEST_F(PolygonNodeConverterTest, TestShells) {
    std::vector<const TFHENodeSection*> input{
        sectionShell(1, 1, 5, 5, 9, 9), sectionShell(8, 9, 5, 5, 6, 9), sectionShell(4, 9, 5, 5, 2, 9)};
    std::vector<const TFHENodeSection*> expected{
        sectionShell(1, 1, 5, 5, 9, 9), sectionShell(8, 9, 5, 5, 6, 9), sectionShell(4, 9, 5, 5, 2, 9)};
    checkConversion(input, expected);
}

// Test case for one shell and one hole.
TEST_F(PolygonNodeConverterTest, TestShellAndHole) {
    std::vector<const TFHENodeSection*> input{sectionShell(1, 1, 5, 5, 9, 9), sectionHole(6, 0, 5, 5, 4, 0)};
    std::vector<const TFHENodeSection*> expected{sectionShell(1, 1, 5, 5, 4, 0), sectionShell(6, 0, 5, 5, 9, 9)};
    checkConversion(input, expected);
}

// Test case for shells with holes in between.
TEST_F(PolygonNodeConverterTest, TestShellsAndHoles) {
    std::vector<const TFHENodeSection*> input{
        sectionShell(1, 1, 5, 5, 9, 9),
        sectionHole(6, 0, 5, 5, 4, 0),
        sectionShell(8, 8, 5, 5, 1, 8),
        sectionHole(4, 8, 5, 5, 6, 8)};
    std::vector<const TFHENodeSection*> expected{
        sectionShell(1, 1, 5, 5, 4, 0),
        sectionShell(6, 0, 5, 5, 9, 9),
        sectionShell(4, 8, 5, 5, 1, 8),
        sectionShell(8, 8, 5, 5, 6, 8)};
    checkConversion(input, expected);
}

// Test case for a shell with two holes.
TEST_F(PolygonNodeConverterTest, TestShellAndTwoHoles) {
    std::vector<const TFHENodeSection*> input{
        sectionShell(1, 1, 5, 5, 9, 9), sectionHole(7, 0, 5, 5, 6, 0), sectionHole(4, 0, 5, 5, 3, 0)};
    std::vector<const TFHENodeSection*> expected{
        sectionShell(1, 1, 5, 5, 3, 0), sectionShell(4, 0, 5, 5, 6, 0), sectionShell(7, 0, 5, 5, 9, 9)};
    checkConversion(input, expected);
}

// Test case for holes only.
TEST_F(PolygonNodeConverterTest, TestHoles) {
    std::vector<const TFHENodeSection*> input{sectionHole(7, 0, 5, 5, 6, 0), sectionHole(4, 0, 5, 5, 3, 0)};
    std::vector<const TFHENodeSection*> expected{sectionShell(4, 0, 5, 5, 6, 0), sectionShell(7, 0, 5, 5, 3, 0)};
    checkConversion(input, expected);
}