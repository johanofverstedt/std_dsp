
#ifndef STD_DSP_PROCESS_BASE_GUARD
#define STD_DSP_PROCESS_BASE_GUARD

#include <algorithm>

#include "../base/std_dsp_mem.h"
#include "../base/base.h"

namespace std_dsp {
	template <typename T1, typename T2>
	inline
	constexpr bool supports_fast_processing(const T1& x, const T2& y) {
		return supports_fast_processing(x) && supports_fast_processing(y);
	}
	template <typename T1, typename T2, typename T3>
	inline
	constexpr bool supports_fast_processing(const T1& x, const T2& y, const T3& z) {
		return supports_fast_processing(x, y) && supports_fast_processing(z);
	}

	template <typename T>
	inline
	constexpr storage_size_t fast_count(const T& x, storage_size_t n) {
		return (std::min)(fast_count(x), n);
	}
	template <typename T1, typename T2>
	inline
	constexpr storage_size_t fast_count(const T1& x, const T2& y, storage_size_t n) {
		return (std::min)(fast_count(x, n), fast_count(y));
	}
	template <typename T1, typename T2, typename T3>
	inline
	constexpr storage_size_t fast_count(const T1& x, const T2& y, const T3& z, storage_size_t n) {
		return (std::min)(fast_count(x, y, n), fast_count(z));
	}
}

#endif
