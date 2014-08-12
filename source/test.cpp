
#include "base/base.h"
#include "base/std_dsp_computational_basis.h"
#include "base/std_dsp_mem.h"
#include "containers/iterators.h"

#include "stateless_algorithms/mono.h"

#include <iostream>

int main(int argc, char** argv) {
	std_dsp::static_storage<2, 256> buf;
	for(int i = 0; i < 256; ++i)
		*(buf.begin() + i) = i;
	std_dsp::static_storage<2, 256> buf_r;
	for(int i = 0; i < 256; ++i)
		*(buf_r.begin() + i) = i;

	{
	auto it1 = buf.begin();
	for(auto i = 0; i < 256; ++i, ++it1)
		std::cout << *it1 << " ";
	std::cout << std::endl;

	auto it2 = std_dsp::make_reverse_iterator(buf_r.begin(), 256);
	for(auto i = 0; i < 256; ++i, ++it2)
		std::cout << *it2 << " ";
	std::cout << std::endl;

	}

	{
/*	auto it1 = buf.begin();
	std_dsp::inplace_transform(it1, 256, std_dsp::transform_functors::multiply_op(2.0));
	for(auto i = 0; i < 256; ++i, ++it1)
		std::cout << *it1 << " ";
	std::cout << std::endl;
*/
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

	}

	return 0;
}