#include "hecrypto.h"

namespace SpatialFHE {
    const char* HESchemeStrings[] = {"BGV", "BFV", "CKKS"};
    HECrypto::HECrypto() {
        
    }

    HECrypto::~HECrypto() {}

    const char *HECrypto::HESchemeToString(HEScheme scheme) {
        return HESchemeStrings[static_cast<int>(scheme)];
    }

}  // namespace SpatialFHE
