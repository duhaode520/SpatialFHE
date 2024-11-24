#ifndef PHANTOMCRYPTO_H
#define PHANTOMCRYPTO_H
#include "cuda_runtime.h"
#include "phantom.h"
#include "context.cuh"
#include "HECrypto.h"

namespace SpatialFHE {
    class PhantomCrypto : public HECrypto {
        struct PhantomEncryptionParams {
            HECrypto::HEScheme schemeType;
            size_t polyModulusDegree;
            uint64_t plainModulus;
            std::vector<int> coeffModulusBits;
            std::vector<ulong> coeffModulusPrimes;
            int scaleFactor;
            double scale;
        };

    private:
        std::shared_ptr<PhantomEncryptionParams> params;
        std::shared_ptr<phantom::EncryptionParameters> phantomParams;
        std::shared_ptr<PhantomContext> context;
        PhantomPublicKey publicKey;
        PhantomSecretKey secretKey;
        PhantomRelinKey relinKey;
        PhantomGaloisKey galoisKeys;
        std::shared_ptr<PhantomCKKSEncoder> ckksEncoder;
        std::shared_ptr<PhantomBatchEncoder> batchEncoder;
        size_t slot_count{};
        bool batching{};

        void update_encryption_params(CryptoParams &params);
        void initPhantomParams();
        void set_encoder(HEScheme scheme);
        static phantom::scheme_type set_phantom_scheme(HEScheme scheme);

        void parms_unify(PhantomCiphertext& src, PhantomCiphertext&dst);
        void parms_unify(PhantomPlaintext &src, PhantomCiphertext &dst) const;

        void _encrypt(PhantomCiphertext &ct, PhantomPlaintext const &pt);
        void _decrypt(PhantomPlaintext &pt, PhantomCiphertext const &ct);

        void _encode(PhantomPlaintext &pt, double d);
        void _encode(PhantomPlaintext &pt, long l);

        void _add(PhantomCiphertext &ct_1, PhantomCiphertext const &ct_2);
        void _add(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2);
        void _add_plain(PhantomCiphertext &ct, PhantomPlaintext const &pt);
        void _full_adder(
            std::vector<PhantomCiphertext> &result,
            std::vector<PhantomCiphertext> const &vec_ct_1,
            std::vector<PhantomCiphertext> const &vec_ct_2,
            size_t max_count);
        void _sub(PhantomCiphertext &ct_1, PhantomCiphertext const &ct_2);
        void _sub(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2);

        void _multiply(PhantomCiphertext &ct_1, PhantomCiphertext const &ct_2);
        void _multiply(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2);
        void _multiply_plain(PhantomCiphertext &ct, PhantomPlaintext const &pt);

        void _square(PhantomCiphertext &result, PhantomCiphertext const &ct);
        void _power(PhantomCiphertext &result, PhantomCiphertext const &ct, int const &n);

        void _rotate(PhantomCiphertext &result, PhantomCiphertext const &ct, int const &step);
        void _rotate_columns(PhantomCiphertext &result, PhantomCiphertext const &ct);
        void _shift(PhantomCiphertext &result, PhantomCiphertext const &ct, int const &n);

        void _and(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2);
        void _or(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2);
        void _xor(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2);
        void _xnor(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2);
        void _not(PhantomCiphertext &result, PhantomCiphertext const &ct);

        void _mask(PhantomCiphertext &result, PhantomCiphertext const &ct, int const &index);
        void _mask(PhantomCiphertext &result, PhantomCiphertext const &ct, std::vector<int> const &indices);

        void _total_sum(PhantomCiphertext &result, PhantomCiphertext const &ct);
        void _running_sum(PhantomCiphertext &result, PhantomCiphertext const &ct);

    public:
        PhantomCrypto();
        explicit PhantomCrypto(CryptoParams &params);
        explicit PhantomCrypto(const std::string &param_string);
        ~PhantomCrypto() override;

        void GenerateKeyPair(const std::string &pubKeyFilename, const std::string &secKeyFilename) override;

        void LoadPublicKey(const std::string &pubKeyFilename) override;

        PlainText Encode(double d) override;
        PlainText Encode(long l) override;
        PlainText Encode(const std::string &str) override;
        PlainText Encode(const std::vector<double> &vec) override;
        PlainText Encode(const std::vector<long> &vec) override;
        std::vector<PlainText> EncodeMany(const std::vector<double> &vec) override;
        std::vector<PlainText> EncodeMany(const std::vector<long> &vec) override;

        CipherText Encrypt(const PlainText &pt) override;
        std::string Encrypt(const std::string &str) override;
        std::string Encrypt(const std::vector<std::string> &spt_vec) override;

        void LoadSecretKey(const std::string &secKeyFilename) override;
        void Decode(std::vector<double> &vec, const PlainText &pt) override;
        void Decode(std::vector<long> &vec, const PlainText &pt) override;

