#include "PhantomCrypto.cuh"
#include "FSManager.h"
#include "phantom.h"
#include <b64/decode.h>

using namespace std;
namespace SpatialFHE {
    void PhantomCrypto::update_encryption_params(CryptoParams &crypto_params) {
        this->params = make_shared<PhantomEncryptionParams>();
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

    void PhantomCrypto::initPhantomParams() {
        phantom::scheme_type type = set_phantom_scheme(this->params->schemeType);
        phantom::EncryptionParameters encParams(type);
        encParams.set_poly_modulus_degree(this->params->polyModulusDegree);
        if (this->params->schemeType == HECrypto::HEScheme::BFV) {
            encParams.set_coeff_modulus(phantom::arith::CoeffModulus::BFVDefault(this->params->polyModulusDegree));
        } else if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            encParams.set_coeff_modulus(
                phantom::arith::CoeffModulus::Create(this->params->polyModulusDegree, this->params->coeffModulusBits));
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

        this->phantomParams = make_shared<phantom::EncryptionParameters>(encParams);
    }
    void PhantomCrypto::set_encoder(HEScheme scheme) {
        if (scheme == HEScheme::BFV) {
            this->batchEncoder = make_shared<PhantomBatchEncoder>(*this->context);
            this->slot_count = this->batchEncoder->slot_count();
        } else if (scheme == HEScheme::CKKS) {
            this->ckksEncoder = make_shared<PhantomCKKSEncoder>(*this->context);
            this->slot_count = this->ckksEncoder->slot_count();
        }
    }

    phantom::scheme_type PhantomCrypto::set_phantom_scheme(HEScheme scheme) {
        if (scheme == HECrypto::HEScheme::BFV) {
            return phantom::scheme_type::bfv;
        } else if (scheme == HECrypto::HEScheme::CKKS) {
            return phantom::scheme_type::ckks;
        } else {
            cerr << "Invalid scheme type" << endl;
            throw std::runtime_error("Invalid scheme type");
        }
    }
    void PhantomCrypto::parms_unify(PhantomCiphertext &src, PhantomCiphertext &dst) {
        while (src.chain_index() > dst.chain_index()) {
            phantom::rescale_to_next_inplace(*context, dst);
        }
        while (src.chain_index() < dst.chain_index()) {
            phantom::rescale_to_next_inplace(*context, src);
        }
    }

    void PhantomCrypto::parms_unify(PhantomPlaintext &src, PhantomCiphertext &dst) const {
        while (src.chain_index() < dst.chain_index()) {
            phantom::mod_switch_to_next_inplace(*context, src);
        }
    }

    void PhantomCrypto::_encrypt(PhantomCiphertext &ct, PhantomPlaintext const &pt) {
        publicKey.encrypt_asymmetric(*context, pt, ct);
    }

    void PhantomCrypto::_decrypt(PhantomPlaintext &pt, PhantomCiphertext const &ct) {
        secretKey.decrypt(*context, ct, pt);
    }

    void PhantomCrypto::_encode(PhantomPlaintext &pt, double d) {
        vector<double> vec(this->slot_count, d);
        this->ckksEncoder->encode(*context, vec, this->params->scale, pt);
    }

    void PhantomCrypto::_encode(PhantomPlaintext &pt, long l) {
        vector<ulong> vec(this->slot_count, static_cast<ulong>(l));
        this->batchEncoder->encode(*context, vec, pt);
    }

    void PhantomCrypto::_add(PhantomCiphertext &ct_1, PhantomCiphertext const &ct_2) {
        phantom::add_inplace(*context, ct_1, ct_2);
    }

    void PhantomCrypto::_add(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2) {
        result = phantom::add(*context, ct_1, ct_2);
    }

    void PhantomCrypto::_add_plain(PhantomCiphertext &ct, PhantomPlaintext const &pt) {
        phantom::add_plain_inplace(*context, ct, pt);
    }

    void PhantomCrypto::_full_adder(
        std::vector<PhantomCiphertext> &result,
        std::vector<PhantomCiphertext> const &vec_ct_1,
        std::vector<PhantomCiphertext> const &vec_ct_2,
        size_t max_count) {
        throw std::logic_error("Not implemented");
    }

    void PhantomCrypto::_sub(PhantomCiphertext &ct_1, PhantomCiphertext const &ct_2) {
        phantom::sub_inplace(*context, ct_1, ct_2);
    }

    void PhantomCrypto::_sub(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2) {
        result = phantom::sub(*context, ct_1, ct_2);
    }

    void PhantomCrypto::_multiply(PhantomCiphertext &ct_1, PhantomCiphertext const &ct_2) {
        phantom::multiply_and_relin_inplace(*context, ct_1, ct_2, relinKey);
        if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            phantom::rescale_to_next_inplace(*context, ct_1);
        }
    }

