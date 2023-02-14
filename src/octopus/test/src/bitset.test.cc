#include <gtest/gtest.h>

#include "utils/DynamicBitset.hh"
#include <list>
#include <functional>

using namespace octopus;

bool record(std::vector<int> &records_p, int val_p)
{
	records_p.push_back(val_p);
	return false;
}

TEST(DynamicBitsetTest, empty)
{
	DynamicBitset bitset_l(10);

	EXPECT_EQ(10ul, bitset_l.size());

	// to store values set to 1 in the bitset
	std::vector<int> vals_l;
	bitset_l.for_each(std::bind(record, std::ref(vals_l), std::placeholders::_1));

	ASSERT_EQ(0ul, vals_l.size());
}

TEST(DynamicBitsetTest, set)
{
	DynamicBitset bitset_l(10);

	bitset_l.set(3, true);
	bitset_l.set(5, true);

	// to store values set to 1 in the bitset
	std::vector<int> vals_l;
	bitset_l.for_each(std::bind(record, std::ref(vals_l), std::placeholders::_1));

	ASSERT_EQ(2ul, vals_l.size());
	EXPECT_EQ(3, vals_l.at(0));
	EXPECT_EQ(5, vals_l.at(1));

	bitset_l.set(3, false);

	vals_l.clear();
	bitset_l.for_each(std::bind(record, std::ref(vals_l), std::placeholders::_1));

	ASSERT_EQ(1ul, vals_l.size());
	EXPECT_EQ(5, vals_l.at(0));
}

TEST(DynamicBitsetTest, or)
{
	DynamicBitset bitsetA_l(10);

	bitsetA_l.set(3, true);
	bitsetA_l.set(5, true);

	DynamicBitset bitsetB_l(10);

	bitsetB_l.set(2, true);
	bitsetB_l.set(5, true);

	bitsetA_l |= bitsetB_l;

	// to store values set to 1 in the bitset
	std::vector<int> vals_l;
	bitsetA_l.for_each(std::bind(record, std::ref(vals_l), std::placeholders::_1));

	ASSERT_EQ(3ul, vals_l.size());
	EXPECT_EQ(2, vals_l.at(0));
	EXPECT_EQ(3, vals_l.at(1));
	EXPECT_EQ(5, vals_l.at(2));
}

TEST(DynamicBitsetTest, and)
{
	DynamicBitset bitsetA_l(10);

	bitsetA_l.set(3, true);
	bitsetA_l.set(5, true);

	DynamicBitset bitsetB_l(10);

	bitsetB_l.set(2, true);
	bitsetB_l.set(5, true);

	bitsetA_l &= bitsetB_l;

	// to store values set to 1 in the bitset
	std::vector<int> vals_l;
	bitsetA_l.for_each(std::bind(record, std::ref(vals_l), std::placeholders::_1));

	ASSERT_EQ(1ul, vals_l.size());
	EXPECT_EQ(5, vals_l.at(0));
}

TEST(DynamicBitsetTest, over_one)
{
	DynamicBitset bitset_l(321);

	EXPECT_EQ(321ul, bitset_l.size());

	bitset_l.set(317, true);
	bitset_l.set(12, true);

	// to store values set to 1 in the bitset
	std::vector<int> vals_l;
	bitset_l.for_each(std::bind(record, std::ref(vals_l), std::placeholders::_1));

	ASSERT_EQ(2ul, vals_l.size());
	EXPECT_EQ(12, vals_l.at(0));
	EXPECT_EQ(317, vals_l.at(1));
}
