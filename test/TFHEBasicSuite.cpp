//
// Created by ubuntu on 1/11/25.
//

#include <gtest/gtest.h>

#include <chrono>

#include "tfhe.h"

using namespace std;

class TFHEBasicSuite : public ::testing::Test {
protected:
    const int N = 100;
    const int interval = 10;
    const double ns_per_ms = 1000000;
};

TEST_F(TFHEBasicSuite, IntDecPerfTest) {
    Config* config = nullptr;
    ConfigBuilder* config_builder = nullptr;

    config_builder_default(&config_builder);
    config_builder_use_custom_parameters(&config_builder, SHORTINT_V0_11_PARAM_MESSAGE_2_CARRY_2_KS_PBS_GAUSSIAN_2M64);
    // config_builder_default_with_small_encryption(&config_builder);
    config_builder_build(config_builder, &config);
    ClientKey* client_key = nullptr;
    ServerKey* server_key = nullptr;
    generate_keys(config, &client_key, &server_key);
    set_server_key(server_key);
    PublicKey* public_key;
    public_key_new(client_key, &public_key);

    FheInt32* val = nullptr;
    cout << "Warm in" << endl;
    for (int i = 0; i < 5; i++) {
        fhe_int32_try_encrypt_with_public_key_i32(0, public_key, &val);
        int tmp;
        fhe_int32_decrypt(val, client_key, &tmp);
        fhe_int32_destroy(val);
    }

    cout << "Start Test Int32 Decryption" << endl;
    long total = 0;
    for (int i = 0; i < N; i++) {
        if (i % interval == 0) {
            cout << i << "..." << endl;
        }
        fhe_int32_try_encrypt_with_public_key_i32(0, public_key, &val);
        int tmp;
        auto start = std::chrono::high_resolution_clock::now();
        fhe_int32_decrypt(val, client_key, &tmp);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = end - start;
        fhe_int32_destroy(val);
        total += duration.count();
    }
    cout << endl;
    cout << "Total time: " << total / ns_per_ms << endl;
    cout << "Avg time:" << static_cast<double>(total / ns_per_ms) / N << endl;
    client_key_destroy(client_key);
    server_key_destroy(server_key);
    public_key_destroy(public_key);
}

TEST_F(TFHEBasicSuite, BoolDecPerfTest) {
    Config* config = nullptr;
    ConfigBuilder* config_builder = nullptr;

    config_builder_default(&config_builder);
    config_builder_use_custom_parameters(&config_builder, SHORTINT_V0_11_PARAM_MESSAGE_2_CARRY_2_KS_PBS_GAUSSIAN_2M64);
    // config_builder_default_with_small_encryption(&config_builder);
    config_builder_build(config_builder, &config);
    ClientKey* client_key = nullptr;
    ServerKey* server_key = nullptr;
    generate_keys(config, &client_key, &server_key);
    set_server_key(server_key);
    PublicKey* public_key;
    public_key_new(client_key, &public_key);

    FheBool* val = nullptr;
    cout << "Warm in" << endl;
    for (int i = 0; i < 5; i++) {
        fhe_bool_try_encrypt_with_public_key_bool(false, public_key, &val);
        bool tmp;
        fhe_bool_decrypt(val, client_key, &tmp);
        fhe_bool_destroy(val);
    }

    cout << "Start Test bool Decryption" << endl;
    long total = 0;
    for (int i = 0; i < N; i++) {
        if (i % interval == 0) {
            cout << i << "..." << endl;
        }
        fhe_bool_try_encrypt_with_public_key_bool(false, public_key, &val);
        bool tmp;
        auto start = std::chrono::high_resolution_clock::now();
        fhe_bool_decrypt(val, client_key, &tmp);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = end - start;
        fhe_bool_destroy(val);
        total += duration.count();
    }
    cout << endl;
    cout << "Total time: " << total / ns_per_ms << endl;
    cout << "Avg time:" << static_cast<double>(total / ns_per_ms) / N << endl;
    client_key_destroy(client_key);
    server_key_destroy(server_key);
    public_key_destroy(public_key);
}

