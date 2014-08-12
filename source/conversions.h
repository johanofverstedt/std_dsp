
#ifndef STD_DSP_CONVERSIONS_GUARD
#define STD_DSP_CONVERSIONS_GUARD

#include <cstdint>
#include <iterator>

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
}

#endif
