
#ifndef STD_DSP_COMPUTATIONAL_BASIS_GUARD
#define STD_DSP_COMPUTATIONAL_BASIS_GUARD

#define STD_DSP_SSE

#ifdef STD_DSP_SSE
#include <emmintrin.h>
#include <pmmintrin.h>
#endif

#include "std_dsp_mem.h"

namespace std_dsp {
	using scalar_t = double;

	template <typename N>
	inline
	scalar_t load1(const scalar_t* x, N n) { return *(x + n); }

	template <typename N>
	inline
	void store1(scalar_t* x, N n, scalar_t value) { *(x + n) = value; } 

#ifdef STD_DSP_SSE
	using double2_t = __m128d;

	template <typename N>
	inline
	double2_t load2(const scalar_t* x, N n) { return _mm_load_pd(x + n); }
	
	inline
	double2_t load2u(const scalar_t* x) { return _mm_loadu_pd(x); }
	
	template <typename N>
	inline
	double2_t load2u(const scalar_t* x, N n) { return _mm_loadu_pd(x + n); }
	
	inline
	double2_t load2(scalar_t x) { return _mm_set1_pd(x); }
	
	inline
	double2_t load2(scalar_t x, scalar_t y) { return _mm_set_pd(x, y); }

	inline
	void store2(scalar_t* x, double2_t value) { _mm_store_pd(x, value); }

	template <typename N>
	inline
	void store2(scalar_t* x, N n, double2_t value) { _mm_store_pd(x + n, value); }

	inline
	void store2u(scalar_t* x, double2_t value) { _mm_storeu_pd(x, value); }

	template <typename N>
	inline
	void store2u(scalar_t* x, N n, double2_t value) { _mm_storeu_pd(x + n, value); }

	template <typename N>
	inline
	void store1(scalar_t* x, N n, double2_t value) { _mm_store_ps(x + n, value); }

	inline
	double2_t zero() { return _mm_setzero_pd(); }
	inline
	double2_t negate(double2_t x) { return _mm_sub_pd(zero(), x); }
	inline
	double2_t add(double2_t x, double2_t y) { return _mm_add_pd(x, y); }
	inline
	double2_t hadd(double2_t x, double2_t y) { return _mm_hadd_pd(x, y); }
	inline
	double2_t add_hi_sub_lo(double2_t x, double2_t y) { return _mm_addsub_pd(x, y); }
	inline
	double2_t interleave_lo(double2_t x, double2_t y) { return _mm_unpacklo_pd(x, y); }
	inline
	double2_t interleave_hi(double2_t x, double2_t y) { return _mm_unpackhi_pd(x, y); }
	inline
	double2_t subtract(double2_t x, double2_t y) { return _mm_sub_pd(x, y); }
	inline
	double2_t multiply(double2_t x, double2_t y) { return _mm_mul_pd(x, y); }
	inline
	double2_t maximum(double2_t x, double2_t y) { return _mm_max_pd(x, y); }
	inline
	double2_t minimum(double2_t x, double2_t y) { return _mm_min_pd(x, y); }
	inline
	double2_t rotate(double2_t x) { return _mm_shuffle_pd(x, x, 1); }
	inline
	void swap(double2_t& x, double2_t& y) { double2_t tmp = x; x = y; y = tmp; }
	inline
	double2_t abs(double2_t x, double2_t sign_bit_mask) { return _mm_andnot_pd(x, sign_bit_mask); }
#else
	using double2_t = double[2];

	template <typename N>
	inline
	double2_t load2(const double* x, N n) { double2_t r = { x[n], x[n+1] }; return r; }

	template <typename N>
	inline
	void store2(double* x, N n, double2_t value) { x[n] = value[0]; x[n+1] = value[1]; } 
#endif
}

#endif
