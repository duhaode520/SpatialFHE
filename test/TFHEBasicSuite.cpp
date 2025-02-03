//
// Created by ubuntu on 1/11/25.
//

#include <gtest/gtest.h>

#include <chrono>

#include "tfhe.h"

using namespace std;

class TFHEBasicSuite : public ::testing::Test {};

TEST_F(TFHEBasicSuite, IntDecPerfTest) {
    Config* config = nullptr;
    ConfigBuilder* config_builder = nullptr;

    config_builder_default(&config_builder);
    config_builder_default_with_small_encryption(&config_builder);
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
    int N = 500;
    for (int i = 0; i < N; i++) {
        if (i % 100 == 0) {
            cout << i << "...";
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
    cout << "Total time: " << total << endl;
    cout << "Avg time:" << static_cast<double>(total) / N << endl;
}

TEST_F(TFHEBasicSuite, BoolDecPerfTest) {
    Config* config = nullptr;
    ConfigBuilder* config_builder = nullptr;

    config_builder_default(&config_builder);
    config_builder_default_with_small_encryption(&config_builder);
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
    int N = 500;
    for (int i = 0; i < N; i++) {
        if (i % 100 == 0) {
            cout << i << "...";
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
    cout << "Total time: " << total << endl;
    cout << "Avg time:" << static_cast<double>(total) / N << endl;
}

TEST_F(TFHEBasicSuite, AddPerfTest) {
    Config* config = nullptr;
    ConfigBuilder* config_builder = nullptr;

    config_builder_default(&config_builder);
    config_builder_default_with_small_encryption(&config_builder);
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
    int N = 500;
    for (int i = 0; i < N; i++) {
        if (N % 100 == 0) {
            cout << i << "...";
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
    cout << "Total time: " << total << endl;
    cout << "Avg time:" << static_cast<double>(total) / N << endl;
}