
#ifndef STD_DSP_STEREO_TRANSFORMS_GUARD
#define STD_DSP_STEREO_TRANSFORMS_GUARD

#include <cstdint>
#include <cassert>
#include <iterator>
#include <algorithm>
#include <array>

#include "../base/std_dsp_computational_basis.h"

namespace std_dsp {
	template <typename I1, typename I2, typename N, typename O1, typename O2, typename Op>
	inline
	void stereo_transform_inner_loop(I1 first1, I2 first2, N n, O1 out1, O2 out2, Op op) {
		assert(n >= 0);

		while(n) {
			--n;
		
			auto result = op(std::make_pair(*first1, *first2));

			++first1;
			++first2;

			*out1 = result.first;
			*out2 = result.second;

			++out1;
			++out2;
		}
	}
	
	template <typename I1, typename I2, typename N, typename O1, typename O2, typename Op>
	inline
	void stereo_transform(I1 first1, I2 first2, N n, O1 out1, O2 out2, Op op) {
		assert(n >= 0);

		while(n) {
			auto n_fast = fast_count(out2, fast_count(first1, first2, out1, n));
			if(n_fast == 0)
				break;

			auto first1_fast = get_fast_iterator(first1);
			auto first2_fast = get_fast_iterator(first2);
			auto out1_fast = get_fast_iterator(out1);
			auto out2_fast = get_fast_iterator(out2);

			stereo_transform_inner_loop(first1_fast, first2_fast, n_fast, out1_fast, out2_fast, op);

			first1 += n_fast;
			first2 += n_fast;
			out1 += n_fast;
			out2 += n_fast;
			n -= n_fast;
		}

		if(n) {
			stereo_transform_inner_loop(first1, first2, n, out1, out2, op);
		}
	}

	template <typename I, typename N, typename O, typename Op>
	inline
	void stereo_transform_inner_loop(I first, N n, O out, Op op) {
		while(n) {
			--n;
			
			auto x1 = *first;
			++first;
			auto x2 = *first;
			++first;

			auto result = op(std::make_pair(x1, x2));

			*out = result.first;
			++out;
			*out = result.second;
			++out;
		}
	}

	template <typename I, typename N, typename O, typename Op>
	inline
	void stereo_transform(I first, N n, O out, Op op) {
		assert(n >= 0);

		while(n) {
			auto n_fast = fast_count(first, out, 2*n);
			auto n_fast_half = n_fast / 2;
			if(n_fast == 0)
				break;
			auto first_fast = get_fast_iterator(first);
			auto out_fast = get_fast_iterator(out);

			stereo_transform_inner_loop(first_fast, n_fast_half, out_fast, op);

			first += n_fast;
			out += n_fast;
			n -= (n_fast_half);
		}

		if(n) {
			stereo_transform_inner_loop(first, n, out, op);
		}
	}

	// - General operators -

	struct swap_op {
		template <typename T>
		inline
		auto operator()(std::pair<T, T> x) -> std::pair<T, T> {
			return std::make_pair(x.second, x.first);
		}
	};
	struct duplicate_left_op {
		template <typename T>
		inline
		auto operator()(std::pair<T, T> x) -> std::pair<T, T> {
			return std::make_pair(x.first, x.first);
		}
	};
	struct duplicate_right_op {
		template <typename T>
		inline
		auto operator()(std::pair<T, T> x) -> std::pair<T, T> {
			return std::make_pair(x.second, x.second);
		}
	};

	// - Floating point operators -

	struct mono_op {
		inline
		std::pair<float, float> operator()(std::pair<float, float> x) {
			const auto y = 0.5f * (x.first + x.second);
			return std::make_pair(y, y);
		}
		inline
		std::pair<double, double> operator()(std::pair<double, double> x) {
			const auto y = 0.5 * (x.first + x.second);
			return std::make_pair(y, y);
		}
	};
	struct mid_side_op {
		inline
		std::pair<float, float> operator()(std::pair<float, float> x) {
			auto m = (x.first + x.second);
			auto s = (x.first - x.second);
			return std::make_pair(m, s);
		}
		inline
		std::pair<double, double> operator()(std::pair<double, double> x) {
			const double m = (x.first + x.second);
			const double s = (x.first - x.second);
			return std::make_pair(m, s);
		}
	};
	struct mid_side_inv_op {
		inline
		std::pair<float, float> operator()(std::pair<float, float> x) {
			const float l = 0.5 * (x.first + x.second);
			const float r = 0.5 * (x.first - x.second);
			return std::make_pair(l, r);
		}
		inline
		std::pair<double, double> operator()(std::pair<double, double> x) {
			const double l = 0.5 * (x.first + x.second);
			const double r = 0.5 * (x.first - x.second);
			return std::make_pair(l, r);
		}
	};
	struct phase_invert_left_op {
		inline
		std::pair<float, float> operator()(std::pair<float, float> x) {
			return std::make_pair(-x.first, x.second);
		}
		inline
		std::pair<double, double> operator()(std::pair<double, double> x) {
			return std::make_pair(-x.first, x.second);
		}
	};
	struct phase_invert_right_op {
		inline
		std::pair<float, float> operator()(std::pair<float, float> x) {
			return std::make_pair(x.first, -x.second);
		}
		inline
		std::pair<double, double> operator()(std::pair<double, double> x) {
			return std::make_pair(x.first, -x.second);
		}
	};

