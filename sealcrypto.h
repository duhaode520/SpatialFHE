#pragma once

#include "hecrypto.h"

namespace SpatialFHE {
    class SEALCrypto : public HECrypto {
        struct SEALEncryptionParams {
            HECrypto::HEScheme schemeType;
            size_t polyModulusDegree;
            uint64_t plainModulus;
            std::vector<int> coeffModulusBits;
            std::vector<int> coeffModulusPrimes;
            double scale;
        };

    private:
        // members

        SEALEncryptionParams* params;
        seal::EncryptionParameters* encryptionParams;
        std::shared_ptr<seal::SEALContext> sealContext;
        seal::PublicKey publicKey;
        seal::SecretKey secretKey;
        seal::RelinKeys relinKeys;
        seal::GaloisKeys galoisKeys;
        seal::CKKSEncoder* ckksEncoder;
        seal::BatchEncoder* batchEncoder;
        size_t slot_count;
        bool batching;
        seal::Encryptor* encryptor;
        seal::Evaluator* evaluator;
        seal::Decryptor* decryptor;
        seal::Ciphertext ctxt_one;
        seal::Ciphertext ctxt_zero;

        // real implmentation of overloaded functions

        void _encrypt(seal::Ciphertext& ct, seal::Plaintext const& pt);
        void _decrypt(seal::Plaintext& pt, seal::Ciphertext const& ct);

        void _add(seal::Ciphertext& ct_1, seal::Ciphertext const& ct_2);
        void _add(seal::Ciphertext& result, seal::Ciphertext const& ct_1, seal::Ciphertext const& ct_2);
        void _add_plain(seal::Ciphertext& ct, seal::Plaintext const& pt);
        void _full_adder(
            std::vector<seal::Ciphertext>& result,
            std::vector<seal::Ciphertext> const& vec_ct_1,
            std::vector<seal::Ciphertext> const& vec_ct_2,
            const size_t max_count);
        void _sub(seal::Ciphertext& ct_1, seal::Ciphertext const& ct_2);
        void _sub(seal::Ciphertext& result, seal::Ciphertext const& ct_1, seal::Ciphertext const& ct_2);

        void _multiply(seal::Ciphertext& ct_1, seal::Ciphertext const& ct_2);
        void _multiply(seal::Ciphertext& result, seal::Ciphertext const& ct_1, seal::Ciphertext const& ct_2);
        void _multiply_plain(seal::Ciphertext& ct, seal::Plaintext const& pt);

        void _square(seal::Ciphertext& result, seal::Ciphertext const& ct);
        void _power(seal::Ciphertext& result, seal::Ciphertext const& ct, int const& n);

        void _rotate(seal::Ciphertext& result, seal::Ciphertext const& ct, int const& step);
        void _rotate_columns(seal::Ciphertext& result, seal::Ciphertext const& ct);
        void _shift(seal::Ciphertext& result, seal::Ciphertext const& ct, int const& n);

        void _and(seal::Ciphertext& result, seal::Ciphertext const& ct_1, seal::Ciphertext const& ct_2);
        void _or(seal::Ciphertext& result, seal::Ciphertext const& ct_1, seal::Ciphertext const& ct_2);
        void _xor(seal::Ciphertext& result, seal::Ciphertext const& ct_1, seal::Ciphertext const& ct_2);
        void _xnor(seal::Ciphertext& result, seal::Ciphertext const& ct_1, seal::Ciphertext const& ct_2);
        void _not(seal::Ciphertext& result, seal::Ciphertext const& ct);

        void _mask(seal::Ciphertext& result, seal::Ciphertext const& ct, int const& index);
        void _mask(seal::Ciphertext& result, seal::Ciphertext const& ct, std::vector<int> const& indices);

        void _total_sum(seal::Ciphertext& result, seal::Ciphertext const& ct);
        void _running_sum(seal::Ciphertext& result, seal::Ciphertext const& ct);

    public:
        SEALCrypto(/* args */);
        ~SEALCrypto();

        // context

        void GenerateKeyPair(
            CryptoParams const& params,
            std::string const& pubKeyFilename,
            std::string const& secKeyFilename) override;
        void LoadKeyPair(std::string const& pubKeyFilename, std::string const& secKeyFilename) override;

        // encode and encrypt

        void LoadPublicKey(std::string const& pubKeyFilename) override;

        PlainText Encode(double d) override;
        PlainText Encode(long d) override;
        PlainText Encode(std::string const& str) override;
        PlainText Encode(std::vector<double> const& vec) override;
        PlainText Encode(std::vector<long> const& vec) override;
        std::vector<PlainText> EncodeMany(std::vector<double> const& vec) override;
        std::vector<PlainText> EncodeMany(std::vector<long> const& vec) override;

        CipherText Encrypt(PlainText const& pt) override;
        std::string Encrypt(std::string const& spt) override;
        std::string Encrypt(std::vector<std::string> const& spt_vec) override;

        // decode and decrypt

