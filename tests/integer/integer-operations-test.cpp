#include <aarith/integer.hpp>
#include <catch.hpp>

using namespace aarith;

SCENARIO("Arithmetic should be constexpr", "[integer][arithmetic]")
{
    GIVEN("Two integers")
    {
        constexpr integer<32> a{4};
        constexpr integer<32> b{8};

        WHEN("Performing addition")
        {

            THEN("The operation is constexpr")
            {
                constexpr integer<32> expected{12};

                const integer<33> result_expanded = expanding_add(a, b);
                const integer<32> result = add(a, b);

                REQUIRE(result_expanded == expected);
                REQUIRE(result == expected);
            }
        }
        WHEN("Performing subtraction")
        {
            THEN("The operation is constexpr")
            {
                constexpr integer<32> expected{4};

                const integer<32> result = sub(b, a);

                REQUIRE(result == expected);
            }
        }
    }
}

SCENARIO("Adding two positive integers", "[integer][arithmetic][addition]")
{
    GIVEN("Two positive integer<N> a and b with N <= word_width")
    {
        static constexpr size_t TestWidth = 16;
        static_assert(integer<TestWidth>::word_count() == 1);

        WHEN("The result a+b still fits into N bits")
        {
            static constexpr uint8_t number_a = 32;
            static constexpr uint8_t number_b = 16;
            const integer<TestWidth> a{number_a};
            const integer<TestWidth> b{number_b};
            const integer<TestWidth> result = add(a, b);
            const integer<TestWidth> result_fun = fun_add(a, b);

            THEN("It should be the correct sum")
            {
                CHECK(result_fun == result);
                REQUIRE(result.word(0) == number_a + number_b);
            }
        }
        WHEN("The result a+b does not fit into N bits")
        {
            static constexpr uint16_t number_a = (1 << TestWidth) - 1;
            static constexpr uint16_t number_b = 1;
            constexpr integer<TestWidth> a{number_a};
            constexpr integer<TestWidth> b{number_b};
            auto const result = add(a, b);
            const integer<TestWidth> result_fun = fun_add(a, b);

            THEN("It should be the masked to fit")
            {
                CHECK(result_fun == result);
                REQUIRE(result.word(0) == 0);
            }
        }
    }
    GIVEN("Two integers with different word_count")
    {
        constexpr integer<64> a{16};
        constexpr integer<128> b{32, 8};
        constexpr integer<128> expected{32, 24};
        WHEN("Adding the integers")
        {
            THEN("All words need to be respected in the operation")
            {
                const auto result = expanding_add(b, a);
                REQUIRE(result == expected);
            }
        }
    }
    GIVEN("Two positive integer<N> a and b with N > word_width")
    {
        static constexpr size_t TestWidth = 128;
        static_assert(integer<TestWidth>::word_count() > 1);

        WHEN("There is a carry into the next word")
        {
            static constexpr uint64_t number_a = 1ULL << 63;
            static constexpr uint64_t number_b = 1ULL << 63;
            constexpr integer<TestWidth> a{number_a};
            constexpr integer<TestWidth> b{number_b};
            auto const result = add(a, b);
            const integer<TestWidth> result_fun = fun_add(a, b);

            THEN("It is added to the next word")
            {
                CHECK(result_fun == result);
                REQUIRE(result.word(1) == 1);
            }
        }

        WHEN("There is no carry into the next word")
        {
            static constexpr uint64_t number_a = 1ULL << 63;
            static constexpr uint64_t number_b = 0;
            constexpr integer<TestWidth> a{number_a};
            constexpr integer<TestWidth> b{number_b};
            auto const result = add(a, b);
            const integer<TestWidth> result_fun = fun_add(a, b);

            THEN("The next word is unchanged")
            {
                REQUIRE(result_fun == result);
                REQUIRE(result.word(1) == 0);
            }
        }
    }
    GIVEN("An integer and its negative")
    {
        constexpr integer<16> a(15);
        constexpr integer<16> a_(-15);
        constexpr integer<16> zero16(0);
        const integer<16> sum16 = add(a, a_);
        const integer<16> sum16_fun = fun_add(a, a_);

        constexpr integer<64> b(150);
        constexpr integer<64> b_(-150);
        constexpr integer<64> zero64(0);
        const integer<64> sum64 = add(b, b_);
        const integer<64> sum64_fun = fun_add(b, b_);

        constexpr integer<150> c(1337);
        constexpr integer<150> c_(-1337);
        constexpr integer<150> zero150(0);
        const integer<150> sum150_fun = fun_add(c, c_);
        const integer<150> sum150 = add(c, c_);

        THEN("The sum should be zero")
        {

            CHECK(sum16_fun == sum16);
            REQUIRE(sum16 == zero16);
            REQUIRE(sum16.is_zero());
            REQUIRE(sum16 == add(a_, a));

            CHECK(sum64_fun == sum64);
            REQUIRE(sum64 == zero64);
            REQUIRE(sum64.is_zero());
            REQUIRE(sum64 == add(b_, b));

            CHECK(sum150_fun == sum150);
            REQUIRE(sum150 == zero150);
            REQUIRE(sum150.is_zero());
            REQUIRE(sum150 == add(c_, c));
        }

        THEN("The sum should be negative")
        {
            REQUIRE_FALSE(sum16.is_negative());
            REQUIRE_FALSE(sum64.is_negative());
            REQUIRE_FALSE(sum150.is_negative());
        }
    }

    GIVEN("The number zero")
    {
        constexpr integer<16> z16;
        constexpr integer<63> z63;
        constexpr integer<150> z150;
        WHEN("Adding a negative number")
        {

            constexpr integer<16> neg16{-5};
            constexpr integer<63> neg63{-5};
            constexpr integer<150> neg150{-5};

            THEN("The result should be the negative number")
            {
                CHECK(add(z16, neg16) == neg16);
                CHECK(add(z63, neg63) == neg63);
                CHECK(add(z150, neg150) == neg150);
            }
        }
    }

    GIVEN("A positive integer and a negative integer with larger absolute value")
    {
        constexpr integer<16> a(15);
        constexpr integer<16> a_(-16);
        constexpr integer<16> zero16(0);
        const integer<16> sum16 = add(a, a_);

        constexpr integer<64> b(150);
        constexpr integer<64> b_(-235);
        constexpr integer<64> zero64(0);
        const integer<64> sum64 = add(b, b_);

        constexpr integer<150> c(1337);
        constexpr integer<150> c_(-5000);
        constexpr integer<150> zero150(0);
        const integer<150> sum150 = add(c, c_);

        THEN("The sum should be negative")
        {
            REQUIRE(sum16.is_negative());
            REQUIRE(sum64.is_negative());
            REQUIRE(sum150.is_negative());
        }
    }
    //    GIVEN("A positive integer and a negative integer with smaller absolute value")
    //    {
    //        const integer<192> a = integer<192>::from_words(1U, 0U, 0U);
    //
    //        const integer<64> b{-1};
    //
    //        WHEN("Adding both numbers")
    //        {
    //            const auto result = expanding_add(a, b);
    //
    //            std::cout << group_digits(to_binary(a), 64) << "\n";
    //            std::cout << group_digits(to_binary(b), 64) << "\n";
    //            std::cout << group_digits(to_binary(width_cast<192>(b)), 64) << "\n";
    //            std::cout << group_digits(to_binary(result), 64) << "\n";
    //        }
    //    }
}

