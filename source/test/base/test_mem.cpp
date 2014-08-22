
//Unit tests for sequence cast algorithms

#include "gtest/gtest.h"

#include <array>
#include <cstdint>

#include "../test_signals.h"

#include "../../base/std_dsp_mem.h"
#include "../../base/std_dsp_computational_basis.h"

template <std_dsp::integer_t CHANNELS, std_dsp::integer_t SIZE, typename S>
void test_storage(S& s) {
	
	//Check alignment

	EXPECT_EQ(0, std_dsp::get_alignment(s.begin(0)));
	EXPECT_EQ(0, std_dsp::get_alignment(s.begin(1)));

	//Check size

	EXPECT_EQ(CHANNELS * SIZE, std::distance(s.begin(), s.end()));

	EXPECT_EQ(SIZE, std::distance(s.begin(0), s.end(0)));
	EXPECT_EQ(SIZE, std::distance(s.begin(1), s.end(1)));

	EXPECT_EQ(SIZE, s.size());
	EXPECT_EQ(CHANNELS, s.channels());

	//Check physical size

	if ((SIZE & 1) == 0)
		EXPECT_EQ(CHANNELS * SIZE, s.physical_size());
	else
		EXPECT_EQ(CHANNELS * (1+SIZE), s.physical_size());

	//Write split

	for(std_dsp::integer_t i = 0; i < CHANNELS; ++i)
		std::fill(s.begin(i), s.end(i), static_cast<double>(i));

	for(std_dsp::integer_t i = 0; i < CHANNELS; ++i)
		EXPECT_TRUE(std_dsp::compare(s.begin(i), static_cast<double>(i), SIZE));

	//Read and write SIMD

	if (SIZE >= 2) {
		for (std_dsp::integer_t i = 0; i < CHANNELS; ++i) {
			std_dsp::double2_t x = std_dsp::load2(s.begin(i), 0);
			x = std_dsp::add(x, x);
			std_dsp::store2(s.begin(i), 0, x);

			EXPECT_NEAR((i * 2.0), (s.begin(i))[0], 0.000001);
			EXPECT_NEAR((i * 2.0), (s.begin(i))[1], 0.000001);
		}
	}

	//Write interleaved

	std::fill(s.begin(), s.end(), static_cast<double>(CHANNELS));

	EXPECT_TRUE(std_dsp::compare(s.begin(), static_cast<double>(CHANNELS), CHANNELS * SIZE));

}

TEST(StorageTest, StaticStorageTest) {

	{
		std_dsp::static_storage<1, 255> s;
		test_storage<1, 255>(s);
	}

	{
		std_dsp::static_storage<2, 256> s;
		test_storage<2, 256>(s);
	}
	{
		std_dsp::static_storage<2, 255> s;
		test_storage<2, 255>(s);
	}
	{
		std_dsp::static_storage<3, 255> s;
		test_storage<3, 255>(s);
	}
	{
		std_dsp::static_storage<4, 255> s;
		test_storage<4, 255>(s);
	}

}

TEST(StorageTest, DynamicStorageTest) {

	{
		std_dsp::dynamic_storage<> s(4, 255);
		test_storage<4, 255>(s);
	}

	{
		std_dsp::dynamic_storage<1> s(0);
		test_storage<1, 0>(s);
	}
		
	{
		std_dsp::dynamic_storage<1> s(255);
		test_storage<1, 255>(s);
	}
	{
		std_dsp::dynamic_storage<2> s(256);
		test_storage<2, 256>(s);
	}
	{
		std_dsp::dynamic_storage<2> s(255);
		test_storage<2, 255>(s);
	}
	{
		std_dsp::dynamic_storage<3> s(255);
		test_storage<3, 255>(s);
	}

}
