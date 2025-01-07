//
// Created by ubuntu on 1/6/25.
//
#include <gtest/gtest.h>
#include "tfhe.h"
#include "TFHEBool.h"
#include "TFHEInt32.h"

using namespace SpatialFHE;

class TfhersDemo : public testing::Test {
protected:
    static ClientKey *client_key;
    static ServerKey *server_key;
    static PublicKey *public_key;

    // 判断点 (x, y) 是否在以 (x1, y1) 和 (x2, y2) 为端点的线段上
    static bool onSegment(int x1, int y1, int x2, int y2, int x, int y) {
        return x <= std::max(x1, x2) && x >= std::min(x1, x2) &&
               y <= std::max(y1, y2) && y >= std::min(y1, y2);
    }

    // 计算方向
    static int orientation(int x1, int y1, int x2, int y2, int x3, int y3) {
        int val = (y2 - y1) * (x3 - x2) - (x2 - x1) * (y3 - y2);
        if (val == 0) return 0;  // collinear
        return (val > 0) ? 1 : -1; // clock or counterclock wise
    }

    // 判断两条线段是否相交
    static bool doIntersect(int x_a1, int y_a1, int x_a2, int y_a2, int x_b1, int y_b1, int x_b2, int y_b2) {
        // 找到四个方向
        int o1 = orientation(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1);
        int o2 = orientation(x_a1, y_a1, x_a2, y_a2, x_b2, y_b2);
        int o3 = orientation(x_b1, y_b1, x_b2, y_b2, x_a1, y_a1);
        int o4 = orientation(x_b1, y_b1, x_b2, y_b2, x_a2, y_a2);

        // 一般情况
        if (o1 != o2 && o3 != o4)
            return true;

        // 特殊情况
        // a1, a2 和 b1 共线且 b1 在 a1a2 上
        if (o1 == 0 && onSegment(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1)) return true;

        // a1, a2 和 b2 共线且 b2 在 a1a2 上
        if (o2 == 0 && onSegment(x_a1, y_a1, x_a2, y_a2, x_b2, y_b2)) return true;

        // b1, b2 和 a1 共线且 a1 在 b1b2 上
        if (o3 == 0 && onSegment(x_b1, y_b1, x_b2, y_b2, x_a1, y_a1)) return true;

        // b1, b2 和 a2 共线且 a2 在 b1b2 上
        if (o4 == 0 && onSegment(x_b1, y_b1, x_b2, y_b2, x_a2, y_a2)) return true;

        return false; // 不相交
    }

    static void SetUpTestCase() {
        // 初始化 TFHE 库
        ConfigBuilder *builder;
        Config *config;

        // Put the builder in a default state without any types enabled
        config_builder_default(&builder);
        // Use the small LWE key for encryption
        config_builder_default_with_small_encryption(&builder);
        // Populate the config
        config_builder_build(builder, &config);
        // Generate the keys using the config
        generate_keys(config, &client_key, &server_key);
        // Set the server key for the current thread
        set_server_key(server_key);
        public_key_new(client_key, &public_key);
    }

    static void TearDownTestCase() {
        // Destroy the keys
        client_key_destroy(client_key);
        server_key_destroy(server_key);
        public_key_destroy(public_key);
    }


    // TFHE 版本的 onSegment
    static FheBool* tfhe_onSegment(FheInt32* x1, FheInt32* y1, FheInt32 *x2, FheInt32 *y2, FheInt32 *x, FheInt32 *y) {
        FheBool* result = nullptr;
        FheInt32* max_x = nullptr;
        FheInt32* min_x = nullptr;
        fhe_int32_max(x1, x2, &max_x);
        fhe_int32_min(x1, x2, &min_x);
        FheInt32* max_y = nullptr;
        FheInt32* min_y = nullptr;
        fhe_int32_max(y1, y2, &max_y);
        fhe_int32_min(y1, y2, &min_y);
        FheBool* x_le_max_x = nullptr;
        fhe_int32_le(x, max_x, &x_le_max_x);
        FheBool* x_ge_min_x = nullptr;
        fhe_int32_ge(x, min_x, &x_ge_min_x);
        FheBool* y_le_max_y = nullptr;
        fhe_int32_le(y, max_y, &y_le_max_y);
        FheBool* y_ge_min_y = nullptr;
        fhe_int32_ge(y, min_y, &y_ge_min_y);
        FheBool* x_le_max_x_and_x_ge_min_x = nullptr;
        fhe_bool_bitand(x_le_max_x, x_ge_min_x, &x_le_max_x_and_x_ge_min_x);
        FheBool* y_le_max_y_and_y_ge_min_y = nullptr;
        fhe_bool_bitand(y_le_max_y, y_ge_min_y, &y_le_max_y_and_y_ge_min_y);
        fhe_bool_bitand(x_le_max_x_and_x_ge_min_x, y_le_max_y_and_y_ge_min_y, &result);

        // clear
        fhe_int32_destroy(max_x);
        fhe_int32_destroy(min_x);
        fhe_int32_destroy(min_y);
        fhe_int32_destroy(max_y);
        fhe_bool_destroy(x_le_max_x);
        fhe_bool_destroy(x_ge_min_x);
        fhe_bool_destroy(y_le_max_y);
        fhe_bool_destroy(y_ge_min_y);
        fhe_bool_destroy(x_le_max_x_and_x_ge_min_x);
        fhe_bool_destroy(y_le_max_y_and_y_ge_min_y);

        return result;
    }

