
#pragma once
#include "TFHEBool.h"
#include "TFHERegisteredType.h"
#include "tfhe.h"

namespace SpatialFHE {

    class TFHEDecimal : public TFHERegisteredType<double> {
    private:
        FheInt64 *data;
        int scale;
#ifdef DEBUG
        double ori;
#endif

    public:
        explicit TFHEDecimal(double value, int scale=0, bool trivial=false);
        TFHEDecimal();
        ~TFHEDecimal() override;
        double decrypt() const override;

        TFHEDecimal(const TFHEDecimal &other);
        TFHEDecimal(TFHEDecimal &&other) noexcept;
        TFHEDecimal &operator=(const TFHEDecimal &other);
        TFHEDecimal &operator=(TFHEDecimal &&other) noexcept;

        [[nodiscard]] bool isNull() const;

        // redefine the operator
        TFHEDecimal operator+(const TFHEDecimal &other) const;
        TFHEDecimal operator-(const TFHEDecimal &other) const;
        TFHEDecimal operator*(const TFHEDecimal &other) const;
        TFHEDecimal operator/(const TFHEDecimal &other) const;
        TFHEBool operator==(const TFHEDecimal &other) const;
        TFHEBool operator!=(const TFHEDecimal &other) const;
        TFHEBool operator>(const TFHEDecimal &other) const;
        TFHEBool operator<(const TFHEDecimal &other) const;
        TFHEBool operator>=(const TFHEDecimal &other) const;
        TFHEBool operator<=(const TFHEDecimal &other) const;
        TFHEDecimal operator-() const;

        TFHEDecimal operator+=(const TFHEDecimal &other);
        TFHEDecimal operator-=(const TFHEDecimal &other);
        TFHEDecimal operator*=(const TFHEDecimal &other);
        TFHEDecimal operator/=(const TFHEDecimal &other);

        // scalar operation
        TFHEDecimal operator+(double other) const;
        TFHEDecimal operator-(double other) const;
        TFHEDecimal operator*(double other) const;
        TFHEDecimal operator/(double other) const;
        TFHEDecimal operator%(double other) const;
        TFHEBool operator==(double other) const;
        TFHEBool operator!=(double other) const;
        TFHEBool operator>(double other) const;
        TFHEBool operator<(double other) const;
        TFHEBool operator>=(double other) const;
        TFHEBool operator<=(double other) const;

        TFHEDecimal operator+=(double other);
        TFHEDecimal operator-=(double other);
        TFHEDecimal operator*=(double other);
        TFHEDecimal operator/=(double other);

        [[nodiscard]] bool eqTrivial(double other) const;
        [[nodiscard]] bool gtTrivial(double other) const;
        [[nodiscard]] bool geTrivial(double other) const;
        [[nodiscard]] bool ltTrivial(double other) const;
        [[nodiscard]] bool leTrivial(double other) const;
        [[nodiscard]] bool neTrivial(double other) const;

        // static method
        static TFHEDecimal max(const TFHEDecimal &a, const TFHEDecimal &b);
        static TFHEDecimal min(const TFHEDecimal &a, const TFHEDecimal &b);

        static void registerContext(TFHEContext *ctx);

        static std::vector<uint8_t> serialize(const TFHEDecimal &item);
        static TFHEDecimal deserialize(std::vector<uint8_t> &data);

        static const TFHEContext* javaGetContext();

        friend class TFHEBool;
        friend class TFHEInt32;
    };

}  // namespace SpatialFHE