        PlainText Decrypt(const CipherText &ct, bool noBatching) override;
        std::string Decrypt(const std::string &sct, bool noBatching) override;

        CipherText toCipherText(const std::string &str) override;
        std::vector<CipherText> toCipherText(const std::vector<std::string> &strs) override;
        PlainText toPlainText(const std::string &str) override;
        std::vector<PlainText> toPlainText(const std::vector<std::string> &strs) override;

        CipherText Add(const CipherText &ct_1, const CipherText &ct_2) override;
        std::string Add(const std::string &sct_1, const std::string &sct_2) override;
        CipherText AddPlain(const CipherText &ct, const PlainText &pt) override;
        std::string AddPlain(const std::string &sct, const std::string &spt) override;

        std::vector<CipherText> FullAdder(
            const std::vector<CipherText> &vec_ct_1,
            const std::vector<CipherText> &vec_ct_2,
            size_t max_count) override;
        std::vector<std::string> FullAdder(
            const std::vector<std::string> &vec_sct_1,
            const std::vector<std::string> &vec_sct_2,
            size_t max_count) override;

        CipherText Multiply(const CipherText &ct_1, const CipherText &ct_2) override;
        std::string Multiply(const std::string &sct_1, const std::string &sct_2) override;
        CipherText MultiplyPlain(const CipherText &ct, const PlainText &pt) override;
        std::string MultiplyPlain(const std::string &sct, const std::string &spt) override;

        CipherText Square(const CipherText &ct) override;
        std::string Square(const std::string &sct) override;

        CipherText Power(const CipherText &ct, const int &n) override;
        std::string Power(const std::string &sct, const int &n) override;

        CipherText Subtract(const CipherText &ct_1, const CipherText &ct_2) override;
        std::string Subtract(const std::string &sct_1, const std::string &sct_2) override;

        CipherText Rotate(const CipherText &ct, const int &n) override;
        std::string Rotate(const std::string &sct, const int &n) override;

        CipherText Shift(const CipherText &ct, const int &n) override;
        std::string Shift(const std::string &sct, const int &n) override;

        CipherText RotateColumns(const CipherText &ct) override;
        std::string RotateColumns(const std::string &sct) override;

        CipherText And(const CipherText &ct_1, const CipherText &ct_2) override;
        std::string And(const std::string &sct_1, const std::string &sct_2) override;

        CipherText Or(const CipherText &ct_1, const CipherText &ct_2) override;
        std::string Or(const std::string &sct_1, const std::string &sct_2) override;

        CipherText Xor(const CipherText &ct_1, const CipherText &ct_2) override;
        std::string Xor(const std::string &sct_1, const std::string &sct_2) override;

        CipherText Xnor(const CipherText &ct_1, const CipherText &ct_2) override;
        std::string Xnor(const std::string &sct_1, const std::string &sct_2) override;

        CipherText Not(const CipherText &ct) override;
        std::string Not(const std::string &sct) override;

        CipherText Mask(const CipherText &ct, const int &index) override;
        CipherText Mask(const CipherText &ct, const std::vector<int> &indices) override;
        std::string Mask(const std::string &sct, const int &index) override;
        std::string Mask(const std::string &sct, const std::vector<int> &indices) override;

        CipherText IsEqual(const CipherText &ct_1, const CipherText &ct_2) override;
        std::string IsEqual(const std::string &sct_1, const std::string &sct_2) override;

        CipherText RunningSum(const CipherText &ct) override;
        std::string RunningSum(const std::string &sct) override;
        CipherText TotalSum(const CipherText &ct) override;
        std::string TotalSum(const std::string &sct) override;

        CipherText buildCipherText(const std::string &str) const override;

        template <typename T>
        void createPlainVector(std::vector<PlainText> &vec, std::vector<T> const &data);

        void toPhantomCiphertext(PhantomCiphertext &ct, CipherText const &c);
        void toPhantomCiphertext(std::vector<PhantomCiphertext> &vec_ct, std::vector<CipherText> const &vec_c);

        void toPhantomPlaintext(PhantomPlaintext &pt, PlainText const &p);
        void toPhantomPlaintext(std::vector<PhantomPlaintext> &vec_pt, std::vector<PlainText> const &vec_p);

        void toCipherText(CipherText &c, PhantomCiphertext const &ct) const;
        void toCipherText(std::vector<CipherText> &vec_c, std::vector<PhantomCiphertext> const &vec_ct) const;

        static void toPlainText(PlainText &p, PhantomPlaintext const &pt);
        static void toPlainText(std::vector<PlainText> &vec_p, std::vector<PhantomPlaintext> const &vec_pt);
    };

}  // namespace SpatialFHE

#endif  // PHANTOMCRYPTO_H