SCENARIO("Multiplying larger integers using the various implementations works", "[integer][arithmetic][multiplication]")
{
    GIVEN("Some large integers") {
        const integer<192> a = integer<192>::from_words(1U, 0U, 0U);
        const integer<192> b = integer<192>::from_words(1U, 1U, 0U);
        const integer<192> c = integer<192>{10};
        const integer<192> one = integer<192>::one();
        const integer<192> ones = integer<192>::all_ones();
        const integer<192> zero = integer<192>::zero();

        std::vector<integer<192>> numbers {a,b,c,one,ones,zero};

        WHEN("Multiplying by zero") {
            THEN("The result is zero") {

                for (const auto & num: numbers) {
                    REQUIRE(naive_mul(num,zero) == zero);
                    REQUIRE(inplace_mul(num,zero) == zero);
                }
            }
        }

        WHEN("Multiplying by one") {
            THEN("The result is unchanged") {
                for (const auto & num: numbers) {
                    REQUIRE(naive_mul(num,one) == num);
                    REQUIRE(inplace_mul(num,one) == num);
                }
            }
        }

        WHEN("Comparing all multiplicatoin variants") {
            THEN("The results should match") {
                for (const auto & n: numbers) {
                    for (const auto & m: numbers) {
                        const auto res_normal = mul(n,m);
                        const auto res_naive = naive_mul(n,m);
                        const auto res_inplace = inplace_mul(n,m);

                        REQUIRE(res_normal == res_naive);
                        REQUIRE(res_naive == res_inplace);
                        REQUIRE(res_normal == res_inplace);
                    }
                }
            }
        }

    }
}

