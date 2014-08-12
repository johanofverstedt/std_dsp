
#ifndef STD_DSP_BASE_GUARD
#define STD_DSP_BASE_GUARD

#include <cstdint>
#include <algorithm>

#ifdef WIN32
#define SSE_ALIGN __declspec(align(16))
#define AVX_ALIGN __declspec(align(32))

#define constexpr
#define noexcept throw()
#else
#define SSE_ALIGN alignas(16)
#define AVX_ALIGN alignas(32)
//#define constexpr
#endif

//General processing performance queries

namespace std_dsp {
	using storage_size_t = std::int64_t;

	inline
	const double* get_ptr(const double* x) {
		return x;
	}
	inline
	double* get_ptr(double* x) {
		return x;
	}

	inline
	constexpr double* get_fast_iterator(double* x) {
		return x;
	}
	inline
	constexpr const double* get_fast_iterator(const double* x) {
		return x;
	}

	inline
	constexpr bool supports_fast_processing(const double*) {
		return true;
	}
	inline
	constexpr bool supports_fast_processing(double*) {
		return true;
	}

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

	inline
	constexpr storage_size_t fast_count(const double*, storage_size_t n) {
		return n;
	}
	inline
	constexpr storage_size_t fast_count(double*, storage_size_t n) {
		return n;
	}
	inline
	constexpr storage_size_t fast_reverse_count(const double*, storage_size_t n) {
		return n;
	}
	inline
	constexpr storage_size_t fast_reverse_count(double*, storage_size_t n) {
		return n;
	}

	template <typename T>
	inline
	constexpr storage_size_t fast_count(const T& x, storage_size_t n) {
		return (std::min)(fast_count(x, n), n);
	}
	template <typename T1, typename T2>
	inline
	constexpr storage_size_t fast_count(const T1& x, const T2& y, storage_size_t n) {
		return (std::min)(fast_count(x, n), fast_count(y, n));
	}
	template <typename T1, typename T2, typename T3>
	inline
	constexpr storage_size_t fast_count(const T1& x, const T2& y, const T3& z, storage_size_t n) {
		return (std::min)(fast_count(x, y, n), fast_count(z, n));
	}
}

#endif