    // TFHE 版本的 orientation
    static FheInt8* tfhe_orientation(FheInt32* x1, FheInt32* y1, FheInt32* x2, FheInt32* y2, FheInt32* x3, FheInt32* y3) {
        FheInt32* val = nullptr;
        FheInt32* y2_minus_y1 = nullptr;
        fhe_int32_sub(y2, y1, &y2_minus_y1);
        FheInt32* x3_minus_x2 = nullptr;
        fhe_int32_sub(x3, x2, &x3_minus_x2);
        FheInt32* x2_minus_x1 = nullptr;
        fhe_int32_sub(x2, x1, &x2_minus_x1);
        FheInt32* y3_minus_y2 = nullptr;
        fhe_int32_sub(y3, y2, &y3_minus_y2);
        FheInt32* y2_minus_y1_mul_x3_minus_x2 = nullptr;
        fhe_int32_mul(y2_minus_y1, x3_minus_x2, &y2_minus_y1_mul_x3_minus_x2);
        FheInt32* x2_minus_x1_mul_y3_minus_y2 = nullptr;
        fhe_int32_mul(x2_minus_x1, y3_minus_y2, &x2_minus_x1_mul_y3_minus_y2);
        fhe_int32_sub(y2_minus_y1_mul_x3_minus_x2, x2_minus_x1_mul_y3_minus_y2, &val);

        FheInt32 *fhe_int32_zero = nullptr;
        fhe_int32_try_encrypt_with_client_key_i32(0, client_key, &fhe_int32_zero);
        FheInt8 *fhe_int8_zero = nullptr;
        fhe_int32_cast_into_fhe_int8(fhe_int32_zero, &fhe_int8_zero);

        FheInt8* result = nullptr;
        FheBool *val_gt_0 = nullptr;
        FheBool *val_lt_0 = nullptr;
        fhe_int32_gt(val, fhe_int32_zero, &val_gt_0);
        fhe_int32_lt(val, fhe_int32_zero, &val_lt_0);

        FheInt8 *result_gt_0 = nullptr;
        FheInt8 *result_lt_0 = nullptr;
        fhe_bool_cast_into_fhe_int8(val_gt_0, &result_gt_0);
        fhe_bool_cast_into_fhe_int8(val_lt_0, &result_lt_0);
        // result = val > 0 ? 1 : (val < 0 ? -1 : 0)
        fhe_int8_add(result_gt_0, fhe_int8_zero, &result);
        fhe_int8_sub(result, result_lt_0, &result);

        // destroy
        fhe_int32_destroy(y2_minus_y1);
        fhe_int32_destroy(x3_minus_x2);
        fhe_int32_destroy(x2_minus_x1);
        fhe_int32_destroy(y3_minus_y2);
        fhe_int32_destroy(y2_minus_y1_mul_x3_minus_x2);
        fhe_int32_destroy(x2_minus_x1_mul_y3_minus_y2);
        fhe_int32_destroy(val);
        fhe_int32_destroy(fhe_int32_zero);
        fhe_int8_destroy(fhe_int8_zero);
        fhe_bool_destroy(val_gt_0);
        fhe_bool_destroy(val_lt_0);
        fhe_int8_destroy(result_gt_0);
        fhe_int8_destroy(result_lt_0);

        return result;
    }

