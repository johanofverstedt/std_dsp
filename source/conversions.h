
#ifndef STD_DSP_CONVERSIONS_GUARD
#define STD_DSP_CONVERSIONS_GUARD

#include <cstdint>
#include <iterator>
#include <algorithm>
#include <array>

#include <iostream>

namespace std_dsp {
	template <typename I, typename N, typename O>
	inline
	void double_to_float(I first, N n, O out) {
		static_assert(std::is_same<typename std::iterator_traits<I>::value_type, double>::value, "Input value type is not double");
		static_assert(std::is_same<typename std::iterator_traits<O>::value_type, float>::value, "Output value type is not float");
		
		while(n) {
			*out = static_cast<float>(*first);
			--n;
			++first;
			++out;
		}
	}
	template <typename I, typename N, typename O>
	inline
	void float_to_double(I first, N n, O out) {
		static_assert(std::is_same<typename std::iterator_traits<I>::value_type, float>::value, "Input value type is not float");
		static_assert(std::is_same<typename std::iterator_traits<O>::value_type, double>::value, "Output value type is not double");
		
		while(n) {
			*out = static_cast<double>(*first);
			--n;
			++first;
			++out;
		}
	}

	//Interleaves the stereo signal from two separate source buffers into
	//a single buffer.
	//Preconditions:
	//Neither first1 nor first2 overlaps with out, but first1 and first2 may be the same
	//Example:
	//[A, B, C], [D, E, F] -> [A, D, B, E, C, F]
	template <typename I1, typename I2, typename N, typename O>
	inline
	void interleave(I1 first1, I2 first2, N n, O out) {
		while(n) {
			--n;
			
			*out = *first1;
			++first1;
			++out;

			*out = *first2;
			++first2;
			++out;
		}
	}

	//Splits an interleaved stereo signal into two separate destination buffers
	//Preconditions:
	//None of first, out1 and out2 overlaps
	//Example:
	//[A, B, C, D, E, F] -> [A, C, E], [B, D, F]
	template <typename I, typename N, typename O1, typename O2>
	inline
	void split(I first, N n, O1 out1, O2 out2) {
		while(n) {
			--n;

			*out1 = *first;
			++first;
			++out1;

			*out2 = *first;
			++first;
			++out2;
		}
	}

	template <typename I, std::int64_t MAX_N = 16LL>
	inline
	void interleave_inplace_small(I x, std::int64_t n) {
		std::array<typename std::iterator_traits<I>::value_type, 2LL * MAX_N> tmp;
		I x2 = x + n;
		std::copy_n(x, n, tmp.begin());
		interleave(tmp.begin(), x2, n, x);
	}

	template <typename I, std::int64_t THRESHOLD_N = 16LL>
	inline
	void interleave_inplace(I x, std::int64_t n) {
		std::cout << "CALL" << std::endl;
		if(n <= 1LL)
			return;
		if(n <= THRESHOLD_N) {
			interleave_inplace_small<I, THRESHOLD_N>(x, n);
			return;
		}
		std::int64_t half = n >> 1LL;
		I middle = x + n;
		I left = x + half;
		I right = middle + half;
		std::rotate(left, middle, right);
		interleave_inplace<I, THRESHOLD_N>(x, half);
		interleave_inplace<I, THRESHOLD_N>(x + (2LL * half), n - half);
	}

	template <typename I, std::int64_t MAX_N = 16LL>
	inline
	void split_inplace_small(I x, std::int64_t n) {
		std::array<typename std::iterator_traits<I>::value_type, 2LL * MAX_N> tmp;
		I x2 = x + n;
		std::copy_n(x, 2LL * n, tmp.begin());
		split(tmp.begin(), n, x, x2);
	}

	template <typename I, std::int64_t THRESHOLD_N = 16LL>
	inline
	void split_inplace(I x, std::int64_t n) {
		if(n <= 1LL)
			return;
		if(n <= THRESHOLD_N) {
			split_inplace_small<I, THRESHOLD_N>(x, n);
			return;
		}
		std::int64_t half = n >> 1LL;
		std::int64_t remainder = n - half;
		I left = x + half;
		I right = x + (n + half);
		I middle = left + half;
		split_inplace(x, half);
		split_inplace(middle, remainder);
		std::rotate(left, middle, right);
	}
}

#endif