SCENARIO("Division of signed integers", "[integer][arithmetic][division]")
{

    GIVEN("The number 1 << 65")
    {
        constexpr integer<70> m = (integer<70>::one() << 65);

        constexpr integer<70> two{2};

        WHEN("Repeatedly dividing by two")
        {
            THEN("The division should work correctly over word boundaries")
            {
                const auto div1 = restoring_division(m, two);

                CHECK(div1.first.word(1) == 1U);
                CHECK(div1.first.word(0) == 0U);
                CHECK(div1.second == integer<70>::zero());

                const auto div2 = restoring_division(div1.first, two);

                CHECK(div2.first.word(1) == 0U);
                CHECK(div2.first.word(0) == (int64_t(1) << int64_t(63)));
                CHECK(div2.second == integer<70>::zero());
            }
        }
    }

    GIVEN("A signed integer<64> n != INT_MIN")
    {

        const int64_t n_ = GENERATE(take(
            100, random(std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::max())));
        const integer<64> n{n_};

        WHEN("Dividing zero by this number")
        {
            THEN("The result should be zero")
            {
                if (!n.is_zero())
                {
                    const auto result = restoring_division(integer<64>::zero(), n);
                    CHECK(result.second == integer<64>::zero());
                    REQUIRE(result.first == integer<64>::zero());
                }
                else
                {
                    REQUIRE_THROWS_AS(restoring_division(n, n), std::runtime_error);
                }
            }
        }

        WHEN("Dividing by itself")
        {
            AND_WHEN("n is not equal to zero")
            {
                THEN("The result should be one without remainder")
                {
                    if (!n.is_zero())
                    {
                        const auto result = restoring_division(n, n);
                        CHECK(result.second == integer<64>::zero());
                        REQUIRE(result.first == integer<61>::one());
                    }
                    else
                    {
                        REQUIRE_THROWS_AS(restoring_division(n, n), std::runtime_error);
                    }
                }
            }
        }

        WHEN("Dividing by zero")
        {
            THEN("An exception should be raised")
            {
                REQUIRE_THROWS_AS(restoring_division(n, integer<64>::zero()), std::runtime_error);
            }
        }

        WHEN("Dividing by one")
        {
            THEN("The result should be the number itself")
            {

                const auto result = restoring_division(n, integer<64>::one());
                CHECK(result.second == integer<64>::zero());
                REQUIRE(result.first == n);
            }
        }

        WHEN("Dividing by minus one")
        {
            THEN("The result should flip its sign")
            {

                const auto result = restoring_division(n, integer<64>::minus_one());
                CHECK(result.second == integer<64>::zero());
                REQUIRE(result.first == -n);
            }
        }

        AND_GIVEN("Another signed integer<64> m != INT_MIN")
        {
            const int64_t m_ = GENERATE(take(100, random(std::numeric_limits<int64_t>::max(),
                                                         std::numeric_limits<int64_t>::max())));
            const integer<64> m{m_};
            WHEN("Dividing the numbers")
            {
                THEN("The behaviour should match its int64_t counterpart")
                {
                    const auto result64_div = n_ / m_;
                    const auto result64_rem = n_ % m_;
                    const auto result = restoring_division(n, m);
                    CHECK(result.first.word(0) ==
                          static_cast<integer<64>::word_type>(result64_div));
                    CHECK(result.second.word(0) ==
                          static_cast<integer<64>::word_type>(result64_rem));
                }
            }
        }
    }

    GIVEN("INT_MIN")
    {
        AND_GIVEN("Minus one")
        {
            WHEN("Dividing INT_MIN by minus one")
            {
                THEN("The result should be INT_MIN")
                {
                    const auto result =
                        restoring_division(integer<64>::min(), integer<64>::minus_one());
                    CHECK(result.first == integer<64>::min());
                    CHECK(result.second == integer<64>::zero());
                }
            }
        }
    }
}

