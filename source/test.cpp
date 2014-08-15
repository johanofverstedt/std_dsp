
#include "std_dsp.h"
#include "base/base.h"
#include "base/std_dsp_computational_basis.h"
#include "base/std_dsp_mem.h"
#include "containers/buffer.h"

#include "stateless_algorithms/mono.h"

#include "perf_time.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/align/aligned_allocator.hpp>

int main(int argc, char** argv) {

/*
	std_dsp::buffer_t<std_dsp::static_storage<2, 256>> test_buf;
	std_dsp::static_storage<2, 256> buf;

	test_buf[0][0] = 1.0;
	test_buf[1][0] = 2.0;
	for(auto cit : test_buf) {
		std::cout << *cit << " ";
	}
	std::cout << std::endl;

	auto c_1 = std_dsp::make_circular_iterator(test_buf[1], 64, 256);
	auto c_2 = std_dsp::make_circular_iterator(test_buf[1], 32, 256);
	auto r_1 = std_dsp::make_reverse_iterator(std_dsp::make_reverse_iterator(c_1, 256), 256);
	auto r_2 = std_dsp::make_reverse_iterator(std_dsp::make_reverse_iterator(c_2, 256), 256);

	std::cout << c_2 - c_1 << std::endl;
	std::cout << std::distance(c_1, c_2) << std::endl;
	std::cout << r_2 - r_1 << std::endl;
	std::cout << std::distance(r_1, r_2) << std::endl;

	test_buf.randomize(4, -2.0, 2.0);
	for(auto cit : test_buf) {
		std::cout << *cit << " ";
	}
	std::cout << std::endl;

	for(int i = 0; i < 256; ++i)
		*(buf.begin() + i) = i;
	std_dsp::static_storage<2, 256> buf_r;
	for(int i = 0; i < 256; ++i)
		*(buf_r.begin() + i) = i;

	{
	auto it1 = buf.begin();
	std_dsp::print(test_buf, std::cout);
	//std_dsp::print_range(it1, 256);
	std::cout << std::endl;

	auto it2 = std_dsp::make_reverse_iterator(buf_r.begin(), 256);
	std_dsp::print(test_buf, std::cout);
	//std_dsp::print_range(it2, 256);
	std::cout << std::endl;

	}
*/
	{
/*	auto it1 = buf.begin();
	std_dsp::inplace_transform(it1, 256, std_dsp::transform_functors::multiply_op(2.0));
	for(auto i = 0; i < 256; ++i, ++it1)
		std::cout << *it1 << " ";
	std::cout << std::endl;
*/
	/*
	auto it2 = buf_r.begin();//std_dsp::make_reverse_iterator(buf_r.begin(), 256);
	std_dsp::copy_transform(
		std_dsp::make_circular_iterator(std_dsp::make_reverse_iterator(buf_r.begin(), 256), 66, 128),
		256, buf.begin(), std_dsp::transform_functors::multiply_op(2.0));
	for(auto i = 0; i < 256; ++i, ++it2)
		std::cout << *it2 << " ";
	std::cout << std::endl;

	auto it1 = buf.begin();
	//std_dsp::inplace_transform(it1, 256, std_dsp::transform_functors::multiply_op(2.0));
	for(auto i = 0; i < 256; ++i, ++it1)
		std::cout << *it1 << " ";
	std::cout << std::endl;
*/
	}

	//std::vector<double> tv;//, boost::alignment::aligned_allocator<double, 16>> tv;
	//for(int i = 0; i < 256; ++i)
	//	tv.push_back(i);
	const std::int64_t COUNT = 93;
	std_dsp::stereo_buffer tv{COUNT};
	for(int i = 0; i < COUNT * 2; ++i)
		*(tv.begin(0) + i) = i;

	std_dsp::interleave_inplace<double*, 1>(tv.begin(0), COUNT);
	std_dsp::deinterleave_inplace<double*, 1>(tv.begin(0), COUNT);

	//auto circ1 = std_dsp::make_circular_iterator(tv.begin(0), 200, 256);
	//auto circ2 = std_dsp::make_circular_iterator(tv.begin(1), 28, 128);

	//std::cout << &(tv[0]) << std::endl;
	//std::cout << &(tv[1]) << std::endl;
	//std::cout << &(tv[2]) << std::endl;
	//std_dsp::width(circ1, 128, circ1, 0.5);
	//std_dsp::width(tv.begin(0), tv.begin(0) + 128, 128, tv.begin(0), tv.begin(0) + 128, 0.5);
	//std_dsp::interleave_inplace2(tv.begin(), 256);//<decltype(tv.begin()), 16>

	for(int i = 0; i < COUNT; ++i) {
		std::cout << tv.begin(0)[i] << " ";
	}
	std::cout << " : " << std::endl;
	for(int i = COUNT; i < 2 * COUNT; ++i) {
		std::cout << tv.begin(0)[i] << " ";
	}

	return 0;
/*
	auto tv_it = tv.begin();
	for(int i = 0; i < 128; ++i) {
		if(*tv_it != i) {
			std::cout << std::endl << i << std::endl;
			return -1;
		}
		++tv_it;

		if(*tv_it != 128 + i) {
			std::cout << std::endl << "*" << i << std::endl;
			return -2;
		}
		++tv_it;
	}
	std_dsp::deinterleave_inplace<decltype(tv.begin()), 16>(tv.begin(), 128);

	for(int i = 0; i < 256; ++i) {
		if(tv[i] != i) {
			std::cout << std::endl << "-3" << std::endl;
			return -3;
		}
	}

	std_dsp::buffer_t<std_dsp::dynamic_storage<2>> b{128};
	b.randomize(-1.0, 1.0);
	std_dsp::buffer_t<std_dsp::dynamic_storage<2>> b2{128};
	std_dsp::copy(b[0], 128, b2[0]);
	std_dsp::copy(b[1], 128, b2[1]);

	std_dsp::interleave_inplace<double*, 16>(b[0], 128);
	if(b == b2) {
		abort();
	}
	std_dsp::deinterleave_inplace<double*, 16>(b[0], 128);

	if(b != b2) {
		abort();
	}*/

	const std::int64_t c = 8192;
	const std::int64_t channels = 2;
	const std::int64_t c2 = channels * c;

	std_dsp::static_storage<channels, c> s;
	for(int i = 0; i < c2; ++i)
		*(s.begin() + i) = i;
	
	std_dsp::interleave_inplace<double*, 64LL>(s.begin(), c);
	std_dsp::deinterleave_inplace(s.begin(), c);

	double t1 = std_dsp::perf_time();
	for(int k = 0; k < 128; ++k)
		std_dsp::interleave_inplace<double*, 128LL>(s.begin(), c);
	double t2 = std_dsp::perf_time();

	for(int i = 0; i < c2; ++i)
		std::cout << s.begin()[i] << " ";

	std::cout << std::endl;

	std_dsp::deinterleave_inplace(s.begin(), c);

	for(int i = 0; i < c2; ++i)
		std::cout << s.begin()[i] << " ";

	std::cout << std::endl;

	std_dsp::interleave_inplace<double*, 64LL>(s.begin(), c);
	std_dsp::deinterleave_inplace(s.begin(), c);

	double t3 = std_dsp::perf_time();
	for(int k = 0; k < 128; ++k)
		std_dsp::interleave_inplace(s.begin(), c);
		//std_dsp::interleave_inplace<double*, 256LL>(s.begin(), c);
	double t4 = std_dsp::perf_time();

	for(int i = 0; i < c2; ++i)
		std::cout << s.begin()[i] << " ";

	std::cout << std::endl;

	std_dsp::deinterleave_inplace(s.begin(), c);

	for(int i = 0; i < c2; ++i)
		std::cout << s.begin()[i] << " ";

	std::cout << std::endl;

	std::cout << (t2 - t1) << ", " << (t4 - t3) << std::endl;
	float x1;
	float x2;
	double y1[2];
	std_dsp::interleave(&x1, &x2, 1, (double*)y1, std_dsp::float_to_double_op());

	return 0;
}