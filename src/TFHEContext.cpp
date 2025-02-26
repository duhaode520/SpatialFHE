//
// Created by ubuntu on 1/9/25.
//

#include "TFHEContext.h"

#include <TFHEInt32.h>

namespace SpatialFHE {
    void TFHEContext::registerType() {
        TFHEInt32::registerContext(this);
        TFHEBool::registerContext(this);
    }

    void TFHEContext::generateKey() {
        config_builder_default(&configBuilder);
#ifndef WITH_FEATURE_GPU
        config_builder_use_custom_parameters(&configBuilder, SHORTINT_V0_11_PARAM_MESSAGE_2_CARRY_2_KS_PBS_GAUSSIAN_2M64);
        // config_builder_default_with_small_encryption(&configBuilder);
#endif
        config_builder_build(configBuilder, &config);
        generate_keys(config, &client_key, &server_key);
#ifdef WITH_FEATURE_GPU
        compressed_server_key_new(client_key, &compressed_server_key);
        compressed_server_key_decompress_to_gpu(compressed_server_key, &cuda_server_key);
        set_cuda_server_key(cuda_server_key);
#else
        set_server_key(server_key);
#endif
        // set_server_key(server_key);
        public_key_new(client_key, &public_key);
    }

    void TFHEContext::serializeKeys(
        const std::string &public_key_path,
        const std::string &client_key_path,
        const std::string &server_key_path) const {
        DynamicBuffer public_buffer, client_buffer, server_buffer;

        public_key_serialize(public_key, &public_buffer);
        FILE *public_key_file = fopen(public_key_path.c_str(), "wb");
        fwrite(public_buffer.pointer, 1, public_buffer.length, public_key_file);
        fclose(public_key_file);
        destroy_dynamic_buffer(&public_buffer);

        client_key_serialize(client_key, &client_buffer);
        FILE *client_key_file = fopen(client_key_path.c_str(), "wb");
        fwrite(client_buffer.pointer, 1, client_buffer.length, client_key_file);
        fclose(client_key_file);
        destroy_dynamic_buffer(&client_buffer);

        server_key_serialize(server_key, &server_buffer);
        FILE *server_key_file = fopen(server_key_path.c_str(), "wb");
        fwrite(server_buffer.pointer, 1, server_buffer.length, server_key_file);
        fclose(server_key_file);
        destroy_dynamic_buffer(&server_buffer);
    }

    void TFHEContext::rpcInit(const std::string &server_url) {
        // split url to host and port
        std::string host;
        int port;
        size_t pos = server_url.find(':');
        if (pos == std::string::npos) {
            host = server_url;
            port = 8080;
        } else {
            host = server_url.substr(0, pos);
            port = std::atoi(server_url.substr(pos + 1).c_str());
        }
        if (!isClient) {
            rpc_server = std::make_unique<rpc::server>(host, port);
            rpc_server->async_run(1);
        }
        rpc_client = std::make_unique<rpc::client>(host, port);
    }

    TFHEContext::TFHEContext() {
        isClient = false;
        rpcInit("127.0.0.1:8080");
        generateKey();
        registerType();
    }

    TFHEContext::TFHEContext(const std::string& server_url, const std::string &public_key_path, const std::string &server_key_path) {
        isClient = true;
        rpcInit(server_url);
        loadPublicKey(public_key_path);
        loadServerKey(server_key_path);
        registerType();
    }

    TFHEContext::TFHEContext(
        const std::string &server_url,
        const std::string &public_key_path,
        const std::string &client_key_path,
        const std::string &server_key_path) {
        isClient = false;
        rpcInit(server_url);
        generateKey();
        serializeKeys(public_key_path, client_key_path, server_key_path);
        registerType();
    }

    TFHEContext::~TFHEContext() {
        if (rpc_server != nullptr) {
            rpc_server->stop();
        }
        client_key_destroy(client_key);
        server_key_destroy(server_key);
        public_key_destroy(public_key);
#ifdef WITH_FEATURE_GPU
        compressed_server_key_destroy(compressed_server_key);
        cuda_server_key_destroy(cuda_server_key);
#endif
    }

    void TFHEContext::loadPublicKey(const std::string &public_key_path) {
        DynamicBufferView public_buffer;
        FILE *public_key_file = fopen(public_key_path.c_str(), "rb");
        fseek(public_key_file, 0, SEEK_END);
        long public_key_size = ftell(public_key_file);
        fseek(public_key_file, 0, SEEK_SET);
        public_buffer.pointer = (uint8_t *)malloc(public_key_size);
        public_buffer.length = public_key_size;
        fread((void*)public_buffer.pointer, 1, public_key_size, public_key_file);
        fclose(public_key_file);
        public_key_deserialize(public_buffer, &public_key);
        free((void*)public_buffer.pointer);

    }

    void TFHEContext::loadServerKey(const std::string &server_key_path) {
        DynamicBufferView server_buffer;
        FILE *server_key_file = fopen(server_key_path.c_str(), "rb");
        fseek(server_key_file, 0, SEEK_END);
        long server_key_size = ftell(server_key_file);
        fseek(server_key_file, 0, SEEK_SET);
        server_buffer.pointer = (uint8_t *)malloc(server_key_size);
        server_buffer.length = server_key_size;
        fread((void*)server_buffer.pointer, 1, server_key_size, server_key_file);
        fclose(server_key_file);

        server_key_deserialize(server_buffer, &server_key);
        free((void*)server_buffer.pointer);
    }

    ClientKey *TFHEContext::getClientKey() const {
        return client_key;
    }

    PublicKey *TFHEContext::getPublicKey() const {
        return public_key;
    }

    const std::unique_ptr<rpc::client> &TFHEContext::getRpcClient() const {
        return rpc_client;
    }

    const std::unique_ptr<rpc::server> &TFHEContext::getRpcServer() const {
        return rpc_server;
    }

    void TFHEContext::setServerKey() const {
#ifdef WITH_FEATURE_GPU
        set_cuda_server_key(cuda_server_key);
#endif
        set_server_key(server_key);
    }

    void TFHEContext::clear() {
        TFHEInt32::context = nullptr;
        TFHEBool::context = nullptr;
    }

}  // namespace SpatialFHE