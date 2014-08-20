
//Unit tests for sequence cast algorithms

#include "gtest/gtest.h"

#include <array>
#include <cstdint>

#include "../test_signals.h"

#include "../../stereo/interleave.h"
#include "../../base/std_dsp_mem.h"

TEST(InterleaveTest, CheckIsInterleaving) {

	std::array<int, 2> a = { 1, 2 };
	std::array<int, 2> b = { 3, 4 };
	std::array<int, 2> b_2 = { 4, 3 };

	std::array<int, 4> c = { 1, 3, 2, 4 };

	EXPECT_TRUE(std_dsp::check_is_interleaving(a.begin(), b.begin(), 2, c.begin()));
	EXPECT_FALSE(std_dsp::check_is_interleaving(a.begin(), b_2.begin(), 2, c.begin()));

}

TEST(InterleaveTest, InterleaveWithCopy) {

	std::array<int, 2> a = { 1, 2 };
	std::array<int, 2> b = { 3, 4 };
	std::array<int, 2> b_2 = { 4, 3 };

	std::array<int, 4> c;

	std_dsp::interleave(a.begin(), b.begin(), 2, c.begin());

	EXPECT_EQ(1, c[0]);
	EXPECT_EQ(3, c[1]);
	EXPECT_EQ(2, c[2]);
	EXPECT_EQ(4, c[3]);

	EXPECT_TRUE(std_dsp::check_is_interleaving(a.begin(), b.begin(), 2, c.begin()));
	EXPECT_FALSE(std_dsp::check_is_interleaving(a.begin(), b_2.begin(), 2, c.begin()));

}

TEST(InterleaveTest, DeinterleaveWithCopy) {

	std::array<int, 2> a = { 0, 0 };
	std::array<int, 2> b = { 0, 0 };
	std::array<int, 2> b_2 = { 0, 0 };

	std::array<int, 4> c = { 1, 3, 2, 4 };

	std_dsp::deinterleave(c.begin(), 2, a.begin(), b.begin());

	EXPECT_EQ(1, a[0]);
	EXPECT_EQ(2, a[1]);
	EXPECT_EQ(3, b[0]);
	EXPECT_EQ(4, b[1]);

	EXPECT_TRUE(std_dsp::check_is_interleaving(a.begin(), b.begin(), 2, c.begin()));
	EXPECT_FALSE(std_dsp::check_is_interleaving(a.begin(), b_2.begin(), 2, c.begin()));

}

// Test the specialized double implementations of interleave/deinterleave more carefully

TEST(InterleaveTest, InterleaveAlignedDoubles) {

	std_dsp::static_storage<2, 1023> a;
	std_dsp::static_storage<2, 1023> b;

	for (std::size_t i = 0; i < 1023; ++i) {
		*(a.begin() + i) = std_dsp::test_signals::alternate_sign_increasing<double>(i);
	}

	std_dsp::interleave(a.begin(), a.begin() + 1023, 1023, b.begin());

	EXPECT_TRUE(std_dsp::check_is_interleaving(a.begin(), a.begin() + 1023, 1023, b.begin()));

	std::fill(a.begin(), a.end(), -1.0);

	std_dsp::deinterleave(b.begin(), 1023, a.begin(), a.begin() + 1023);

	EXPECT_TRUE(std_dsp::check_is_interleaving(a.begin(), a.begin() + 1023, 1023, b.begin()));

	std::fill(b.begin(), b.end(), -1.0);

	//Start the interleaving at an odd index for an alignment not SIMD-compatible

	std_dsp::interleave(a.begin() + 1, a.begin() + 1023, 1022, b.begin());

	EXPECT_TRUE(std_dsp::check_is_interleaving(a.begin() + 1, a.begin() + 1023, 1022, b.begin()));

}

TEST(InterleaveTest, InterleaveInplace) {

	std::array<int, 8> a = { 1, 2, 3, 4, 5, 6, 7, 8 }; //Input
	auto a_copy = a;
	std::array<int, 8> r = { 1, 5, 2, 6, 3, 7, 4, 8 }; //Expected result

	std_dsp::interleave_inplace<decltype(a.begin()), 1>(a.begin(), 4LL);

	for (std::size_t i = 0; i < 8ULL; ++i) {
		EXPECT_EQ(r[i], a[i]);
	}

	EXPECT_TRUE(std_dsp::check_is_interleaving(a_copy.begin(), a_copy.begin() + 4, 4, a.begin()));

	std_dsp::deinterleave_inplace<decltype(a.begin()), 1>(a.begin(), 4LL);

	for (std::size_t i = 0; i < 8ULL; ++i) {
		EXPECT_EQ(a_copy[i], a[i]);
	}

}
