#include <gtest/gtest.h>

#include "utils/Version.hh"

TEST(version, simple)
{
    std::cout<<octopus::getHashVersion()<<std::endl;
}
