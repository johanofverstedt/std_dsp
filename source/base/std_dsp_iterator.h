
#ifndef STD_DSP_ITERATOR_GUARD
#define STD_DSP_ITERATOR_GUARD

#include <cstdint>
#include <iterator>
#include <iterator_traits>

namespace std_dsp {
	template <typename I>
	inline
	typename std::iterator_traits<I>::difference_type
	channels(I it) {
		return it.channels();
	}

	template <>
	inline
	std::int64_t channels<double*>(double* it) {
		return 1;
	}

	template <typename I, typename N>
	inline
	N fast_count(I it, N n) {
		return n;
	}
}

#endif
