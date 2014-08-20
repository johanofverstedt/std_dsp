
//
//	- Conversion algorithms -
//
//	- Copy algorithms - (requires an extra buffer)
//
//	<double_to_float> : converts the double sequence into a float sequence
//  <float_to_double> : converts the float sequence into a double sequence
//
//  <interleave> : converts 2 equally sized sequences from [A, B, C], [1, 2, 3] -> [A, 1, B, 2, C, 3]
//  <deinterleave> : converts an interleaved sequence [A, 1, B, 2, C, 3] -> [A, B, C], [1, 2, 3]
//  <interleave4> : like interleave, but with 4 equally sized sequences
//  <deinterleave4> : like deinterleave, but with 4-channel interleaving
//
//	- Inplace algorithms - (may only consume O(log(n)) of additional stack space)
//
//	<interleave_inplace_with_stack_copy> : Makes a temporary copy on the stack which is then interleaved
//    into the original buffer. The maximum N it can handle must be set via template parameter
//    at compile time. Works in O(n) time and user defined constant.
//	<deinterleave_inplace_with_stack_copy> : Makes a temporary copy on the stack which is then split
//    into the original buffer. The maximum N it can handle must be set via template parameter
//    at compile time. Works in O(n) time and user defined constant.
//
//	<interleave_inplace> : Implements a memory adaptive divide-and-conquer algorithm
//    to do the operation without extra buffers in O(n*log(n)) time and O(log(n)) stack space.
//  <deinterleave_inplace> : Implements a memory adaptive divide-and-conquer algorithm
//    to do the operation without extra buffers in O(n*log(n)) time and O(log(n)) stack space.
//
//  Note: Both interleave_inplace and deinterleaved_inplace can be configured at compile time to
//    stop the recursion and switch to the *_with_stack_copy algorithm when the problem size
//    drops below a threshold. This removes the worst performance issues with these slow algorithms,
//    allowing them to perform fairly well and still be able to handle inputs of any size with
//    only constant memory.
//
//	More notes: All inplace algorithms require split sequences to reside immediately after
//    each other in the iterator space. [A, B, C], [1, 2, 3] must really be [A, B, C, 1, 2, 3].
//    This is a limitation that the copy algorithms do not have.
//
//	- Author: Johan Öfverstedt, 2014 -
//

#ifndef STD_DSP_CONVERSIONS_GUARD
#define STD_DSP_CONVERSIONS_GUARD

#include <cstdint>
#include <cassert>
#include <iterator>
#include <algorithm>
#include <array>

#include "../base/std_dsp_computational_basis.h"

namespace std_dsp {
	namespace detail {
		template <typename T>
		struct interleave_op {
			template <typename I1, typename I2, typename N, typename O>
			void operator()(I1 first1, I2 first2, N n, O out) {
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
		};
		template <>
		struct interleave_op<double> {
			template <typename I1, typename I2, typename N, typename O>
			void operator()(I1 first1, I2 first2, N n, O out) {
				if(check_alignment(first1, first2, out) && !is_odd_aligned(first1) && !is_odd_aligned(first2) && !is_odd_aligned(out)) {
					while(n >= 8) {
						n -= 8;
			
						vector2_t x1 = load2_from(first1, 0);
						vector2_t x2 = load2_from(first1, 2);
						vector2_t x3 = load2_from(first1, 4);
						vector2_t x4 = load2_from(first1, 6);
						vector2_t y1 = load2_from(first2, 0);
						vector2_t y2 = load2_from(first2, 2);
						vector2_t y3 = load2_from(first2, 4);
						vector2_t y4 = load2_from(first2, 6);

						vector2_t lo1 = interleave_lo(x1, y1);
						vector2_t hi1 = interleave_hi(x1, y1);
						vector2_t lo2 = interleave_lo(x2, y2);
						vector2_t hi2 = interleave_hi(x2, y2);
						vector2_t lo3 = interleave_lo(x3, y3);
						vector2_t hi3 = interleave_hi(x3, y3);
						vector2_t lo4 = interleave_lo(x4, y4);
						vector2_t hi4 = interleave_hi(x4, y4);

						store2_to(out, 0, lo1);
						store2_to(out, 2, hi1);
						store2_to(out, 4, lo2);
						store2_to(out, 6, hi2);
						store2_to(out, 8, lo3);
						store2_to(out, 10, hi3);
						store2_to(out, 12, lo4);
						store2_to(out, 14, hi4);

						first1 += 8;
						first2 += 8;
						out += 16;
					}
				}

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
		};
	}

	//Interleaves the stereo signal from two separate source buffers into
	//a single buffer.
	//Preconditions:
	//Neither first1 nor first2 overlaps with out's first half, but first1 and first2 may be the same
	//Example:
	//[A, B, C], [D, E, F] -> [A, D, B, E, C, F]
	template <typename I1, typename I2, typename N, typename O>
	// I1 models InputIterator
	// I2 models InputIterator
	// N models Integral
	// O models OutputIterator
	inline
	void interleave(I1 first1, I2 first2, N n, O out) {
		static_assert(std::is_integral<N>::value, "Count not integral.");
		static_assert(std::is_same<typename std::iterator_traits<I1>::value_type, typename std::iterator_traits<I2>::value_type>::value,
			"Mismatching value types.");
		static_assert(std::is_same<typename std::iterator_traits<I1>::value_type, typename std::iterator_traits<O>::value_type>::value,
			"Mismatching value types.");

		detail::interleave_op<typename std::iterator_traits<I1>::value_type> op;
		op(first1, first2, n, out);
	}

