#include <aarith/float.hpp>

#include "gen_float.hpp"
#include "../test-signature-ranges.hpp"

#include <bitset>
#include <catch.hpp>


using namespace aarith;


TEMPLATE_TEST_CASE_SIG("Addition is commutative",
                       "[normalized_float][arithmetic][addition][invariant]",
                       AARITH_FLOAT_TEST_SIGNATURE, AARIHT_FLOAT_TEMPLATE_RANGE)
{
    using F = normalized_float<E, M>;

    GIVEN("Tow normalized_floats created from native data types")
    {

        F a = GENERATE(take(15, random_float<E, M, FloatGenerationModes::FullyRandom>()));
        F b = GENERATE(take(15, random_float<E, M, FloatGenerationModes::FullyRandom>()));


        WHEN("Adding these numbers")
        {
            F res1{a + b};
            F res2{b + a};
            THEN("The result should not depend on operand order")
            {
                REQUIRE(res1 == res2);
            }
        }
    }
}



TEMPLATE_TEST_CASE_SIG("Zero is the neutral element of the addition",
                       "[normalized_float][arithmetic][addition][invariant]",
                       AARITH_FLOAT_TEST_SIGNATURE, AARIHT_FLOAT_TEMPLATE_RANGE)
{
    using F = normalized_float<E, M>;

    GIVEN("A normalized_float  created from native data types and the number zero")
    {

        F a = GENERATE(take(15, random_float<E, M, FloatGenerationModes::FullyRandom>()));

        WHEN("Adding these numbers")
        {
            F res{a + F::zero()};
            THEN("The result should have left the number untouched")
            {
                REQUIRE(res == a);
            }
        }
    }
}




TEMPLATE_TEST_CASE_SIG("Floating point addition matches its native counterparts",
                       "[normalized_float][arithmetic][addition]",
                       AARITH_FLOAT_TEST_SIGNATURE_WITH_NATIVE_TYPE, AARIHT_FLOAT_TEMPLATE_NATIVE_RANGE_WITH_TYPE)
{

    using F = normalized_float<E, M>;

    F a = GENERATE(take(15, random_float<E, M, FloatGenerationModes::FullyRandom>()));
    F b = GENERATE(take(15, random_float<E, M, FloatGenerationModes::FullyRandom>()));

    Native a_native = static_cast<Native>(a);
    Native b_native = static_cast<Native>(b);

    F res = a + b;
    Native res_native = a_native + b_native;

    F res_native_{res_native};
    Native res_ = static_cast<Native>(res);

    if (!equal_except_rounding(res_native_, res))
    {
        F res__ = a + b;
        std::cout << a << " + " << b << " = " << res__ << "\n";
        std::cout << to_binary(a) << " + \n" << to_binary(b) << " = \n" << to_binary(res) << "\n";
        // std::cout << to_compute_string(a) << "\t+\t" << to_compute_string(b) << " = "
        //          << to_compute_string(res) << "\n";

        std::cout << a_native << " + " << b_native << " = " << res_native << "\n";
        std::cout << to_binary(res) << "\n" << to_binary(res_native_) << "\n";
    }

    CHECK(equal_except_rounding(res_native_, res));
    REQUIRE(equal_except_rounding(F{res_}, F{res_native}));
}


TEMPLATE_TEST_CASE_SIG("Adding to infinity", "[normalized_float][arithmetic][addition]",
                       AARITH_FLOAT_TEST_SIGNATURE, AARIHT_FLOAT_TEMPLATE_RANGE)
{
    using F = normalized_float<E, M>;
    constexpr F neg_inf{F::neg_infinity()};
    constexpr F pos_inf{F::pos_infinity()};

    GIVEN("Infinity")
    {
        WHEN("Adding infinity to infinity")
        {
            F res = pos_inf + pos_inf;
            THEN("The result should still be infinity")
            {
                CHECK(res == pos_inf);
                CHECK(res.is_pos_inf());
                REQUIRE(res.is_inf());
            }
        }
    }
    AND_GIVEN("A random floating point number")
    {

        F f = GENERATE(take(100, random_float<E, M, FloatGenerationModes::NonSpecial>()));

        WHEN("Adding that number to infinity")
        {
            F res = pos_inf + f;
            F res_neg = neg_inf + f;
            THEN("The value should still be infinity")
            {
                CHECK(res_neg == neg_inf);
                CHECK(res_neg.is_inf());
                CHECK(res_neg.is_neg_inf());
                CHECK(res == pos_inf);
                CHECK(res.is_pos_inf());
                REQUIRE(res.is_inf());
            }
        }
    }
}