TEST_F(TFHEBasicSuite, AddPerfTest) {
    Config* config = nullptr;
    ConfigBuilder* config_builder = nullptr;

    config_builder_default(&config_builder);
    config_builder_use_custom_parameters(&config_builder, SHORTINT_V0_11_PARAM_MESSAGE_2_CARRY_2_KS_PBS_GAUSSIAN_2M64);
    // config_builder_default_with_small_encryption(&config_builder);
    config_builder_build(config_builder, &config);
    ClientKey* client_key = nullptr;
    ServerKey* server_key = nullptr;
    generate_keys(config, &client_key, &server_key);
    set_server_key(server_key);
    PublicKey* public_key;
    public_key_new(client_key, &public_key);

    FheInt32* a = nullptr;
    FheInt32* b = nullptr;
    cout << "Warm in" << endl;
    for (int i = 0; i < 5; i++) {
        fhe_int32_try_encrypt_with_public_key_i32(0, public_key, &a);
        fhe_int32_try_encrypt_with_public_key_i32(1, public_key, &b);
        FheInt32* result = nullptr;
        fhe_int32_add(a, b, &result);
        fhe_int32_destroy(a);
        fhe_int32_destroy(b);
        fhe_int32_destroy(result);
    }

    cout << "Start Test Int32 Add" << endl;
    long total = 0;
    for (int i = 0; i < N; i++) {
        if (i % interval == 0) {
            cout << i << "..." << endl;
        }
        fhe_int32_try_encrypt_with_public_key_i32(i, public_key, &a);
        fhe_int32_try_encrypt_with_public_key_i32(i * 2, public_key, &b);
        FheInt32* result = nullptr;
        auto start = std::chrono::high_resolution_clock::now();
        fhe_int32_add(a, b, &result);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = end - start;
        fhe_int32_destroy(a);
        fhe_int32_destroy(b);
        fhe_int32_destroy(result);
        total += duration.count();
    }
    cout << endl;
    cout << "Total time: " << total / ns_per_ms << endl;
    cout << "Avg time:" << static_cast<double>(total / ns_per_ms) / N << endl;
    client_key_destroy(client_key);
    server_key_destroy(server_key);
    public_key_destroy(public_key);
}

#ifdef WITH_FEATURE_GPU
TEST_F(TFHEBasicSuite, GPUIntDecPerfTest) {
    Config* config = nullptr;
    ConfigBuilder* config_builder = nullptr;

    config_builder_default(&config_builder);
    config_builder_build(config_builder, &config);
    ClientKey* client_key = nullptr;
    client_key_generate(config, &client_key);
    CompressedServerKey* compressed_server_key = nullptr;
    compressed_server_key_new(client_key, &compressed_server_key);
    CudaServerKey* cuda_server_key = nullptr;
    compressed_server_key_decompress_to_gpu(compressed_server_key, &cuda_server_key);
    set_cuda_server_key(cuda_server_key);
    PublicKey* public_key;
    public_key_new(client_key, &public_key);

    FheInt32* val = nullptr;
    cout << "Warm in" << endl;
    for (int i = 0; i < 5; i++) {
        fhe_int32_try_encrypt_with_public_key_i32(0, public_key, &val);
        int tmp;
        fhe_int32_decrypt(val, client_key, &tmp);
        fhe_int32_destroy(val);
    }

    cout << "Start Test Int32 Decryption" << endl;
    long total = 0;
    for (int i = 0; i < N; i++) {
        if (i % interval == 0) {
            cout << i << "..." << endl;
        }
        fhe_int32_try_encrypt_with_public_key_i32(0, public_key, &val);
        int tmp;
        auto start = std::chrono::high_resolution_clock::now();
        fhe_int32_decrypt(val, client_key, &tmp);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = (end - start).count();
        fhe_int32_destroy(val);
        total += duration;
    }
    cout << endl;
    cout << "Total time: " << total / ns_per_ms << endl;
    cout << "Avg time:" << static_cast<double>(total / ns_per_ms) / N << endl;
    client_key_destroy(client_key);
    compressed_server_key_destroy(compressed_server_key);
    cuda_server_key_destroy(cuda_server_key);
    public_key_destroy(public_key);
}

