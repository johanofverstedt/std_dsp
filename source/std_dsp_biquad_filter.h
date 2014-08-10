
#ifndef STD_DSP_BIQUAD_FILTER_GUARD
#define STD_DSP_BIQUAD_FILTER_GUARD

#include "base/std_dsp_mem.h"

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

	template <std::size_t CHANNELS>
	struct biquad_state {
		SSE_ALIGN  double x[2 * CHANNELS];

		void reset() {
			for(std::size_t i = 0; i < 2 * CHANNELS; ++i) {
				x[i] = 0.0;
			}
		}
		void undenormalize() {
			std::size_t n = 2 * CHANNELS;
			for(std::size_t i = 0; i < n; ++i) {
				if(fabs(x[i]) < 1.e-15)
                    x[i] = 0.0;
			}
		}
	};

	template <>
	struct biquad_state<2> {
		__m128d w1;
		__m128d w2;

		biquad_state() {
			reset();
		}

		inline
		void reset() {
            double z = 0.0;
			w1 = _mm_loaddup_pd(&z);
			w2 = _mm_loaddup_pd(&z);
		}
		void undenormalize() {
//			std::size_t n = 2 * CHANNELS;
//			for(std::size_t i = 0; i < 2 * CHANNELS; ++i) {
//				if(fabs(x[i]) < 1.e-15)
//                    x[i] = 0.0;
//			}
		}
	};

	template <std::size_t CHANNELS>
	class biquad_filter {
	private:
		//biquad_state<CHANNELS>
	};

	template <std::size_t CHANNELS, typename I, typename N, typename O>
	inline
	biquad_state<CHANNELS> biquad(I first, N n, O out, biquad_coeffs c, biquad_state<CHANNELS> s) {
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
