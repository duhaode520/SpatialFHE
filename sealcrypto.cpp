#include "sealcrypto.h"

using namespace std;

namespace SpatialFHE {
    SEALCrypto::SEALCrypto() : HECrypto() {
        seal::MemoryPoolHandle p_pool;
        // initialize seal context
        this->sealContext = std::make_shared<seal::SEALContext>();
        
        // Initialize keys
        this->publicKey = seal::PublicKey();
        this->secretKey = seal::SecretKey();
        this->relinKeys = seal::RelinKeys();
        this->galoisKeys = seal::GaloisKeys();

        // Initialize ciphertexts ctxt_one and ctxt_zero
        p_pool = seal::MemoryManager::GetPool();
        this->ctxt_one = seal::Ciphertext(p_pool);
        p_pool = seal::MemoryManager::GetPool();
        this->ctxt_zero = seal::Ciphertext(p_pool);
    }

    SEALCrypto::~SEALCrypto() {}

    void SEALCrypto::GenerateKeyPair(
        CryptoParams const &params,
        std::string const &pubKeyFilename,
        std::string const &secKeyFilename) {}

    void SEALCrypto::LoadKeyPair(std::string const &pubKeyFilename, std::string const &secKeyFilename) {}

    void SEALCrypto::LoadPublicKey(std::string const &pubKeyFilename) {}

    PlainText SEALCrypto::Encode(double d) {
        return PlainText();
    }

    PlainText SEALCrypto::Encode(long d) {
        return PlainText();
    }

    PlainText SEALCrypto::Encode(std::string const &str) {
        return PlainText();
    }

    PlainText SEALCrypto::Encode(std::vector<double> const &vec) {
        return PlainText();
    }

    PlainText SEALCrypto::Encode(std::vector<long> const &vec) {
        return PlainText();
    }

    std::vector<PlainText> SEALCrypto::EncodeMany(std::vector<double> const &vec) {
        return std::vector<PlainText>();
    }

    std::vector<PlainText> SEALCrypto::EncodeMany(std::vector<long> const &vec) {
        return std::vector<PlainText>();
    }

    CipherText SEALCrypto::Encrypt(PlainText const &pt) {
        return CipherText();
    }

    std::string SEALCrypto::Encrypt(std::string const &str) {
        return std::string();
    }

    std::string SEALCrypto::Encrypt(std::vector<std::string> const &spt_vec) {
        return std::string();
    }

    void SEALCrypto::LoadSecretkey(std::string const &secKeyFilename) {}

    void SEALCrypto::Decode(std::vector<double> &vec, PlainText const &pt) {}

    void SEALCrypto::Decode(std::vector<long> &vec, PlainText const &pt) {}

    PlainText SEALCrypto::Decrypt(CipherText const &ct, bool noBatching) {
        return PlainText();
    }

    std::string SEALCrypto::Decrypt(std::string const &sct, bool noBatching) {
        return std::string();
    }

    CipherText SEALCrypto::AsCipherText(std::string const &str) {
        return CipherText();
    }

    std::vector<CipherText> SEALCrypto::AsCipherText(std::vector<std::string> const &strs) {
        return std::vector<CipherText>();
    }

    PlainText SEALCrypto::AsPlainText(std::string const &str) {
        return PlainText();
    }

    std::vector<PlainText> SEALCrypto::AsPlainText(std::vector<std::string> const &strs) {
        return std::vector<PlainText>();
    }

    CipherText SEALCrypto::Add(CipherText const &ct_1, CipherText const &ct_2) {
        return CipherText();
    }

    std::string SEALCrypto::Add(std::string const &sct_1, std::string const &sct_2) {
        return std::string();
    }

    CipherText SEALCrypto::AddPlain(CipherText const &ct, PlainText const &pt) {
        return CipherText();
    }

    std::string SEALCrypto::AddPlain(std::string const &sct, std::string const &spt) {
        return std::string();
    }

    std::vector<CipherText> SEALCrypto::FullAdder(
        std::vector<CipherText> const &vec_ct_1,
        std::vector<CipherText> const &vec_ct_2,
        const size_t max_count) {
        return std::vector<CipherText>();
    }

