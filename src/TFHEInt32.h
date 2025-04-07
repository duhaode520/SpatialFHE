#pragma once
#include "TFHEBool.h"
#include "TFHERegisteredType.h"
#include "tfhe.h"

namespace SpatialFHE {

    class TFHEInt32 : public TFHERegisteredType<int> {
    private:
        FheInt32 *data;
#ifdef DEBUG
        int32_t ori;
#endif


        // 应当只用在测试程序中
        static int32_t doDecrypt(std::vector<uint8_t> &data);
        // remoteDecrypt应当只用在测试程序中
        int32_t remoteDecrypt() const;

    public:
        explicit TFHEInt32(int32_t value, bool trivial=false);
        explicit TFHEInt32(FheInt32 *data);
        TFHEInt32();
        ~TFHEInt32() override;
        int decrypt() const override;

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
        TFHEInt32 operator-() const;
        // TFHEInt32 operator<<(const TFHEInt32 &other);
        // TFHEInt32 operator>>(const TFHEInt32 &other);

        TFHEInt32 operator+=(const TFHEInt32 &other);
        TFHEInt32 operator-=(const TFHEInt32 &other);
        TFHEInt32 operator*=(const TFHEInt32 &other);
        TFHEInt32 operator/=(const TFHEInt32 &other);

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

        TFHEInt32 operator+=(int32_t other);
        TFHEInt32 operator-=(int32_t other);
        TFHEInt32 operator*=(int32_t other);
        TFHEInt32 operator/=(int32_t other);

        [[nodiscard]] bool eqTrivial(int32_t other) const;
        [[nodiscard]] bool gtTrivial(int32_t other) const;
        [[nodiscard]] bool geTrivial(int32_t other) const;
        [[nodiscard]] bool ltTrivial(int32_t other) const;
        [[nodiscard]] bool leTrivial(int32_t other) const;
        [[nodiscard]] bool neTrivial(int32_t other) const;



        // static method
        static TFHEInt32 max(const TFHEInt32 &a, const TFHEInt32 &b);
        static TFHEInt32 min(const TFHEInt32 &a, const TFHEInt32 &b);

        static void registerContext(TFHEContext *ctx);

        static std::vector<uint8_t> serialize(const TFHEInt32 &item);
        static TFHEInt32 deserialize(std::vector<uint8_t> &data);

        static const TFHEContext* javaGetContext();

        friend class TFHEBool;
    };

}  // namespace SpatialFHE
