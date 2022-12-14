#pragma once

// std
#include <array>
#include <cstddef> // std::size_t
#include <cstdint>
#include <filesystem>
#include <iterator> // std::begin, std::end
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant> // std::monostate
#include <vector>

// external
#include <boost/container_hash/hash.hpp>

//
// Macros
//
// NOLINTNEXTLINE(readability-identifier-naming)
#define Fn auto
// NOLINTNEXTLINE(readability-identifier-naming)
#define unreachable() __builtin_unreachable()

namespace poac {

//
// Namespaces
//
namespace fs = std::filesystem;

//
// Data types
//
using u8 = std::uint8_t; // NOLINT(readability-identifier-naming)
using u16 = std::uint16_t; // NOLINT(readability-identifier-naming)
using u32 = std::uint32_t; // NOLINT(readability-identifier-naming)
using u64 = std::uint64_t; // NOLINT(readability-identifier-naming)

using i8 = std::int8_t; // NOLINT(readability-identifier-naming)
using i16 = std::int16_t; // NOLINT(readability-identifier-naming)
using i32 = std::int32_t; // NOLINT(readability-identifier-naming)
using i64 = std::int64_t; // NOLINT(readability-identifier-naming)

using usize = std::size_t; // NOLINT(readability-identifier-naming)

using f32 = float; // NOLINT(readability-identifier-naming)
using f64 = double; // NOLINT(readability-identifier-naming)

using String = std::string;
using StringRef = std::string_view;
static_assert(String::npos == StringRef::npos, "npos should be the same");

using Path = fs::path;

template <typename T, usize N>
using Arr = std::array<T, N>;
template <typename T>
using Vec = std::vector<T>;

template <typename T>
using Option = std::optional<T>;

struct NoneT : protected std::monostate {
  constexpr Fn operator==(const usize rhs) const->bool {
    return String::npos == rhs;
  }

  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr operator std::nullopt_t() const { return std::nullopt; }

  template <typename T>
  constexpr operator Option<T>() const { // NOLINT(google-explicit-constructor)
    return std::nullopt;
  }
};
inline constexpr NoneT None; // NOLINT(readability-identifier-naming)

template <typename K, typename V>
using Map = std::map<K, V>;
template <typename K, typename V, typename H = boost::hash<K>>
using HashMap = std::unordered_map<K, V, H>;

template <typename K>
using HashSet = std::unordered_set<K, boost::hash<K>>;

//
// String literals
//
using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

inline Fn operator""_path(const char* str, usize /*unused*/)->Path {
  return str;
}

//
// Utilities
//
template <typename T, typename U>
inline void append(Vec<T>& a, const Vec<U>& b) {
  a.insert(a.end(), b.cbegin(), b.cend());
}

template <typename K, typename V, typename H>
inline void append(HashMap<K, V, H>& a, const HashMap<K, V, H>& b) {
  a.insert(b.cbegin(), b.cend());
}

} // namespace poac
