
#ifndef STD_DSP_BIQUAD_FILTER_GUARD
#define STD_DSP_BIQUAD_FILTER_GUARD

#include "base/base.h"
#include "base/std_dsp_mem.h"
#include "base/computational_basis.h"

#include <cstdint>
#include <type_traits>

//SSE headers
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>

namespace std_dsp {
	struct biquad_coeffs {
		double a1, a2;
		double b0, b1, b2;
	};

	template <integer_t CHANNELS>
	struct biquad_state {
		double w1[CHANNELS];
		double w2[CHANNELS];

		void reset() {
			for(integer_t i = 0; i < CHANNELS; ++i) {
				w1[i] = 0.0;
				w2[i] = 0.0;
			}
		}
		void undenormalize() {
			for(integer_t i = 0; i < CHANNELS; ++i) {
				if(fabs(w1[i]) < 1.e-15)
                    w1[i] = 0.0;
				if(fabs(w2[i]) < 1.e-15)
                    w2[i] = 0.0;
			}
		}
	};

	template <std::size_t CHANNELS>
	class biquad_filter {
	private:
		//biquad_state<CHANNELS>
	};

	template <integer_t CHANNELS>
	struct biquad_op;

	template <>
	struct biquad_op<1> {
		template <typename I, typename N, typename O>
		biquad_state<1> operator()(I first, N n, O out, biquad_coeffs c, biquad_state<1> s) {
			double w1 = s.w1[0];
			double w2 = s.w2[0];
			while(n) {
				--n;

				const double w0 = *first - c.a1 * w1 - c.a2 * w2;
				*out = c.b0 * w0 + c.b1 * w1 + c.b2 * w2;

				w2 = w1;
				w1 = w0;

				++first;
				++out;
			}

			s.w1 = w1;
			s.w2 = w2;
			s.undenormalize();
			return s;
		}
	};

	template <>
	struct biquad_op<2> {
		template <typename I, typename N, typename O>
		biquad_state<2> operator()(I first, N n, O out, biquad_coeffs c, biquad_state<2> s) {
			vector2_t w1 = load2_from(s.w1[0], s.w1[1]);;
			vector2_t w2 = load2_from(s.w2[0], s.w2[1]);;
			
			vector2_t a1 = load2_from(c.a1);
			vector2_t a2 = load2_from(c.a2);
			vector2_t b0 = load2_from(c.b0);
			vector2_t b1 = load2_from(c.b1);
			vector2_t b2 = load2_from(c.b2);

			while(n) {
				--n;

				vector2_t input = load2_from(first);

				vector2_t w0 = subtract(input, subtract(multiply(a1, w1), multiply(a2, w2)));
				vector2_t result = add(multiply(b0, w0), add(multiply(b1, w1), multiply(b2, w2)));

				w2 = w1;
				w1 = w0;

				store2_to(out, result);

				first += 2;
				out += 2;
			}

			s.w1 = w1;
			s.w2 = w2;
			s.undenormalize();
			return s;
		}
	};

	template <std::size_t CHANNELS, typename I, typename N, typename O>
	inline
	biquad_state<CHANNELS> biquad(I first, N n, O out, biquad_coeffs c, biquad_state<CHANNELS> s) {
		biquad_op<CHANNELS> op;
		return op(first, n, out, c, s);

		if(CHANNELS == 1) {
			while(n) {
				--n;

				const double w1 = s.x[0];
				const double w2 = s.x[1];
				const double w0 = *first - c.a1 * w1 - c.a2 * w2;
				*out = c.b0 * w0 + c.b1 * w1 + c.b2 * w2;

				s.x[0] = w0;
				s.x[1] = w1;

				++first;
				++out;
			}
		} else if(CHANNELS == 2) {
			_MM_SET_FLUSH_ZERO_MODE (_MM_FLUSH_ZERO_ON);

			__m128d _a1, _a2;
			__m128d _b0, _b1, _b2;
			__m128d _w0, _w1, _w2;
			_a1 = _mm_loaddup_pd(&c.a1);
			_a2 = _mm_loaddup_pd(&c.a2);
			_b0 = _mm_loaddup_pd(&c.b0);
			_b1 = _mm_loaddup_pd(&c.b1);
			_b2 = _mm_loaddup_pd(&c.b2);

			_w1 = _mm_loaddup_pd(&s.x[0]);
			_w2 = _mm_loaddup_pd(&s.x[2]);
			
			__m128d _tmp_0, _tmp_1, _tmp_2;

			while(n) {
				--n;

				_tmp_1 = _mm_mul_pd(_a1, _w1);
				_tmp_2 = _mm_mul_pd(_a2, _w2);

				_w0 = _mm_load_pd(first);
				_w0 = _mm_sub_pd(_w0, _tmp_1);
				_w0 = _mm_sub_pd(_w0, _tmp_2);

				_tmp_1 = _mm_mul_pd(_b1, _w1);
				_tmp_2 = _mm_mul_pd(_b2, _w2);
				_tmp_0 = _mm_mul_pd(_b0, _w0);

				_tmp_1 = _mm_add_pd(_tmp_1, _tmp_2);
				_tmp_0 = _mm_add_pd(_tmp_0, _tmp_1);

				_w2 = _w1;
				_w1 = _w0;

				_mm_store_pd(out, _tmp_0);

				++first;
				++out;
			}

			_mm_store_pd(&s.x[0], _w1);
			_mm_store_pd(&s.x[2], _w2);

			_MM_SET_FLUSH_ZERO_MODE (_MM_FLUSH_ZERO_OFF);
		} else {
			while(n) {
				--n;

				for(N i = 0; i < CHANNELS; ++i) {
					const double w1 = s.x[i * 2];
					const double w2 = s.x[i * 2 + 1];
					const double w0 = *first - c.a1 * w1 - c.a2 * w2;
					*out = c.b0 * w0 + c.b1 * w1 + c.b2 * w2;

					s.x[i * 2] = w0;
					s.x[i * 2 + 1] = w1;

					++first;
					++out;
				}
			}
		}
		
		s.undenormalize();
		return s;
	}
/*
	template <std::size_t CHANNELS, typename I, typename N, typename O>
	inline
	biquad_state<CHANNELS> biquad(I first, N n, O out, biquad_coeffs c, biquad_state<CHANNELS> s) {
		while(n) {
			--n;

			for(N i = 0; i < CHANNELS; ++i) {
				const double w1 = s.x[i * 2];
				const double w2 = s.x[i * 2 + 1];
				const double w0 = *first - c.a1 * w1 - c.a2 * w2;
				*out = c.b0 * w0 + c.b1 * w1 + c.b2 * w2;

				s.x[i * 2] = w0;
				s.x[i * 2 + 1] = w1;
			}
		}
	}
*/
	/*

        size_t index_offset = 0;
        
        while(param_frames > 0) {
            
            size_t local_frames = MIN(param_frames, 8192);
            
            for(size_t i = 0; i < local_frames; ++i) {
                
                const double w0 = param_input[index_offset + i] - a1 * w1 - a2 * w2;
                param_output[index_offset + i] = b0 * w0 + b1 * w1 + b2 * w2;
                
                w2 = w1;
                w1 = w0;
                
            }
            
            param_frames -= local_frames;
            
            undenormalise(w1);
            undenormalise(w2);
            
            index_offset += local_frames;
            
        }
        */
}

#endif
