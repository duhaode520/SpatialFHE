#include "SealCrypto.h"
#include <b64/decode.h>
#include <seal/ciphertext.h>
#include <seal/plaintext.h>
#include <cmath>
#include <sstream>
#include <variant>
#include <vector>
#include "HECrypto.h"
#include "PlainText.h"
#include "FSManager.h"
#include "HDFSManager.h"


using namespace std;

namespace SpatialFHE {
    SEALCrypto::SEALCrypto() : HECrypto() {
        // initialize seal context
        // Initialize keys
        this->publicKey = seal::PublicKey();
        this->secretKey = seal::SecretKey();
        this->relinKeys = seal::RelinKeys();
        this->galoisKeys = seal::GaloisKeys();

        // Initialize ciphertexts ctxt_one and ctxt_zero
        this->ctxt_one = seal::Ciphertext();
        this->ctxt_zero = seal::Ciphertext();

        // Initialize pointers
        this->sealParams = nullptr;
        this->params = nullptr;
        this->encryptor = nullptr;
        this->decryptor = nullptr;
        this->evaluator = nullptr;
        this->ckksEncoder = nullptr;
        this->batchEncoder = nullptr;
    }

    SEALCrypto::SEALCrypto(CryptoParams &params) : SEALCrypto() {
        this->update_encryption_params(params);
        this->initSealParams();
        this->sealContext = std::make_shared<seal::SEALContext>(*this->sealParams);
        this->set_encoder(this->params->schemeType);
    }

    SEALCrypto::SEALCrypto(const std::string &params_string) : SEALCrypto() {
        CryptoParams params;
        rapidjson::Document doc;
        parse_params(params, doc, params_string);
        this->update_encryption_params(params);
        this->initSealParams();
        this->sealContext = std::make_shared<seal::SEALContext>(*this->sealParams);
        this->evaluator = make_shared<seal::Evaluator>(*this->sealContext);
        this->set_encoder(this->params->schemeType);
    }

    SEALCrypto::~SEALCrypto() = default;

    void SEALCrypto::GenerateKeyPair(std::string const &pubKeyFilename, std::string const &secKeyFilename) {
        // keygen
        seal::KeyGenerator keygen(*this->sealContext);

        this->secretKey = keygen.secret_key();

        keygen.create_public_key(this->publicKey);

        if (this->sealContext->using_keyswitching()) {
            keygen.create_relin_keys(this->relinKeys);
            if (this->batching) {
                keygen.create_galois_keys(this->galoisKeys);
            }
        }

        this->encryptor = make_shared<seal::Encryptor>(*this->sealContext, this->publicKey);
        this->decryptor = make_shared<seal::Decryptor>(*this->sealContext, this->secretKey);
        this->evaluator = make_shared<seal::Evaluator>(*this->sealContext);

        // save keys to file
        const std::shared_ptr<FSManager> secretKeyFS = FSManager::createFSManager(secKeyFilename);
        const std::shared_ptr<FSManager> publicKeyFS = FSManager::createFSManager(pubKeyFilename);
        publicKeyFS->OpenOutputStream();
        secretKeyFS->OpenOutputStream();
        this->publicKey.save(publicKeyFS->GetOutputStream());
        this->secretKey.save(secretKeyFS->GetOutputStream());
        publicKeyFS->CloseOutputStream();
        secretKeyFS->CloseOutputStream();
        if (this->sealContext->using_keyswitching()) {
            const std::shared_ptr<FSManager> relinKeyFS = FSManager::createFSManager(pubKeyFilename + ".relin");
            relinKeyFS->OpenOutputStream();
            this->relinKeys.save(relinKeyFS->GetOutputStream());
            relinKeyFS->CloseOutputStream();
            if (this->batching) {
                const std::shared_ptr<FSManager> galoisKeyFS = FSManager::createFSManager(pubKeyFilename + ".galois");
                galoisKeyFS->OpenOutputStream();
                this->galoisKeys.save(galoisKeyFS->GetOutputStream());
                galoisKeyFS->CloseOutputStream();
            }
        }
    }

    void SEALCrypto::LoadPublicKey(std::string const &pubKeyFilename) {
        const std::shared_ptr<FSManager> publicKeyFS = FSManager::createFSManager(pubKeyFilename);
        publicKeyFS->OpenInputStream();
        this->publicKey.load(*this->sealContext, publicKeyFS->GetInputStream());
        publicKeyFS->CloseInputStream();
        this->encryptor = make_shared<seal::Encryptor>(*this->sealContext, this->publicKey);
        if (this->sealContext->using_keyswitching()) {
            const std::shared_ptr<FSManager> relinKeyFS = FSManager::createFSManager(pubKeyFilename + ".relin");
            relinKeyFS->OpenInputStream();
            this->relinKeys.load(*this->sealContext, relinKeyFS->GetInputStream());
            relinKeyFS->CloseInputStream();
            if (this->batching) {
                const std::shared_ptr<FSManager> galoisKeyFS = FSManager::createFSManager(pubKeyFilename + ".galois");
                galoisKeyFS->OpenInputStream();
                this->galoisKeys.load(*this->sealContext, galoisKeyFS->GetInputStream());
                galoisKeyFS->CloseInputStream();
            }
        }
    }

    PlainText SEALCrypto::Encode(double d) {
        seal::Plaintext ptxt = seal::Plaintext();
        this->_encode(ptxt, d);
        return PlainText(ptxt);
    }

    PlainText SEALCrypto::Encode(long l) {
        seal::Plaintext ptxt = seal::Plaintext();
        this->_encode(ptxt, l);
        return PlainText(ptxt);
    }

