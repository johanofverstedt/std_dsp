
//Unit tests for sequence cast algorithms

#include "gtest/gtest.h"

#include <array>
#include <cstdint>

#include "../test_signals.h"

#include "../../cast/seq_cast.h"

TEST(SequenceCastTest, StaticCast) {

	//Perform tests with unsigned and signed integers

	std::array<std::uint32_t, 16> a;
	std::array<std::int32_t, 16> b;

	a.fill(0xFFFFFFFFU);
	std_dsp::seq_cast(a.begin(), a.end(), b.begin());

	for(auto x : b) {
		EXPECT_EQ(-1, x);
	}

	//Perform tests with floats and doubles

	std::array<float, 16> c;
	std::array<double, 16> d;
	
	//Fill arrays with garbage
	c.fill(0.1f);
	d.fill(0.1);

	for (std::size_t i = 0; i < c.size(); ++i) {
		c[i] = std_dsp::test_signals::alternate_sign_increasing<float>(i);
	}

	std_dsp::seq_cast(c.begin(), c.end(), d.begin());

	for (std::size_t i = 0; i < d.size(); ++i) {
		EXPECT_EQ(static_cast<double>(std_dsp::test_signals::alternate_sign_increasing<float>(i)), d[i]);
	}
	//Fill c array with garbage
	c.fill(0.1f);

	std_dsp::seq_cast(d.begin(), d.end(), c.begin());

	for (std::size_t i = 0; i < c.size(); ++i) {
		EXPECT_EQ(static_cast<float>(static_cast<double>(std_dsp::test_signals::alternate_sign_increasing<float>(i))), c[i]);
	}

}

TEST(SequenceCastTest, StaticCastN) {

	std::array<unsigned int, 16> a;
	std::array<int, 16> b;

	a.fill(0xFFFFFFFFU);
	std_dsp::seq_cast_n(a.begin(), a.size(), b.begin());

	for(auto x : b) {
		EXPECT_EQ(-1, x);
	}

}

TEST(SequenceCastTest, CastingIterator) {
	
	//Perform tests with unsigned and signed integers

	std::array<std::uint32_t, 16> a;
	std::array<std::int32_t, 16> b;

	a.fill(0xFFFFFFFFU);
	b.fill(7);

	auto ait_begin = std_dsp::make_casting_iterator<std::int32_t>(a.begin());
	auto ait_end = std_dsp::make_casting_iterator<std::int32_t>(a.end());

	std::copy(ait_begin, ait_end, b.begin());

	for (auto x : b) {
		EXPECT_EQ(-1, x);
	}

}