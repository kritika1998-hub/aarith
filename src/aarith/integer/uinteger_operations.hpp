#pragma once

#include <aarith/core/traits.hpp>
#include <aarith/core/word_array_operations.hpp>
#include <aarith/integer/uinteger.hpp>
#include <aarith/integer/uinteger_comparisons.hpp>

#include <iostream>

namespace aarith {



/**
 * @brief Adds two unsigned integers of, possibly, different bit widths.
 *
 * @tparam W Width of the first summand
 * @tparam V Width of the second summand
 * @param a First summand
 * @param b Second summand
 * @param initial_carry True if there is an initial carry coming in
 * @return Sum of correct maximal bit width
 */
template <size_t W, size_t V>
[[nodiscard]] uinteger<std::max(W, V) + 1> expanding_add(const uinteger<W>& a, const uinteger<V>& b,
                                                         const bool initial_carry = false)
{
    static_assert(is_integral_v<uinteger<W>>);
    static_assert(is_unsigned_v<uinteger<W>>);
    static_assert(is_integral_v<uinteger<V>>);
    static_assert(is_unsigned_v<uinteger<V>>);

    constexpr size_t res_width = std::max(W, V) + 1U;

    uinteger<res_width> sum;
    using word_type = typename uinteger<res_width>::word_type;
    uinteger<res_width> a_ = width_cast<res_width>(a);
    uinteger<res_width> b_ = width_cast<res_width>(b);

    word_type carry = initial_carry ? 1U : 0U;

    for (auto i = 0U; i < sum.word_count(); ++i)
    {

        word_type word_a{a_.word(i)};
        word_type word_b{b_.word(i)};

        auto partial_sum = word_a + word_b;
        word_type new_carry = (partial_sum < word_a || partial_sum < word_b) ? 1U : 0U;
        partial_sum += carry;
        carry = (new_carry || partial_sum < word_a || partial_sum < word_b) ? 1U : 0U;
        sum.set_word(i, partial_sum);
    }
    return sum;
}

/**
 * @brief Subtracts two unsigned integers of, possibly, different bit widths.
 *
 * Expanding does not, in contrast to @see expanding_add, ensure that no underflow will happen. It
 * simply makes sure that the resulting bit width is the larger of the both input bit widths.
 *
 * @tparam W Width of the minuend
 * @tparam V Width of the subtrahend
 * @param a Minuend
 * @param b Subtrahend
 * @return Difference of correct bit width
 */
template <size_t W, size_t V>
[[nodiscard]] uinteger<std::max(W, V)> expanding_sub(const uinteger<W>& a, const uinteger<V>& b)
{
    constexpr size_t res_width = std::max(W, V);
    uinteger<res_width> result{sub(width_cast<res_width>(a), width_cast<res_width>(b))};

    return result;
}

/**
 * @brief Adds two unsigned integers
 *
 * @tparam UInteger The unsigned integer instance used for the addition
 * @param a First summand
 * @param b Second summand
 * @return Sum of a and b
 */
template <size_t W>[[nodiscard]] uinteger<W> add(const uinteger<W>& a, const uinteger<W>& b)
{
    uinteger<W + 1> result = expanding_add<W, W>(a, b);
    return width_cast<W>(result);
}

/**
 * @brief Computes the difference of two unsigned integers.
 *
 * @tparam W the bit width of the operands
 * @param a Minuend
 * @param b Subtrahend
 * @return Difference between a and b
 */
template <size_t W>[[nodiscard]] auto sub(const uinteger<W>& a, const uinteger<W>& b) -> uinteger<W>
{
    static_assert(is_integral_v<uinteger<W>>);
    static_assert(is_unsigned_v<uinteger<W>>);

    auto result = expanding_add(a, ~b, true);
    return width_cast<W>(result);
}

/**
 * @brief Multiplies two unsigned integers expanding the bit width so that the result fits.
 *
 *
 * @tparam W The bit width of the first multiplicant
 * @tparam V The bit width of the second multiplicant
 * @param a First multiplicant
 * @param b Second multiplicant
 * @return Product of a and b
 */
template <std::size_t W, std::size_t V>
[[nodiscard]] uinteger<W + V> expanding_mul(const uinteger<W>& a, const uinteger<V>& b)
{
    constexpr std::size_t res_width = W + V;
    uinteger<res_width> result{0U};
    if constexpr (res_width <= 64)
    {
        uint64_t result_uint64 = a.word(0) * b.word(0);
        result.set_word(0, result_uint64);
    }
    else
    {
        static_assert(is_integral_v<uinteger<res_width>>);
        static_assert(is_unsigned_v<uinteger<res_width>>);

        const auto leading_zeroes = V - count_leading_zeroes(b);
        uinteger<res_width> a_ = width_cast<res_width>(a);

        auto bit_index = 0U;
        while (bit_index < leading_zeroes)
        {
            if (b.bit(bit_index))
            {
                result = add(result, a_ << bit_index);
            }
            ++bit_index;
        }
    }
    return result;
}

/**
 * @brief Multiplies two unsigned integers.
 *
 * @note No Type conversion is performed. If the bit widths do not match, the code will not compile!
 *
 * This implements the simplest multiplication algorithm (binary "long multiplication") that adds up
 * the partial products everywhere where the first multiplicand has a 1 bit. The simplicity, of
 * course, comes at the cost of performance.
 *
 * @tparam W The bit width of the multiplicants
 * @param a First multiplicant
 * @param b Second multiplicant
 * @return Product of a and b
 */
template <size_t W>[[nodiscard]] uinteger<W> mul(const uinteger<W>& a, const uinteger<W>& b)
{
    return width_cast<W>(expanding_mul(a, b));
}

/**
 * @brief Implements the restoring division algorithm.
 *
 * @see https://en.wikipedia.org/wiki/Division_algorithm#Restoring_division
 *
 * @param numerator The number that is to be divided
 * @param denominator The number that divides the other number
 * @tparam W Width of the numbers used in division.
 *
 * @return Pair of (quotient, remainder)
 *
 */
template <std::size_t W, std::size_t V>
[[nodiscard]] std::pair<uinteger<W>, uinteger<W>> restoring_division(const uinteger<W>& numerator,
                                                                     const uinteger<V>& denominator)

{
    using UInteger = uinteger<W>;
    using LargeUInteger = uinteger<2 * W>;

    if (denominator.is_zero())
    {
        throw std::runtime_error("Attempted division by zero");
    }

    // Cover some special cases in order to speed everything up
    if (numerator == denominator)
    {
        return std::make_pair(UInteger::one(), UInteger::zero());
    }
    if (numerator.is_zero())
    {
        return std::make_pair(UInteger::zero(), UInteger::zero());
    }
    if (numerator < denominator)
    {
        return std::make_pair(UInteger::zero(), numerator);
    }
    if (denominator == UInteger::one())
    {
        return std::make_pair(numerator, UInteger::zero());
    }

    // Perform restoring division in all other cases
    const auto n = numerator.width();
    const LargeUInteger D = (width_cast<2 * W>(denominator) << n);
    LargeUInteger R = width_cast<2 * W>(numerator);
    UInteger Q = UInteger::zero();

    for (size_t i = 0; i < n; ++i)
    {
        const auto bit = (n - 1) - i;
        const LargeUInteger TwoR = (R << 1);
        if (TwoR >= D)
        {
            R = sub(TwoR, D);
            Q.set_bit(bit, true);
        }
        else
        {
            R = TwoR;
            Q.set_bit(bit, false);
        }
    }

    const uinteger<W> remainder = width_cast<W>(R >> n);

    return std::make_pair(Q, remainder);
}

template <class UInteger>
[[nodiscard]] auto remainder(const UInteger& numerator, const UInteger& denominator) -> UInteger
{
    return restoring_division(numerator, denominator).second;
}

template <class UInteger>
[[nodiscard]] auto div(const UInteger& numerator, const UInteger& denominator) -> UInteger
{
    return restoring_division(numerator, denominator).first;
}

} // namespace aarith

namespace aarith::exact_operators {

template <size_t Width>
auto operator+(const uinteger<Width>& lhs, const uinteger<Width>& rhs) -> uinteger<Width>
{
    return add(lhs, rhs);
}

template <size_t Width>
auto operator-(const uinteger<Width>& lhs, const uinteger<Width>& rhs) -> uinteger<Width>
{
    return sub(lhs, rhs);
}

template <size_t Width>
auto operator*(const uinteger<Width>& lhs, const uinteger<Width>& rhs) -> uinteger<Width>
{
    return mul(lhs, rhs);
}

template <size_t Width>
auto operator/(const uinteger<Width>& lhs, const uinteger<Width>& rhs) -> uinteger<Width>
{
    return div(lhs, rhs);
}

template <size_t Width>
auto operator%(const uinteger<Width>& lhs, const uinteger<Width>& rhs) -> uinteger<Width>
{
    return remainder(lhs, rhs);
}

} // namespace aarith::exact_operators
