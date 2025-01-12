#pragma once
#include "TFHEBool.h"
#include "tfhe.h"
#include "TFHERegisteredType.h"

namespace SpatialFHE {

class TFHEInt32 : public TFHERegisteredType<int> {
private:
    FheInt32* data;
public:
    explicit TFHEInt32(int32_t value);
    explicit TFHEInt32(FheInt32* data);
    TFHEInt32();
    ~TFHEInt32() override;
    int decrypt() override;

    TFHEInt32(const TFHEInt32 &other);
    TFHEInt32(TFHEInt32 &&other) noexcept;
    TFHEInt32 &operator=(const TFHEInt32 &other);
    TFHEInt32 &operator=(TFHEInt32 &&other) noexcept;

    [[nodiscard]] bool isNull() const;

    // redefine the operator
    TFHEInt32 operator+(const TFHEInt32 &other) const;
    TFHEInt32 operator-(const TFHEInt32 &other) const;
    TFHEInt32 operator*(const TFHEInt32 &other) const;
    TFHEInt32 operator/(const TFHEInt32 &other) const;
    TFHEInt32 operator%(const TFHEInt32 &other) const;
    TFHEBool operator==(const TFHEInt32 &other) const;
    TFHEBool operator!=(const TFHEInt32 &other) const;
    TFHEBool operator>(const TFHEInt32 &other) const;
    TFHEBool operator<(const TFHEInt32 &other) const;
    TFHEBool operator>=(const TFHEInt32 &other) const;
    TFHEBool operator<=(const TFHEInt32 &other) const;
    TFHEInt32 operator&(const TFHEInt32 &other) const;
    TFHEInt32 operator|(const TFHEInt32 &other) const;
    TFHEInt32 operator^(const TFHEInt32 &other) const;
    TFHEInt32 operator~() const;
    // TFHEInt32 operator<<(const TFHEInt32 &other);
    // TFHEInt32 operator>>(const TFHEInt32 &other);

    // scalar operation
    TFHEInt32 operator+(int32_t other) const;
    TFHEInt32 operator-(int32_t other) const;
    TFHEInt32 operator*(int32_t other) const;
    TFHEInt32 operator/(int32_t other) const;
    TFHEInt32 operator%(int32_t other) const;
    TFHEBool operator==(int32_t other) const;
    TFHEBool operator!=(int32_t other) const;
    TFHEBool operator>(int32_t other) const;
    TFHEBool operator<(int32_t other) const;
    TFHEBool operator>=(int32_t other) const;
    TFHEBool operator<=(int32_t other) const;
    TFHEInt32 operator&(int32_t other) const;
    TFHEInt32 operator|(int32_t other) const;
    TFHEInt32 operator^(int32_t other) const;
    TFHEInt32 operator<<(int32_t other) const;
    TFHEInt32 operator>>(int32_t other) const;

    
    // static method
    static TFHEInt32 max(const TFHEInt32 &a, const TFHEInt32 &b);
    static TFHEInt32 min(const TFHEInt32 &a, const TFHEInt32 &b);

    friend class TFHEBool;
};

} // SpatialFHE
