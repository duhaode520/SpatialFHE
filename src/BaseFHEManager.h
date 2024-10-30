//
// Created by ubuntu on 10/14/24.
//

#ifndef BASEFHEMANAGER_H
#define BASEFHEMANAGER_H
#include "hecrypto.h"

namespace SpatialFHE {
    class BaseFHEManager {
    protected:
        std::shared_ptr<HECrypto> crypto;
        std::string publicKeyPath;

    public:
        // TODO: 后面可以再加上 HELibrary，现在只用 SEAL
        BaseFHEManager();
        BaseFHEManager(
            const std::string& publicKeyPath,
            const std::string& secretKeyPath,
            const std::string& paramsString,
            bool isInit);
        virtual ~BaseFHEManager() = default;

        // encode
        [[nodiscard]] PlainText encode(double d) const;
        [[nodiscard]] PlainText encode(long l) const;
        [[nodiscard]] PlainText encode(std::string const& str) const;
        [[nodiscard]] PlainText encode(std::vector<double> const& vec) const;
        // remains encodeMany not implemented

        [[nodiscard]] CipherText encrypt(PlainText const& pt) const;

        [[nodiscard]] CipherText encodeAndEncrypt(double d) const;
        [[nodiscard]] CipherText encodeAndEncrypt(long l) const;

        void decode(std::vector<double>& vec, PlainText const& pt) const;
        void decode(std::vector<long>& vec, PlainText const& pt) const;

        [[nodiscard]] PlainText decrypt(CipherText const& ct) const;

        void decodeAndDecrypt(std::vector<double>& vec, CipherText const& ct) const;
        void decodeAndDecrypt(std::vector<long>& vec, CipherText const& ct) const;

        [[nodiscard]] CipherText add(CipherText const& ct1, CipherText const& ct2) const;
        [[nodiscard]] CipherText addPlain(CipherText const& ct, PlainText const& pt) const;
        [[nodiscard]] CipherText sub(CipherText const& ct1, CipherText const& ct2) const;
        [[nodiscard]] CipherText multiply(CipherText const& ct1, CipherText const& ct2) const;
        [[nodiscard]] CipherText multiplyPlain(CipherText const& ct, PlainText const& pt) const;
        [[nodiscard]] CipherText square(CipherText const& ct) const;

        [[nodiscard]] CipherText buildCipherText(std::string const& str) const;
    };
}  // namespace SpatialFHE

#endif //BASEFHEMANAGER_H