TEST_F(TFHEBasicSuite, GPUBoolDecPerfTest) {
    Config* config = nullptr;
    ConfigBuilder* config_builder = nullptr;

    config_builder_default(&config_builder);
    config_builder_build(config_builder, &config);
    ClientKey* client_key = nullptr;
    client_key_generate(config, &client_key);
    CompressedServerKey* compressed_server_key = nullptr;
    compressed_server_key_new(client_key, &compressed_server_key);
    CudaServerKey* cuda_server_key = nullptr;
    compressed_server_key_decompress_to_gpu(compressed_server_key, &cuda_server_key);
    set_cuda_server_key(cuda_server_key);
    PublicKey* public_key;
    public_key_new(client_key, &public_key);

    FheBool* val = nullptr;
    cout << "Warm in" << endl;
    for (int i = 0; i < 5; i++) {
        fhe_bool_try_encrypt_with_public_key_bool(false, public_key, &val);
        bool tmp;
        fhe_bool_decrypt(val, client_key, &tmp);
        fhe_bool_destroy(val);
    }

    cout << "Start Test bool Decryption" << endl;
    long total = 0;
    for (int i = 0; i < N; i++) {
        if (i % interval == 0) {
            cout << i << "..." << endl;
        }
        fhe_bool_try_encrypt_with_public_key_bool(false, public_key, &val);
        bool tmp;
        auto start = std::chrono::high_resolution_clock::now();
        fhe_bool_decrypt(val, client_key, &tmp);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = (end - start).count();
        fhe_bool_destroy(val);
        total += duration;
    }
    cout << endl;
    cout << "Total time: " << total / ns_per_ms << endl;
    cout << "Avg time:" << static_cast<double>(total / ns_per_ms) / N << endl;

    client_key_destroy(client_key);
    compressed_server_key_destroy(compressed_server_key);
    cuda_server_key_destroy(cuda_server_key);
    public_key_destroy(public_key);
}

TEST_F(TFHEBasicSuite, GPUAddPerfTest) {
    Config* config = nullptr;
    ConfigBuilder* config_builder = nullptr;

    config_builder_default(&config_builder);
    config_builder_build(config_builder, &config);
    ClientKey* client_key = nullptr;
    client_key_generate(config, &client_key);
    CompressedServerKey* compressed_server_key = nullptr;
    compressed_server_key_new(client_key, &compressed_server_key);
    CudaServerKey* cuda_server_key = nullptr;
    compressed_server_key_decompress_to_gpu(compressed_server_key, &cuda_server_key);
    set_cuda_server_key(cuda_server_key);
    PublicKey* public_key;
    public_key_new(client_key, &public_key);

    FheInt32* a = nullptr;
    FheInt32* b = nullptr;
    cout << "Warm in" << endl;
    for (int i = 0; i < 5; i++) {
        fhe_int32_try_encrypt_with_public_key_i32(0, public_key, &a);
        fhe_int32_try_encrypt_with_public_key_i32(1, public_key, &b);
        FheInt32* result = nullptr;
        fhe_int32_add(a, b, &result);
        fhe_int32_destroy(a);
        fhe_int32_destroy(b);
        fhe_int32_destroy(result);
    }

    cout << "Start Test Int32 Add" << endl;
    long total = 0;
    for (int i = 0; i < N; i++) {
        if (i % interval == 0) {
            cout << i << "..." << endl;
        }
        fhe_int32_try_encrypt_with_public_key_i32(i, public_key, &a);
        fhe_int32_try_encrypt_with_public_key_i32(i * 2, public_key, &b);
        FheInt32* result = nullptr;
        auto start = std::chrono::high_resolution_clock::now();
        fhe_int32_add(a, b, &result);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = (end - start).count();
        fhe_int32_destroy(a);
        fhe_int32_destroy(b);
        fhe_int32_destroy(result);
        total += duration;
    }
    cout << endl;
    cout << "Total time: " << total / ns_per_ms << endl;
    cout << "Avg time:" << static_cast<double>(total / ns_per_ms) / N << endl;
    client_key_destroy(client_key);
    compressed_server_key_destroy(compressed_server_key);
    cuda_server_key_destroy(cuda_server_key);
    public_key_destroy(public_key);
}

TEST_F(TFHEBasicSuite, GPUDiv) {
    Config* config = nullptr;
    ConfigBuilder* config_builder = nullptr;

    config_builder_default(&config_builder);
    config_builder_build(config_builder, &config);
    ClientKey* client_key = nullptr;
    client_key_generate(config, &client_key);
    CompressedServerKey* compressed_server_key = nullptr;
    compressed_server_key_new(client_key, &compressed_server_key);
    CudaServerKey* cuda_server_key = nullptr;
    compressed_server_key_decompress_to_gpu(compressed_server_key, &cuda_server_key);
    set_cuda_server_key(cuda_server_key);
    PublicKey* public_key;
    public_key_new(client_key, &public_key);

    FheInt32* a = nullptr;
    FheInt32* b = nullptr;
    fhe_int32_try_encrypt_with_public_key_i32(10, public_key, &a);
    fhe_int32_try_encrypt_with_public_key_i32(2, public_key, &b);
    FheInt32* result = nullptr;
    fhe_int32_div(a, b, &result);
    fhe_int32_destroy(a);
    fhe_int32_destroy(b);
    fhe_int32_destroy(result);

    client_key_destroy(client_key);
    compressed_server_key_destroy(compressed_server_key);
    cuda_server_key_destroy(cuda_server_key);
    public_key_destroy(public_key);
}
#endif
