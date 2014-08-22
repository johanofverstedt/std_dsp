
#ifndef STD_DSP_STEREO_TRANSFORMS_GUARD
#define STD_DSP_STEREO_TRANSFORMS_GUARD

#include <cstdint>
#include <cassert>
#include <iterator>
#include <algorithm>
#include <array>

#include "../base/std_dsp_computational_basis.h"

namespace std_dsp {
	template <typename T>
	struct stereo_transform_kernel {
		template <typename I1, typename I2, typename N, typename O1, typename O2, typename Op>
		inline
		void operator()(I1 first1, I2 first2, N n, O1 out1, O2 out2, Op op) {
			assert(n >= 0);

			while (n) {
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
	};
	template <>
	struct stereo_transform_kernel<double> {
		template <typename I1, typename I2, typename N, typename O1, typename O2, typename Op>
		inline
		void operator()(I1 first1, I2 first2, N n, O1 out1, O2 out2, Op op) {
			assert(n >= 0);

			if (!is_odd_aligned(first1) && !is_odd_aligned(first2) && !is_odd_aligned(out1) && !is_odd_aligned(out2)) {
				while (n >= N(8)) {
					n -= N(8);

					vector2_t x1 = load2_from(first1, 0);
					vector2_t x2 = load2_from(first1, 2);
					vector2_t x3 = load2_from(first1, 4);
					vector2_t x4 = load2_from(first1, 6);
					vector2_t y1 = load2_from(first2, 0);
					vector2_t y2 = load2_from(first2, 2);
					vector2_t y3 = load2_from(first2, 4);
					vector2_t y4 = load2_from(first2, 6);

					first1 += 8;
					first2 += 8;

					std::pair<vector2_t, vector2_t> r1 = op(std::make_pair(x1, y1));
					std::pair<vector2_t, vector2_t> r2 = op(std::make_pair(x2, y2));
					std::pair<vector2_t, vector2_t> r3 = op(std::make_pair(x3, y3));
					std::pair<vector2_t, vector2_t> r4 = op(std::make_pair(x4, y4));

					store2_to(out1, 0, r1.first);
					store2_to(out1, 2, r2.first);
					store2_to(out1, 4, r3.first);
					store2_to(out1, 6, r4.first);
					store2_to(out2, 0, r1.second);
					store2_to(out2, 2, r2.second);
					store2_to(out2, 4, r3.second);
					store2_to(out2, 6, r4.second);

					out1 += 8;
					out2 += 8;
				}
			}

			while (n) {
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
	};

	template <typename T>
	struct stereo_transform_interleaved_kernel {
		template <typename I, typename N, typename O, typename Op>
		inline
		void operator()(I first, N n, O out, Op op) {
			while (n) {
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
	};

	template <>
	struct stereo_transform_interleaved_kernel<double> {
		template <typename I, typename N, typename O, typename Op>
		inline
		void operator()(I first, N n, O out, Op op) {

			while (n >= 8) {
				n -= 8;

				vector2_t x1 = load2_from(first, 0);
				vector2_t x2 = load2_from(first, 2);
				vector2_t x3 = load2_from(first, 4);
				vector2_t x4 = load2_from(first, 6);
				vector2_t x5 = load2_from(first, 8);
				vector2_t x6 = load2_from(first, 10);
				vector2_t x7 = load2_from(first, 12);
				vector2_t x8 = load2_from(first, 14);

				first += 16;

				x1 = op(x1);
				x2 = op(x2);
				x3 = op(x3);
				x4 = op(x4);
				x5 = op(x5);
				x6 = op(x6);
				x7 = op(x7);
				x8 = op(x8);

				store2_to(out, 0, x1);
				store2_to(out, 2, x2);
				store2_to(out, 4, x3);
				store2_to(out, 6, x4);
				store2_to(out, 8, x5);
				store2_to(out, 10, x6);
				store2_to(out, 12, x7);
				store2_to(out, 14, x8);

				out += 16;
			}

			while (n) {
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
	};

	template <typename I1, typename I2, typename N, typename O1, typename O2, typename Op>
	inline
	void stereo_transform(I1 first1, I2 first2, N n, O1 out1, O2 out2, Op op) {
		assert(n >= 0);

		stereo_transform_kernel<typename std::iterator_traits<I1>::value_type> kernel;

		while(n) {
			auto n_fast = fast_count(out2, fast_count(first1, first2, out1, n));
			if(n_fast == 0)
				break;

			auto first1_fast = get_fast_iterator(first1);
			auto first2_fast = get_fast_iterator(first2);
			auto out1_fast = get_fast_iterator(out1);
			auto out2_fast = get_fast_iterator(out2);

			kernel(first1_fast, first2_fast, n_fast, out1_fast, out2_fast, op);

			first1 += n_fast;
			first2 += n_fast;
			out1 += n_fast;
			out2 += n_fast;
			n -= n_fast;
		}

		if(n) {
			stereo_transform_kernel<bool> slow_kernel;
			slow_kernel(first1, first2, n, out1, out2, op);
		}
	}

	template <typename I, typename N, typename O, typename Op>
	inline
	void stereo_transform(I first, N n, O out, Op op) {
		assert(n >= 0);

		stereo_transform_interleaved_kernel<typename std::iterator_traits<I>::value_type> kernel;

		while(n) {
			auto n_fast = fast_count(first, out, 2*n);
			auto n_fast_half = n_fast / 2;
			if(n_fast == 0)
				break;
			auto first_fast = get_fast_iterator(first);
			auto out_fast = get_fast_iterator(out);

			kernel(first_fast, n_fast_half, out_fast, op);

			first += n_fast;
			out += n_fast;
			n -= (n_fast_half);
		}

		if(n) {
			stereo_transform_interleaved_kernel<bool> slow_kernel;
			slow_kernel(first, n, out, op);
		}
	}

	// - General operators -

	struct swap_op {
		template <typename T>
		inline
		auto operator()(const std::pair<T, T>& x) -> std::pair<T, T> {
			return std::make_pair(x.second, x.first);
		}
		inline
		vector2_t operator()(vector2_t x) {
			return rotate(x);
		}
	};
	struct duplicate_left_op {
		template <typename T>
		inline
		auto operator()(std::pair<T, T> x) -> std::pair<T, T> {
			return std::make_pair(x.first, x.first);
		}
		inline
		vector2_t operator()(vector2_t x) {
			return interleave_lo(x, x);
		}
	};
	struct duplicate_right_op {
		template <typename T>
		inline
		auto operator()(std::pair<T, T> x) -> std::pair<T, T> {
			return std::make_pair(x.second, x.second);
		}
		inline
		vector2_t operator()(vector2_t x) {
			return interleave_hi(x, x);
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
		inline
		std::pair<vector2_t, vector2_t> operator()(std::pair<vector2_t, vector2_t> x) {
			static const vector2_t half = load2_from(0.5);
			const vector2_t y = multiply(half, add(x.first, x.second));
			return std::make_pair(y, y);
		}
		inline
		vector2_t operator()(vector2_t x) {
			static const vector2_t half = load2_from(0.5);
			const vector2_t xr = rotate(x);

			const vector2_t y = multiply(half, add(x, xr));
			return y;
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
		inline
		std::pair<vector2_t, vector2_t> operator()(const std::pair<vector2_t, vector2_t>& x) {
			const vector2_t m = add(x.first, x.second);
			const vector2_t s = subtract(x.first, x.second);
			return std::make_pair(m, s);
		}
		inline
		vector2_t operator()(vector2_t x) {
			return rotate(add_hi_sub_lo(x, x));
		}		
	};
	struct mid_side_inv_op {
		inline
		std::pair<float, float> operator()(std::pair<float, float> x) {
			const float l = 0.5f * (x.first + x.second);
			const float r = 0.5f * (x.first - x.second);
			return std::make_pair(l, r);
		}
		inline
		std::pair<double, double> operator()(std::pair<double, double> x) {
			const double l = 0.5 * (x.first + x.second);
			const double r = 0.5 * (x.first - x.second);
			return std::make_pair(l, r);
		}
		inline
		std::pair<vector2_t, vector2_t> operator()(const std::pair<vector2_t, vector2_t>& x) {
			static const vector2_t half = load2_from(0.5);
			const vector2_t m = add(x.first, x.second);
			const vector2_t s = subtract(x.first, x.second);
			return std::make_pair(multiply(half, m), multiply(half, s));
		}
		inline
		vector2_t operator()(vector2_t x) {
			static const vector2_t half = load2_from(0.5);
			return multiply(half, rotate(add_hi_sub_lo(x, x)));
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
