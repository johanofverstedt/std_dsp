
#ifndef STD_DSP_ALIGNMENT_GUARD
#define STD_DSP_ALIGNMENT_GUARD

#include <cstdint>
#include <tuple>
#include "std_dsp_ranges.h"

#include "base.h"

namespace std_dsp {
	template <typename I>
	inline
	bool is_aligned(I first) {
		std::size_t alignment = reinterpret_cast<std::size_t>(first);
		return (alignment & 15) == 0;
	}

	template <typename I>
	inline
	bool is_odd_aligned(I first) {
		std::size_t alignment = reinterpret_cast<std::size_t>(first);
		return (alignment & 15) == 8;
	}

	template <typename I1, typename I2>
	inline
	bool compare_alignment(I1 first1, I2 first2) {
		std::size_t alignment1 = reinterpret_cast<std::size_t>(first1);
		std::size_t alignment2 = reinterpret_cast<std::size_t>(first2);
		return (alignment1 & 15) == (alignment2 & 15);
	}

	inline
	std::pair<std::size_t, std::size_t> unroll_partition_2(std::size_t n) {
		std::size_t n_remainder = n & 1;
		return std::pair<std::size_t, std::size_t>(n - n_remainder, n_remainder);
	}
	inline
	std::pair<std::size_t, std::size_t> unroll_partition_4(std::size_t n) {
		std::size_t n_remainder = n & 3;
		return std::pair<std::size_t, std::size_t>(n - n_remainder, n_remainder);
	}
	inline
	std::pair<std::size_t, std::size_t> unroll_partition_8(std::size_t n) {
		std::size_t n_remainder = n & 7;
		return std::pair<std::size_t, std::size_t>(n - n_remainder, n_remainder);
	}
	inline
	std::pair<std::size_t, std::size_t> unroll_partition_16(std::size_t n) {
		std::size_t n_remainder = n & 15;
		return std::pair<std::size_t, std::size_t>(n - n_remainder, n_remainder);
	}
}

#endif
