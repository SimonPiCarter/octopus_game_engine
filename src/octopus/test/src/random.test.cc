#include <gtest/gtest.h>

#include <random>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

TEST(random, std)
{
    std::mt19937 gen_l(42);

    std::uniform_int_distribution<> distModel_l(0, 10);

    for(uint32_t i = 0; i < 10 ; ++ i)
    {
        std::cout<<distModel_l(gen_l)<<std::endl;
    }
}

TEST(random, boost)
{
    boost::random::mt19937 gen_l(42);

    boost::random::uniform_int_distribution<> distModel_l(0, 10);

    for(uint32_t i = 0; i < 10 ; ++ i)
    {
        std::cout<<distModel_l(gen_l)<<std::endl;
    }
}
