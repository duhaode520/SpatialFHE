#pragma once

#include <seal/seal.h>
#include <rapidjson/document.h>

#include <map>
#include <string>
#include <vector>
#include "ciphertext.h"
#include "plaintext.h"

namespace SpatialFHE {
    extern const char* HESchemeStrings[];

    class HECrypto {
    private:
        /* data */

    public:
        enum class HEScheme {
            BGV,
            BFV,
            CKKS,
        };

        enum class HELibrary { SEAL, HELIB };

        using CryptoParams = std::map<std::string, rapidjson::Value>;
        HECrypto();
        ~HECrypto();

        // context

        virtual void GenerateKeyPair(
            CryptoParams& params,
            std::string const& pubKeyFilename,
            std::string const& secKeyFilename) = 0;
        virtual void LoadKeyPair(std::string const& pubKeyFilename, std::string const& secKeyFilename) = 0;

        // encode and encrypt

        virtual void LoadPublicKey(std::string const& pubKeyFilename) = 0;

        virtual PlainText Encode(double d) = 0;
        virtual PlainText Encode(long d) = 0;
        virtual PlainText Encode(std::string const& str) = 0;
        virtual PlainText Encode(std::vector<double> const& vec) = 0;
        virtual PlainText Encode(std::vector<long> const& vec) = 0;
        virtual std::vector<PlainText> EncodeMany(std::vector<double> const& vec) = 0;
        virtual std::vector<PlainText> EncodeMany(std::vector<long> const& vec) = 0;

        virtual CipherText Encrypt(PlainText const& pt) = 0;
        virtual std::string Encrypt(std::string const& str) = 0;
        virtual std::string Encrypt(std::vector<std::string> const& spt_vec) = 0;

        // decode and decrypt

        virtual void LoadSecretkey(std::string const& secKeyFilename) = 0;

        virtual void Decode(std::vector<double>& vec, PlainText const& pt) = 0;
        virtual void Decode(std::vector<long>& vec, PlainText const& pt) = 0;

        virtual PlainText Decrypt(CipherText const& ct, bool noBatching) = 0;
        virtual std::string Decrypt(std::string const& sct, bool noBatching) = 0;

        // transform to CipherText and PlainText

        virtual CipherText toCipherText(std::string const& str) = 0;
        virtual std::vector<CipherText> toCipherText(std::vector<std::string> const& strs) = 0;

        virtual PlainText toPlainText(std::string const& str) = 0;
        virtual std::vector<PlainText> toPlainText(std::vector<std::string> const& strs) = 0;

        // algebraic operations

        virtual CipherText Add(CipherText const& ct_1, CipherText const& ct_2) = 0;
        virtual std::string Add(std::string const& sct_1, std::string const& sct_2) = 0;

        virtual CipherText AddPlain(CipherText const& ct, PlainText const& pt) = 0;
        virtual std::string AddPlain(std::string const& sct, std::string const& spt) = 0;

        virtual std::vector<CipherText> FullAdder(
            std::vector<CipherText> const& vec_ct_1,
            std::vector<CipherText> const& vec_ct_2,
            const size_t max_count) = 0;
        virtual std::vector<std::string> FullAdder(
            std::vector<std::string> const& vec_sct_1,
            std::vector<std::string> const& vec_sct_2,
            const size_t max_count) = 0;

        virtual CipherText Multiply(CipherText const& ct_1, CipherText const& ct_2) = 0;
        virtual std::string Multiply(std::string const& sct_1, std::string const& sct_2) = 0;

        virtual CipherText MultiplyPlain(CipherText const& ct, PlainText const& pt) = 0;
        virtual std::string MultiplyPlain(std::string const& sct, std::string const& spt) = 0;

        virtual CipherText Square(CipherText const& ct) = 0;
        virtual std::string Square(std::string const& sct) = 0;

        virtual CipherText Power(CipherText const& ct, int const& n) = 0;
        virtual std::string Power(std::string const& sct, int const& n) = 0;

        virtual CipherText Subtract(CipherText const& ct_1, CipherText const& ct_2) = 0;
        virtual std::string Subtract(std::string const& sct_1, std::string const& sct_2) = 0;

        virtual CipherText Rotate(CipherText const& ct, int const& n) = 0;
        virtual std::string Rotate(std::string const& sct, int const& n) = 0;

        virtual CipherText Shift(CipherText const& ct, int const& n) = 0;
        virtual std::string Shift(std::string const& sct, int const& n) = 0;

        virtual CipherText RotateColumns(CipherText const& ct) = 0;
        virtual std::string RotateColumns(std::string const& sct) = 0;

        // logical operations

        virtual CipherText And(CipherText const& ct_1, CipherText const& ct_2) = 0;
        virtual std::string And(std::string const& sct_1, std::string const& sct_2) = 0;

        virtual CipherText Or(CipherText const& ct_1, CipherText const& ct_2) = 0;
        virtual std::string Or(std::string const& sct_1, std::string const& sct_2) = 0;

        virtual CipherText Xor(CipherText const& ct_1, CipherText const& ct_2) = 0;
        virtual std::string Xor(std::string const& sct_1, std::string const& sct_2) = 0;

        virtual CipherText Xnor(CipherText const& ct_1, CipherText const& ct_2) = 0;
        virtual std::string Xnor(std::string const& sct_1, std::string const& sct_2) = 0;

        virtual CipherText Not(CipherText const& ct) = 0;
        virtual std::string Not(std::string const& sct) = 0;

        virtual CipherText Mask(CipherText const& ct, int const& index) = 0;
        virtual CipherText Mask(CipherText const& ct, std::vector<int> const& indices) = 0;

        virtual std::string Mask(std::string const& sct, int const& index) = 0;
        virtual std::string Mask(std::string const& sct, std::vector<int> const& indices) = 0;

        virtual CipherText IsEqual(CipherText const& ct_1, CipherText const& ct_2) = 0;
        virtual std::string IsEqual(std::string const& sct_1, std::string const& sct_2) = 0;

        // aggregation
        virtual CipherText RunningSum(CipherText const& ct) = 0;
        virtual std::string RunningSum(std::string const& sct) = 0;

        virtual CipherText TotalSum(CipherText const& ct) = 0;
        virtual std::string TotalSum(std::string const& sct) = 0;

        // gets
        // unknown use, deprecated
        // virtual void* GetCryptoParam(std::string const& param) = 0;

        static const char* HESchemeToString(HEScheme scheme);
    };

}  // namespace SpatialFHE
