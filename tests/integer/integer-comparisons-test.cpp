#include <aarith/integer.hpp>
#include <catch.hpp>

using namespace aarith;

SCENARIO("Comparing two positive sintegers", "[uinteger][utility]")
{
    GIVEN("Two integer<N> a and b with a < b")
    {
        WHEN("N <= word_width")
        {
            const size_t TestWidth = 16;
            static constexpr uint16_t number_a = 7;
            static constexpr uint16_t number_b = 23;
            const integer<TestWidth> a{number_a};
            const integer<TestWidth> b{number_b};

            THEN("operator< returns true")
            {
                REQUIRE(a < b);
            }
            THEN("operator<= returns true")
            {
                REQUIRE(a <= b);
            }
            THEN("operator> returns false")
            {
                REQUIRE_FALSE(a > b);
            }
            THEN("operator>= returns false")
            {
                REQUIRE_FALSE(a >= b);
            }
            THEN("operator== returns false")
            {
                REQUIRE_FALSE(a == b);
            }
            THEN("operator!= returns true")
            {
                REQUIRE(a != b);
            }
        }
        WHEN("N > word_width")
        {
            const size_t TestWidth = 80;
            integer<TestWidth> const a{7, 0};
            integer<TestWidth> const b{23, 0};

            integer<TestWidth> const c{7, 0};
            integer<TestWidth> const d{0, 23};

            THEN("operator< returns true")
            {
                REQUIRE(a < b);
                REQUIRE_FALSE(c < d);
            }
            THEN("operator<= returns true")
            {
                REQUIRE(a <= b);
                REQUIRE_FALSE(c <= d);
            }
            THEN("operator> returns false")
            {
                REQUIRE_FALSE(a > b);
                REQUIRE(c > d);
            }
            THEN("operator>= returns false")
            {
                REQUIRE_FALSE(a >= b);
                REQUIRE(c >= d);
            }
            THEN("operator== returns false")
            {
                REQUIRE_FALSE(a == b);
                REQUIRE_FALSE(c == d);
            }
            THEN("operator!= returns true")
            {
                REQUIRE(a != b);
                REQUIRE(c != d);
            }
        }
    }
}
SCENARIO("Comparing two positive sintegers with different bit widths", "[uinteger][utility]")
{
    GIVEN("Two integer<N> a and b with a < b with different bit widths")
    {
        WHEN("N <= word_width")
        {
            const size_t TestWidth = 16;
            static constexpr uint16_t number_a = 7;
            static constexpr uint16_t number_b = 23;
            const integer<TestWidth> a{number_a};
            const integer<TestWidth + 10> b{number_b};

            THEN("operator< returns true")
            {
                REQUIRE(a < b);
            }
            THEN("operator<= returns true")
            {
                REQUIRE(a <= b);
            }
            THEN("operator> returns false")
            {
                REQUIRE_FALSE(a > b);
            }
            THEN("operator>= returns false")
            {
                REQUIRE_FALSE(a >= b);
            }
            THEN("operator== returns false")
            {
                REQUIRE_FALSE(a == b);
            }
            THEN("operator!= returns true")
            {
                REQUIRE(a != b);
            }
        }
    }
    GIVEN("Two integer<N> a and b with a < b with different bit widths")
    {

        const constexpr size_t small = 32;
        const constexpr size_t big = 150;
        static constexpr uint16_t number_a = 7;
        static constexpr uint16_t number_b = 23;
        const integer<small> a{number_a};
        const integer<small> a_neg{-number_a};
        const integer<big> b = integer<big>::from_words(number_b, 0U, 0U);

        const integer<big> c_neg{-number_b};

        THEN("operator< returns true")
        {
            REQUIRE(a_neg < b);
            REQUIRE(a < b);
        }
        THEN("operator<= returns true")
        {
            REQUIRE(a_neg <= b);
            REQUIRE(a <= b);
        }
        THEN("operator> returns false")
        {
            REQUIRE_FALSE(a_neg > b);
            REQUIRE_FALSE(a > b);
        }
        THEN("operator>= returns false")
        {
            REQUIRE_FALSE(a_neg >= b);
            REQUIRE_FALSE(a >= b);
        }
        THEN("operator== returns false")
        {
            REQUIRE_FALSE(a_neg == b);
            REQUIRE_FALSE(a == b);
        }
        THEN("operator!= returns true")
        {
            REQUIRE(a_neg != b);
            REQUIRE(a != b);
        }
    }
}

SCENARIO("Investigating the comparison of max and min values",
         "[integer][comparison][utility]")
{
    GIVEN("integer<8>::max/min")
    {
        using T = integer<8>;
        T min = T::min();
        T max = T::max();

        T min_from_limits = std::numeric_limits<T>::min();
        T max_from_limits = std::numeric_limits<T>::max();

        WHEN("Comparing the integer::max/min and numeric_limits::max/min")
        {
            THEN("The values should be identical")
            {
                REQUIRE(min == min_from_limits);
                REQUIRE(max == max_from_limits);

                REQUIRE(!(min != min_from_limits));
                REQUIRE(!(max != max_from_limits));
            }
        }

        WHEN("Constructing min and max value into a larger integer")
        {

            std::cout << to_binary(T::min()) << "\n";

            integer<9> min = T::min();
            integer<9> max = T::max();
            THEN("min should be negative")
            {
                REQUIRE(min.is_negative());
            }
            AND_THEN("the values should match the values from the integers with the smaller width")
            {
                CHECK(min == T::min());
                REQUIRE(max == T::max());
            }
            AND_THEN("min should be smaller than max")
            {
                REQUIRE(min < max);
            }
        }

        WHEN("Comparing these values")
        {
            THEN("The result should make sense")
            {
                REQUIRE(min < max);
                REQUIRE(min_from_limits < max_from_limits);

                REQUIRE(min <= max);
                REQUIRE(min_from_limits <= max_from_limits);

                REQUIRE(!(min > max));
                REQUIRE(!(min_from_limits > max_from_limits));

                REQUIRE(!(min >= max));
                REQUIRE(!(min_from_limits >= max_from_limits));

                REQUIRE(min != max);
                REQUIRE(min_from_limits != max_from_limits);

                REQUIRE(!(min == max));
                REQUIRE(!(min_from_limits == max_from_limits));
            }
        }
    }

    GIVEN("integer<16>::max/min")
    {
        using T = integer<16>;
        T min = T::min();
        T max = T::max();

        T min_from_limits = std::numeric_limits<T>::min();
        T max_from_limits = std::numeric_limits<T>::max();

        WHEN("Comparing the integer::max/min and numeric_limits::max/min")
        {
            THEN("The values should be identical")
            {
                REQUIRE(min == min_from_limits);
                REQUIRE(max == max_from_limits);

                REQUIRE(!(min != min_from_limits));
                REQUIRE(!(max != max_from_limits));
            }
        }

        WHEN("Comparing these values")
        {
            THEN("The result should make sense")
            {
                REQUIRE(min < max);
                REQUIRE(min_from_limits < max_from_limits);

                REQUIRE(min <= max);
                REQUIRE(min_from_limits <= max_from_limits);

                REQUIRE(!(min > max));
                REQUIRE(!(min_from_limits > max_from_limits));

                REQUIRE(!(min >= max));
                REQUIRE(!(min_from_limits >= max_from_limits));

                REQUIRE(min != max);
                REQUIRE(min_from_limits != max_from_limits);

                REQUIRE(!(min == max));
                REQUIRE(!(min_from_limits == max_from_limits));
            }
        }
    }
}
