
//Unit tests for stereo transform algorithms

#include "gtest/gtest.h"

#include <algorithm>
#include <array>
#include <cstdint>

#include "../test_signals.h"

#include "../../stereo/stereo_transforms.h"
#include "../../base/std_dsp_mem.h"

TEST(StereoTransformsTest, SwapChannelsSplit) {

	const std_dsp::integer_t COUNT = 255;

	std_dsp::static_storage<2, COUNT> buf;
	std_dsp::static_storage<2, COUNT> ref_buf;

	std::fill(buf.begin(0), buf.end(0), 0.0);
	std::fill(buf.begin(1), buf.end(1), 1.0);
	std::fill(ref_buf.begin(0), ref_buf.end(0), 1.0);
	std::fill(ref_buf.begin(1), ref_buf.end(1), 0.0);

	std_dsp::swap_channels(buf.begin(0), buf.begin(1), COUNT, buf.begin(0), buf.begin(1));

	EXPECT_TRUE(std_dsp::compare_split(buf, ref_buf));

}

TEST(StereoTransformsTest, SwapChannelsInterleaved) {

	const std_dsp::integer_t COUNT = 255;

	std_dsp::static_storage<2, COUNT> buf;
	std_dsp::static_storage<2, COUNT> ref_buf;

	auto buf_it = buf.begin();

	for (std_dsp::integer_t i = 0; i < COUNT; ++i) {
		*buf_it = 0.0;
		++buf_it;
		*buf_it = 1.0;
		++buf_it;
	}

	auto ref_it = ref_buf.begin();

	for (std_dsp::integer_t i = 0; i < COUNT; ++i) {
		*ref_it = 1.0;
		++ref_it;
		*ref_it = 0.0;
		++ref_it;
	}

	std_dsp::swap_channels(buf.begin(), COUNT, buf.begin());

	EXPECT_TRUE(std_dsp::compare_interleaved(buf, ref_buf));

}

TEST(StereoTransformsTest, MonoSplit) {

	const std_dsp::integer_t COUNT = 255;

	std_dsp::static_storage<2, COUNT> buf;
	std_dsp::static_storage<2, COUNT> ref_buf;

	std::fill(buf.begin(0), buf.end(0), 0.0);
	std::fill(buf.begin(1), buf.end(1), 1.0);
	std::fill(ref_buf.begin(0), ref_buf.end(0), 0.5);
	std::fill(ref_buf.begin(1), ref_buf.end(1), 0.5);

	std_dsp::mono(buf.begin(0), buf.begin(1), COUNT, buf.begin(0), buf.begin(1));

	EXPECT_TRUE(std_dsp::compare_split(buf, ref_buf));

}

TEST(StereoTransformsTest, MonoInterleaved) {

	const std_dsp::integer_t COUNT = 255;

	std_dsp::static_storage<2, COUNT> buf;
	std_dsp::static_storage<2, COUNT> ref_buf;

	auto buf_it = buf.begin();

	//The -i and +i should cancel each other out leaving 0.5
	//which is the average of 0 and 1:

	for (std_dsp::integer_t i = 0; i < COUNT; ++i) {
		*buf_it = 0.0 - i;
		++buf_it;
		*buf_it = 1.0 + i;
		++buf_it;
	}

	auto ref_it = ref_buf.begin();

	for (std_dsp::integer_t i = 0; i < COUNT; ++i) {
		*ref_it = 0.5;
		++ref_it;
		*ref_it = 0.5;
		++ref_it;
	}

	std_dsp::mono(buf.begin(), COUNT, buf.begin());
	
	EXPECT_TRUE(std_dsp::compare_interleaved(buf, ref_buf));

}
