
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
	scalar_t load1_from(const scalar_t* x, N n) { return *(x + n); }

	template <typename N>
	inline
	void store1_to(scalar_t* x, N n, scalar_t value) { *(x + n) = value; } 

#ifdef STD_DSP_SSE
	using vector2_t = __m128d;

	template <typename N>
	inline
	vector2_t load2_from(const scalar_t* x, N n) { return _mm_load_pd(x + n); }
	
	inline
	vector2_t load2u_from(const scalar_t* x) { return _mm_loadu_pd(x); }
	
	template <typename N>
	inline
	vector2_t load2u_from(const scalar_t* x, N n) { return _mm_loadu_pd(x + n); }
	
	inline
	vector2_t load2_from(scalar_t x) { return _mm_set1_pd(x); }
	
	inline
	vector2_t load2_from(scalar_t x, scalar_t y) { return _mm_set_pd(x, y); }

	inline
	void store2_to(scalar_t* x, vector2_t value) { _mm_store_pd(x, value); }

	template <typename N>
	inline
	void store2_to(scalar_t* x, N n, vector2_t value) { _mm_store_pd(x + n, value); }

	inline
	void store2u_to(scalar_t* x, vector2_t value) { _mm_storeu_pd(x, value); }

	template <typename N>
	inline
	void store2u_to(scalar_t* x, N n, vector2_t value) { _mm_storeu_pd(x + n, value); }

	template <typename N>
	inline
	void store1_to(scalar_t* x, N n, vector2_t value) { _mm_store_ps(x + n, value); }

	inline
	vector2_t zero() { return _mm_setzero_pd(); }
	inline
	vector2_t negate(vector2_t x) { return _mm_sub_pd(zero(), x); }
	inline
	vector2_t add(vector2_t x, vector2_t y) { return _mm_add_pd(x, y); }
	inline
	vector2_t hadd(vector2_t x, vector2_t y) { return _mm_hadd_pd(x, y); }
	inline
	vector2_t subtract(vector2_t x, vector2_t y) { return _mm_sub_pd(x, y); }
	inline
	vector2_t multiply(vector2_t x, vector2_t y) { return _mm_mul_pd(x, y); }
	inline
	vector2_t maximum(vector2_t x, vector2_t y) { return _mm_max_pd(x, y); }
	inline
	vector2_t minimum(vector2_t x, vector2_t y) { return _mm_min_pd(x, y); }
	inline
	vector2_t rotate(vector2_t x) { return _mm_shuffle_pd(x, x, 1); }
	inline
	void swap(vector2_t& x, vector2_t& y) { vector2_t tmp = x; x = y; y = tmp; }
	inline
	vector2_t abs(vector2_t x, vector2_t sign_bit_mask) { return _mm_andnot_pd(x, sign_bit_mask); }
#else
	using vector2_t = double[2];

	template <typename N>
	inline
	vector2_t load2_from(const double* x, N n) { vector2_t r = { x[n], x[n+1] }; return r; }

	template <typename N>
	inline
	void store2_to(double* x, N n, vector2_t value) { x[n] = value[0]; x[n+1] = value[1]; } 
#endif
}

#endif