    PlainText SEALCrypto::Encode(std::string const &str) {
        if (this->params->schemeType == HECrypto::HEScheme::BFV) {
            return this->Encode(stol(str));
        } else if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            return this->Encode(stod(str));
        } else {
            throw invalid_argument("Invalid scheme type");
        }
    }

    PlainText SEALCrypto::Encode(std::vector<double> const &vec) {
        seal::Plaintext ptxt = seal::Plaintext();
        this->ckksEncoder->encode(vec, this->params->scale, ptxt);
        return PlainText(ptxt);
    }

    PlainText SEALCrypto::Encode(std::vector<long> const &vec) {
        seal::Plaintext ptxt = seal::Plaintext();
        this->batchEncoder->encode(vec, ptxt);
        return PlainText(ptxt);
    }

    std::vector<PlainText> SEALCrypto::EncodeMany(std::vector<double> const &vec) {
        vector<PlainText> ptxt_vec;
        if (this->slot_count >= vec.size()) {
            ptxt_vec.emplace_back(this->Encode(vec));
        } else {
            this->createPlainVector<double>(ptxt_vec, vec);
        }
        return ptxt_vec;
    }

    std::vector<PlainText> SEALCrypto::EncodeMany(std::vector<long> const &vec) {
        vector<PlainText> ptxt_vec;
        if (this->slot_count >= vec.size()) {
            ptxt_vec.emplace_back(this->Encode(vec));
        } else {
            this->createPlainVector<long>(ptxt_vec, vec);
        }
        return ptxt_vec;
    }

    CipherText SEALCrypto::Encrypt(PlainText const &pt) {
        seal::Plaintext ptxt = seal::Plaintext();
        seal::Ciphertext ctxt = seal::Ciphertext();
        this->toSealPlaintext(ptxt, pt);
        this->_encrypt(ctxt, ptxt);
        return CipherText(ctxt);
    }

    std::string SEALCrypto::Encrypt(std::string const &spt) {
        return this->Encrypt(PlainText(spt)).toString();
    }

    std::string SEALCrypto::Encrypt(std::vector<std::string> const &spt_vec) {
        PlainText pt;
        if (this->params->schemeType == HECrypto::HEScheme::BFV) {
            vector<long> vec;
            vec.resize(spt_vec.size());
            transform(spt_vec.begin(), spt_vec.end(), vec.begin(), [](string const &s) { return stol(s); });
            pt = this->Encode(vec);
        } else if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            vector<double> vec;
            vec.resize(spt_vec.size());
            transform(spt_vec.begin(), spt_vec.end(), vec.begin(), [](string const &s) { return stod(s); });
            pt = this->Encode(vec);
        }
        return this->Encrypt(pt).toString();
    }

    void SEALCrypto::LoadSecretKey(std::string const &secKeyFilename) {
        const std::shared_ptr<FSManager> secretKeyFS = FSManager::createFSManager(secKeyFilename);
        secretKeyFS->OpenInputStream();
        this->secretKey.load(*this->sealContext, secretKeyFS->GetInputStream());
        secretKeyFS->CloseInputStream();
        this->decryptor = make_shared<seal::Decryptor>(*this->sealContext, this->secretKey);
    }

    void SEALCrypto::Decode(std::vector<double> &vec, PlainText const &pt) {
        seal::Plaintext ptxt = seal::Plaintext();
        this->toSealPlaintext(ptxt, pt);
        this->ckksEncoder->decode(ptxt, vec);
    }

    void SEALCrypto::Decode(std::vector<long> &vec, PlainText const &pt) {
        seal::Plaintext ptxt = seal::Plaintext();
        this->toSealPlaintext(ptxt, pt);
        this->batchEncoder->decode(ptxt, vec);
    }

    PlainText SEALCrypto::Decrypt(CipherText const &ct, bool noBatching) {
        seal::Ciphertext ctxt = seal::Ciphertext();
        seal::Plaintext ptxt = seal::Plaintext();
        this->toSealCiphertext(ctxt, ct);
        this->_decrypt(ptxt, ctxt);
        // TODO: handle noBatching
        return PlainText(ptxt);
    }

    std::string SEALCrypto::Decrypt(std::string const &sct, bool noBatching) {
        return this->Decrypt(CipherText(sct), noBatching).toString();
    }

    CipherText SEALCrypto::toCipherText(std::string const &sct) {
        seal::Ciphertext ctxt;
        base64::decoder decoder;
        stringstream decoded, in(sct);
        decoder.decode(in, decoded);
        ctxt.load(*this->sealContext, decoded);
        return CipherText(ctxt);
    }

    std::vector<CipherText> SEALCrypto::toCipherText(std::vector<std::string> const &scts) {
        vector<CipherText> vec;
        transform(
            scts.begin(), scts.end(), back_inserter(vec), [this](string const &s) { return this->toCipherText(s); });
        return vec;
    }

    PlainText SEALCrypto::toPlainText(std::string const &spt) {
        seal::Plaintext ptxt;
        stringstream ss(spt);
        ptxt.load(*this->sealContext, ss);
        return PlainText(ptxt);
    }

    std::vector<PlainText> SEALCrypto::toPlainText(std::vector<std::string> const &spts) {
        vector<PlainText> vec;
        transform(
            spts.begin(), spts.end(), back_inserter(vec), [this](string const &s) { return this->toPlainText(s); });
        return vec;
    }

    CipherText SEALCrypto::Add(CipherText const &ct_1, CipherText const &ct_2) {
        seal::Ciphertext ctxt_1 = seal::Ciphertext();
        seal::Ciphertext ctxt_2 = seal::Ciphertext();

        this->toSealCiphertext(ctxt_1, ct_1);
        this->toSealCiphertext(ctxt_2, ct_2);

        this->_add(ctxt_1, ctxt_2);
        return CipherText(ctxt_1);
    }

    std::string SEALCrypto::Add(std::string const &sct_1, std::string const &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->Add(ct_1, ct_2);
        return result.toString();
    }

    CipherText SEALCrypto::AddPlain(CipherText const &ct, PlainText const &pt) {
        seal::Ciphertext ctxt = seal::Ciphertext();
        seal::Plaintext ptxt = seal::Plaintext();
        this->toSealCiphertext(ctxt, ct);
        this->toSealPlaintext(ptxt, pt);

        this->_add_plain(ctxt, ptxt);
        return CipherText(ctxt);
    }

    std::string SEALCrypto::AddPlain(std::string const &sct, std::string const &spt) {
        PlainText pt = PlainText(spt);
        CipherText ct = CipherText(sct);
        CipherText result = this->AddPlain(ct, pt);
        return result.toString();
    }

    std::vector<CipherText> SEALCrypto::FullAdder(
        std::vector<CipherText> const &vec_ct_1,
        std::vector<CipherText> const &vec_ct_2,
        const size_t max_count) {
        vector<seal::Ciphertext> vec_ctxt_1;
        vector<seal::Ciphertext> vec_ctxt_2;
        vector<seal::Ciphertext> vec_ctxt_result;
        this->toSealCiphertext(vec_ctxt_1, vec_ct_1);
        this->toSealCiphertext(vec_ctxt_2, vec_ct_2);
        this->_full_adder(vec_ctxt_result, vec_ctxt_1, vec_ctxt_2, max_count);

        vector<CipherText> vec_result;
        toCipherText(vec_result, vec_ctxt_result);
        return vec_result;
    }

    std::vector<string> SEALCrypto::FullAdder(
        std::vector<string> const &vec_ct_1,
        std::vector<string> const &vec_ct_2,
        const size_t max_count) {
        vector<CipherText> ct_1 = this->toCipherText(vec_ct_1);
        vector<CipherText> ct_2 = this->toCipherText(vec_ct_2);
        vector<CipherText> result = this->FullAdder(ct_1, ct_2, max_count);
        vector<string> sresult;
        transform(result.begin(), result.end(), back_inserter(sresult), [](CipherText &ct) { return ct.toString(); });
        return sresult;
    }

    CipherText SEALCrypto::Multiply(CipherText const &ct_1, CipherText const &ct_2) {
        seal::Ciphertext ctxt_1 = seal::Ciphertext();
        seal::Ciphertext ctxt_2 = seal::Ciphertext();

        this->toSealCiphertext(ctxt_1, ct_1);
        this->toSealCiphertext(ctxt_2, ct_2);

        this->_multiply(ctxt_1, ctxt_2);
        return CipherText(ctxt_1);
    }

    std::string SEALCrypto::Multiply(std::string const &sct_1, std::string const &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->Multiply(ct_1, ct_2);
        return result.toString();
    }

    CipherText SEALCrypto::MultiplyPlain(CipherText const &ct, PlainText const &pt) {
        seal::Ciphertext ctxt = seal::Ciphertext();
        seal::Plaintext ptxt = seal::Plaintext();
        this->toSealCiphertext(ctxt, ct);
        this->toSealPlaintext(ptxt, pt);

        this->_multiply_plain(ctxt, ptxt);
        return CipherText(ctxt);
    }

    std::string SEALCrypto::MultiplyPlain(std::string const &sct, std::string const &spt) {
        PlainText pt = PlainText(spt);
        CipherText ct = CipherText(sct);
        CipherText result = this->MultiplyPlain(ct, pt);
        return result.toString();
    }

    CipherText SEALCrypto::Square(CipherText const &ct) {
        seal::Ciphertext ctxt = seal::Ciphertext();
        seal::Ciphertext result_ctxt = seal::Ciphertext();
        this->toSealCiphertext(ctxt, ct);
        this->_square(result_ctxt, ctxt);
        return CipherText(result_ctxt);
    }

    std::string SEALCrypto::Square(std::string const &sct) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Square(ct);
        return result.toString();
    }

    CipherText SEALCrypto::Power(CipherText const &ct, int const &n) {
        if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            throw invalid_argument("Power operation is not supported in CKKS scheme");
        }
        seal::Ciphertext ctxt = seal::Ciphertext();
        seal::Ciphertext result = seal::Ciphertext();
        this->toSealCiphertext(ctxt, ct);
        this->_power(result, ctxt, n);
        return CipherText(result);
    }

    std::string SEALCrypto::Power(std::string const &sct, int const &n) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Power(ct, n);
        return result.toString();
    }

    CipherText SEALCrypto::Subtract(CipherText const &ct_1, CipherText const &ct_2) {
        seal::Ciphertext ctxt_1 = seal::Ciphertext();
        seal::Ciphertext ctxt_2 = seal::Ciphertext();

        this->toSealCiphertext(ctxt_1, ct_1);
        this->toSealCiphertext(ctxt_2, ct_2);

        this->_sub(ctxt_1, ctxt_2);
        return CipherText(ctxt_1);
    }

    std::string SEALCrypto::Subtract(std::string const &sct_1, std::string const &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->Subtract(ct_1, ct_2);
        return result.toString();
    }

    CipherText SEALCrypto::Rotate(CipherText const &ct, int const &n) {
        seal::Ciphertext ctxt = seal::Ciphertext();
        seal::Ciphertext result = seal::Ciphertext();
        this->toSealCiphertext(ctxt, ct);
        this->_rotate(result, ctxt, n);
        return CipherText(result);
    }

    std::string SEALCrypto::Rotate(std::string const &sct, int const &n) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Rotate(ct, n);
        return result.toString();
    }

    CipherText SEALCrypto::Shift(CipherText const &ct, int const &n) {
        seal::Ciphertext ctxt = seal::Ciphertext();
        seal::Ciphertext result = seal::Ciphertext();
        this->toSealCiphertext(ctxt, ct);
        this->_shift(result, ctxt, n);
        return CipherText(result);
    }

    std::string SEALCrypto::Shift(std::string const &sct, int const &n) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Shift(ct, n);
        return result.toString();
    }

    CipherText SEALCrypto::RotateColumns(CipherText const &ct) {
        seal::Ciphertext ctxt = seal::Ciphertext();
        seal::Ciphertext result = seal::Ciphertext();
        this->toSealCiphertext(ctxt, ct);
        this->_rotate_columns(result, ctxt);
        return CipherText(result);
    }

    std::string SEALCrypto::RotateColumns(std::string const &sct) {
        CipherText ct = CipherText(sct);
        CipherText result = this->RotateColumns(ct);
        return result.toString();
    }

    CipherText SEALCrypto::And(CipherText const &ct_1, CipherText const &ct_2) {
        seal::Ciphertext ctxt_1 = seal::Ciphertext();
        seal::Ciphertext ctxt_2 = seal::Ciphertext();
        this->toSealCiphertext(ctxt_1, ct_1);
        this->toSealCiphertext(ctxt_2, ct_2);
        seal::Ciphertext result = seal::Ciphertext();
        this->_and(result, ctxt_1, ctxt_2);
        return CipherText(result);
    }

    std::string SEALCrypto::And(std::string const &sct_1, std::string const &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->And(ct_1, ct_2);
        return result.toString();
    }

    CipherText SEALCrypto::Or(CipherText const &ct_1, CipherText const &ct_2) {
        seal::Ciphertext ctxt_1 = seal::Ciphertext();
        seal::Ciphertext ctxt_2 = seal::Ciphertext();
        this->toSealCiphertext(ctxt_1, ct_1);
        this->toSealCiphertext(ctxt_2, ct_2);
        seal::Ciphertext result = seal::Ciphertext();
        this->_or(result, ctxt_1, ctxt_2);
        return CipherText(result);
    }

    std::string SEALCrypto::Or(std::string const &sct_1, std::string const &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->Or(ct_1, ct_2);
        return result.toString();
    }

    CipherText SEALCrypto::Xor(CipherText const &ct_1, CipherText const &ct_2) {
        seal::Ciphertext ctxt_1 = seal::Ciphertext();
        seal::Ciphertext ctxt_2 = seal::Ciphertext();
        this->toSealCiphertext(ctxt_1, ct_1);
        this->toSealCiphertext(ctxt_2, ct_2);
        seal::Ciphertext result = seal::Ciphertext();
        this->_xor(result, ctxt_1, ctxt_2);
        return CipherText(result);
    }

    std::string SEALCrypto::Xor(std::string const &sct_1, std::string const &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->Xor(ct_1, ct_2);
        return result.toString();
    }

    CipherText SEALCrypto::Xnor(CipherText const &ct_1, CipherText const &ct_2) {
        seal::Ciphertext ctxt_1 = seal::Ciphertext();
        seal::Ciphertext ctxt_2 = seal::Ciphertext();
        this->toSealCiphertext(ctxt_1, ct_1);
        this->toSealCiphertext(ctxt_2, ct_2);
        seal::Ciphertext result = seal::Ciphertext();
        this->_xnor(result, ctxt_1, ctxt_2);
        return CipherText(result);
    }

    std::string SEALCrypto::Xnor(std::string const &sct_1, std::string const &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->Xnor(ct_1, ct_2);
        return result.toString();
    }

    CipherText SEALCrypto::Not(CipherText const &ct) {
        seal::Ciphertext ctxt = seal::Ciphertext();
        this->toSealCiphertext(ctxt, ct);
        this->_not(ctxt, ctxt);
        return CipherText(ctxt);
    }

    std::string SEALCrypto::Not(std::string const &sct) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Not(ct);
        return result.toString();
    }

    CipherText SEALCrypto::Mask(CipherText const &ct, int const &index) {
        seal::Ciphertext ctxt = seal::Ciphertext();
        seal::Ciphertext result = seal::Ciphertext();
        this->toSealCiphertext(ctxt, ct);
        this->_mask(result, ctxt, index);
        return CipherText(result);
    }

    CipherText SEALCrypto::Mask(CipherText const &ct, std::vector<int> const &indices) {
        seal::Ciphertext ctxt = seal::Ciphertext();
        seal::Ciphertext result = seal::Ciphertext();
        this->toSealCiphertext(ctxt, ct);
        this->_mask(result, ctxt, indices);
        return CipherText(result);
    }

    std::string SEALCrypto::Mask(std::string const &sct, int const &index) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Mask(ct, index);
        return result.toString();
    }

    std::string SEALCrypto::Mask(std::string const &sct, std::vector<int> const &indices) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Mask(ct, indices);
        return result.toString();
    }

    CipherText SEALCrypto::IsEqual(CipherText const &ct_1, CipherText const &ct_2) {
        return this->Xnor(ct_1, ct_2);
    }

    std::string SEALCrypto::IsEqual(std::string const &sct_1, std::string const &sct_2) {
        return this->Xnor(sct_1, sct_2);
    }

    CipherText SEALCrypto::RunningSum(CipherText const &ct) {
        seal::Ciphertext ctxt = seal::Ciphertext();
        seal::Ciphertext result = seal::Ciphertext();
        this->toSealCiphertext(ctxt, ct);
        this->_running_sum(result, ctxt);
        return CipherText(result);
    }

    std::string SEALCrypto::RunningSum(std::string const &sct) {
        CipherText ct = CipherText(sct);
        CipherText result = this->RunningSum(ct);
        return result.toString();
    }

    CipherText SEALCrypto::TotalSum(CipherText const &ct) {
        seal::Ciphertext ctxt = seal::Ciphertext();
        seal::Ciphertext result = seal::Ciphertext();
        this->toSealCiphertext(ctxt, ct);
        this->_total_sum(result, ctxt);
        return CipherText(result);
    }

    std::string SEALCrypto::TotalSum(std::string const &sct) {
        CipherText ct = CipherText(sct);
        CipherText result = this->TotalSum(ct);
        return result.toString();
    }

    template <typename T>
    void SEALCrypto::createPlainVector(std::vector<PlainText> &vec, std::vector<T> const &data) {
        int splits = (data.size() - 1) / this->slot_count + 1;
        auto start = data.begin();
        for (int i = 0; i < splits; i++) {
            auto end = start + this->slot_count;
            if (end > data.end()) {
                end = data.end();
            }
            vector<T> sub_data(start, end);
            vec.emplace_back(this->Encode(sub_data));
            start += this->slot_count;
        }
    }

    template <typename T>
    void SEALCrypto::createMask(std::vector<T> &mask, const int &index) {
        fill(mask.begin(), mask.end(), false);
        mask[index] = 1;
    }

    template <typename T>
    void SEALCrypto::createMask(std::vector<T> &mask, const std::vector<int> &indices) {
        fill(mask.begin(), mask.end(), false);
        for (auto &index : indices) {
            mask[index] = 1;
        }
    }

    template <typename T>
    void SEALCrypto::createShiftMask(std::vector<T> &mask, const int step, const size_t n) {
        if (step > -n && step < n) {
            if (step < 0) {
                for (int i = -1; i >= step; i--) {
                    mask[i + mask.size()] = 0;
                }
            } else if (step > 0) {
                for (int i = 0; i < step; i++) {
                    mask[i] = 0;
                }
            }
        } else {
            mask.assign(mask.size(), 0);
        }
    }

    seal::Ciphertext SEALCrypto::getOne() {
        if (this->ctxt_one.size() == 0) {
            seal::Plaintext ptxt = seal::Plaintext();
            if (this->params->schemeType == HECrypto::HEScheme::BFV)
                this->_encode(ptxt, 1l);
            else if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
                this->_encode(ptxt, 1.0);
            }
            this->_encrypt(this->ctxt_one, ptxt);
        }
        return this->ctxt_one;
    }

    seal::Ciphertext SEALCrypto::getZero() {
        if (this->ctxt_zero.size() == 0) {
            seal::Plaintext ptxt = seal::Plaintext();
            if (this->params->schemeType == HECrypto::HEScheme::BFV)
                this->_encode(ptxt, 0l);
            else if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
                this->_encode(ptxt, 0.0);
            }
            this->_encrypt(this->ctxt_zero, ptxt);
        }
        return this->ctxt_zero;
    }

    void SEALCrypto::initSealParams() {
        const seal::scheme_type type = set_fhe_scheme(this->params->schemeType);
        seal::EncryptionParameters encParams(type);
        encParams.set_poly_modulus_degree(this->params->polyModulusDegree);

        if (!all_of(this->params->coeffModulusPrimes.begin(), this->params->coeffModulusPrimes.end(), [](long i) {
                return i == 0;
            })) {
            vector<seal::Modulus> coeff_modulus(this->params->coeffModulusPrimes.size());
            for (auto &prime : this->params->coeffModulusPrimes) {
                coeff_modulus.emplace_back(prime);
            }
            encParams.set_coeff_modulus(coeff_modulus);
        } else {
            if (this->params->schemeType == HECrypto::HEScheme::BFV) {
                encParams.set_coeff_modulus(seal::CoeffModulus::BFVDefault(this->params->polyModulusDegree));
            } else if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
                encParams.set_coeff_modulus(
                    seal::CoeffModulus::Create(this->params->polyModulusDegree, this->params->coeffModulusBits));
                // TODO: check if batching is properly set
                this->batching = true;
            }

            vector<ulong> coeffmod_primes;
            vector<ulong> coeffmod_bits;
            for (auto &mod : encParams.coeff_modulus()) {
                coeffmod_primes.push_back(mod.value());
                coeffmod_bits.push_back(mod.bit_count());
            }

            this->params->coeffModulusBits = vector<int>(coeffmod_bits.begin(), coeffmod_bits.end());
            this->params->coeffModulusPrimes = coeffmod_primes;
        }

        this->sealParams = make_shared<seal::EncryptionParameters>(encParams);
    }

    void SEALCrypto::toSealCiphertext(seal::Ciphertext &ct, CipherText const &c) {
        CipherTextData data = c.getData();
        if (holds_alternative<string>(data)) {
            string s = get<string>(data);
            base64::decoder decoder;
            stringstream decoded, in(s);
            decoder.decode(in, decoded);
            ct.load(*this->sealContext, decoded);
        } else if (holds_alternative<seal::Ciphertext>(data)) {
            ct = get<seal::Ciphertext>(data);
        } else {
            throw invalid_argument("Invalid CipherTextData type");
        }
    }

    void SEALCrypto::toSealCiphertext(std::vector<seal::Ciphertext> &vec_ct, std::vector<CipherText> const &vec_c) {
        transform(vec_c.begin(), vec_c.end(), back_inserter(vec_ct), [this](CipherText const &c) {
            seal::Ciphertext ct;
            this->toSealCiphertext(ct, c);
            return ct;
        });
    }

    void SEALCrypto::toSealPlaintext(seal::Plaintext &pt, PlainText const &p) {
        PlainTextData data = p.getData();

        if (holds_alternative<int>(data)) {
            int value = get<int>(data);
            vector<long> vec = {value};
            this->batchEncoder->encode(vec, pt);
        } else if (holds_alternative<string>(data)) {
            string s = get<string>(data);
            if (this->params->schemeType == HECrypto::HEScheme::BFV) {
                istringstream iss(s);
                vector<long> vec((istream_iterator<long>(iss)), istream_iterator<long>());
                this->batchEncoder->encode(vec, pt);
            }
        } else if (holds_alternative<seal::Plaintext>(data)) {
            pt = get<seal::Plaintext>(data);
        } else {
            throw invalid_argument("Invalid PlainTextData type");
        }
    }

    void SEALCrypto::toSealPlaintext(std::vector<seal::Plaintext> &vec_pt, std::vector<PlainText> const &vec_p) {
        transform(vec_p.begin(), vec_p.end(), back_inserter(vec_pt), [this](PlainText const &p) {
            seal::Plaintext pt;
            this->toSealPlaintext(pt, p);
            return pt;
        });
    }
    CipherText SEALCrypto::buildCipherText(std::string const &str) const {
        stringstream ss(str);
        base64::decoder b64decoder;
        stringstream decoded;
        b64decoder.decode(ss, decoded);

        seal::Ciphertext ct;
        ct.load(*sealContext, decoded);
        return CipherText(ct);
    }

    void SEALCrypto::toCipherText(CipherText &c, seal::Ciphertext const &ct) {
        c.setData(ct);
    }

    void SEALCrypto::toCipherText(std::vector<CipherText> &vec_c, std::vector<seal::Ciphertext> const &vec_ct) const {
        transform(vec_ct.begin(), vec_ct.end(), back_inserter(vec_c), [this](seal::Ciphertext const &ct) {
            CipherText c;
            toCipherText(c, ct);
            return c;
        });
    }

    void SEALCrypto::toPlainText(PlainText &p, seal::Plaintext const &pt) {}

    void SEALCrypto::toPlainText(std::vector<PlainText> &vec_p, std::vector<seal::Plaintext> const &vec_pt) {}

    void SEALCrypto::_encrypt(seal::Ciphertext &ct, seal::Plaintext const &pt) {
        this->encryptor->encrypt(pt, ct);
    }

    void SEALCrypto::_decrypt(seal::Plaintext &pt, seal::Ciphertext const &ct) {
        this->decryptor->decrypt(ct, pt);
    }

    void SEALCrypto::_encode(seal::Plaintext &pt, double d) {
        this->ckksEncoder->encode(d, this->params->scale, pt);
    }

    void SEALCrypto::_encode(seal::Plaintext &pt, long l) {
        vector<long> vec(this->slot_count, l);
        this->batchEncoder->encode(vec, pt);
    }

    void SEALCrypto::_add(seal::Ciphertext &ct_1, seal::Ciphertext const &ct_2) {
        this->evaluator->add_inplace(ct_1, ct_2);
    }

    void SEALCrypto::_add(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {
        this->evaluator->add(ct_1, ct_2, result);
    }

    void SEALCrypto::_add_plain(seal::Ciphertext &ct, seal::Plaintext const &pt) {
        this->evaluator->add_plain_inplace(ct, pt);
    }

    void SEALCrypto::_full_adder(
        std::vector<seal::Ciphertext> &result,
        std::vector<seal::Ciphertext> const &vec_ct_1,
        std::vector<seal::Ciphertext> const &vec_ct_2,
        const size_t max_count) {
        seal::Ciphertext x, y, temp, carry, carry_old;
        carry_old = this->getZero();

        ulong size = max(vec_ct_1.size(), vec_ct_2.size()) + 1;
        result.resize(size);

        for (int i = 0; i < size; i++) {
            if (i < vec_ct_1.size()) {
                x = vec_ct_1[i];
            } else {
                x = this->getZero();
            }

            if (i < vec_ct_2.size()) {
                y = vec_ct_2[i];
            } else {
                y = this->getZero();
            }

            _xor(carry, x, y);
            _and(carry, carry, carry_old);
            _and(temp, x, y);
            _xor(carry, carry, temp);
            _xor(result[i], x, y);
            _xor(result[i], result[i], carry_old);
            carry_old = carry;
        }
    }

    void SEALCrypto::_sub(seal::Ciphertext &ct_1, seal::Ciphertext const &ct_2) {
        this->evaluator->sub_inplace(ct_1, ct_2);
    }

    void SEALCrypto::_sub(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {
        this->evaluator->sub(ct_1, ct_2, result);
    }

    void SEALCrypto::_multiply(seal::Ciphertext &ct_1, seal::Ciphertext const &ct_2) {
        this->evaluator->multiply_inplace(ct_1, ct_2);
        if (this->params->plainModulus == 2) {
            this->evaluator->relinearize_inplace(ct_1, this->relinKeys);
        }
        if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            this->evaluator->rescale_to_next_inplace(ct_1);
        }
    }

    void SEALCrypto::_multiply(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {
        this->evaluator->multiply(ct_1, ct_2, result);
        if (this->params->plainModulus == 2) {
            this->evaluator->relinearize_inplace(result, this->relinKeys);
        }
        if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            this->evaluator->rescale_to_next_inplace(result);
        }
    }

    void SEALCrypto::_multiply_plain(seal::Ciphertext &ct, seal::Plaintext const &pt) {
        this->evaluator->multiply_plain_inplace(ct, pt);
        if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            this->evaluator->rescale_to_next_inplace(ct);
        }
    }

    void SEALCrypto::_square(seal::Ciphertext &result, seal::Ciphertext const &ct) {
        this->evaluator->square(ct, result);
        if (this->params->plainModulus == 2) {
            this->evaluator->relinearize_inplace(result, this->relinKeys);
        }
        if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            this->evaluator->rescale_to_next_inplace(result);
        }
    }

    void SEALCrypto::_power(seal::Ciphertext &result, seal::Ciphertext const &ct, int const &n) {
        this->evaluator->exponentiate(ct, n, this->relinKeys, result);
    }

    void SEALCrypto::_rotate(seal::Ciphertext &result, seal::Ciphertext const &ct, int const &step) {
        result = ct;
        if (ct.size() > 2) {
            this->evaluator->relinearize_inplace(result, this->relinKeys);
        } else {
            if (this->params->schemeType == HECrypto::HEScheme::BFV) {
                this->evaluator->rotate_rows_inplace(result, -step, this->galoisKeys);
            } else if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
                this->evaluator->rotate_vector_inplace(result, -step, this->galoisKeys);
            } else {
                cerr << "Invalid scheme type" << endl;
                throw std::runtime_error("Invalid scheme type");
            }
        }
    }

    void SEALCrypto::_rotate_columns(seal::Ciphertext &result, seal::Ciphertext const &ct) {
        if (this->params->schemeType == HECrypto::HEScheme::BFV) {
            result = ct;
            if (ct.size() > 2) {
                this->evaluator->relinearize_inplace(result, this->relinKeys);
            } else {
                this->evaluator->rotate_columns_inplace(result, this->galoisKeys);
            }
        } else {
            cerr << "Invalid scheme type" << endl;
            throw std::runtime_error("Invalid scheme type");
        }
    }

    void SEALCrypto::_shift(seal::Ciphertext &result, seal::Ciphertext const &ct, int const &step) {
        seal::Plaintext mask;
        if (this->params->schemeType == HECrypto::HEScheme::BFV) {
            vector<ulong> tmp_mask(this->slot_count);
            createShiftMask<ulong>(tmp_mask, step, this->slot_count);
            this->batchEncoder->encode(tmp_mask, mask);
            this->_rotate(result, ct, step);
            this->evaluator->multiply_plain_inplace(result, mask);
        } else if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            vector<double> tmp_mask(this->slot_count);
            createShiftMask<double>(tmp_mask, step, this->slot_count);
            this->ckksEncoder->encode(tmp_mask, this->params->scale, mask);
            this->_rotate(result, ct, step);
            this->evaluator->mod_switch_to_inplace(mask, result.parms_id());
            this->evaluator->multiply_plain_inplace(result, mask);
            this->evaluator->rescale_to_next_inplace(result);
            result.scale() = this->params->scale;
        } else {
            cerr << "Invalid scheme type" << endl;
            throw std::runtime_error("Invalid scheme type");
        }
    }

    void SEALCrypto::_and(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {
        this->evaluator->multiply(ct_1, ct_2, result);
    }

    void SEALCrypto::_or(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {
        seal::Ciphertext mult12 = seal::Ciphertext();
        this->_multiply(mult12, ct_1, ct_2);
        seal::Ciphertext add12 = seal::Ciphertext();
        this->_add(add12, ct_1, ct_2);
        parms_unify(add12, mult12);
        this->_sub(result, add12, mult12);
    }

    void SEALCrypto::_xor(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {
        if (this->params->plainModulus == 2) {
            this->_add(result, ct_1, ct_2);
        } else {
            seal::Ciphertext mult12 = seal::Ciphertext();
            this->_multiply(mult12, ct_1, ct_2);
            seal::Plaintext ptxt = seal::Plaintext();
            this->_encode(ptxt, 2.0);
            this->parms_unify(ptxt, mult12);
            this->_multiply_plain(mult12, ptxt);
            seal::Ciphertext add12 = seal::Ciphertext();
            this->_add(add12, ct_1, ct_2);
            this->parms_unify(add12, mult12);
            this->_sub(result, add12, mult12);
        }
    }

    void SEALCrypto::_xnor(seal::Ciphertext &result, seal::Ciphertext const &ct_1, seal::Ciphertext const &ct_2) {
        if (this->params->plainModulus == 2) {
            this->_add(result, ct_1, ct_2);
            this->_add(result, this->getOne());
        } else {
            this->_not(result, ct_2);
            this->_xor(result, ct_1, result);
        }
    }

    void SEALCrypto::_not(seal::Ciphertext &result, seal::Ciphertext const &ct) {
        this->_sub(result, this->getOne(), ct);
    }

    void SEALCrypto::_mask(seal::Ciphertext &result, seal::Ciphertext const &ct, int const &index) {
        seal::Plaintext ptxt_mask;
        result = ct;
        if (this->params->schemeType == HECrypto::HEScheme::BFV) {
            vector<ulong> mask;
            mask.resize(this->slot_count, 0);
            createMask<ulong>(mask, index);
            this->batchEncoder->encode(mask, ptxt_mask);
            this->evaluator->multiply_plain_inplace(result, ptxt_mask);
        } else if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            vector<double> mask;
            mask.resize(this->slot_count, 0);
            createMask<double>(mask, index);
            this->ckksEncoder->encode(mask, this->params->scale, ptxt_mask);
            this->evaluator->mod_switch_to_inplace(ptxt_mask, result.parms_id());
            this->evaluator->multiply_plain_inplace(result, ptxt_mask);
            this->evaluator->rescale_to_next_inplace(result);
            result.scale() = this->params->scale;
        } else {
            cerr << "Invalid scheme type" << endl;
            throw std::runtime_error("Invalid scheme type");
        }
    }

    void SEALCrypto::_mask(seal::Ciphertext &result, seal::Ciphertext const &ct, std::vector<int> const &indices) {
        seal::Plaintext ptxt_mask;
        result = ct;
        if (this->params->schemeType == HECrypto::HEScheme::BFV) {
            vector<ulong> mask;
            mask.resize(this->slot_count, 0);
            createMask<ulong>(mask, indices);
            this->batchEncoder->encode(mask, ptxt_mask);
            this->evaluator->multiply_plain_inplace(result, ptxt_mask);
        } else if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            vector<double> mask;
            mask.resize(this->slot_count, 0);
            createMask<double>(mask, indices);
            this->ckksEncoder->encode(mask, this->params->scale, ptxt_mask);
            this->evaluator->mod_switch_to_inplace(ptxt_mask, result.parms_id());
            this->evaluator->multiply_plain_inplace(result, ptxt_mask);
            this->evaluator->rescale_to_next_inplace(result);
            result.scale() = this->params->scale;
        } else {
            cerr << "Invalid scheme type" << endl;
            throw std::runtime_error("Invalid scheme type");
        }
    }

    void SEALCrypto::_total_sum(seal::Ciphertext &result, seal::Ciphertext const &ct) {
        seal::Ciphertext tmp;
        result = ct;
        size_t limit =
            (this->params->schemeType == HECrypto::HEScheme::BFV) ? (this->slot_count >> 1) - 1 : this->slot_count >> 1;
        for (int i = 1; i <= limit; i++) {
            this->_rotate(tmp, ct, i);
            this->_add(result, tmp);
        }
        if (this->params->schemeType == HECrypto::HEScheme::BFV) {
            this->_rotate_columns(tmp, result);
            this->_add(result, tmp);
        }
    }

    void SEALCrypto::_running_sum(seal::Ciphertext &result, seal::Ciphertext const &ct) {
        seal::Ciphertext tmp;
        result = ct;
        for (int i = 0; i <= 2; i++) {
            this->_shift(tmp, ct, 1 << i);
            if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
                this->evaluator->mod_switch_to_inplace(tmp, result.parms_id());
            }
            this->_add(result, tmp);
        }
    }

    void SEALCrypto::update_encryption_params(CryptoParams &crypto_params) {
        this->params = make_shared<SEALEncryptionParams>();
        if (crypto_params.find("PlaintextModulus") != crypto_params.end()) {
            this->params->plainModulus = crypto_params["PlaintextModulus"].GetInt64();
        }
        if (crypto_params.find("PolyModulusDegree") != crypto_params.end()) {
            this->params->polyModulusDegree = crypto_params["PolyModulusDegree"].GetInt64();
        }
        if (crypto_params.find("ScaleFactor") != crypto_params.end()) {
            this->params->scaleFactor = crypto_params["ScaleFactor"].GetInt64();
            this->params->scale = pow(2.0, this->params->scaleFactor);
        }
        if (crypto_params.find("SchemeType") != crypto_params.end()) {
            this->params->schemeType = parse_HE_scheme(crypto_params["SchemeType"].GetString());
        }
        if (crypto_params.find("CoeffModulusBits") != crypto_params.end()) {
            vector<long> lvec = to_long_vec(crypto_params["CoeffModulusBits"]);
            this->params->coeffModulusBits = vector<int>(lvec.begin(), lvec.end());
        }
        if (crypto_params.find("CoeffModulusPrimes") != crypto_params.end()) {
            vector<long> lvec = to_long_vec(crypto_params["CoeffModulusPrimes"]);
            this->params->coeffModulusPrimes = vector<ulong>(lvec.begin(), lvec.end());
        }
    }

    seal::scheme_type SEALCrypto::set_fhe_scheme(HECrypto::HEScheme scheme) {
        if (scheme == HECrypto::HEScheme::BFV) {
            return seal::scheme_type::bfv;
        } else if (scheme == HECrypto::HEScheme::CKKS) {
            return seal::scheme_type::ckks;
        } else {
            cerr << "Invalid scheme type" << endl;
            throw std::runtime_error("Invalid scheme type");
        }
    }

    void SEALCrypto::set_encoder(HECrypto::HEScheme scheme) {
        // TODO: given that the IntegerEncoder is deprecated, we should find a way to replace it.
        if (scheme == HECrypto::HEScheme::BFV) {
            this->batchEncoder = make_shared<seal::BatchEncoder>(*this->sealContext);
            this->slot_count = this->batchEncoder->slot_count();
        } else if (scheme == HECrypto::HEScheme::CKKS) {
            this->ckksEncoder = make_shared<seal::CKKSEncoder>(*this->sealContext);
            this->slot_count = this->ckksEncoder->slot_count();
        }
    }

    void SEALCrypto::parms_unify(seal::Ciphertext &src, seal::Ciphertext &dst) {
        if (this->params->schemeType != HECrypto::HEScheme::CKKS) {
            cerr << "Invalid scheme type" << endl;
            throw invalid_argument("Invalid scheme type");
        }

        if (src.parms_id() != dst.parms_id()) {
            dst.scale() = src.scale();
            this->evaluator->mod_switch_to_inplace(src, dst.parms_id());
        }
    }

    void SEALCrypto::parms_unify(seal::Plaintext &src, seal::Ciphertext &dst) const {
        if (this->params->schemeType != HECrypto::HEScheme::CKKS) {
            cerr << "Invalid scheme type" << endl;
            throw invalid_argument("Invalid scheme type");
        }

        if (src.parms_id() != dst.parms_id()) {
            this->evaluator->mod_switch_to_inplace(src, dst.parms_id());
        }
    }

}  // namespace SpatialFHE
