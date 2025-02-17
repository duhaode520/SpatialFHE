//
// Created by ubuntu on 1/9/25.
//

#ifndef TFHECRYPTO_H
#define TFHECRYPTO_H
#include <rpc/client.h>
#include <rpc/server.h>
#include <tfhe.h>

#include <string>

namespace SpatialFHE {

    class TFHEContext {
    private:
        Config* config = nullptr;
        ConfigBuilder* configBuilder = nullptr;
        PublicKey* public_key = nullptr;
        ServerKey* server_key = nullptr;
        ClientKey* client_key = nullptr;

        bool isClient;
        // OTClient
        std::unique_ptr<rpc::client> rpc_client;

        // OTServer
        std::unique_ptr<rpc::server> rpc_server;

        void registerType();
        void generateKey();
        void serializeKeys(
            const std::string& public_key_path,
            const std::string& client_key_path,
            const std::string& server_key_path) const;

        void rpcInit(const std::string& server_url);

    public:
        TFHEContext();
        TFHEContext(
            const std::string& server_url,
            const std::string& public_key_path,
            const std::string& server_key_path);
        TFHEContext(
            const std::string& server_url,
            const std::string& public_key_path,
            const std::string& client_key_path,
            const std::string& server_key_path);

        ~TFHEContext();

        void loadPublicKey(const std::string& public_key_path);
        void loadServerKey(const std::string& server_key_path);

        [[nodiscard]] ClientKey* getClientKey() const;
        [[nodiscard]] PublicKey* getPublicKey() const;

        [[nodiscard]] const std::unique_ptr<rpc::client>& getRpcClient() const;
        [[nodiscard]] const std::unique_ptr<rpc::server>& getRpcServer() const;

        // only for test
        void clear();
    };

}  // namespace SpatialFHE

#endif  // TFHECRYPTO_H