SCENARIO("Multiplying unsigned integers", "[integer][arithmetic][multiplication]")
{
    GIVEN("The largest integer value")
    {
        const uinteger<64> a64{std::numeric_limits<uint64_t>::max()};
        const uinteger<32> a32{std::numeric_limits<uint32_t>::max()};

        const uint64_t result32_uint64 =
            static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) *
            static_cast<uint64_t>(std::numeric_limits<uint32_t>::max());

        const uinteger<8> a8{std::numeric_limits<uint8_t>::max()};
        const uinteger<5> a5{31U};

        WHEN("Mutliplying this value with itself")
        {
            THEN("The expanding_mul should not truncate the result")
            {
                const auto result64 = expanding_mul(a64, a64);
                const auto result32 = expanding_mul(a32, a32);
                const auto result8 = expanding_mul(a8, a8);
                const auto result5 = expanding_mul(a5, a5);



                CHECK(result64.width() == 128);
                CHECK(result64 > a64);

                CHECK(result32.width() == 64);
                CHECK(result32.word(0) == result32_uint64);
                CHECK(result32 > a32);

                CHECK(result8.width() == 16);
                CHECK(result8.word(0) == 65025U);
                CHECK(result8 > a8);

                CHECK(result5.width() == 10);
                CHECK(result5.word(0) == 961U);
                CHECK(result5 > a5);
            }
        }
    }
}
SCENARIO("Multiplying signed integers", "[integer][arithmetic][multiplication]")
{
    GIVEN("Two signed integers m and r")
    {
        THEN("Then the multiplication should be done correctly")
        {
        }

        WHEN("m is the most negative number")
        {
            THEN("The algorithm should still work")
            {
                constexpr integer<8> m{-16};
                constexpr integer<8> r{2};

                const integer<8> res = mul(m, r);
                const integer<8> res_naive = naive_mul(m,r);

                int8_t mi = -16;
                int8_t ri = 2;

                int8_t resi = mi * ri;
                CHECK(res == res_naive);
                CHECK((uint8_t)res.word(0) == (uint8_t)resi);
            }
        }
    }
}

SCENARIO("Absolute value computation", "[integer][operations][utility]")
{
    GIVEN("The smallest possible value")
    {
        constexpr integer<150> min = std::numeric_limits<integer<150>>::min();
        THEN("The absolute value of that value is the value again")
        {
            const integer<150> absolute = abs(min);
            REQUIRE(absolute == min);
        }

        THEN("The 'real' absolute value is 2^(W-1)")
        {
            const uinteger<150> abs = expanding_abs(min);
            CHECK(abs.word(0) == 0U);
            CHECK(abs.word(1) == 0U);
            REQUIRE(abs.word(2) == (1U << 21U));
        }
    }

    GIVEN("Any non-smallest value")
    {
        using sint = integer<64>;
        const int32_t val_32 =
            GENERATE(take(50, random(std::numeric_limits<int32_t>::min() + 1, -1)));
        const int64_t val_64 = GENERATE(
            take(50, random(std::numeric_limits<int64_t>::min() + 1, static_cast<int64_t>(-1))));
        const sint a{val_32};
        const sint b{val_64};

        THEN("Computing abs is idempotent")
        {
            REQUIRE(abs(abs(a)) == abs(a));
            REQUIRE(abs(abs(b)) == abs(b));

            // not "really" applying it twice but close enough
            REQUIRE(abs(sint(expanding_abs(a))) == sint(expanding_abs(a)));
            REQUIRE(abs(sint(expanding_abs(b))) == sint(expanding_abs(b)));
        }

        // does not work for int64 as, for some reason, there is no matching abs function for that
        THEN("Computing abs should match its int32 type counterpart")
        {
            REQUIRE(abs(a).word(0) == abs(val_32));
            REQUIRE(expanding_abs(a).word(0) == abs(val_32));
        }
    }
}

