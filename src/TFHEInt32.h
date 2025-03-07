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

        // /**
        // * @brief Decrypt comparison data from remote, data range in -1, 0, 1
        // *
        // * @param data conjunction of the encrypted data, * including equal number of encrypted -1, 0, 1
        // * @return decrypted
        // */
        // static std::vector<int32_t> doDecryptComparison(std::vector<std::vector<std::byte>> &data);
        //
        // int32_t remoteDecrypt();

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

        friend class TFHEBool;
    };

}  // namespace SpatialFHE
