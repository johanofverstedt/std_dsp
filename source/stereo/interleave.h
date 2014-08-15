
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

#include "binary_counter.h"

namespace std_dsp {
	template <typename I, typename N, typename O>
	// I models InputIterator (ValueType(I) = double)
	// N models Integral
	// O models OutputIterator (ValueType(O) = float)
	inline
	void double_to_float(I first, N n, O out) {
		static_assert(std::is_integral<N>::value, "Count not integral.");
		static_assert(std::is_same<typename std::iterator_traits<I>::value_type, double>::value, "Input value type is not double.");
		static_assert(std::is_same<typename std::iterator_traits<O>::value_type, float>::value, "Output value type is not float.");
		
		while(n) {
			*out = static_cast<float>(*first);
			--n;
			++first;
			++out;
		}
	}
	template <typename I, typename N, typename O>
	// I models InputIterator (ValueType(I) = float)
	// N models Integral
	// O models OutputIterator (ValueType(O) = double)
	inline
	void float_to_double(I first, N n, O out) {
		static_assert(std::is_integral<N>::value, "Count not integral.");
		static_assert(std::is_same<typename std::iterator_traits<I>::value_type, float>::value, "Input value type is not float.");
		static_assert(std::is_same<typename std::iterator_traits<O>::value_type, double>::value, "Output value type is not double.");
		
		while(n) {
			*out = static_cast<double>(*first);
			--n;
			++first;
			++out;
		}
	}

	struct double_to_float_op {
		inline
		float operator()(double x) {
			return static_cast<float>(x);
		}
	};
	struct float_to_double_op {
		inline
		double operator()(float x) {
			return static_cast<double>(x);
		}
	};

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

	template <typename I>
	void swap_sequences(I first1, I first2, std::int64_t n) {
		while(n) {
			--n;
			std::swap(*first1, *first2);
			++first1;
			++first2;
		}
	}

	template <typename I>
	struct counter_value {
		I first1;
		I first2;
		std::int64_t n;

		counter_value() {}
		counter_value(I first1, I first2, std::int64_t n) : first1(first1), first2(first2), n(n) {}

		inline
		friend
		bool operator==(const counter_value& x, const counter_value& y) {
			return x.first1 == y.first1 && x.first2 == y.first2 && x.n == y.n;
		}
		inline
		friend
		bool operator!=(const counter_value& x, const counter_value& y) {
			return !(x == y);
		}
	};

	template <typename I>
	struct interleave_op {
		inline
		counter_value<I> operator()(counter_value<I> x, counter_value<I> y) {
			if(x.n == y.n)
				swap_sequences(x.first2, y.first1, x.n);
			else
				std::rotate(y.first1, y.first1 + y.n, x.first2 + x.n);
			return counter_value<I>(x.first1, x.first2, x.n + y.n);
		}		
	};

	template <typename I>
	inline
	void interleave_inplace_with_counter(I first, std::int64_t n) {
		I end = first + (2*n);
		I first2 = first + n;

		binary_counter<interleave_op<I>, counter_value<I>> counter(interleave_op<I>(), counter_value<I>(end, end, 0));

		while(n) {
			--n;
			counter.add(counter_value<I>(first, first2, 1));
			++first;
			++first2;
		}

		counter.reduce();
	}
}

#endif
