#include "hecrypto.h"

namespace SpatialFHE {
    HECrypto::HECrypto() {
        
    }

    HECrypto::~HECrypto() {}

    const char *HECrypto::HESchemeToString(HEScheme scheme) {
        return HESchemeStrings[static_cast<int>(scheme)];
    }

}  // namespace SpatialFHE