    std::vector<string> SEALCrypto::FullAdder(
        std::vector<string> const &vec_ct_1,
        std::vector<string> const &vec_ct_2,
        const size_t max_count) {
        return std::vector<string>();
    }

    CipherText SEALCrypto::Multiply(CipherText const &ct_1, CipherText const &ct_2) {
        return CipherText();
    }

    std::string SEALCrypto::Multiply(std::string const &sct_1, std::string const &sct_2) {
        return std::string();
    }

    CipherText SEALCrypto::MultiplyPlain(CipherText const &ct, PlainText const &pt) {
        return CipherText();
    }

    std::string SEALCrypto::MultiplyPlain(std::string const &sct, std::string const &spt) {
        return std::string();
    }

    CipherText SEALCrypto::Square(CipherText const &ct) {
        return CipherText();
    }

    std::string SEALCrypto::Square(std::string const &sct) {
        return std::string();
    }

    CipherText SEALCrypto::Power(CipherText const &ct, int const &n) {
        return CipherText();
    }

    std::string SEALCrypto::Power(std::string const &sct, int const &n) {
        return std::string();
    }

    CipherText SEALCrypto::Subtract(CipherText const &ct_1, CipherText const &ct_2) {
        return CipherText();
    }

    std::string SEALCrypto::Subtract(std::string const &sct_1, std::string const &sct_2) {
        return std::string();
    }

    CipherText SEALCrypto::Rotate(CipherText const &ct, int const &n) {
        return CipherText();
    }

    std::string SEALCrypto::Rotate(std::string const &sct, int const &n) {
        return std::string();
    }

    CipherText SEALCrypto::Shift(CipherText const &ct, int const &n) {
        return CipherText();
    }

    std::string SEALCrypto::Shift(std::string const &sct, int const &n) {
        return std::string();
    }

    CipherText SEALCrypto::RotateColumns(CipherText const &ct) {
        return CipherText();
    }

    std::string SEALCrypto::RotateColumns(std::string const &sct) {
        return std::string();
    }

    CipherText SEALCrypto::And(CipherText const &ct_1, CipherText const &ct_2) {
        return CipherText();
    }

    std::string SEALCrypto::And(std::string const &sct_1, std::string const &sct_2) {
        return std::string();
    }

    CipherText SEALCrypto::Or(CipherText const &ct_1, CipherText const &ct_2) {
        return CipherText();
    }

    std::string SEALCrypto::Or(std::string const &sct_1, std::string const &sct_2) {
        return std::string();
    }

    CipherText SEALCrypto::Xor(CipherText const &ct_1, CipherText const &ct_2) {
        return CipherText();
    }

    std::string SEALCrypto::Xor(std::string const &sct_1, std::string const &sct_2) {
        return std::string();
    }

    CipherText SEALCrypto::Xnor(CipherText const &ct_1, CipherText const &ct_2) {
        return CipherText();
    }

    std::string SEALCrypto::Xnor(std::string const &sct_1, std::string const &sct_2) {
        return std::string();
    }

    CipherText SEALCrypto::Not(CipherText const &ct) {
        return CipherText();
    }

    std::string SEALCrypto::Not(std::string const &sct) {
        return std::string();
    }

    CipherText SEALCrypto::Mask(CipherText const &ct, int const &index) {
        return CipherText();
    }

    CipherText SEALCrypto::Mask(CipherText const &ct, std::vector<int> const &indices) {
        return CipherText();
    }

    std::string SEALCrypto::Mask(std::string const &sct, int const &index) {
        return std::string();
    }

    std::string SEALCrypto::Mask(std::string const &sct, std::vector<int> const &indices) {
        return std::string();
    }

    CipherText SEALCrypto::IsEqual(CipherText const &ct_1, CipherText const &ct_2) {
        return CipherText();
    }

    std::string SEALCrypto::IsEqual(std::string const &sct_1, std::string const &sct_2) {
        return std::string();
    }

    CipherText SEALCrypto::RunningSum(CipherText const &ct) {
        return CipherText();
    }

