#include <gtest/gtest.h>

#include <random>

TEST(random, simple)
{
    std::mt19937 gen_l(42);

    std::uniform_int_distribution<> distModel_l(0, 10);

    for(size_t i = 0; i < 10 ; ++ i)
    {
        std::cout<<distModel_l(gen_l)<<std::endl;
    }
}
