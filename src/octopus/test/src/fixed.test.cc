#include <gtest/gtest.h>
#include <iostream>
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

    EXPECT_TRUE(square_root(fixed(16.47)) == fixed(sqrt(16.47)));
}
