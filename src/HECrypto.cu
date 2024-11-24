#include "HECrypto.h"

using namespace std;
namespace SpatialFHE {
    // std::string HESchemeStrings[] = {"BGV", "BFV", "CKKS"};
    HECrypto::HECrypto() = default;

    HECrypto::~HECrypto() = default;
    void HECrypto::LoadKeyPair(std::string const& pubKeyFilename, std::string const& secKeyFilename) {
        this->LoadPublicKey(pubKeyFilename);
        this->LoadSecretKey(secKeyFilename);
    }

    // std::string HECrypto::HESchemeToString(HEScheme scheme) {
    //     return HESchemeStrings[static_cast<int>(scheme)];
    // }
    void HECrypto::parse_params(
        HECrypto::CryptoParams& params,
        rapidjson::Document& doc,
        std::string const& param_string) {
        doc.Parse(param_string.c_str());
        for (auto& m : doc.GetObject()) {
            params[m.name.GetString()] = m.value;
        }
    }
    HECrypto::HEScheme HECrypto::parse_HE_scheme(std::string const& scheme) {
        if (scheme == "BFV") {
            return HEScheme::BFV;
        }
        if (scheme == "CKKS") {
            return HEScheme::CKKS;
        }

        std::cerr << "Invalid scheme type: " << scheme << std::endl;
        throw std::invalid_argument("Invalid scheme type");
    }

    std::vector<long> HECrypto::to_long_vec(rapidjson::Value& data) {
        std::vector<long> vec;
        for (rapidjson::SizeType i = 0; i < data.Size(); i++) {
            if (data[i].IsInt64())
                vec.push_back(data[i].GetInt64());
            else
                throw std::invalid_argument("Invalid data type");
        }
        return vec;
    }


}  // namespace SpatialFHE
