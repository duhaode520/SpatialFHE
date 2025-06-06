#pragma once
#include "tfhe.h"
#include "TFHERegisteredType.h"

namespace SpatialFHE {
    class TFHEInt32;
    class TFHEDecimal;  // 添加前向声明

    class TFHEBool : public TFHERegisteredType<bool> {
    private:
        FheBool *data;
#ifdef DEBUG
        bool ori;
#endif

        /**
         * Decrypt data from remote
         * @param data 0-1 balanced serialized TFHEBool, each element is a vector of bytes.
         * Shortest vector should have two elements, which are TFHEBool(0) and TFHEBool(1)
         * @return decrypted data
         */
        static std::vector<bool> doDecrypt(std::vector<std::vector<uint8_t>> &data);

        bool remoteDecrypt() const;

    public:
        explicit TFHEBool(bool value, bool trivial = false);
        explicit TFHEBool(FheBool *data);
        TFHEBool();
        ~TFHEBool() override;

        TFHEBool(const TFHEBool &other);
        TFHEBool(TFHEBool &&other) noexcept;

        bool isNull() const;

        TFHEBool &operator=(const TFHEBool &other);

        TFHEBool &operator=(TFHEBool &&other) noexcept;

        bool decrypt() const override;

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
        explicit operator TFHEDecimal() const;  // 添加转换为TFHEDecimal的函数声明

        friend class TFHEInt32;
        friend class TFHEInt64;
        friend class TFHEDecimal;

        // static instance
        static TFHEBool tfhe_true;
        static TFHEBool tfhe_false;
        static void init();

        static void registerContext(TFHEContext* ctx) ;
        static bool isLocalTrue(const TFHEBool &other);
        static bool isLocalFalse(const TFHEBool &other);

        // 添加序列化和反序列化函数的声明
        static std::vector<uint8_t> serialize(const TFHEBool &item);
        static TFHEBool deserialize(std::vector<uint8_t> &data);
    };

}  // namespace SpatialFHE