SCENARIO("Expanding subtraction works correctly", "[integer][arithmetic][subtraction]")
{
    GIVEN("A n-bit min and a m-bit (m>n)  max")
    {
        static constexpr integer<4> min4 = integer<4>::min();
        static const integer<8> max8 = integer<8>::max();
        static constexpr integer<8> expected = integer<8>{-135};

        THEN("Subtracting max from min should correctly yield (min-max) as it now fits the width")
        {
            const auto result = expanding_sub(min4, max8);
            REQUIRE(result == expected);
        }
    }

    GIVEN("A n-bit min and a m-bit (m<n) max")
    {
        static constexpr integer<8> min8 = integer<8>::min();
        static const integer<4> max4 = integer<4>::max();
        static const integer<8> expected =
            add(sub(integer<8>::max(), integer<8>(integer<4>::max())), integer<8>{1U});

        THEN("Subtracting max from min should give (max-min)+1")
        {
            auto const result = expanding_sub(min8, max4);
            REQUIRE(result == expected);
        }
    }
}

SCENARIO("Width casting of signed integers", "[integer][utility]")
{
    GIVEN("A positive integer")
    {
        constexpr integer<16> i16{400};
        constexpr integer<32> i32{400};
        constexpr integer<150> i150{354346546};

        WHEN("Expanding the width")
        {

            const integer<24> i16e = width_cast<24>(i16);
            const integer<50> i32e = width_cast<50>(i32);
            const integer<200> i150e = width_cast<200>(i150);

            THEN("The numerical value should not have changed")
            {
                CHECK(i16 == i16e);
                CHECK(i32 == i32e);
                REQUIRE(i150 == i150e);
            }
        }
        WHEN("Reducing the width")
        {
            THEN("The first bits are simply dropped")
            {
                const integer<8> i16r = width_cast<8>(i16);
                const integer<20> i32r = width_cast<20>(i32);
                const integer<2> i150r = width_cast<2>(i150);
                CHECK(i16r == integer<8>{400 - 256});
                CHECK(i32r == i32);
                CHECK(i150r == integer<2>{2});
                //                std::cout << group_digits(to_binary(i150),8) << "\n";
                //                std::cout << group_digits(to_binary(i150r),8) << "\n";
            }
        }
    }
    GIVEN("A negative integer")
    {
        constexpr integer<16> i16{-400};
        constexpr integer<32> i32{-400};
        constexpr integer<150> i150{-354346546};

        WHEN("Expanding the width")
        {

            const integer<24> i16e = width_cast<24>(i16);
            const integer<50> i32e = width_cast<50>(i32);
            const integer<200> i150e = width_cast<200>(i150);

            THEN("The numerical value should not have changed")
            {
                CHECK(i16 == i16e);
                CHECK(i32 == i32e);
                REQUIRE(i150 == i150e);
            }
        }
        WHEN("Reducing the width")
        {
            THEN("The first bits are simply dropped")
            {
                const integer<8> i16r = width_cast<8>(i16);
                const integer<20> i32r = width_cast<20>(i32);
                const integer<2> i150r = width_cast<2>(i150);

                CHECK(i16r == integer<8>{112});
                CHECK(i32r == i32);
                CHECK(i150r == integer<2>{2});
            }
        }
    }
}

SCENARIO("Unary minus operation", "[integer][arithmetic][utility]")
{
    GIVEN("The smallest possible value")
    {
        constexpr integer<150> min = std::numeric_limits<integer<150>>::min();
        THEN("The unary minus value of that value is the value again")
        {
            const integer<150> minus_min = -min;
            REQUIRE(minus_min == min);
        }
    }

    GIVEN("Any non-smallest value")
    {
        using sint = integer<64>;
        const int32_t val_32 =
            GENERATE(take(50, random(std::numeric_limits<int32_t>::min() + 1, -1)));
        const int64_t val_64 = GENERATE(
            take(50, random(std::numeric_limits<int64_t>::min() + 1, static_cast<int64_t>(-1))));
        const sint a{val_32};
        const sint b{val_64};

        THEN("Unary minus is self-inverse")
        {
            REQUIRE(-(-a) == a);
            REQUIRE(-(-b) == b);
        }

        // does not work for int64 as, for some reason, there is no matching abs function for that
        THEN("Computing abs should match its int32 type counterpart")
        {
            REQUIRE((-a).word(0) == -val_32);
        }
    }
}

