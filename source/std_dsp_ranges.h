
#ifndef STD_DSP_RANGES_GUARD
#define STD_DSP_RANGES_GUARD

#include <utility>

namespace std_dsp {
	using range_n_t = std::size_t;
	using range_t = std::pair<double*, range_n_t>;

	template <typename N>
	inline
	range_t make_range(double* first, N n) {
		return range_t(first, range_n_t(n));
	}

	inline
	double* range_first(const range_t& r) { return r.first; }
	inline
	range_n_t range_size(const range_t& r) { return r.second; }
	inline
	double& range_at(const range_t& r, std::size_t n) {
		assert(n < range_size(r));
		return *(range_first(r) + n);
	}

	inline
	void range_advance(range_t& r) {
		assert(range_size(r) > 0);
		++r.first;
		--r.second;
	}
}

#endif