    void PhantomCrypto::_multiply(
        PhantomCiphertext &result,
        PhantomCiphertext const &ct_1,
        PhantomCiphertext const &ct_2) {
        result = phantom::multiply_and_relin(*context, ct_1, ct_2, relinKey);
        if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            phantom::rescale_to_next_inplace(*context, result);
        }
    }

    void PhantomCrypto::_multiply_plain(PhantomCiphertext &ct, PhantomPlaintext const &pt) {
        phantom::multiply_plain_inplace(*context, ct, pt);
        if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            phantom::rescale_to_next_inplace(*context, ct);
        }
    }

    void PhantomCrypto::_square(PhantomCiphertext &result, PhantomCiphertext const &ct) {
        result = phantom::multiply_and_relin(*context, ct, ct, relinKey);
        if (this->params->schemeType == HEScheme::CKKS) {
            phantom::rescale_to_next_inplace(*context, result);
        }
    }

    void PhantomCrypto::_power(PhantomCiphertext &result, PhantomCiphertext const &ct, int const &n) {
        throw std::logic_error("Not implemented");
    }

    void PhantomCrypto::_rotate(PhantomCiphertext &result, PhantomCiphertext const &ct, int const &step) {
        result = phantom::rotate(*context, ct, step, galoisKeys);
    }

    void PhantomCrypto::_rotate_columns(PhantomCiphertext &result, PhantomCiphertext const &ct) {
        throw std::logic_error("Not implemented");
    }

    void PhantomCrypto::_shift(PhantomCiphertext &result, PhantomCiphertext const &ct, int const &n) {
        throw std::logic_error("Not implemented");
    }

    void PhantomCrypto::_and(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2) {
        _multiply(result, ct_1, ct_2);
    }

    void PhantomCrypto::_or(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2) {
        PhantomCiphertext mult12;
        _multiply(mult12, ct_1, ct_2);
        PhantomCiphertext add12;
        _add(add12, ct_1, ct_2);
        parms_unify(add12, mult12);
        _sub(result, mult12, add12);
    }

    void PhantomCrypto::_xor(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2) {
        PhantomCiphertext mult12;
        _multiply(mult12, ct_1, ct_2);
        PhantomPlaintext ptxt;
        _encode(ptxt, 2.0);
        parms_unify(ptxt, mult12);
        _multiply_plain(mult12, ptxt);
        PhantomCiphertext add12;
        _add(add12, ct_1, ct_2);
        parms_unify(add12, mult12);
        _sub(result, mult12, add12);
    }

    void PhantomCrypto::_xnor(PhantomCiphertext &result, PhantomCiphertext const &ct_1, PhantomCiphertext const &ct_2) {
        _not(result, ct_2);
        _xor(result, ct_1, result);
    }

    void PhantomCrypto::_not(PhantomCiphertext &result, PhantomCiphertext const &ct) {
        PhantomPlaintext ptxt;
        _encode(ptxt, -1.0);
        PhantomCiphertext one;
        _encrypt(one, ptxt);
        _sub(result, one, ct);
    }

    void PhantomCrypto::_mask(PhantomCiphertext &result, PhantomCiphertext const &ct, int const &index) {
        throw std::logic_error("Not implemented");
    }

    void PhantomCrypto::_mask(PhantomCiphertext &result, PhantomCiphertext const &ct, std::vector<int> const &indices) {
        throw std::logic_error("Not implemented");
    }

    void PhantomCrypto::_total_sum(PhantomCiphertext &result, PhantomCiphertext const &ct) {
        throw std::logic_error("Not implemented");
    }

    void PhantomCrypto::_running_sum(PhantomCiphertext &result, PhantomCiphertext const &ct) {
        throw std::logic_error("Not implemented");
    }

    PhantomCrypto::PhantomCrypto() {
        this->publicKey = PhantomPublicKey();
        this->secretKey = PhantomSecretKey();
        this->relinKey = PhantomRelinKey();
        this->galoisKeys = PhantomGaloisKey();

        // Pointers
        this->phantomParams = nullptr;
        this->context = nullptr;
        this->ckksEncoder = nullptr;
        this->batchEncoder = nullptr;
    }
    PhantomCrypto::PhantomCrypto(CryptoParams &params) : PhantomCrypto() {
        this->update_encryption_params(params);
        this->initPhantomParams();
        this->context = std::make_shared<PhantomContext>(*this->phantomParams);
        this->set_encoder(this->params->schemeType);
    }

    PhantomCrypto::PhantomCrypto(const std::string &param_string) : PhantomCrypto() {
        CryptoParams params;
        rapidjson::Document doc;
        parse_params(params, doc, param_string);
        this->update_encryption_params(params);
        this->initPhantomParams();
        this->context = std::make_shared<PhantomContext>(*this->phantomParams);
        this->set_encoder(this->params->schemeType);
    }

    PhantomCrypto::~PhantomCrypto() = default;

    void PhantomCrypto::GenerateKeyPair(const std::string &pubKeyFilename, const std::string &secKeyFilename) {
        this->secretKey = PhantomSecretKey(*this->context);
        this->publicKey = this->secretKey.gen_publickey(*this->context);
        this->relinKey = this->secretKey.gen_relinkey(*this->context);
        this->galoisKeys = this->secretKey.create_galois_keys(*this->context);

        // save keys to file
        const std::shared_ptr<FSManager> secretKeyFS = FSManager::createFSManager(secKeyFilename);
        const std::shared_ptr<FSManager> publicKeyFS = FSManager::createFSManager(pubKeyFilename);
        publicKeyFS->OpenOutputStream();
        secretKeyFS->OpenOutputStream();
        this->publicKey.save(publicKeyFS->GetOutputStream());
        this->secretKey.save(secretKeyFS->GetOutputStream());
        publicKeyFS->CloseOutputStream();
        secretKeyFS->CloseOutputStream();

        const std::shared_ptr<FSManager> relinKeyFS = FSManager::createFSManager(pubKeyFilename + ".relin");
        relinKeyFS->OpenOutputStream();
        this->relinKey.save(relinKeyFS->GetOutputStream());
        relinKeyFS->CloseOutputStream();
        const std::shared_ptr<FSManager> galoisKeyFS = FSManager::createFSManager(pubKeyFilename + ".galois");
        galoisKeyFS->OpenOutputStream();
        this->galoisKeys.save(galoisKeyFS->GetOutputStream());
        galoisKeyFS->CloseOutputStream();
    }

    void PhantomCrypto::LoadPublicKey(const std::string &pubKeyFilename) {
        const std::shared_ptr<FSManager> publicKeyFS = FSManager::createFSManager(pubKeyFilename);
        publicKeyFS->OpenInputStream();
        this->publicKey.load(publicKeyFS->GetInputStream());
        publicKeyFS->CloseInputStream();
        const std::shared_ptr<FSManager> relinKeyFS = FSManager::createFSManager(pubKeyFilename + ".relin");
        relinKeyFS->OpenInputStream();
        this->relinKey.load(relinKeyFS->GetInputStream());
        relinKeyFS->CloseInputStream();
        const std::shared_ptr<FSManager> galoisKeyFS = FSManager::createFSManager(pubKeyFilename + ".galois");
        galoisKeyFS->OpenInputStream();
        this->galoisKeys.load(galoisKeyFS->GetInputStream());
        galoisKeyFS->CloseInputStream();
    }

    PlainText PhantomCrypto::Encode(double d) {
        PhantomPlaintext ptxt = PhantomPlaintext();
        this->_encode(ptxt, d);
        return PlainText(ptxt);
    }

    PlainText PhantomCrypto::Encode(long l) {
        PhantomPlaintext ptxt = PhantomPlaintext();
        this->_encode(ptxt, l);
        return PlainText(ptxt);
    }

    PlainText PhantomCrypto::Encode(const std::string &str) {
        if (this->params->schemeType == HECrypto::HEScheme::BFV) {
            return this->Encode(stol(str));
        } else if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            return this->Encode(stod(str));
        } else {
            throw invalid_argument("Invalid scheme type");
        }
    }
    PlainText PhantomCrypto::Encode(const std::vector<double> &vec) {
        PhantomPlaintext ptxt = PhantomPlaintext();
        this->ckksEncoder->encode(*this->context, vec, this->params->scale, ptxt);
        return PlainText(ptxt);
    }
    PlainText PhantomCrypto::Encode(const std::vector<long> &vec) {
        PhantomPlaintext ptxt = PhantomPlaintext();
        // transform long to uint_64
        vector<ulong> uvec;
        for (auto &l : vec) {
            uvec.push_back(static_cast<ulong>(l));
        }
        this->batchEncoder->encode(*this->context, uvec, ptxt);
        return PlainText(ptxt);
    }

    std::vector<PlainText> PhantomCrypto::EncodeMany(const std::vector<double> &vec) {
        vector<PlainText> ptxt_vec;
        if (this->slot_count >= vec.size()) {
            ptxt_vec.emplace_back(this->Encode(vec));
        } else {
            this->createPlainVector<double>(ptxt_vec, vec);
        }
        return ptxt_vec;
    }
    std::vector<PlainText> PhantomCrypto::EncodeMany(const std::vector<long> &vec) {
        vector<PlainText> ptxt_vec;
        if (this->slot_count >= vec.size()) {
            ptxt_vec.emplace_back(this->Encode(vec));
        } else {
            this->createPlainVector<long>(ptxt_vec, vec);
        }
        return ptxt_vec;
    }

    CipherText PhantomCrypto::Encrypt(const PlainText &pt) {
        PhantomPlaintext ptxt = PhantomPlaintext();
        PhantomCiphertext ctxt = PhantomCiphertext();
        this->toPhantomPlaintext(ptxt, pt);
        this->_encrypt(ctxt, ptxt);
        return CipherText(ctxt);
    }

    std::string PhantomCrypto::Encrypt(const std::string &str) {
        return this->Encrypt(PlainText(str)).toString();
    }

    std::string PhantomCrypto::Encrypt(const std::vector<std::string> &spt_vec) {
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

    void PhantomCrypto::LoadSecretKey(const std::string &secKeyFilename) {
        const std::shared_ptr<FSManager> secretKeyFS = FSManager::createFSManager(secKeyFilename);
        secretKeyFS->OpenInputStream();
        this->secretKey.load(secretKeyFS->GetInputStream());
        secretKeyFS->CloseInputStream();
    }

    void PhantomCrypto::Decode(std::vector<double> &vec, const PlainText &pt) {
        PhantomPlaintext ptxt;
        this->toPhantomPlaintext(ptxt, pt);
        this->ckksEncoder->decode(*this->context, ptxt, vec);
    }

    void PhantomCrypto::Decode(std::vector<long> &vec, const PlainText &pt) {
        PhantomPlaintext ptxt;
        this->toPhantomPlaintext(ptxt, pt);
        vector<ulong>  ul_vector = this->batchEncoder->decode(*this->context, ptxt);
        for (auto &ul : ul_vector) {
            vec.push_back(static_cast<long>(ul));
        }
    }

    PlainText PhantomCrypto::Decrypt(const CipherText &ct, bool noBatching) {
        PhantomCiphertext ctxt;
        PhantomPlaintext ptxt;
        this->toPhantomCiphertext(ctxt, ct);
        this->_decrypt(ptxt, ctxt);
        return PlainText(ptxt);
    }

    std::string PhantomCrypto::Decrypt(const std::string &sct, bool noBatching) {
        return this->Decrypt(CipherText(sct), noBatching).toString();
    }

    CipherText PhantomCrypto::toCipherText(const std::string &str) {
        PhantomCiphertext ctxt;
        base64::decoder decoder;
        stringstream decoded, in(str);
        decoder.decode(in, decoded);
        ctxt.load(decoded);
        return CipherText(ctxt);
    }

    std::vector<CipherText> PhantomCrypto::toCipherText(const std::vector<std::string> &strs) {
        vector<CipherText> vec_ct;
        transform(
            strs.begin(), strs.end(), back_inserter(vec_ct), [this](string const &s) { return this->toCipherText(s); });
        return vec_ct;
    }

    PlainText PhantomCrypto::toPlainText(const std::string &str) {
        PhantomPlaintext ptxt;
        stringstream ss(str);
        ptxt.load(ss);
        return PlainText(ptxt);
    }

    std::vector<PlainText> PhantomCrypto::toPlainText(const std::vector<std::string> &strs) {
        vector<PlainText> vec_pt;
        transform(
            strs.begin(), strs.end(), back_inserter(vec_pt), [this](string const &s) { return this->toPlainText(s); });
        return vec_pt;
    }

    CipherText PhantomCrypto::Add(const CipherText &ct_1, const CipherText &ct_2) {
        PhantomCiphertext ctxt_1, ctxt_2;
        this->toPhantomCiphertext(ctxt_1, ct_1);
        this->toPhantomCiphertext(ctxt_2, ct_2);

        this->_add(ctxt_1, ctxt_2);
        return CipherText(ctxt_1);
    }

    std::string PhantomCrypto::Add(const std::string &sct_1, const std::string &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->Add(ct_1, ct_2);
        return result.toString();
    }

    CipherText PhantomCrypto::AddPlain(const CipherText &ct, const PlainText &pt) {
        PhantomCiphertext ctxt;
        PhantomPlaintext ptxt;
        this->toPhantomCiphertext(ctxt, ct);
        this->toPhantomPlaintext(ptxt, pt);
        this->_add_plain(ctxt, ptxt);
        return CipherText(ctxt);
    }

    std::string PhantomCrypto::AddPlain(const std::string &sct, const std::string &spt) {
        PlainText pt = PlainText(spt);
        CipherText ct = CipherText(sct);
        CipherText result = this->AddPlain(ct, pt);
        return result.toString();
    }

    std::vector<CipherText> PhantomCrypto::FullAdder(
        const std::vector<CipherText> &vec_ct_1,
        const std::vector<CipherText> &vec_ct_2,
        size_t max_count) {
        vector<PhantomCiphertext> vec_ctxt_1;
        vector<PhantomCiphertext> vec_ctxt_2;
        vector<PhantomCiphertext> vec_ctxt_result;
        this->toPhantomCiphertext(vec_ctxt_1, vec_ct_1);
        this->toPhantomCiphertext(vec_ctxt_2, vec_ct_2);
        this->_full_adder(vec_ctxt_result, vec_ctxt_1, vec_ctxt_2, max_count);

        vector<CipherText> vec_result;
        toCipherText(vec_result, vec_ctxt_result);
        return vec_result;
    }

    std::vector<std::string> PhantomCrypto::FullAdder(
        const std::vector<std::string> &vec_sct_1,
        const std::vector<std::string> &vec_sct_2,
        size_t max_count) {
        vector<CipherText> ct_1 = this->toCipherText(vec_sct_1);
        vector<CipherText> ct_2 = this->toCipherText(vec_sct_2);
        vector<CipherText> result = this->FullAdder(ct_1, ct_2, max_count);
        vector<string> sresult;
        transform(result.begin(), result.end(), back_inserter(sresult), [](CipherText &ct) { return ct.toString(); });
        return sresult;
    }

    CipherText PhantomCrypto::Multiply(const CipherText &ct_1, const CipherText &ct_2) {
        PhantomCiphertext ctxt_1, ctxt_2;
        this->toPhantomCiphertext(ctxt_1, ct_1);
        this->toPhantomCiphertext(ctxt_2, ct_2);

        this->_multiply(ctxt_1, ctxt_2);
        return CipherText(ctxt_1);
    }

    std::string PhantomCrypto::Multiply(const std::string &sct_1, const std::string &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->Multiply(ct_1, ct_2);
        return result.toString();
    }

    CipherText PhantomCrypto::MultiplyPlain(const CipherText &ct, const PlainText &pt) {
        PhantomCiphertext ctxt;
        PhantomPlaintext ptxt;
        this->toPhantomCiphertext(ctxt, ct);
        this->toPhantomPlaintext(ptxt, pt);
        this->_multiply_plain(ctxt, ptxt);
        return CipherText(ctxt);
    }

    std::string PhantomCrypto::MultiplyPlain(const std::string &sct, const std::string &spt) {
        PlainText pt = PlainText(spt);
        CipherText ct = CipherText(sct);
        CipherText result = this->MultiplyPlain(ct, pt);
        return result.toString();
    }

    CipherText PhantomCrypto::Square(const CipherText &ct) {
        PhantomCiphertext ctxt, result;
        this->toPhantomCiphertext(ctxt, ct);
        this->_square(result, ctxt);
        return CipherText(result);
    }

    std::string PhantomCrypto::Square(const std::string &sct) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Square(ct);
        return result.toString();
    }

    CipherText PhantomCrypto::Power(const CipherText &ct, const int &n) {
        if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            throw invalid_argument("Power operation is not supported in CKKS scheme");
        }
        PhantomCiphertext ctxt = PhantomCiphertext();
        PhantomCiphertext result = PhantomCiphertext();
        this->toPhantomCiphertext(ctxt, ct);
        this->_power(result, ctxt, n);
        return CipherText(result);
    }

    std::string PhantomCrypto::Power(const std::string &sct, const int &n) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Power(ct, n);
        return result.toString();
    }

    CipherText PhantomCrypto::Subtract(const CipherText &ct_1, const CipherText &ct_2) {
        PhantomCiphertext ctxt_1, ctxt_2;
        this->toPhantomCiphertext(ctxt_1, ct_1);
        this->toPhantomCiphertext(ctxt_2, ct_2);
        this->_sub(ctxt_1, ctxt_2);
        return CipherText(ctxt_1);
    }

    std::string PhantomCrypto::Subtract(const std::string &sct_1, const std::string &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->Subtract(ct_1, ct_2);
        return result.toString();
    }

    CipherText PhantomCrypto::Rotate(const CipherText &ct, const int &n) {
        PhantomCiphertext ctxt, result;
        this->toPhantomCiphertext(ctxt, ct);
        this->_rotate(result, ctxt, n);
        return CipherText(result);
    }

    std::string PhantomCrypto::Rotate(const std::string &sct, const int &n) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Rotate(ct, n);
        return result.toString();
    }

    CipherText PhantomCrypto::Shift(const CipherText &ct, const int &n) {
        if (this->params->schemeType == HECrypto::HEScheme::CKKS) {
            throw invalid_argument("Power operation is not supported in CKKS scheme");
        }

        PhantomCiphertext ctxt = PhantomCiphertext();
        PhantomCiphertext result = PhantomCiphertext();
        this->toPhantomCiphertext(ctxt, ct);
        this->_shift(result, ctxt, n);
        return CipherText(result);
    }

    std::string PhantomCrypto::Shift(const std::string &sct, const int &n) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Shift(ct, n);
        return result.toString();
    }

    CipherText PhantomCrypto::RotateColumns(const CipherText &ct) {
        PhantomCiphertext ctxt, result;
        this->toPhantomCiphertext(ctxt, ct);
        this->_rotate_columns(result, ctxt);
        return CipherText(result);
    }

    std::string PhantomCrypto::RotateColumns(const std::string &sct) {
        CipherText ct = CipherText(sct);
        CipherText result = this->RotateColumns(ct);
        return result.toString();
    }

    CipherText PhantomCrypto::And(const CipherText &ct_1, const CipherText &ct_2) {
        PhantomCiphertext ctxt_1, ctxt_2, result;
        this->toPhantomCiphertext(ctxt_1, ct_1);
        this->toPhantomCiphertext(ctxt_2, ct_2);

        this->_and(result, ctxt_1, ctxt_2);
        return CipherText(ctxt_1);
    }

    std::string PhantomCrypto::And(const std::string &sct_1, const std::string &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->And(ct_1, ct_2);
        return result.toString();
    }

    CipherText PhantomCrypto::Or(const CipherText &ct_1, const CipherText &ct_2) {
        PhantomCiphertext ctxt_1, ctxt_2, result;
        this->toPhantomCiphertext(ctxt_1, ct_1);
        this->toPhantomCiphertext(ctxt_2, ct_2);

        this->_or(result, ctxt_1, ctxt_2);
        return CipherText(ctxt_1);
    }

    std::string PhantomCrypto::Or(const std::string &sct_1, const std::string &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->Or(ct_1, ct_2);
        return result.toString();
    }

    CipherText PhantomCrypto::Xor(const CipherText &ct_1, const CipherText &ct_2) {
        PhantomCiphertext ctxt_1, ctxt_2, result;
        this->toPhantomCiphertext(ctxt_1, ct_1);
        this->toPhantomCiphertext(ctxt_2, ct_2);

        this->_xor(result, ctxt_1, ctxt_2);
        return CipherText(ctxt_1);
    }

    std::string PhantomCrypto::Xor(const std::string &sct_1, const std::string &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->Xor(ct_1, ct_2);
        return result.toString();
    }

    CipherText PhantomCrypto::Xnor(const CipherText &ct_1, const CipherText &ct_2) {
        PhantomCiphertext ctxt_1, ctxt_2, result;
        this->toPhantomCiphertext(ctxt_1, ct_1);
        this->toPhantomCiphertext(ctxt_2, ct_2);

        this->_xnor(result, ctxt_1, ctxt_2);
        return CipherText(ctxt_1);
    }

    std::string PhantomCrypto::Xnor(const std::string &sct_1, const std::string &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->Xnor(ct_1, ct_2);
        return result.toString();
    }

    CipherText PhantomCrypto::Not(const CipherText &ct) {
        PhantomCiphertext ctxt, result;
        this->toPhantomCiphertext(ctxt, ct);
        this->_not(result, ctxt);
        return CipherText(result);
    }

    std::string PhantomCrypto::Not(const std::string &sct) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Not(ct);
        return result.toString();
    }

    CipherText PhantomCrypto::Mask(const CipherText &ct, const int &index) {
        PhantomCiphertext ctxt, result;
        this->toPhantomCiphertext(ctxt, ct);
        this->_mask(result, ctxt, index);
        return CipherText(result);
    }

    CipherText PhantomCrypto::Mask(const CipherText &ct, const std::vector<int> &indices) {
        PhantomCiphertext ctxt, result;
        this->toPhantomCiphertext(ctxt, ct);
        this->_mask(result, ctxt, indices);
        return CipherText(result);
    }

    std::string PhantomCrypto::Mask(const std::string &sct, const int &index) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Mask(ct, index);
        return result.toString();
    }

    std::string PhantomCrypto::Mask(const std::string &sct, const std::vector<int> &indices) {
        CipherText ct = CipherText(sct);
        CipherText result = this->Mask(ct, indices);
        return result.toString();
    }

    CipherText PhantomCrypto::IsEqual(const CipherText &ct_1, const CipherText &ct_2) {
        PhantomCiphertext ctxt_1, ctxt_2, result;
        this->toPhantomCiphertext(ctxt_1, ct_1);
        this->toPhantomCiphertext(ctxt_2, ct_2);
        this->_xnor(result, ctxt_1, ctxt_2);
        return CipherText(result);
    }

    std::string PhantomCrypto::IsEqual(const std::string &sct_1, const std::string &sct_2) {
        CipherText ct_1 = CipherText(sct_1);
        CipherText ct_2 = CipherText(sct_2);
        CipherText result = this->IsEqual(ct_1, ct_2);
        return result.toString();
    }

    CipherText PhantomCrypto::RunningSum(const CipherText &ct) {
        PhantomCiphertext ctxt, result;
        this->toPhantomCiphertext(ctxt, ct);
        this->_running_sum(result, ctxt);
        return CipherText(result);
    }

    std::string PhantomCrypto::RunningSum(const std::string &sct) {
        CipherText ct = CipherText(sct);
        CipherText result = this->RunningSum(ct);
        return result.toString();
    }

    CipherText PhantomCrypto::TotalSum(const CipherText &ct) {
        PhantomCiphertext ctxt, result;
        this->toPhantomCiphertext(ctxt, ct);
        this->_total_sum(result, ctxt);
        return CipherText(result);
    }

    std::string PhantomCrypto::TotalSum(const std::string &sct) {
        CipherText ct = CipherText(sct);
        CipherText result = this->TotalSum(ct);
        return result.toString();
    }

    CipherText PhantomCrypto::buildCipherText(const std::string &str) const {
        PhantomCiphertext ctxt;
        base64::decoder decoder;
        stringstream decoded, in(str);
        decoder.decode(in, decoded);
        ctxt.load(decoded);
        return CipherText(ctxt);
    }

    template <typename T>
    void PhantomCrypto::createPlainVector(std::vector<PlainText> &vec, std::vector<T> const &data) {
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
    void PhantomCrypto::toPhantomCiphertext(PhantomCiphertext &ct, CipherText const &c) {
        CipherTextData data = c.getData();
        if (holds_alternative<string>(data)) {
            string s = get<string>(data);
            base64::decoder decoder;
            stringstream decoded, in(s);
            decoder.decode(in, decoded);
            ct.load(decoded);
        } else if (holds_alternative<PhantomCiphertext>(data)) {
            ct = get<PhantomCiphertext>(data);
        } else {
            throw invalid_argument("Invalid CipherTextData type");
        }
    }

    void PhantomCrypto::toPhantomCiphertext(
        std::vector<PhantomCiphertext> &vec_ct,
        std::vector<CipherText> const &vec_c) {
        transform(vec_c.begin(), vec_c.end(), back_inserter(vec_ct), [this](CipherText const &c) {
            PhantomCiphertext ct;
            this->toPhantomCiphertext(ct, c);
            return ct;
        });
    }
    void PhantomCrypto::toPhantomPlaintext(PhantomPlaintext &pt, PlainText const &p) {
        PlainTextData data = p.getData();

        if (holds_alternative<int>(data)) {
            int value = get<int>(data);
            vector<ulong> vec = {static_cast<ulong>(value)};
            this->batchEncoder->encode(*context, vec, pt);
        } else if (holds_alternative<string>(data)) {
            string s = get<string>(data);
            if (this->params->schemeType == HECrypto::HEScheme::BFV) {
                istringstream iss(s);
                vector<ulong> vec((istream_iterator<ulong>(iss)), istream_iterator<ulong>());
                this->batchEncoder->encode(*context, vec, pt);
            }
        } else if (holds_alternative<PhantomPlaintext>(data)) {
            pt = get<PhantomPlaintext>(data);
        } else {
            throw invalid_argument("Invalid PlainTextData type");
        }
    }
    void PhantomCrypto::toPhantomPlaintext(std::vector<PhantomPlaintext> &vec_pt, std::vector<PlainText> const &vec_p) {
        transform(vec_p.begin(), vec_p.end(), back_inserter(vec_pt), [this](PlainText const &p) {
            PhantomPlaintext pt;
            this->toPhantomPlaintext(pt, p);
            return pt;
        });
    }
    void PhantomCrypto::toCipherText(CipherText &c, PhantomCiphertext const &ct) const {
        c.setData(ct);
    }
    void PhantomCrypto::toCipherText(std::vector<CipherText> &vec_c, std::vector<PhantomCiphertext> const &vec_ct)
        const {
        transform(vec_ct.begin(), vec_ct.end(), back_inserter(vec_c), [this](PhantomCiphertext const &ct) {
            CipherText c;
            toCipherText(c, ct);
            return c;
        });
    }

    void PhantomCrypto::toPlainText(PlainText &p, PhantomPlaintext const &pt) {
        throw std::logic_error("Not implemented");
    }
    void PhantomCrypto::toPlainText(std::vector<PlainText> &vec_p, std::vector<PhantomPlaintext> const &vec_pt) {
        throw std::logic_error("Not implemented");
    }
}  // namespace SpatialFHE