	template <typename I1, typename I2, typename N, typename O, typename Op>
	// I1 models InputIterator
	// I2 models InputIterator
	// N models Integral
	// O models OutputIterator
	// Op models UnaryFunction
	inline
	void interleave(I1 first1, I2 first2, N n, O out, Op op) {
		static_assert(std::is_integral<N>::value, "Count not integral.");
		static_assert(std::is_same<typename std::iterator_traits<I1>::value_type, typename std::iterator_traits<I2>::value_type>::value,
			"Input iterators have different value type.");
		static_assert(std::is_same<typename std::iterator_traits<O>::value_type, decltype(op(*first1))>::value,
			"Op return type not matching the value type of output.");

		while(n) {
			--n;
			
			*out = op(*first1);
			++first1;
			++out;

			*out = op(*first2);
			++first2;
			++out;
		}
	}

	template <typename I1, typename I2, typename N, typename O>
	// I1 models InputIterator
	// I2 models InputIterator
	// N models Integral
	// O models OutputIterator
	inline
	bool check_is_interleaving(I1 first1, I2 first2, N n, O out) {
		while(n) {
			--n;
			
			if(*out != *first1)
				return false;
			++first1;
			++out;

			if(*out != *first2)
				return false;
			++first2;
			++out;
		}
		return true;
	}

	//Interleaves the stereo signal from two separate source buffers into
	//a single buffer.
	//Preconditions:
	//Neither first1 nor first2 overlaps with out's first half, but first1 and first2 may be the same
	//Example:
	//[A, B, C], [D, E, F] -> [A, D, B, E, C, F]
	template <typename I1, typename I2, typename I3, typename I4, typename N, typename O>
	// I1 models InputIterator
	// I2 models InputIterator
	// I3 models InputIterator
	// I4 models InputIterator
	// N models Integral
	// O models OutputIterator
	inline
	void interleave4(I1 first1, I2 first2, I3 first3, I4 first4, N n, O out) {
		static_assert(std::is_integral<N>::value, "Count not integral.");
		static_assert(std::is_same<typename std::iterator_traits<I1>::value_type, typename std::iterator_traits<I2>::value_type>::value,
			"Mismatching value types.");
		static_assert(std::is_same<typename std::iterator_traits<I1>::value_type, typename std::iterator_traits<O>::value_type>::value,
			"Mismatching value types.");

		while(n) {
			--n;
			
			*out = *first1;
			++first1;
			++out;

			*out = *first2;
			++first2;
			++out;

			*out = *first3;
			++first3;
			++out;

			*out = *first4;
			++first4;
			++out;
		}
	}	

	namespace detail {
		template <typename T>
		struct deinterleave_op {
			template <typename I, typename N, typename O1, typename O2>
			void operator()(I first, N n, O1 out1, O2 out2) {
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
		};
		template <>
		struct deinterleave_op<double> {
			template <typename I, typename N, typename O1, typename O2>
			void operator()(I first, N n, O1 out1, O2 out2) {
				if(check_alignment(first, out1, out2) && !is_odd_aligned(first) && !is_odd_aligned(out1) && !is_odd_aligned(out2)) {
					while(n >= 8) {
						n -= 8;
			
						vector2_t x1 = load2_from(first, 0);
						vector2_t x2 = load2_from(first, 2);
						vector2_t x3 = load2_from(first, 4);
						vector2_t x4 = load2_from(first, 6);
						vector2_t x5 = load2_from(first, 8);
						vector2_t x6 = load2_from(first, 10);
						vector2_t x7 = load2_from(first, 12);
						vector2_t x8 = load2_from(first, 14);

						vector2_t lo1 = interleave_lo(x1, x2);
						vector2_t hi1 = interleave_hi(x1, x2);
						vector2_t lo2 = interleave_lo(x3, x4);
						vector2_t hi2 = interleave_hi(x3, x4);
						vector2_t lo3 = interleave_lo(x5, x6);
						vector2_t hi3 = interleave_hi(x5, x6);
						vector2_t lo4 = interleave_lo(x7, x8);
						vector2_t hi4 = interleave_hi(x7, x8);

						store2_to(out1, 0, lo1);
						store2_to(out1, 2, lo2);
						store2_to(out1, 4, lo3);
						store2_to(out1, 6, lo4);
						store2_to(out2, 0, hi1);
						store2_to(out2, 2, hi2);
						store2_to(out2, 4, hi3);
						store2_to(out2, 6, hi4);

						first += 16;
						out1 += 8;
						out2 += 8;
					}
				}

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
		};
	}

