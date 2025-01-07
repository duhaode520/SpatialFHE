#pragma once
#include "tfhe.h"

namespace SpatialFHE {
    class TFHEInt32;

    class TFHEBool {
    private:
        FheBool *data;

    public:
        TFHEBool(PublicKey *public_key, bool value);
        explicit TFHEBool(FheBool *data);
        TFHEBool();
        ~TFHEBool();

        TFHEBool(const TFHEBool &other);
        TFHEBool(TFHEBool &&other) noexcept;

        TFHEBool &operator=(const TFHEBool &other);

        TFHEBool &operator=(TFHEBool &&other) noexcept ;

        bool decrypt(const ClientKey *client_key) const;

        // redefine the operator
        TFHEBool operator&(const TFHEBool &other);
        TFHEBool operator|(const TFHEBool &other);
        TFHEBool operator^(const TFHEBool &other);
        TFHEBool operator!();
        TFHEBool operator==(const TFHEBool &other);
        TFHEBool operator!=(const TFHEBool &other);
        TFHEBool operator&&(const TFHEBool &other);
        TFHEBool operator||(const TFHEBool &other);

        explicit operator TFHEInt32() const;

        friend class TFHEInt32;
    };

}  // namespace SpatialFHE