    // TFHE 版本的 doIntersect
    static FheBool* tfhe_doIntersect(FheInt32* x_a1, FheInt32* y_a1, FheInt32* x_a2, FheInt32* y_a2,
                                   FheInt32* x_b1, FheInt32* y_b1, FheInt32* x_b2, FheInt32* y_b2) {
        FheBool* result = nullptr;
        FheInt8* o1 = tfhe_orientation(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1);
        FheInt8* o2 = tfhe_orientation(x_a1, y_a1, x_a2, y_a2, x_b2, y_b2);
        FheInt8* o3 = tfhe_orientation(x_b1, y_b1, x_b2, y_b2, x_a1, y_a1);
        FheInt8* o4 = tfhe_orientation(x_b1, y_b1, x_b2, y_b2, x_a2, y_a2);

        FheBool* o1_ne_o2 = nullptr;
        fhe_int8_ne(o1, o2, &o1_ne_o2);
        FheBool* o3_ne_o4 = nullptr;
        fhe_int8_ne(o3, o4, &o3_ne_o4);
        fhe_bool_bitand(o1_ne_o2, o3_ne_o4, &result);

        FheInt8 *fhe_int8_zero = nullptr;
        fhe_int8_try_encrypt_with_client_key_i8(0, client_key, &fhe_int8_zero);

        FheBool* o1_eq_0 = nullptr;
        fhe_int8_eq(o1, fhe_int8_zero, &o1_eq_0);
        FheBool* on_segment_1 = tfhe_onSegment(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1);
        FheBool* o1_eq_0_and_on_segment_1 = nullptr;
        fhe_bool_bitand(o1_eq_0, on_segment_1, &o1_eq_0_and_on_segment_1);
        FheBool* o2_eq_0 = nullptr;
        fhe_int8_eq(o2, fhe_int8_zero, &o2_eq_0);
        FheBool* on_segment_2 = tfhe_onSegment(x_a1, y_a1, x_a2, y_a2, x_b2, y_b2);
        FheBool* o2_eq_0_and_on_segment_2 = nullptr;
        fhe_bool_bitand(o2_eq_0, on_segment_2, &o2_eq_0_and_on_segment_2);
        FheBool* o3_eq_0 = nullptr;
        fhe_int8_eq(o3, fhe_int8_zero, &o3_eq_0);
        FheBool* on_segment_3 = tfhe_onSegment(x_b1, y_b1, x_b2, y_b2, x_a1, y_a1);
        FheBool* o3_eq_0_and_on_segment_3 = nullptr;
        fhe_bool_bitand(o3_eq_0, on_segment_3, &o3_eq_0_and_on_segment_3);
        FheBool* o4_eq_0 = nullptr;
        fhe_int8_eq(o4, fhe_int8_zero, &o4_eq_0);
        FheBool* on_segment_4 = tfhe_onSegment(x_b1, y_b1, x_b2, y_b2, x_a2, y_a2);
        FheBool* o4_eq_0_and_on_segment_4 = nullptr;
        fhe_bool_bitand(o4_eq_0, on_segment_4, &o4_eq_0_and_on_segment_4);

        fhe_bool_bitor(result, o1_eq_0_and_on_segment_1, &result);
        fhe_bool_bitor(result, o2_eq_0_and_on_segment_2, &result);
        fhe_bool_bitor(result, o3_eq_0_and_on_segment_3, &result);
        fhe_bool_bitor(result, o4_eq_0_and_on_segment_4, &result);

        // clear
        fhe_int8_destroy(fhe_int8_zero);
        fhe_int8_destroy(o1);
        fhe_int8_destroy(o2);
        fhe_int8_destroy(o3);
        fhe_int8_destroy(o4);
        fhe_bool_destroy(o1_ne_o2);
        fhe_bool_destroy(o3_ne_o4);
        fhe_bool_destroy(o1_eq_0);
        fhe_bool_destroy(on_segment_1);
        fhe_bool_destroy(o1_eq_0_and_on_segment_1);
        fhe_bool_destroy(o2_eq_0);
        fhe_bool_destroy(on_segment_2);
        fhe_bool_destroy(o2_eq_0_and_on_segment_2);
        fhe_bool_destroy(o3_eq_0);
        fhe_bool_destroy(on_segment_3);
        fhe_bool_destroy(o3_eq_0_and_on_segment_3);
        fhe_bool_destroy(o4_eq_0);
        fhe_bool_destroy(on_segment_4);
        fhe_bool_destroy(o4_eq_0_and_on_segment_4);

        return result;
    }

    // SpatialFHE 版本的 onSegment
    static TFHEBool spatial_onSegement(TFHEInt32 x1, TFHEInt32 y1,
                                        TFHEInt32 x2, TFHEInt32 y2,
                                        TFHEInt32 x, TFHEInt32 y) {
        TFHEBool result = x <= TFHEInt32::max(x1, x2) && x >= TFHEInt32::min(x1, x2) &&
                              y <= TFHEInt32::max(y1, y2) && y >= TFHEInt32::min(y1, y2);
        return result;
    }