SCENARIO("MIN/MAX Values behave as expected", "[integer][operation][utility]")
{
    GIVEN("The min and max value")
    {
        constexpr size_t w = 50;
        constexpr integer<w> min = integer<w>::min();
        const integer<w> max = integer<w>::max();
        constexpr integer<w> one(1U);
        THEN("Adding/subtracting one should wrap around")
        {
            const auto sum = add(max, one);
            REQUIRE(sum == min);
            REQUIRE(sum == fun_add(max, one));

            REQUIRE(sub(min, one) == max);
        }

        THEN("Taking the absolute value of the min value yields the same value")
        {
            const auto abs_min = abs(min);
            REQUIRE(abs_min == min);
        }
    }
}



SCENARIO("Left/right shifting sintegers", "[integer][operation][utility]")
{
    GIVEN("A positive integer")
    {
        constexpr integer<150> a{0U, 1U, 0U};
        constexpr integer<150> b{8, 8, 8};
        constexpr integer<150> b_{4, 4, 4};
        constexpr integer<150> b__{2, 2, 2};
        constexpr integer<150> b___{1, 1, 1};
        WHEN("Right Shfiting")
        {
            THEN("It should behave like division by a power of two")
            {
                //                std::cout << group_digits(to_binary(a),64) << "\n";
                //                std::cout << group_digits(to_binary(b), 64) << "\n";
                //                std::cout << group_digits(to_binary(b >> 1), 64) << "\n";
                //                std::cout << group_digits(to_binary(b >> 2), 64) << "\n";
                //                std::cout << group_digits(to_binary(b >> 4), 64) << "\n";

                REQUIRE((b >> 1) == b_);
                REQUIRE((b >> 2) == b__);
                REQUIRE((b_ >> 1) == b__);
                REQUIRE((b >> 3) == b___);
                REQUIRE((b__ >> 1) == b___);
            }
            THEN("It should move correctly over word boundaries")
            {
                constexpr auto k = (a >> 1);
                constexpr auto b____ = integer<150>(0U, 0U, (uint64_t(1) << 63U));
                //                std::cout << group_digits(to_binary(k), 64) << "\n";
                //                std::cout << group_digits(to_binary(b), 64) << "\n";
                REQUIRE(k == b____);
            }

            THEN("The it should also work when moving farther than the word width")
            {
                constexpr integer<150> c{12U, 0U, 0U};
                constexpr integer c_shifted = c >> 68;
                constexpr auto b____ = integer<150>(0U, 0U, (uint64_t(11) << 62U));
                REQUIRE(c_shifted == b____);
                //                std::cout << group_digits(to_binary(c), 64) << "\n";
                //                std::cout << group_digits(to_binary(c_shifted), 64) << "\n";
            }
        }
        WHEN("Left Shifting")
        {
            THEN("It should behave like multiplication by a power of two")
            {

                constexpr integer<150> _b___ = b___ << 1;
                constexpr integer<150> __b___ = b___ << 2;
                constexpr integer<150> ___b___ = b___ << 3;
                REQUIRE(_b___ == b__);
                REQUIRE(__b___ == b_);
                REQUIRE(___b___ == b);
                //                        REQUIRE((b___<<2) == b_);
                //                        REQUIRE((b___<<3) == b);
            }
        }
    }

    GIVEN("The integer -1")
    {
        WHEN("Right shifting")
        {
            THEN("-1 should not be affected")
            {
                constexpr integer<150> minus_one(-1);
                constexpr integer<150> shifted1 = minus_one >> 1;
                constexpr integer<150> shifted2 = minus_one >> 22;
                constexpr integer<150> shifted3 = minus_one >> 23;
                constexpr integer<150> shifted4 = minus_one >> 149;

                // TODO why the hell is the constexpr above working and now below?
                const integer<150> shifted5 = minus_one >> 150;
                const integer<150> shifted6 = minus_one >> 1151;

                CHECK(shifted1 == minus_one);
                CHECK(shifted2 == minus_one);
                REQUIRE(shifted3 == minus_one);
                REQUIRE(shifted4 == minus_one);
                REQUIRE(shifted5 == minus_one);
                REQUIRE(shifted6 == minus_one);
            }
        }
    }
}


