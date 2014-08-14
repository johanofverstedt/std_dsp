
#ifndef STD_DSP_IO_GUARD
#define STD_DSP_IO_GUARD

#include <iostream>
#include <cstdint>

#include "../base/std_dsp_mem.h"

namespace std_dsp {
	template <typename I, typename N>
	inline
	void print_range(I first, N n) {
		std::cout << "[";
		while(n) {
			std::cout << *first;
			++first;
			if(n != 1)
				std::cout << ", ";
			--n;
		}
		std::cout << "]";
	}

	template <integer_t CHANNELS, typename I, typename N>
	inline
	void print_interleaved(I first, N n) {
		std::cout << "[";
		while(n) {
			std::cout << "(";
			std::size_t m = CHANNELS;
			while(m) {
				--m;
				std::cout << *first;
				++first;
				if(m > 0)
					std::cout << ", ";
			}
			std::cout << ")";

			if(n > 1)
				std::cout << ", ";
		}
		std::cout << "]";
	}
}

#endif
