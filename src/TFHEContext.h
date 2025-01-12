//
// Created by ubuntu on 1/9/25.
//

#ifndef TFHECRYPTO_H
#define TFHECRYPTO_H
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
        // OTClient
        // OTServer

        void registerType();
        void generateKey();
        void serializeKeys(
            const std::string& public_key_path,
            const std::string& client_key_path,
            const std::string& server_key_path) const;
    public:
        TFHEContext();
        TFHEContext(const std::string &public_key_path, const std::string &server_key_path);
        TFHEContext(
            const std::string& public_key_path,
            const std::string& client_key_path,
            const std::string& server_key_path);

        ~TFHEContext();

        void loadPublicKey(const std::string& public_key_path);
        void loadServerKey(const std::string& server_key_path);

        [[nodiscard]] ClientKey* getClientKey() const;
        [[nodiscard]] PublicKey* getPublicKey() const;

        // remote decryption


    };

} // SpatialFHE

#endif //TFHECRYPTO_H
