
#include "gtest/gtest.h"

#include "../../source/std_dsp.h"

TEST(BufferTest, Initialization) {
	using namespace std_dsp;
	buffer<1> buf1;
	buf1.resize(256);
	buf1.clear();

	buffer<1> buf2;
	buf2.resize(256);
	buf2.clear();

	EXPECT_EQ(256, buf1.size());
	EXPECT_EQ(1, buf1.channels());
	EXPECT_TRUE(buf1 == buf2);
}