#pragma once

#include <cstdint>
#include <optional>

namespace jsoncpp {

/// \brief Type kind of JSON value.
enum class TypeKind : uint8_t {
  kNull = 0b000,
  kBool = 0b010,
  kNumber = 0b011,
  kString = 0b100,
  kRaw = 0b101,
  kObject = 0b110,
  kArray = 0b111,
};

/// \brief Subtype of JSON value. The subtype is used to distinguish different
///        types of values that share the same basic type. For example, `true`
///        and `false` are both of basic type `kBool`, but they have different
///        subtypes.
///
/// typo: kTrue = 0b01 not 0x11
enum class BoolSubType : uint8_t { kFalse = 0b00, kTrue = 0b01 };

// typo: kReal = 0b10 not 0x10
enum class NumberSubType : uint8_t { kUInt = 0b00, kSInt = 0b01, kReal = 0b10 };

enum class StringSubType : uint8_t {
  /// The string is owned by the JSON value and MAYBE freed when the JSON value
  /// is destroyed.
  kStringOwned = 0b00,

  /// The string is borrowed by the JSON value and MUST NOT be freed when
  /// the JSON value is destroyed. The caller MUST ensure that the string
  /// remains valid for the lifetime of the JSON value.
  kStringBorrowed = 0b01
};

/// \brief Type flag of JSON value. The type flag is a compact representation of
///        type kind and subtype of a JSON value.
///
/// The type flag is represented as a single byte and is organized as follows:
///
///                  Bit Layout
/// ┌───────────────┬───────────┬───────────────────┐
/// │   Reserved    │  SubType  │       Kind        │
/// ├───────────────┼───────────┼───────────────────┤
/// │   7   6   5   │   4   3   │    2   1   0      │
/// └───────────────┴───────────┴───────────────────┘
class TypeFlag {
  static constexpr uint8_t kBasicTypeBits = 3;
  static constexpr uint8_t kBasicTypeMask = 0x07;
  static constexpr uint8_t kSubTypeMask = 0x03;
  static constexpr uint8_t kReservedMask = 0xE0;

 public:
  static constexpr bool IsValidRawValue(uint8_t value) noexcept {
    if ((value & kReservedMask) != 0) return false;

    TypeKind kind = static_cast<TypeKind>(value & kBasicTypeMask);
    uint8_t subtype = (value >> kBasicTypeBits) & kSubTypeMask;

    switch (kind) {
      case TypeKind::kRaw:
      case TypeKind::kNull:
      case TypeKind::kArray:
      case TypeKind::kObject:
        return subtype == 0;

      case TypeKind::kBool:
        return subtype == static_cast<uint8_t>(BoolSubType::kFalse) ||
               subtype == static_cast<uint8_t>(BoolSubType::kTrue);

      case TypeKind::kNumber:
        return subtype == static_cast<uint8_t>(NumberSubType::kUInt) ||
               subtype == static_cast<uint8_t>(NumberSubType::kSInt) ||
               subtype == static_cast<uint8_t>(NumberSubType::kReal);

      case TypeKind::kString:
        return subtype == static_cast<uint8_t>(StringSubType::kStringOwned) ||
               subtype == static_cast<uint8_t>(StringSubType::kStringBorrowed);

      default:
        break;
    }

    return false;
  }

  /// \brief Create a type flag from a raw value. The raw value is a single byte
  ///        that encodes the type kind and subtype of a JSON value. The caller
  ///        MUST ensure that the raw value is valid, i.e., it must satisfy the
  ///        following conditions:
  ///
  /// - The reserved bits (bits 5-7) must be zero.
  /// - The subtype bits (bits 3-4) must be valid for the corresponding basic
  ///   type (bits 0-2).
  ///
  /// \param value The raw value to create the type flag from.
  /// \return      A type flag created from the raw value, or `std::nullopt` if
  ///              the raw value is invalid.
  constexpr static std::optional<TypeFlag> FromRawValue(
      uint8_t value) noexcept {
    if (!IsValidRawValue(value)) return std::nullopt;

    return TypeFlag(value);
  }

  /// \brief Create a type flag for null value.
  ///
  /// \return A type flag representing a null value.
  constexpr static TypeFlag Null() noexcept {
    return TypeFlag(TypeKind::kNull);
  }

  /// \brief Create a type flag for array value.
  ///
  /// \return A type flag representing an array value.
  constexpr static TypeFlag Array() noexcept {
    return TypeFlag(TypeKind::kArray);
  }

  constexpr static TypeFlag Raw() noexcept { return TypeFlag(TypeKind::kRaw); }

