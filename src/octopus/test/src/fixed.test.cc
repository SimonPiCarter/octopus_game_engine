#include <gtest/gtest.h>
#include <iostream>
#include <cmath>
#include "utils/Fixed.hh"

using fixed = numeric::fixed<16, 16>;

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
	numeric::fixed<32, 32> a32(-0.23);
	numeric::fixed<32, 32> b32(-0.07);

	std::cout<<a32<<std::endl;
	std::cout<<b32<<std::endl;
	std::cout<<a32+b32<<std::endl;

	numeric::fixed<32, 32> square_a32 = a32 * a32;
	numeric::fixed<32, 32> square_b32 = b32 * b32;
	std::cout<<square_a32<<std::endl;
	std::cout<<square_b32<<std::endl;
}
