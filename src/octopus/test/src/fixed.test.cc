#include <gtest/gtest.h>
#include <iostream>
#include <cmath>
#include "utils/Fixed.hh"
#include "utils/FixedPoint.hh"

using fixed = numeric::fixed<16, 16>;
using fixed_octopus = octopus::FixedPoint<100000000>;

TEST(fixedTest, simple)
{
    numeric::fixed<8, 8> a8 = 50.25;

    EXPECT_TRUE((fixed(10.5) * 3)          == 31.5);
	EXPECT_TRUE((fixed(10.5) * fixed(3))   == 31.5);
	EXPECT_TRUE((3 * fixed(10.5))          == 31.5);
	EXPECT_TRUE((fixed(10.5) * 3.0)        == 31.5);
	EXPECT_TRUE((fixed(10.5) * fixed(3.0)) == 31.5);
	EXPECT_TRUE((3.0 * fixed(10.5))        == 31.5);

    EXPECT_TRUE(square_root(fixed(16.)) == fixed(4.));

    EXPECT_TRUE(square_root(fixed(16.47)) == fixed(std::sqrt(16.47)));
}

TEST(fixedTest, small)
{
	numeric::fixed<32, 32> a32(-0.002357968564546);
	numeric::fixed<32, 32> b32(-0.000757968564546);

	std::cout<<a32<<std::endl;
	std::cout<<b32<<std::endl;
	std::cout<<a32+b32<<std::endl;

	numeric::fixed<32, 32> square_a32 = a32 * a32;
	numeric::fixed<32, 32> square_b32 = b32 * b32;
	std::cout<<square_a32<<std::endl;
	std::cout<<square_b32<<std::endl;
}

TEST(fixedPointTest, simple)
{
	EXPECT_TRUE((fixed_octopus(10.5) * 3)                       == 31.5);
	EXPECT_TRUE((fixed_octopus(10.5) * fixed_octopus(3))        == 31.5);
	EXPECT_TRUE((3 * fixed_octopus(10.5))                       == 31.5);
	EXPECT_TRUE((fixed_octopus(10.5) * 3.0)                     == 31.5);
	EXPECT_TRUE((fixed_octopus(10.5) * fixed_octopus(3.0))      == 31.5);
	EXPECT_TRUE((3.0 * fixed_octopus(10.5))                     == 31.5);

	EXPECT_TRUE((fixed_octopus(31.5) / 3)                       == 10.5);
	EXPECT_TRUE((fixed_octopus(31.5) / fixed_octopus(3))        == 10.5);
	EXPECT_TRUE((fixed_octopus(31.5) / 3.0)                     == 10.5);
	EXPECT_TRUE((fixed_octopus(31.5) / fixed_octopus(3.0))      == 10.5);

	EXPECT_TRUE(square_root(fixed(16.)) == fixed(4.));

	EXPECT_TRUE(square_root(fixed(16.47)) == fixed(std::sqrt(16.47)));
}

TEST(fixedPointTest, small)
{
	fixed_octopus a32(-0.002357968564546);
	fixed_octopus b32(-0.000757968564546);

	EXPECT_TRUE(a32 == fixed_octopus(-235796, true));
	EXPECT_TRUE(b32 == fixed_octopus(-75796, true));
	EXPECT_TRUE(a32+b32 == fixed_octopus(-311592, true));

	fixed_octopus square_a32 = a32 * a32;
	fixed_octopus square_b32 = b32 * b32;

	EXPECT_TRUE(555 == square_a32.data());
	EXPECT_TRUE(57 == square_b32.data());
	EXPECT_TRUE(235614 == numeric::square_root(square_a32).data());
	EXPECT_TRUE(75810 == numeric::square_root(square_b32).data());
}

TEST(fixedPointTest, ceil)
{
	fixed_octopus test_l(1234, true);

	EXPECT_EQ(100000000, numeric::ceil(test_l).data());

	fixed_octopus test2_l(100001234, true);

	EXPECT_EQ(200000000, numeric::ceil(test2_l).data());
}

TEST(fixedPointTest, floor)
{
	fixed_octopus test_l(1234, true);

	EXPECT_EQ(0, numeric::floor(test_l).data());

	fixed_octopus test2_l(100001234, true);

	EXPECT_EQ(100000000, numeric::floor(test2_l).data());
}
