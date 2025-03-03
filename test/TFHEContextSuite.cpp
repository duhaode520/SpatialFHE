//
// Created by ubuntu on 3/3/25.
//

#include <gtest/gtest.h>
#include <filesystem>

#define private public
#include "TFHEContext.h"
#undef private

namespace SpatialFHE {

class TFHEContextTest : public ::testing::Test {
protected:
    const std::string public_key_path = "public_key_test.bin";
    const std::string client_key_path = "client_key_test.bin";
    const std::string server_key_path = "server_key_test.bin";

    void TearDown() override {
        // 测试结束后删除生成的密钥文件
        std::filesystem::remove(public_key_path);
        std::filesystem::remove(client_key_path);
        std::filesystem::remove(server_key_path);
    }
};

TEST_F(TFHEContextTest, KeySerializationAndDeserialization) {
    // 第一步：创建上下文并生成密钥，然后序列化
    {
        TFHEContext context;
        context.serializeKeys(public_key_path, client_key_path, server_key_path);
        
        // 验证文件是否创建成功
        ASSERT_TRUE(std::filesystem::exists(public_key_path));
        ASSERT_TRUE(std::filesystem::exists(client_key_path));
        ASSERT_TRUE(std::filesystem::exists(server_key_path));
    }

    // 第二步：创建新的上下文，加载公钥和服务器密钥
    {
        TFHEContext clientContext("127.0.0.1:8081", public_key_path, server_key_path);
        ASSERT_NE(clientContext.getPublicKey(), nullptr);
        
        // 验证密钥是否可用
        // 注: 这里我们只能验证密钥指针不为空，因为没有提供验证密钥内容的方法
    }

    // 第三步：测试完整的密钥系统（生成和加载）
    {
        // 使用本地URL创建服务器上下文
        std::string server_url = "127.0.0.1:8082";
        TFHEContext serverContext(server_url, public_key_path, client_key_path, server_key_path);
        
        // 然后创建客户端上下文，从文件中加载密钥
        TFHEContext clientContext(server_url, public_key_path, server_key_path);
        
        // 验证两个上下文是否都能正确初始化
        ASSERT_NE(serverContext.getClientKey(), nullptr);
        ASSERT_NE(serverContext.getPublicKey(), nullptr);
        ASSERT_NE(clientContext.getPublicKey(), nullptr);
    }
}

}  // namespace SpatialFHE

