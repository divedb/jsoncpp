#include <gtest/gtest.h>

#include "jsoncpp/type.hh"

namespace jsoncpp {
namespace {

TEST(TypeFlagTest, SizeIsOneByte) { EXPECT_EQ(sizeof(TypeFlag), 1u); }

TEST(TypeFlagTest, NullFactory) {
  auto flag = TypeFlag::Null();

  EXPECT_TRUE(flag.IsNull());
  EXPECT_FALSE(flag.IsBool());
  EXPECT_FALSE(flag.IsNumber());
  EXPECT_FALSE(flag.IsString());
  EXPECT_FALSE(flag.IsArray());
  EXPECT_FALSE(flag.IsObject());
  EXPECT_FALSE(flag.IsRaw());
}

TEST(TypeFlagTest, RawFactory) {
  auto flag = TypeFlag::Raw();

  EXPECT_TRUE(flag.IsRaw());
  EXPECT_FALSE(flag.IsNull());
  EXPECT_FALSE(flag.IsBool());
  EXPECT_FALSE(flag.IsNumber());
  EXPECT_FALSE(flag.IsString());
  EXPECT_FALSE(flag.IsArray());
  EXPECT_FALSE(flag.IsObject());
}

TEST(TypeFlagTest, ObjectFactory) {
  auto flag = TypeFlag::Object();

  EXPECT_TRUE(flag.IsObject());
  EXPECT_FALSE(flag.IsNull());
  EXPECT_FALSE(flag.IsBool());
  EXPECT_FALSE(flag.IsNumber());
  EXPECT_FALSE(flag.IsString());
  EXPECT_FALSE(flag.IsArray());
  EXPECT_FALSE(flag.IsRaw());
}

TEST(TypeFlagTest, ArrayFactory) {
  auto flag = TypeFlag::Array();

  EXPECT_TRUE(flag.IsArray());
  EXPECT_FALSE(flag.IsNull());
  EXPECT_FALSE(flag.IsBool());
  EXPECT_FALSE(flag.IsNumber());
  EXPECT_FALSE(flag.IsString());
  EXPECT_FALSE(flag.IsObject());
  EXPECT_FALSE(flag.IsRaw());
}

TEST(TypeFlagTest, TrueFactory) {
  auto flag = TypeFlag::True();

  EXPECT_TRUE(flag.IsBool());
  EXPECT_TRUE(flag.IsTrue());
  EXPECT_FALSE(flag.IsFalse());
  EXPECT_FALSE(flag.IsNull());
  EXPECT_FALSE(flag.IsNumber());
  EXPECT_FALSE(flag.IsString());
  EXPECT_FALSE(flag.IsArray());
  EXPECT_FALSE(flag.IsObject());
  EXPECT_FALSE(flag.IsRaw());
}

TEST(TypeFlagTest, FalseFactory) {
  auto flag = TypeFlag::False();

  EXPECT_TRUE(flag.IsBool());
  EXPECT_TRUE(flag.IsFalse());
  EXPECT_FALSE(flag.IsTrue());
  EXPECT_FALSE(flag.IsNull());
  EXPECT_FALSE(flag.IsNumber());
  EXPECT_FALSE(flag.IsString());
  EXPECT_FALSE(flag.IsArray());
  EXPECT_FALSE(flag.IsObject());
  EXPECT_FALSE(flag.IsRaw());
}

TEST(TypeFlagTest, TrueAndFalseAreDifferent) {
  EXPECT_NE(TypeFlag::True(), TypeFlag::False());
}

TEST(TypeFlagTest, UIntFactory) {
  auto flag = TypeFlag::UInt();
  EXPECT_TRUE(flag.IsNumber());
  EXPECT_TRUE(flag.IsUInt());
  EXPECT_FALSE(flag.IsSInt());
  EXPECT_FALSE(flag.IsReal());
  EXPECT_FALSE(flag.IsNull());
  EXPECT_FALSE(flag.IsBool());
  EXPECT_FALSE(flag.IsString());
  EXPECT_FALSE(flag.IsArray());
  EXPECT_FALSE(flag.IsObject());
  EXPECT_FALSE(flag.IsRaw());
}

TEST(TypeFlagTest, SIntFactory) {
  auto flag = TypeFlag::SInt();

  EXPECT_TRUE(flag.IsNumber());
  EXPECT_TRUE(flag.IsSInt());
  EXPECT_FALSE(flag.IsUInt());
  EXPECT_FALSE(flag.IsReal());
  EXPECT_FALSE(flag.IsNull());
  EXPECT_FALSE(flag.IsBool());
  EXPECT_FALSE(flag.IsString());
  EXPECT_FALSE(flag.IsArray());
  EXPECT_FALSE(flag.IsObject());
  EXPECT_FALSE(flag.IsRaw());
}

TEST(TypeFlagTest, RealFactory) {
  auto flag = TypeFlag::Real();

  EXPECT_TRUE(flag.IsNumber());
  EXPECT_TRUE(flag.IsReal());
  EXPECT_FALSE(flag.IsUInt());
  EXPECT_FALSE(flag.IsSInt());
  EXPECT_FALSE(flag.IsNull());
  EXPECT_FALSE(flag.IsBool());
  EXPECT_FALSE(flag.IsString());
  EXPECT_FALSE(flag.IsArray());
  EXPECT_FALSE(flag.IsObject());
  EXPECT_FALSE(flag.IsRaw());
}

TEST(TypeFlagTest, NumberSubtypesAreDistinct) {
  EXPECT_NE(TypeFlag::UInt(), TypeFlag::SInt());
  EXPECT_NE(TypeFlag::UInt(), TypeFlag::Real());
  EXPECT_NE(TypeFlag::SInt(), TypeFlag::Real());
}

TEST(TypeFlagTest, OwnedStringFactory) {
  auto flag = TypeFlag::OwnedString();

  EXPECT_TRUE(flag.IsString());
  EXPECT_TRUE(flag.IsOwnedString());
  EXPECT_FALSE(flag.IsBorrowedString());
  EXPECT_FALSE(flag.IsNull());
  EXPECT_FALSE(flag.IsBool());
  EXPECT_FALSE(flag.IsNumber());
  EXPECT_FALSE(flag.IsArray());
  EXPECT_FALSE(flag.IsObject());
  EXPECT_FALSE(flag.IsRaw());
}

TEST(TypeFlagTest, BorrowedStringFactory) {
  auto flag = TypeFlag::BorrowedString();

  EXPECT_TRUE(flag.IsString());
  EXPECT_TRUE(flag.IsBorrowedString());
  EXPECT_FALSE(flag.IsOwnedString());
  EXPECT_FALSE(flag.IsNull());
  EXPECT_FALSE(flag.IsBool());
  EXPECT_FALSE(flag.IsNumber());
  EXPECT_FALSE(flag.IsArray());
  EXPECT_FALSE(flag.IsObject());
  EXPECT_FALSE(flag.IsRaw());
}

TEST(TypeFlagTest, StringSubtypesAreDistinct) {
  EXPECT_NE(TypeFlag::OwnedString(), TypeFlag::BorrowedString());
}

class IsValidRawValueTest
    : public ::testing::TestWithParam<std::tuple<uint8_t, bool>> {};

TEST_P(IsValidRawValueTest, CheckValidity) {
  auto [value, expected] = GetParam();
  EXPECT_EQ(TypeFlag::IsValidRawValue(value), expected)
      << "For value " << static_cast<int>(value);
}

INSTANTIATE_TEST_SUITE_P(
    ValidValues, IsValidRawValueTest,
    ::testing::Values(
        // Null (category 0b000, subtype must be 0)
        std::make_tuple(0b00000000, true),

        // Bool (category 0b010, subtype 0-1 valid)
        std::make_tuple(0b00000010, true),   // Bool + kFalse
        std::make_tuple(0b00001010, true),   // Bool + kTrue
        std::make_tuple(0b00010010, false),  // Bool + subtype 2 (invalid)

        // Number (category 0b011, subtype 0-2 valid)
        std::make_tuple(0b00000011, true),   // Number + kUInt
        std::make_tuple(0b00001011, true),   // Number + kSInt
        std::make_tuple(0b00010011, true),   // Number + kReal
        std::make_tuple(0b00011011, false),  // Number + subtype 3 (invalid)

        // String (category 0b100, subtype 0-1 valid)
        std::make_tuple(0b00000100, true),   // String + Owned
        std::make_tuple(0b00001100, true),   // String + Borrowed
        std::make_tuple(0b00010100, false),  // String + subtype 2 (invalid)

        // Raw (category 0b101, subtype must be 0)
        std::make_tuple(0b00000101, true),
        std::make_tuple(0b00001101, false),  // Raw + subtype (invalid)

        // Object (category 0b110, subtype must be 0)
        std::make_tuple(0b00000110, true),
        std::make_tuple(0b00001110, false),  // Object + subtype (invalid)

        // Array (category 0b111, subtype must be 0)
        std::make_tuple(0b00000111, true),
        std::make_tuple(0b00001111, false),  // Array + subtype (invalid)

        // Reserved bits (bits 5-7) must be 0
        std::make_tuple(0b00100000, false), std::make_tuple(0b01000000, false),
        std::make_tuple(0b10000000, false), std::make_tuple(0b11100000, false),

        // Unused category 0b001
        std::make_tuple(0b00000001, false)));

TEST(FromRawValueTest, ReturnsNulloptForInvalidValues) {
  // Reserved bits set
  EXPECT_EQ(TypeFlag::FromRawValue(0x80), std::nullopt);
  EXPECT_EQ(TypeFlag::FromRawValue(0xE0), std::nullopt);

  // Invalid category (0b001)
  EXPECT_EQ(TypeFlag::FromRawValue(0x01), std::nullopt);

  // Invalid subtypes
  EXPECT_EQ(TypeFlag::FromRawValue(0b00010010), std::nullopt);  // Bool + sub 2
  EXPECT_EQ(TypeFlag::FromRawValue(0b00011011), std::nullopt);  // Num + sub 3
  EXPECT_EQ(TypeFlag::FromRawValue(0b00010100), std::nullopt);  // Str + sub 2
  EXPECT_EQ(TypeFlag::FromRawValue(0b00001101), std::nullopt);  // Raw + sub
  EXPECT_EQ(TypeFlag::FromRawValue(0b00001110), std::nullopt);  // Obj + sub
  EXPECT_EQ(TypeFlag::FromRawValue(0b00001111), std::nullopt);  // Arr + sub
}

TEST(FromRawValueTest, ReturnsValueForValidRawValues) {
  // Null
  auto null_flag = TypeFlag::FromRawValue(0x00);
  ASSERT_TRUE(null_flag.has_value());
  EXPECT_TRUE(null_flag->IsNull());

  // True
  auto true_flag = TypeFlag::FromRawValue(0b00001010);
  ASSERT_TRUE(true_flag.has_value());
  EXPECT_TRUE(true_flag->IsTrue());

  // False
  auto false_flag = TypeFlag::FromRawValue(0b00000010);
  ASSERT_TRUE(false_flag.has_value());
  EXPECT_TRUE(false_flag->IsFalse());

  // UInt
  auto uint_flag = TypeFlag::FromRawValue(0b00000011);
  ASSERT_TRUE(uint_flag.has_value());
  EXPECT_TRUE(uint_flag->IsUInt());

  // SInt
  auto sint_flag = TypeFlag::FromRawValue(0b00001011);
  ASSERT_TRUE(sint_flag.has_value());
  EXPECT_TRUE(sint_flag->IsSInt());

  // Real
  auto real_flag = TypeFlag::FromRawValue(0b00010011);
  ASSERT_TRUE(real_flag.has_value());
  EXPECT_TRUE(real_flag->IsReal());

  // Owned String
  auto owned_str = TypeFlag::FromRawValue(0b00000100);
  ASSERT_TRUE(owned_str.has_value());
  EXPECT_TRUE(owned_str->IsOwnedString());

  // Borrowed String
  auto borrowed_str = TypeFlag::FromRawValue(0b00001100);
  ASSERT_TRUE(borrowed_str.has_value());
  EXPECT_TRUE(borrowed_str->IsBorrowedString());

  // Raw
  auto raw_flag = TypeFlag::FromRawValue(0b00000101);
  ASSERT_TRUE(raw_flag.has_value());
  EXPECT_TRUE(raw_flag->IsRaw());

  // Object
  auto obj_flag = TypeFlag::FromRawValue(0b00000110);
  ASSERT_TRUE(obj_flag.has_value());
  EXPECT_TRUE(obj_flag->IsObject());

  // Array
  auto arr_flag = TypeFlag::FromRawValue(0b00000111);
  ASSERT_TRUE(arr_flag.has_value());
  EXPECT_TRUE(arr_flag->IsArray());
}

TEST(FromRawValueConsistencyTest, NullConsistency) {
  auto from_raw = TypeFlag::FromRawValue(0x00);
  ASSERT_TRUE(from_raw.has_value());
  EXPECT_EQ(from_raw, TypeFlag::Null());
}

TEST(FromRawValueConsistencyTest, TrueConsistency) {
  auto from_raw = TypeFlag::FromRawValue(0b00001010);
  ASSERT_TRUE(from_raw.has_value());
  EXPECT_EQ(from_raw, TypeFlag::True());
}

TEST(FromRawValueConsistencyTest, FalseConsistency) {
  auto from_raw = TypeFlag::FromRawValue(0b00000010);
  ASSERT_TRUE(from_raw.has_value());
  EXPECT_EQ(from_raw, TypeFlag::False());
}

TEST(FromRawValueConsistencyTest, UIntConsistency) {
  auto from_raw = TypeFlag::FromRawValue(0b00000011);
  ASSERT_TRUE(from_raw.has_value());
  EXPECT_EQ(from_raw, TypeFlag::UInt());
}

TEST(FromRawValueConsistencyTest, SIntConsistency) {
  auto from_raw = TypeFlag::FromRawValue(0b00001011);
  ASSERT_TRUE(from_raw.has_value());
  EXPECT_EQ(from_raw, TypeFlag::SInt());
}

TEST(FromRawValueConsistencyTest, RealConsistency) {
  auto from_raw = TypeFlag::FromRawValue(0b00010011);
  ASSERT_TRUE(from_raw.has_value());
  EXPECT_EQ(from_raw, TypeFlag::Real());
}

TEST(FromRawValueConsistencyTest, OwnedStringConsistency) {
  auto from_raw = TypeFlag::FromRawValue(0b00000100);
  ASSERT_TRUE(from_raw.has_value());
  EXPECT_EQ(from_raw, TypeFlag::OwnedString());
}

TEST(FromRawValueConsistencyTest, BorrowedStringConsistency) {
  auto from_raw = TypeFlag::FromRawValue(0b00001100);
  ASSERT_TRUE(from_raw.has_value());
  EXPECT_EQ(from_raw, TypeFlag::BorrowedString());
}

TEST(FromRawValueConsistencyTest, RawConsistency) {
  auto from_raw = TypeFlag::FromRawValue(0b00000101);
  ASSERT_TRUE(from_raw.has_value());
  EXPECT_EQ(from_raw, TypeFlag::Raw());
}

TEST(FromRawValueConsistencyTest, ObjectConsistency) {
  auto from_raw = TypeFlag::FromRawValue(0b00000110);
  ASSERT_TRUE(from_raw.has_value());
  EXPECT_EQ(from_raw, TypeFlag::Object());
}

TEST(FromRawValueConsistencyTest, ArrayConsistency) {
  auto from_raw = TypeFlag::FromRawValue(0b00000111);
  ASSERT_TRUE(from_raw.has_value());
  EXPECT_EQ(from_raw, TypeFlag::Array());
}

}  // namespace
}  // namespace jsoncpp
