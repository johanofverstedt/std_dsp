
#ifndef STD_DSP_ALIGNMENT_GUARD
#define STD_DSP_ALIGNMENT_GUARD

#include <iostream>
#include <cstdint>
#include <tuple>
#include "../std_dsp_ranges.h"

#include "base.h"

namespace std_dsp {
	inline
	std::size_t get_alignment(const double* first) {
		return reinterpret_cast<std::size_t>(first) & 15;
	}

	template <typename I>
	inline
	bool is_aligned(I first) {
		std::size_t alignment = get_alignment(first);
		return (alignment & 15) == 0;
	}

	template <typename I>
	inline
	bool is_odd_aligned(I first) {
		std::size_t alignment = get_alignment(first);
		return (alignment & 15) == 8;
	}

	template <typename I>
	inline
	bool check_alignment(I first) {
		std::size_t alignment = get_alignment(first);
		return alignment == 0 || alignment == 8;
	}
	template <typename I1, typename I2>
	inline
	bool check_alignment(I1 first1, I2 first2) {
		std::size_t alignment1 = get_alignment(first1);
		std::size_t alignment2 = get_alignment(first2);
		if(alignment1 != 0 && alignment1 != 8)
			return false;
		return alignment1 == alignment2;
	}
	template <typename I1, typename I2, typename I3>
	inline
	bool check_alignment(I1 first1, I2 first2, I3 first3) {
		std::size_t alignment1 = get_alignment(first1);
		std::size_t alignment2 = get_alignment(first2);
		std::size_t alignment3 = get_alignment(first3);
		if(alignment1 != 0 && alignment1 != 8)
			return false;
		return alignment1 == alignment2 && alignment1 == alignment3;
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