        void LoadSecretkey(std::string const& secKeyFilename) override;

        void Decode(std::vector<double>& vec, PlainText const& pt) override;
        void Decode(std::vector<long>& vec, PlainText const& pt) override;

        PlainText Decrypt(CipherText const& ct, bool noBatching) override;
        std::string Decrypt(std::string const& sct, bool noBatching) override;

        // transform to CipherText and PlainText

        CipherText AsCipherText(std::string const& str) override;
        std::vector<CipherText> AsCipherText(std::vector<std::string> const& strs) override;

        PlainText AsPlainText(std::string const& str) override;
        std::vector<PlainText> AsPlainText(std::vector<std::string> const& strs) override;

        // algebraic operations

        CipherText Add(CipherText const& ct_1, CipherText const& ct_2) override;
        std::string Add(std::string const& sct_1, std::string const& sct_2) override;

        CipherText AddPlain(CipherText const& ct, PlainText const& pt) override;
        std::string AddPlain(std::string const& sct, std::string const& spt) override;

        std::vector<CipherText> FullAdder(
            std::vector<CipherText> const& vec_ct_1,
            std::vector<CipherText> const& vec_ct_2,
            const size_t max_count) override;
        std::vector<std::string> FullAdder(
            std::vector<std::string> const& vec_sct_1,
            std::vector<std::string> const& vec_sct_2,
            const size_t max_count) override;

        CipherText Multiply(CipherText const& ct_1, CipherText const& ct_2) override;
        std::string Multiply(std::string const& sct_1, std::string const& sct_2) override;

        CipherText MultiplyPlain(CipherText const& ct, PlainText const& pt) override;
        std::string MultiplyPlain(std::string const& sct, std::string const& spt) override;

        CipherText Square(CipherText const& ct) override;
        std::string Square(std::string const& sct) override;

        CipherText Power(CipherText const& ct, int const& n) override;
        std::string Power(std::string const& sct, int const& n) override;

        CipherText Subtract(CipherText const& ct_1, CipherText const& ct_2) override;
        std::string Subtract(std::string const& sct_1, std::string const& sct_2) override;

        CipherText Rotate(CipherText const& ct, int const& n) override;
        std::string Rotate(std::string const& sct, int const& n) override;

        CipherText Shift(CipherText const& ct, int const& n) override;
        std::string Shift(std::string const& sct, int const& n) override;

        CipherText RotateColumns(CipherText const& ct) override;
        std::string RotateColumns(std::string const& sct) override;

        // logical operations

        CipherText And(CipherText const& ct_1, CipherText const& ct_2) override;
        std::string And(std::string const& sct_1, std::string const& sct_2) override;

        CipherText Or(CipherText const& ct_1, CipherText const& ct_2) override;
        std::string Or(std::string const& sct_1, std::string const& sct_2) override;

        CipherText Xor(CipherText const& ct_1, CipherText const& ct_2) override;
        std::string Xor(std::string const& sct_1, std::string const& sct_2) override;

        CipherText Xnor(CipherText const& ct_1, CipherText const& ct_2) override;
        std::string Xnor(std::string const& sct_1, std::string const& sct_2) override;

        CipherText Not(CipherText const& ct) override;
        std::string Not(std::string const& sct) override;

        CipherText Mask(CipherText const& ct, int const& index) override;
        CipherText Mask(CipherText const& ct, std::vector<int> const& indices) override;

        std::string Mask(std::string const& sct, int const& index) override;
        std::string Mask(std::string const& sct, std::vector<int> const& indices) override;

        CipherText IsEqual(CipherText const& ct_1, CipherText const& ct_2) override;
        std::string IsEqual(std::string const& sct_1, std::string const& sct_2) override;

        // aggregation
        CipherText RunningSum(CipherText const& ct) override;
        std::string RunningSum(std::string const& sct) override;

        CipherText TotalSum(CipherText const& ct) override;
        std::string TotalSum(std::string const& sct) override;

        template <typename T>
        void createPlainVector(std::vector<PlainText>& vec, std::vector<T> const& data);

        template <typename T>
        void createMask(std::vector<T>& mask, const int& index);

        template <typename T>
        void createShiftMask(std::vector<T>& mask, const int step, const int n);

        // get functions
        CipherText getOne();
        CipherText getZero();

        // set functions
        void setEncoder(HECrypto::HEScheme scheme);
        void setHEScheme(HECrypto::HEScheme scheme);
        void setEncryptionParams(CryptoParams const& params);

        // tranform functions
        void toSealCipherText(seal::Ciphertext& ct, CipherText const& c);
        void toSealCipherText(std::vector<seal::Ciphertext>& vec_ct, std::vector<CipherText> const& vec_c);

        void toSealPlainText(seal::Plaintext& pt, PlainText const& p);
        void toSealPlainText(std::vector<seal::Plaintext>& vec_pt, std::vector<PlainText> const& vec_p);
    };

}  // namespace SpatialFHE
