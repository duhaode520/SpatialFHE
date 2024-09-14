#include "hecrypto.h"

namespace SpatialFHE {
    // std::string HESchemeStrings[] = {"BGV", "BFV", "CKKS"};
    HECrypto::HECrypto() = default;

    HECrypto::~HECrypto() = default;

    // std::string HECrypto::HESchemeToString(HEScheme scheme) {
    //     return HESchemeStrings[static_cast<int>(scheme)];
    // }
    void HECrypto::ParseParams(HECrypto::CryptoParams& params, rapidjson::Document& doc, std::string const& param_string) {
        doc.Parse(param_string.c_str());
        for (auto& m : doc.GetObject()) {
                params[m.name.GetString()] = m.value;
        }
    }

}  // namespace SpatialFHE
