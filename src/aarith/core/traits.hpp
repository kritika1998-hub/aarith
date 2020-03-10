#pragma once

#include <limits>
#include <type_traits>

namespace aarith {

template <class Type> class is_word_array
{
public:
    static constexpr bool value = false;
};

template <class Type> inline constexpr bool is_word_array_v = is_word_array<Type>::value;

template <class Type> class is_integral
{
public:
    static constexpr bool value = false;
};

template <class Type> inline constexpr bool is_integral_v = is_integral<Type>::value;

template <class Type> class is_unsigned
{
public:
    static constexpr bool value = false;
};

template <class Type> inline constexpr bool is_unsigned_v = is_unsigned<Type>::value;

template <class Type> class is_float
{
public:
    static constexpr bool value = false;
};

template <class Type> inline constexpr bool is_float_v = is_float<Type>::value;

template <typename A, typename B>
inline constexpr bool same_word_type = std::is_same_v<typename  A::word_type, typename B::word_type>;

template <typename A, typename B>
inline constexpr bool same_sign = (is_unsigned_v<A> == is_unsigned_v<B>);

} // namespace aarith