    static TFHEInt32 spatial_orientation(TFHEInt32 x1, TFHEInt32 y1,
                                         TFHEInt32 x2, TFHEInt32 y2,
                                         TFHEInt32 x3, TFHEInt32 y3) {
        TFHEInt32 val = (y2 - y1) * (x3 - x2) - (x2 - x1) * (y3 - y2);
        TFHEInt32 zero(public_key, 0);
        return TFHEInt32(val > zero) - TFHEInt32(val < zero);
    }

    static TFHEBool spatial_doIntersect(TFHEInt32 x_a1, TFHEInt32 y_a1,
                                        TFHEInt32 x_a2, TFHEInt32 y_a2,
                                        TFHEInt32 x_b1, TFHEInt32 y_b1,
                                        TFHEInt32 x_b2, TFHEInt32 y_b2) {
        TFHEInt32 o1 = spatial_orientation(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1);
        TFHEInt32 o2 = spatial_orientation(x_a1, y_a1, x_a2, y_a2, x_b2, y_b2);
        TFHEInt32 o3 = spatial_orientation(x_b1, y_b1, x_b2, y_b2, x_a1, y_a1);
        TFHEInt32 o4 = spatial_orientation(x_b1, y_b1, x_b2, y_b2, x_a2, y_a2);

        TFHEBool result = (o1 != o2) && (o3 != o4);
        TFHEInt32 zero(public_key, 0);
        return result || (o1 == zero && spatial_onSegement(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1)) ||
               (o2 == zero && spatial_onSegement(x_a1, y_a1, x_a2, y_a2, x_b2, y_b2)) ||
               (o3 == zero && spatial_onSegement(x_b1, y_b1, x_b2, y_b2, x_a1, y_a1)) ||
               (o4 == zero && spatial_onSegement(x_b1, y_b1, x_b2, y_b2, x_a2, y_a2));
        }
};

ClientKey *TfhersDemo::client_key = nullptr;
ServerKey *TfhersDemo::server_key = nullptr;
PublicKey *TfhersDemo::public_key = nullptr;

TEST_F(TfhersDemo, IntersectTestInPlain) {
    // Case 1: General case, lines intersect
    EXPECT_TRUE(doIntersect(1, 1, 10, 10, 10, 1, 1, 10));

    // Case 2: General case, lines do not intersect
    EXPECT_FALSE(doIntersect(1, 1, 10, 1, 1, 2, 10, 3));

    // Case 3: Collinear and overlapping
    EXPECT_TRUE(doIntersect(1, 1, 10, 1, 5, 1, 15, 1));

    // Case 4: Collinear but not overlapping
    EXPECT_FALSE(doIntersect(1, 1, 10, 1, 11, 1, 20, 1));

    // Case 5: End point touching
    EXPECT_TRUE(doIntersect(1, 1, 10, 1, 10, 1, 20, 1));

    // Case 6: One line segment is a point on the other line segment
    EXPECT_TRUE(doIntersect(1, 1, 10, 1, 5, 1, 5, 1));

    // Case 7: Lines intersect at an endpoint
    EXPECT_TRUE(doIntersect(1, 1, 10, 10, 10, 10, 20, 20));

    // Case 8: Lines are parallel and non-overlapping
    EXPECT_FALSE(doIntersect(1, 1, 10, 10, 1, 2, 10, 11));
}

