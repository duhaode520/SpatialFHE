#pragma once
#include "tfhe.h"
#include "TFHERegisteredType.h"

namespace SpatialFHE {
    class TFHEInt32;

    class TFHEBool : public TFHERegisteredType<bool> {
    private:
        FheBool *data;

    public:
        explicit TFHEBool(bool value);
        explicit TFHEBool(FheBool *data);
        TFHEBool();
        ~TFHEBool() override;

        TFHEBool(const TFHEBool &other);
        TFHEBool(TFHEBool &&other) noexcept;

        bool isNull() const;

        TFHEBool &operator=(const TFHEBool &other);

        TFHEBool &operator=(TFHEBool &&other) noexcept;

        bool decrypt() override;

        // redefine the operator
        TFHEBool operator&(const TFHEBool &other) const;
        TFHEBool operator|(const TFHEBool &other) const;
        TFHEBool operator^(const TFHEBool &other) const;
        TFHEBool operator!() const;
        TFHEBool operator==(const TFHEBool &other) const;
        TFHEBool operator!=(const TFHEBool &other) const;
        TFHEBool operator&&(const TFHEBool &other) const;
        TFHEBool operator||(const TFHEBool &other) const;

        // scalar operation
        TFHEBool operator&(bool other) const;
        TFHEBool operator|(bool other) const;
        TFHEBool operator^(bool other) const;
        TFHEBool operator==(bool other) const;
        TFHEBool operator!=(bool other) const;
        TFHEBool operator&&(bool other) const;
        TFHEBool operator||(bool other) const;

        explicit operator TFHEInt32() const;

        friend class TFHEInt32;

        // static instance
        static TFHEBool tfhe_true;
        static TFHEBool tfhe_false;
        static void init();

        static void registerContext(TFHEContext* ctx) ;
        static bool isLocalTrue(const TFHEBool &other);
        static bool isLocalFalse(const TFHEBool &other);
    };

}  // namespace SpatialFHE