	//Splits an interleaved stereo signal into two separate destination buffers
	//Preconditions:
	//None of first, out1 and out2 overlaps
	//Example:
	//[A, B, C, D, E, F] -> [A, C, E], [B, D, F]
	template <typename I, typename N, typename O1, typename O2>
	// I models InputIterator
	// N models Integral
	// O1 models OutputIterator
	// O2 models OutputIterator
	inline
	void deinterleave(I first, N n, O1 out1, O2 out2) {
		static_assert(std::is_integral<N>::value, "Count not integral.");
		static_assert(std::is_same<typename std::iterator_traits<O1>::value_type, typename std::iterator_traits<O2>::value_type>::value,
			"Mismatching value types.");
		static_assert(std::is_same<typename std::iterator_traits<I>::value_type, typename std::iterator_traits<O1>::value_type>::value,
			"Mismatching value types.");

		detail::deinterleave_op<typename std::iterator_traits<I>::value_type> op;
		op(first, n, out1, out2);
	}

	template <typename I, typename N, typename O1, typename O2, typename Op>
	// I models InputIterator
	// N models Integral
	// O1 models OutputIterator
	// O2 models OutputIterator
	// Op models UnaryFunction
	inline
	void deinterleave(I first, N n, O1 out1, O2 out2, Op op) {
		static_assert(std::is_integral<N>::value, "Count not integral.");
		static_assert(std::is_same<typename std::iterator_traits<O1>::value_type, typename std::iterator_traits<O2>::value_type>::value,
			"Input iterators have different value type.");
		static_assert(std::is_same<typename std::iterator_traits<O1>::value_type, decltype(op(*first))>::value,
			"Op return type not matching the value type of output.");

		while(n) {
			--n;

			*out1 = op(*first);
			++first;
			++out1;

			*out2 = op(*first);
			++first;
			++out2;
		}
	}

	template <typename I, typename N, typename O1, typename O2, typename O3, typename O4>
	// I models InputIterator
	// N models Integral
	// O1 models OutputIterator
	// O2 models OutputIterator
	// O3 models OutputIterator
	// O4 models OutputIterator
	inline
	void deinterleave4(I first, N n, O1 out1, O2 out2, O3 out3, O4 out4) {
		static_assert(std::is_integral<N>::value, "Count not integral.");
		static_assert(std::is_same<typename std::iterator_traits<O1>::value_type, typename std::iterator_traits<O2>::value_type>::value,
			"Mismatching value types.");
		static_assert(std::is_same<typename std::iterator_traits<I>::value_type, typename std::iterator_traits<O1>::value_type>::value,
			"Mismatching value types.");

		while(n) {
			--n;

			*out1 = *first;
			++first;
			++out1;

			*out2 = *first;
			++first;
			++out2;

			*out3 = *first;
			++first;
			++out3;

			*out4 = *first;
			++first;
			++out4;
		}
	}

	template <typename I, std::int64_t MAX_N = 16LL>
	// I models ForwardIterator
	inline
	void interleave_inplace_with_stack_copy(I x, std::int64_t n) {
		using value_type = typename std::iterator_traits<I>::value_type;
		static_assert(MAX_N >= 1, "MAX_N must be greater than or equal to 1.");

		assert(n >= 0);
		assert(n <= MAX_N);

		std::array<value_type, MAX_N> tmp;
		I x2 = x + n;
		std::copy_n(x, n, tmp.begin());
		interleave(tmp.begin(), x2, n, x);
	}

	template <typename I, std::int64_t THRESHOLD_N = 64LL>
	// I models RandomAccessIterator
	inline
	void interleave_inplace(I x, std::int64_t n) {
		if(n <= 1LL)
			return;
		if(n <= THRESHOLD_N) {
			interleave_inplace_with_stack_copy<I, THRESHOLD_N>(x, n);
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
	// I models ForwardIterator
	inline
	void deinterleave_inplace_with_stack_copy(I x, std::int64_t n) {
		using value_type = typename std::iterator_traits<I>::value_type;
		static_assert(MAX_N >= 1, "MAX_N must be greater than or equal to 1.");

		assert(n >= 0);
		assert(n <= MAX_N);

		std::array<value_type, 2LL * MAX_N> tmp;
		I x2 = x + n;
		std::copy_n(x, 2LL * n, tmp.begin());
		deinterleave(tmp.begin(), n, x, x2);
	}

	template <typename I, std::int64_t THRESHOLD_N = 64LL>
	// I models RandomAccessIterator
	inline
	void deinterleave_inplace(I x, std::int64_t n) {
		if(n <= 1LL)
			return;
		if(n <= THRESHOLD_N) {
			deinterleave_inplace_with_stack_copy<I, THRESHOLD_N>(x, n);
			return;
		}
		std::int64_t half = n >> 1LL;
		std::int64_t remainder = n - half;
		I left = x + half;
		I right = left + n;//x + (n + half);
		I middle = left + half;
		deinterleave_inplace(x, half);
		deinterleave_inplace(middle, remainder);
		std::rotate(left, middle, right);
	}
}

#endif
