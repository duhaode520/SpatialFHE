//
// Created by ubuntu on 2/20/25.
//

#ifndef TFHEBASESUITE_H
#define TFHEBASESUITE_H
#include <gtest/gtest.h>
#include <TFHEContext.h>

class TFHEBaseSuite : public ::testing::Test {

    std::unique_ptr<SpatialFHE::TFHEContext> context;

    void SetUp() override {
        context = std::make_unique<SpatialFHE::TFHEContext>();
    }

    void TearDown() override {
        context->clear();
        context.reset();
    }
};

#endif //TFHEBASESUITE_H
