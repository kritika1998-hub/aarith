#include "../test-signature-ranges.hpp"
#include <aarith/float.hpp>
#include <aarith/float/approx_operations.hpp>
#include <catch.hpp>

using namespace aarith;

TEMPLATE_TEST_CASE_SIG("Anytime addition", "[normalized_float][arithmetic][constexpr]",
                       AARITH_FLOAT_TEST_SIGNATURE_WITH_NATIVE_TYPE,
                       AARIHT_FLOAT_TEMPLATE_NATIVE_RANGE_WITH_TYPE)
{
    using F = normalized_float<E, M>;

    GIVEN("Float/Double numbers")
    {

        Native a_ = GENERATE(take(15, random<Native>(std::numeric_limits<Native>::min(),
                                                     std::numeric_limits<Native>::max())));
        Native b_ = GENERATE(take(15, random<Native>(std::numeric_limits<Native>::min(),
                                                     std::numeric_limits<Native>::max())));

        F a{a_};
        F b{b_};

        WHEN("Computing all mantissa bits correctly for the addition")
        {
            F res = anytime_add(a, b, M +1);
            THEN("The result should match the usual normalized_float result")
            {
                F res_exact = add(a, b);
                if (res != res_exact)
                {
                    std::cout << to_binary(res) << "\n";
                    std::cout << to_binary(res_exact) << "\n";
                }
                REQUIRE(res == res_exact);
            }
        }

        WHEN("Computing all mantissa bits correctly for the subtraction")
        {
            F res = anytime_sub(a, b, M +1);
            THEN("The result should match the usual normalized_float result")
            {
                F res_exact = sub(a, b);
                if (res != res_exact)
                {
                    std::cout << to_binary(res) << "\n";
                    std::cout << to_binary(res_exact) << "\n";
                }
                REQUIRE(res == res_exact);
            }
        }

        WHEN("Computing all mantissa bits correctly for the multiplication")
        {
            F res = anytime_mul(a, b, M +1);
            THEN("The result should match the usual normalized_float result")
            {
                F res_exact = mul(a, b);
                if (res != res_exact)
                {
                    std::cout << to_binary(res) << "\n";
                    std::cout << to_binary(res_exact) << "\n";
                }
                REQUIRE(res == res_exact);
            }
        }

        WHEN("Computing all mantissa bits correctly for the division")
        {
            F res = anytime_div(a, b, M +1);
            THEN("The result should match the usual normalized_float result")
            {
                F res_exact = div(a, b);
                if (res != res_exact)
                {
                    std::cout << to_binary(res) << "\n";
                    std::cout << to_binary(res_exact) << "\n";
                }
                REQUIRE(res == res_exact);
            }
        }
    }
}