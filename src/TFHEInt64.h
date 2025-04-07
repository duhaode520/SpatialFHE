
#pragma once
#include "TFHEBool.h"
#include "TFHERegisteredType.h"
#include "tfhe.h"

namespace SpatialFHE {

    class TFHEInt64 : public TFHERegisteredType<int64_t> {
    private:
        FheInt64 *data;
#ifdef DEBUG
        int64_t ori;
#endif

    public:
        explicit TFHEInt64(int64_t value, bool trivial=false);
        explicit TFHEInt64(FheInt64 *data);
        TFHEInt64();
        ~TFHEInt64() override;
        int64_t decrypt() const override;

        TFHEInt64(const TFHEInt64 &other);
        TFHEInt64(TFHEInt64 &&other) noexcept;
        TFHEInt64 &operator=(const TFHEInt64 &other);
        TFHEInt64 &operator=(TFHEInt64 &&other) noexcept;

        [[nodiscard]] bool isNull() const;

        // redefine the operator
        TFHEInt64 operator+(const TFHEInt64 &other) const;
        TFHEInt64 operator-(const TFHEInt64 &other) const;
        TFHEInt64 operator*(const TFHEInt64 &other) const;
        TFHEInt64 operator/(const TFHEInt64 &other) const;
        TFHEInt64 operator%(const TFHEInt64 &other) const;
        TFHEBool operator==(const TFHEInt64 &other) const;
        TFHEBool operator!=(const TFHEInt64 &other) const;
        TFHEBool operator>(const TFHEInt64 &other) const;
        TFHEBool operator<(const TFHEInt64 &other) const;
        TFHEBool operator>=(const TFHEInt64 &other) const;
        TFHEBool operator<=(const TFHEInt64 &other) const;
        TFHEInt64 operator&(const TFHEInt64 &other) const;
        TFHEInt64 operator|(const TFHEInt64 &other) const;
        TFHEInt64 operator^(const TFHEInt64 &other) const;
        TFHEInt64 operator~() const;
        TFHEInt64 operator-() const;

        TFHEInt64 operator+=(const TFHEInt64 &other);
        TFHEInt64 operator-=(const TFHEInt64 &other);
        TFHEInt64 operator*=(const TFHEInt64 &other);
        TFHEInt64 operator/=(const TFHEInt64 &other);

        // scalar operation
        TFHEInt64 operator+(int64_t other) const;
        TFHEInt64 operator-(int64_t other) const;
        TFHEInt64 operator*(int64_t other) const;
        TFHEInt64 operator/(int64_t other) const;
        TFHEInt64 operator%(int64_t other) const;
        TFHEBool operator==(int64_t other) const;
        TFHEBool operator!=(int64_t other) const;
        TFHEBool operator>(int64_t other) const;
        TFHEBool operator<(int64_t other) const;
        TFHEBool operator>=(int64_t other) const;
        TFHEBool operator<=(int64_t other) const;
        TFHEInt64 operator&(int64_t other) const;
        TFHEInt64 operator|(int64_t other) const;
        TFHEInt64 operator^(int64_t other) const;
        TFHEInt64 operator<<(int64_t other) const;
        TFHEInt64 operator>>(int64_t other) const;

        TFHEInt64 operator+=(int64_t other);
        TFHEInt64 operator-=(int64_t other);
        TFHEInt64 operator*=(int64_t other);
        TFHEInt64 operator/=(int64_t other);

        [[nodiscard]] bool eqTrivial(int64_t other) const;
        [[nodiscard]] bool gtTrivial(int64_t other) const;
        [[nodiscard]] bool geTrivial(int64_t other) const;
        [[nodiscard]] bool ltTrivial(int64_t other) const;
        [[nodiscard]] bool leTrivial(int64_t other) const;
        [[nodiscard]] bool neTrivial(int64_t other) const;

        // static method
        static TFHEInt64 max(const TFHEInt64 &a, const TFHEInt64 &b);
        static TFHEInt64 min(const TFHEInt64 &a, const TFHEInt64 &b);

        static void registerContext(TFHEContext *ctx);

        static std::vector<uint8_t> serialize(const TFHEInt64 &item);
        static TFHEInt64 deserialize(std::vector<uint8_t> &data);

        static const TFHEContext* javaGetContext();

        friend class TFHEBool;
    };

}  // namespace SpatialFHE