    std::string SEALCrypto::RunningSum(std::string const &sct) {
        return std::string();
    }

    CipherText SEALCrypto::TotalSum(CipherText const &ct) {
        return CipherText();
    }

    std::string SEALCrypto::TotalSum(std::string const &sct) {
        return std::string();
    }

    template <typename T>
    void SEALCrypto::createPlainVector(std::vector<seal::Plaintext> &vec, std::vector<T> const &data) {}

    template <typename T>
    void SEALCrypto::createMask(std::vector<T> &mask, const int &index) {}

    template <typename T>
    void SEALCrypto::createShiftMask(std::vector<T> &mask, const int step, const int n) {}

    CipherText SEALCrypto::getOne() {
        return CipherText();
    }

    CipherText SEALCrypto::getZero() {
        return CipherText();
    }

    void SEALCrypto::setEncoder(HECrypto::HEScheme scheme) {}

    void SEALCrypto::setHEScheme(HECrypto::HEScheme scheme) {}

    void SEALCrypto::setEncryptionParams(CryptoParams const &params) {}

    void SEALCrypto::toSealCipherText(seal::Ciphertext &ct, CipherText const &c) {}

    void SEALCrypto::toSealCipherText(std::vector<seal::Ciphertext> &vec_ct, std::vector<CipherText> const &vec_c) {}

    void SEALCrypto::toSealPlainText(seal::Plaintext &pt, PlainText const &p) {}

    void SEALCrypto::toSealPlainText(std::vector<seal::Plaintext> &vec_pt, std::vector<PlainText> const &vec_p) {}

    void SEALCrypto::_encrypt(seal::Ciphertext &ct, seal::Plaintext const &pt) {}

    void SEALCrypto::_decrypt(seal::Plaintext &pt, seal::Ciphertext const &ct) {}

    void SEALCrypto::_add(seal::Ciphertext &ct_1, seal::Ciphertext const &ct_2) {}

    void SEALCrypto::_add(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {}

    void SEALCrypto::_add_plain(seal::Ciphertext &ct, seal::Plaintext const &pt) {}

    void SEALCrypto::_full_adder(
        std::vector<seal::Ciphertext> &result,
        std::vector<seal::Ciphertext> const &vec_ct_1,
        std::vector<seal::Ciphertext> const &vec_ct_2,
        const size_t max_count) {}

    void SEALCrypto::_sub(seal::Ciphertext &ct_1, seal::Ciphertext const &ct_2) {}

    void SEALCrypto::_sub(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {}

    void SEALCrypto::_multiply(seal::Ciphertext &ct_1, seal::Ciphertext const &ct_2) {}

    void SEALCrypto::_multiply(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {}

    void SEALCrypto::_multiply_plain(seal::Ciphertext &ct, seal::Plaintext const &pt) {}

    void SEALCrypto::_square(seal::Ciphertext &result, seal::Ciphertext const &ct) {}

    void SEALCrypto::_power(seal::Ciphertext &result, seal::Ciphertext const &ct, int const &n) {}

    void SEALCrypto::_rotate(seal::Ciphertext &result, seal::Ciphertext const &ct, int const &step) {}

    void SEALCrypto::_rotate_columns(seal::Ciphertext &result, seal::Ciphertext const &ct) {}

    void SEALCrypto::_shift(seal::Ciphertext &result, seal::Ciphertext const &ct, int const &n) {}

    void SEALCrypto::_and(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {}

    void SEALCrypto::_or(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {}

    void SEALCrypto::_xor(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {}

    void SEALCrypto::_xnor(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {}

    void SEALCrypto::_not(seal::Ciphertext &result, seal::Ciphertext const &ct) {}

    void SEALCrypto::_mask(seal::Ciphertext &result, seal::Ciphertext const &ct, int const &index) {}

    void SEALCrypto::_mask(seal::Ciphertext &result, seal::Ciphertext const &ct, std::vector<int> const &indices) {}

    void SEALCrypto::_total_sum(seal::Ciphertext &result, seal::Ciphertext const &ct) {}

}  // namespace SpatialFHE