	// - Double precision operators -

	class width_op {
	private:
		double left_coeffs[2];
		double right_coeffs[2];
	public:
		width_op() {}
		explicit width_op(double w) {
			left_coeffs[0] = 0.5 + 0.5 * w;
			left_coeffs[1] = 0.5 - 0.5 * w;
			right_coeffs[0] = 0.5 - 0.5 * w;
			right_coeffs[1] = 0.5 + 0.5 * w;
		}

		inline
		std::pair<double, double> operator()(std::pair<double, double> x) {
			const double r1 = x.first * left_coeffs[0];
			const double r2 = x.second * left_coeffs[1];
			const double r3 = x.first * right_coeffs[0];
			const double r4 = x.second * right_coeffs[1];
			return std::make_pair(r1 + r2, r3 + r4);
		}
	};

	// - General algorithms - 

	template <typename I1, typename I2, typename N, typename O1, typename O2>
	inline
	void swap_channels(I1 first1, I2 first2, N n, O1 out1, O2 out2) {
		stereo_transform(first1, first2, n, out1, out2, swap_op());
	}
	template <typename I, typename N, typename O>
	inline
	void swap_channels(I first, N n, O out) {
		stereo_transform(first, n, out, swap_op());
	}
	template <typename I1, typename I2, typename N, typename O1, typename O2>
	inline
	void duplicate_left(I1 first1, I2 first2, N n, O1 out1, O2 out2) {
		stereo_transform(first1, first2, n, out1, out2, duplicate_left_op());
	}
	template <typename I, typename N, typename O>
	inline
	void duplicate_left(I first, N n, O out) {
		stereo_transform(first, n, out, duplicate_left_op());
	}
	template <typename I1, typename I2, typename N, typename O1, typename O2>
	inline
	void duplicate_right(I1 first1, I2 first2, N n, O1 out1, O2 out2) {
		stereo_transform(first1, first2, n, out1, out2, duplicate_right_op());
	}
	template <typename I, typename N, typename O>
	inline
	void duplicate_right(I first, N n, O out) {
		stereo_transform(first, n, out, duplicate_right_op());
	}

	// - Floating point algorithms -

	template <typename I1, typename I2, typename N, typename O1, typename O2>
	inline
	void mono(I1 first1, I2 first2, N n, O1 out1, O2 out2) {
		stereo_transform(first1, first2, n, out1, out2, mono_op());
	}
	template <typename I, typename N, typename O>
	inline
	void mono(I first, N n, O out) {
		stereo_transform(first, n, out, mono_op());
	}
	template <typename I1, typename I2, typename N, typename O1, typename O2>
	inline
	void mid_side(I1 first1, I2 first2, N n, O1 out1, O2 out2) {
		stereo_transform(first1, first2, n, out1, out2, mid_side_op());
	}
	template <typename I, typename N, typename O>
	inline
	void mid_side(I first, N n, O out) {
		stereo_transform(first, n, out, mid_side_op());
	}
	template <typename I1, typename I2, typename N, typename O1, typename O2>
	inline
	void mid_side_inv(I1 first1, I2 first2, N n, O1 out1, O2 out2) {
		stereo_transform(first1, first2, n, out1, out2, mid_side_inv_op());
	}
	template <typename I, typename N, typename O>
	inline
	void mid_side_inv(I first, N n, O out) {
		stereo_transform(first, n, out, mid_side_inv_op());
	}
	template <typename I1, typename I2, typename N, typename O1, typename O2>
	inline
	void phase_invert_left(I1 first1, I2 first2, N n, O1 out1, O2 out2) {
		stereo_transform(first1, first2, n, out1, out2, phase_invert_left_op());
	}
	template <typename I, typename N, typename O>
	inline
	void phase_invert_left(I first, N n, O out) {
		stereo_transform(first, n, out, phase_invert_left_op());
	}	
	template <typename I1, typename I2, typename N, typename O1, typename O2>
	inline
	void phase_invert_right(I1 first1, I2 first2, N n, O1 out1, O2 out2) {
		stereo_transform(first1, first2, n, out1, out2, phase_invert_right_op());
	}
	template <typename I, typename N, typename O>
	inline
	void phase_invert_right(I first, N n, O out) {
		stereo_transform(first, n, out, phase_invert_right_op());
	}

	// - Double precision algorithms -

	template <typename I1, typename I2, typename N, typename O1, typename O2>
	inline
	void width(I1 first1, I2 first2, N n, O1 out1, O2 out2, double w) {
		if(fabs(w) < 0.000001) {
			mono(first1, first2, n, out1, out2);
			return;
		}
		stereo_transform(first1, first2, n, out1, out2, width_op(w));
	}
	template <typename I, typename N, typename O>
	inline
	void width(I first, N n, O out, double w) {
		if(fabs(w) < 0.000001) {
			mono(first, n, out);
			return;
		}
		stereo_transform(first, n, out, width_op(w));
	}
}

#endif