SCENARIO("Right-shift asigning sintegers", "[integer][operation][utility]")
{
    GIVEN("A positive integer")
    {

        constexpr integer<150> b_{4, 4, 4};
        constexpr integer<150> b__{2, 2, 2};
        constexpr integer<150> b___{1, 1, 1};
        WHEN("Right Shfiting")
        {
            THEN("It should behave like division by a power of two")
            {
                //                std::cout << group_digits(to_binary(a),64) << "\n";
                //                std::cout << group_digits(to_binary(b), 64) << "\n";
                //                std::cout << group_digits(to_binary(b >> 1), 64) << "\n";
                //                std::cout << group_digits(to_binary(b >> 2), 64) << "\n";
                //                std::cout << group_digits(to_binary(b >> 4), 64) << "\n";

                integer<150> b{8, 8, 8};
                b >>= 1;
                REQUIRE(b == b_);
                b >>= 1;
                REQUIRE(b == b__);
                b>>=1;
                REQUIRE(b == b___);
            }
            THEN("It should move correctly over word boundaries")
            {
                integer<150> a{0U, 1U, 0U};
                a >>= 1;
                constexpr auto b____ = integer<150>(0U, 0U, (uint64_t(1) << 63U));
                //                std::cout << group_digits(to_binary(k), 64) << "\n";
                //                std::cout << group_digits(to_binary(b), 64) << "\n";
                REQUIRE(a == b____);
            }

            THEN("The it should also work when moving farther than the word width")
            {
                integer<150> c{12U, 0U, 0U};
                constexpr auto b____ = integer<150>(0U, 0U, (uint64_t(11) << 62U));
                c >>= 68;
                REQUIRE(c == b____);
            }
        }
    }

    GIVEN("The integer -1")
    {
        WHEN("Right shifting")
        {
            THEN("-1 should not be affected")
            {
                constexpr integer<150> minus_one(-1);
                constexpr integer<150> shifted1 = minus_one >> 1;
                constexpr integer<150> shifted2 = minus_one >> 22;
                constexpr integer<150> shifted3 = minus_one >> 23;
                constexpr integer<150> shifted4 = minus_one >> 149;

                // TODO why the hell is the constexpr above working and now below?
                const integer<150> shifted5 = minus_one >> 150;
                const integer<150> shifted6 = minus_one >> 1151;

                CHECK(shifted1 == minus_one);
                CHECK(shifted2 == minus_one);
                REQUIRE(shifted3 == minus_one);
                REQUIRE(shifted4 == minus_one);
                REQUIRE(shifted5 == minus_one);
                REQUIRE(shifted6 == minus_one);
            }
        }
    }
}

SCENARIO("Computing the signum of an integer", "[integer][operation][utility]")
{
    GIVEN("The number  zero")
    {
        THEN("The signum should be zero")
        {
            REQUIRE(signum(integer<8>::zero()) == 0);
            REQUIRE(signum(integer<1>::zero()) == 0);
            REQUIRE(signum(integer<16>::zero()) == 0);
            REQUIRE(signum(integer<32>::zero()) == 0);
            REQUIRE(signum(integer<64>::zero()) == 0);
            REQUIRE(signum(integer<128>::zero()) == 0);
            REQUIRE(signum(integer<300>::zero()) == 0);
            REQUIRE(signum(integer<1313>::zero()) == 0);
        }
    }
    GIVEN("A non-negative number")
    {
        THEN("The signum should be one")
        {
            int8_t val_a =
                GENERATE(take(100, random(int8_t(1), std::numeric_limits<int8_t>::max())));

            REQUIRE(signum(integer<8>{val_a}) == 1);
            REQUIRE(signum(integer<16>{val_a}) == 1);
            REQUIRE(signum(integer<17>{val_a}) == 1);
            REQUIRE(signum(integer<32>{val_a}) == 1);
            REQUIRE(signum(integer<64>{val_a}) == 1);
            REQUIRE(signum(integer<128>{val_a}) == 1);
            REQUIRE(signum(integer<256>{val_a, val_a}) == 1);
        }
    }
    GIVEN("A negative number")
    {
        THEN("The signum should be one")
        {
            int8_t val_a =
                GENERATE(take(100, random(std::numeric_limits<int8_t>::min(), int8_t(-1))));

            REQUIRE(signum(integer<8>{val_a}) == -1);
            REQUIRE(signum(integer<16>{val_a}) == -1);
            REQUIRE(signum(integer<17>{val_a}) == -1);
            REQUIRE(signum(integer<32>{val_a}) == -1);
            REQUIRE(signum(integer<64>{val_a}) == -1);
            REQUIRE(signum(integer<128>{val_a}) == -1);
        }
    }
}