TEST_F(TfhersDemo, IntersectTestInFHE) {
        // Case 1: General case, lines intersect
        FheInt32 *x_a1 = nullptr;
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &x_a1);
        FheInt32 *y_a1 = nullptr;
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a1);
        FheInt32 *x_a2 = nullptr;
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_a2);
        FheInt32 *y_a2 = nullptr;
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &y_a2);
        FheInt32 *x_b1 = nullptr;
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_b1);
        FheInt32 *y_b1 = nullptr;
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_b1);
        FheInt32 *x_b2 = nullptr;
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &x_b2);
        FheInt32 *y_b2 = nullptr;
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &y_b2);
        FheBool *fhe_result = tfhe_doIntersect(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1, x_b2, y_b2);
        bool result;
        fhe_bool_decrypt(fhe_result, client_key, &result);
        EXPECT_TRUE(result);
        fhe_bool_destroy(fhe_result);
    
        // Case 2: General case, lines do not intersect
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &x_a1);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a1);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_a2);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a2);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &x_b1);
        fhe_int32_try_encrypt_with_client_key_i32(2, client_key, &y_b1);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_b2);
        fhe_int32_try_encrypt_with_client_key_i32(3, client_key, &y_b2);
        fhe_result = tfhe_doIntersect(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1, x_b2, y_b2);
        fhe_bool_decrypt(fhe_result, client_key, &result);
        EXPECT_FALSE(result);
        fhe_bool_destroy(fhe_result);

        // Case 3: Collinear and overlapping
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &x_a1);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a1);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_a2);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a2);
        fhe_int32_try_encrypt_with_client_key_i32(5, client_key, &x_b1);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_b1);
        fhe_int32_try_encrypt_with_client_key_i32(15, client_key, &x_b2);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_b2);
        fhe_result = tfhe_doIntersect(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1, x_b2, y_b2);
        fhe_bool_decrypt(fhe_result, client_key, &result);
        EXPECT_TRUE(result);
        fhe_bool_destroy(fhe_result);

        // Case 4: Collinear but not overlapping
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &x_a1);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a1);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_a2);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a2);
        fhe_int32_try_encrypt_with_client_key_i32(11, client_key, &x_b1);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_b1);
        fhe_int32_try_encrypt_with_client_key_i32(20, client_key, &x_b2);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_b2);
        fhe_result = tfhe_doIntersect(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1, x_b2, y_b2);
        fhe_bool_decrypt(fhe_result, client_key, &result);
        EXPECT_FALSE(result);
        fhe_bool_destroy(fhe_result);

        // Case 5: End point touching
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &x_a1);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a1);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_a2);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a2);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_b1);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_b1);
        fhe_int32_try_encrypt_with_client_key_i32(20, client_key, &x_b2);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_b2);
        fhe_result = tfhe_doIntersect(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1, x_b2, y_b2);
        fhe_bool_decrypt(fhe_result, client_key, &result);
        EXPECT_TRUE(result);
        fhe_bool_destroy(fhe_result);

        // Case 6: One line segment is a point on the other line segment
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &x_a1);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a1);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_a2);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a2);
        fhe_int32_try_encrypt_with_client_key_i32(5, client_key, &x_b1);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_b1);
        fhe_int32_try_encrypt_with_client_key_i32(5, client_key, &x_b2);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_b2);
        fhe_result = tfhe_doIntersect(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1, x_b2, y_b2);
        fhe_bool_decrypt(fhe_result, client_key, &result);
        EXPECT_TRUE(result);
        fhe_bool_destroy(fhe_result);

        // Case 7: Lines intersect at an endpoint
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &x_a1);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a1);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_a2);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &y_a2);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_b1);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &y_b1);
        fhe_int32_try_encrypt_with_client_key_i32(20, client_key, &x_b2);
        fhe_int32_try_encrypt_with_client_key_i32(20, client_key, &y_b2);
        fhe_result = tfhe_doIntersect(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1, x_b2, y_b2);
        fhe_bool_decrypt(fhe_result, client_key, &result);
        EXPECT_TRUE(result);

        // Case 8: Lines are parallel and non-overlapping
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &x_a1);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &y_a1);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_a2);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &y_a2);
        fhe_int32_try_encrypt_with_client_key_i32(1, client_key, &x_b1);
        fhe_int32_try_encrypt_with_client_key_i32(2, client_key, &y_b1);
        fhe_int32_try_encrypt_with_client_key_i32(10, client_key, &x_b2);
        fhe_int32_try_encrypt_with_client_key_i32(11, client_key, &y_b2);
        fhe_result = tfhe_doIntersect(x_a1, y_a1, x_a2, y_a2, x_b1, y_b1, x_b2, y_b2);
        fhe_bool_decrypt(fhe_result, client_key, &result);
        EXPECT_FALSE(result);


        // clear
        fhe_int32_destroy(x_a1);
        fhe_int32_destroy(y_a1);
        fhe_int32_destroy(x_a2);
        fhe_int32_destroy(y_a2);
        fhe_int32_destroy(x_b1);
        fhe_int32_destroy(y_b1);
        fhe_int32_destroy(x_b2);
        fhe_int32_destroy(y_b2);
}

TEST_F(TfhersDemo, IntersectTestSpatialFHE) {
    TFHEBool result 
        = spatial_doIntersect(TFHEInt32(public_key, 1), TFHEInt32(public_key, 1), 
                            TFHEInt32(public_key, 10), TFHEInt32(public_key, 10), 
                            TFHEInt32(public_key, 10), TFHEInt32(public_key, 1), 
                            TFHEInt32(public_key, 1), TFHEInt32(public_key, 10));
    EXPECT_TRUE(result.decrypt(client_key));

}
