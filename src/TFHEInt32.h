#pragma once
#include "TFHEBool.h"
#include "tfhe.h"

namespace SpatialFHE {

class TFHEInt32 {
private:
    FheInt32* data;
public:
    TFHEInt32(PublicKey* public_key, int32_t value);
    explicit TFHEInt32(FheInt32* data);
    TFHEInt32();
    ~TFHEInt32();

    TFHEInt32(const TFHEInt32 &other);
    TFHEInt32(TFHEInt32 &&other) noexcept;
    TFHEInt32 &operator=(const TFHEInt32 &other);
    TFHEInt32 &operator=(TFHEInt32 &&other) noexcept;

    // redefine the operator
    TFHEInt32 operator+(const TFHEInt32 &other);
    TFHEInt32 operator-(const TFHEInt32 &other);
    TFHEInt32 operator*(const TFHEInt32 &other);
    TFHEInt32 operator/(const TFHEInt32 &other);
    TFHEInt32 operator%(const TFHEInt32 &other);
    TFHEBool operator==(const TFHEInt32 &other);
    TFHEBool operator!=(const TFHEInt32 &other);
    TFHEBool operator>(const TFHEInt32 &other);
    TFHEBool operator<(const TFHEInt32 &other);
    TFHEBool operator>=(const TFHEInt32 &other);
    TFHEBool operator<=(const TFHEInt32 &other);
    TFHEInt32 operator&(const TFHEInt32 &other);
    TFHEInt32 operator|(const TFHEInt32 &other);
    TFHEInt32 operator^(const TFHEInt32 &other);
    TFHEInt32 operator~();
    // TFHEInt32 operator<<(const TFHEInt32 &other);
    // TFHEInt32 operator>>(const TFHEInt32 &other);
    
    // static method
    static TFHEInt32 max(const TFHEInt32 &a, const TFHEInt32 &b);
    static TFHEInt32 min(const TFHEInt32 &a, const TFHEInt32 &b);

    friend class TFHEBool;
};

} // SpatialFHE
