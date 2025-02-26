//
// Created by ubuntu on 2/17/25.
//
#include <TFHEContext.h>
#include <gtest/gtest.h>
#include <rpc/client.h>
#include <rpc/server.h>

#include <chrono>
#include <thread>

using namespace SpatialFHE;
#define private public
#include <TFHEBool.h>
#undef private
class RemoteDecryptSuite : public ::testing::Test {
};

// 测试 "add" 函数
TEST_F(RemoteDecryptSuite, doDecryptTest) {
    std::unique_ptr<TFHEContext> context = std::make_unique<TFHEContext>();
    rpc::client client("127.0.0.1", 8080);
    TFHEBool positive = TFHEBool(true);
    TFHEBool negative = TFHEBool(false);
    bool result = positive.remoteDecrypt();
    bool result2 = negative.remoteDecrypt();
    EXPECT_EQ(result, true);
    EXPECT_EQ(result2, false);
    // auto result = client.call("doDecrypt");
}
