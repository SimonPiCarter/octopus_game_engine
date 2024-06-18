#include <gtest/gtest.h>

#include "serialization/crc/FastCRC.hh"

TEST(crcTest, simple)
{
	uint32_t test_l = 234567890;

	FastCRC32 crc32_l;
	uint32_t crc_l = crc32_l.crc32((const uint8_t *)&test_l, sizeof(test_l));

	EXPECT_EQ(uint32_t(244866797), crc_l);
}
