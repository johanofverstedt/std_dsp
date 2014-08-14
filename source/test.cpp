
#include "std_dsp.h"
#include "base/base.h"
#include "base/std_dsp_computational_basis.h"
#include "base/std_dsp_mem.h"
#include "containers/buffer.h"

#include "stateless_algorithms/mono.h"

#include "perf_time.h"

#include <algorithm>
#include <iostream>

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

	const std::int64_t c = 8192;
	const std::int64_t channels = 2;
	const std::int64_t c2 = channels * c;

	std_dsp::static_storage<channels, c> s;
	for(int i = 0; i < c2; ++i)
		*(s.begin() + i) = i;
	
	std_dsp::interleave_inplace<double*, 64LL>(s.begin(), c);
	std_dsp::split_inplace(s.begin(), c);

	double t1 = std_dsp::perf_time();
	for(int k = 0; k < 128; ++k)
		std_dsp::interleave_inplace<double*, 8192LL>(s.begin(), c);
	double t2 = std_dsp::perf_time();

	for(int i = 0; i < c2; ++i)
		std::cout << s.begin()[i] << " ";

	std::cout << std::endl;

	std_dsp::split_inplace(s.begin(), c);

	for(int i = 0; i < c2; ++i)
		std::cout << s.begin()[i] << " ";

	std::cout << std::endl;

	std_dsp::interleave_inplace<double*, 64LL>(s.begin(), c);
	std_dsp::split_inplace(s.begin(), c);

	double t3 = std_dsp::perf_time();
	for(int k = 0; k < 128; ++k)
		std_dsp::interleave_inplace<double*,64LL>(s.begin(), c);
	double t4 = std_dsp::perf_time();

	for(int i = 0; i < c2; ++i)
		std::cout << s.begin()[i] << " ";

	std::cout << std::endl;

	std_dsp::split_inplace(s.begin(), c);

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