SCENARIO("Adding two floating-point numbers", "[normalized_float][arithmetic][addition]")
{
    GIVEN("Single precision floats (E = 8, M = 23)")
    {
        static constexpr size_t E = 8;
        static constexpr size_t M = 23;

        WHEN("The second operand is -a.")
        {
            static constexpr float number_a = 93.211546F;
            static constexpr float number_b = -number_a;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("The result should be 0")
            {
                REQUIRE(result == normalized_float<E, M>(0.f));
            }
        }
        WHEN("The second operand is 0.")
        {
            static constexpr float number_a = 93.211546F;
            static constexpr float number_b = 0;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("The result should be a")
            {
                REQUIRE(result == a);
            }
        }
        WHEN("The first operand is negative, and the second operand is 0.")
        {
            static constexpr float number_a = -93.211546F;
            static constexpr float number_b = 0;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("The result should be a")
            {
                REQUIRE(result == a);
            }
        }
        WHEN("The first operand is 0.")
        {
            static constexpr float number_a = 0;
            static constexpr float number_b = 93.211546F;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("The result should be b")
            {
                REQUIRE(result == b);
            }
        }
        WHEN("The first operand is 0, and the second operand is negative.")
        {
            static constexpr float number_a = 0;
            static constexpr float number_b = -93.211546F;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("The result should be b")
            {
                REQUIRE(result == b);
            }
        }
        WHEN("The absolute of the first operand is higher than the absolute of the first operand, "
             "both operands are positive and the result is smaller infinity.")
        {
            static constexpr float number_a = 213.22154F;
            static constexpr float number_b = 93.211546F;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("It should be the correct sum.")
            {
                REQUIRE(equal_except_rounding(result, normalized_float<E, M>(number_a + number_b)));
            }
        }
        WHEN("The absolute of the first operand is higher than the absolute of the first operand, "
             "operand 1 is negative, and the result is not infinity.")
        {
            static constexpr float number_a = -213.22154F;
            static constexpr float number_b = 93.211546F;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("It should be the correct sum.")
            {
                REQUIRE(equal_except_rounding(result, normalized_float<E, M>(number_a + number_b)));
            }
        }
        WHEN("The absolute of the first operand is higher than the absolute of the first operand, "
             "operand 2 is negative, and the result is not infinity.")
        {
            static constexpr float number_a = 213.22154F;
            static constexpr float number_b = -93.211546F;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("It should be the correct sum.")
            {
                REQUIRE(equal_except_rounding(result, normalized_float<E, M>(number_a + number_b)));
            }
        }
        WHEN("The absolute of the first operand is higher than the absolute of the first operand, "
             "both operands are negative, and the result is not infinity.")
        {
            static constexpr float number_a = -213.22154F;
            static constexpr float number_b = -93.211546F;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("It should be the correct sum.")
            {
                REQUIRE(equal_except_rounding(result, normalized_float<E, M>(number_a + number_b)));
            }
        }
        WHEN("The absolute of the second operand is higher than the absolute of the first operand, "
             "both operands are positive and the result is smaller infinity.")
        {
            static constexpr float number_a = 93.211546F;
            static constexpr float number_b = 213.22154F;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("It should be the correct sum.")
            {
                REQUIRE(equal_except_rounding(result, normalized_float<E, M>(number_a + number_b)));
            }
        }
        WHEN("The absolute of the second operand is higher than the absolute of the first operand, "
             "operand 1 is negative, and the result is not infinity.")
        {
            static constexpr float number_a = -93.211546F;
            static constexpr float number_b = 213.22154F;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("It should be the correct sum.")
            {
                REQUIRE(equal_except_rounding(result, normalized_float<E, M>(number_a + number_b)));
            }
        }
        WHEN("The absolute of the second operand is higher than the absolute of the first operand, "
             "operand 2 is negative, and the result is not infinity.")
        {
            static constexpr float number_a = 93.211546F;
            static constexpr float number_b = -213.22154F;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("It should be the correct sum.")
            {
                REQUIRE(equal_except_rounding(result, normalized_float<E, M>(number_a + number_b)));
            }
        }
        WHEN("The absolute of the second operand is higher than the absolute of the first operand, "
             "both operands are negative, and the result is not infinity.")
        {
            static constexpr float number_a = -93.211546F;
            static constexpr float number_b = -213.22154F;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("It should be the correct sum.")
            {
                REQUIRE(equal_except_rounding(result, normalized_float<E, M>(number_a + number_b)));
            }
        }
        WHEN("When one operand is a > 1 and the other is 0 < b < 1.")
        {
            static constexpr float number_a = 2.75F;
            static constexpr float number_b = 0.5F;
            const normalized_float<E, M> a{number_a};
            const normalized_float<E, M> b{number_b};
            const normalized_float<E, M> result = add(a, b);

            THEN("It should be the correct sum.")
            {
                REQUIRE(equal_except_rounding(result, normalized_float<E, M>(number_a + number_b)));
            }
        }
    }
}


SCENARIO("IEEE-754 denormalized number computations: float, double",
         "[normalized_float][denormalized][ieee-754][computation]")
{
    GIVEN("Two denormalized float numbers")
    {
        using nfloat = normalized_float<8, 23>;
        WHEN("The result of the addition is still denormalized > 0")
        {
            unsigned int a_i = 0b00000000000000000000000000000010;
            unsigned int b_i = 0b00000000000000000000000000000001;
            float* a_if = reinterpret_cast<float*>(&a_i);
            float* b_if = reinterpret_cast<float*>(&b_i);
            float a = *a_if;
            float b = *b_if;

            float res_f = a + b;
            const auto nfa = nfloat(a);
            const auto nfb = nfloat(b);
            auto res = nfa + nfb;

            THEN("The results should be the same as the float computation.")
            {
                REQUIRE(static_cast<float>(res) == res_f);
            }
        }
        WHEN("The result of the addition is normalized")
        {
            unsigned int a_i = 0b00000000011111111111111111111111;
            unsigned int b_i = 0b00000000011111111111111111111111;
            float* a_if = reinterpret_cast<float*>(&a_i);
            float* b_if = reinterpret_cast<float*>(&b_i);
            float a = *a_if;
            float b = *b_if;
            auto const a_nf = nfloat(a);
            auto const b_nf = nfloat(b);

            float res_f = a + b;
            auto res = a_nf + b_nf;

            THEN("The results should be the same as the float computation.")
            {
                REQUIRE(static_cast<float>(res) == res_f);
            }
        }
    }
}