  /// \brief Create a type flag for object value.
  ///
  /// \return A type flag representing an object value.
  constexpr static TypeFlag Object() noexcept {
    return TypeFlag(TypeKind::kObject);
  }

  /// \brief Create a type flag for true boolean value.
  ///
  /// \return A type flag representing a true boolean value.
  constexpr static TypeFlag True() noexcept {
    // TODO: typo, WithSubtype
    return TypeFlag(TypeKind::kBool).WithSubType(BoolSubType::kTrue);
  }

  /// \brief Create a type flag for false boolean value.
  ///
  /// \return A type flag representing a false boolean value.
  constexpr static TypeFlag False() noexcept {
    // TODO: typo, WithSubtype
    return TypeFlag(TypeKind::kBool).WithSubType(BoolSubType::kFalse);
  }

  /// \brief Create a type flag for signed integer value.
  ///
  /// \return A type flag representing a signed integer value.
  constexpr static TypeFlag SInt() noexcept {
    return TypeFlag(TypeKind::kNumber).WithSubType(NumberSubType::kSInt);
  }

  /// \brief Create a type flag for unsigned integer value.
  ///
  /// \return A type flag representing an unsigned integer value.
  constexpr static TypeFlag UInt() noexcept {
    return TypeFlag(TypeKind::kNumber).WithSubType(NumberSubType::kUInt);
  }

  /// \brief Create a type flag for real number value.
  ///
  /// \return A type flag representing a real number value.
  constexpr static TypeFlag Real() noexcept {
    return TypeFlag(TypeKind::kNumber).WithSubType(NumberSubType::kReal);
  }

  /// \brief Create a type flag for owned string value.
  ///
  /// \return A type flag representing an owned string value.
  constexpr static TypeFlag OwnedString() noexcept {
    return TypeFlag(TypeKind::kString).WithSubType(StringSubType::kStringOwned);
  }

  /// \brief Create a type flag for borrowed string value.
  ///
  /// \return A type flag representing a borrowed string value.
  constexpr static TypeFlag BorrowedString() noexcept {
    return TypeFlag(TypeKind::kString)
        .WithSubType(StringSubType::kStringBorrowed);
  }

  constexpr bool IsNull() const noexcept {
    return GetKind() == TypeKind::kNull;
  }

  constexpr bool IsBool() const noexcept {
    return GetKind() == TypeKind::kBool;
  }

  constexpr bool IsNumber() const noexcept {
    return GetKind() == TypeKind::kNumber;
  }

  constexpr bool IsString() const noexcept {
    return GetKind() == TypeKind::kString;
  }

  constexpr bool IsArray() const noexcept {
    return GetKind() == TypeKind::kArray;
  }

  constexpr bool IsObject() const noexcept {
    return GetKind() == TypeKind::kObject;
  }

  constexpr bool IsRaw() const noexcept { return GetKind() == TypeKind::kRaw; }

  constexpr bool IsFalse() const noexcept { return value_ == False().value_; }
  constexpr bool IsTrue() const noexcept { return value_ == True().value_; }
  constexpr bool IsSInt() const noexcept { return value_ == SInt().value_; }
  constexpr bool IsUInt() const noexcept { return value_ == UInt().value_; }
  constexpr bool IsReal() const noexcept { return value_ == Real().value_; }
  constexpr bool IsOwnedString() const noexcept {
    return value_ == OwnedString().value_;
  }

  constexpr bool IsBorrowedString() const noexcept {
    return value_ == BorrowedString().value_;
  }

  bool operator==(const TypeFlag&) const noexcept = default;

 private:
  constexpr explicit TypeFlag(uint8_t value) noexcept : value_(value) {}
  constexpr explicit TypeFlag(TypeKind kind) noexcept
      : value_(static_cast<uint8_t>(kind)) {}

  constexpr TypeKind GetKind() const noexcept {
    return static_cast<TypeKind>(value_ & kBasicTypeMask);
  }

  constexpr TypeFlag WithSubType(uint8_t subtype) const noexcept {
    // subtype << 3
    return TypeFlag(static_cast<uint8_t>(value_ | (subtype << kBasicTypeBits)));
  }

  constexpr TypeFlag WithSubType(BoolSubType subtype) const noexcept {
    return WithSubType(static_cast<uint8_t>(subtype));
  }

  constexpr TypeFlag WithSubType(NumberSubType subtype) const noexcept {
    return WithSubType(static_cast<uint8_t>(subtype));
  }

  constexpr TypeFlag WithSubType(StringSubType subtype) const noexcept {
    return WithSubType(static_cast<uint8_t>(subtype));
  }

  uint8_t value_;
};

}  // namespace jsoncpp