
#ifndef STD_DSP_TERNARY_TRANSFORMS_GUARD
#define STD_DSP_TERNARY_TRANSFORMS_GUARD

#include "../../base/std_dsp_computational_basis.h"

#include "functors.h"

namespace std_dsp {
	template <typename N, typename Op>
	inline
	void ternary_transform(const double* first1, const double* first2, const double* first3, N n, double* out, Op op) {
		std::pair<std::size_t, std::size_t> partitions = unroll_partition_8(n);
		while(partitions.first) {
			vector2_t x10 = load2_from(first1, 0);
			vector2_t x11 = load2_from(first1, 2);
			vector2_t x12 = load2_from(first1, 4);
			vector2_t x13 = load2_from(first1, 6);
			vector2_t x20 = load2_from(first2, 0);
			vector2_t x21 = load2_from(first2, 2);
			vector2_t x22 = load2_from(first2, 4);
			vector2_t x23 = load2_from(first2, 6);
			vector2_t x30 = load2_from(first3, 0);
			vector2_t x31 = load2_from(first3, 2);
			vector2_t x32 = load2_from(first3, 4);
			vector2_t x33 = load2_from(first3, 6);

			partitions.first -= 8;

			x10 = op(x10, x20, x30);
			x11 = op(x11, x21, x31);
			x12 = op(x12, x22, x32);
			x13 = op(x13, x23, x33);

			store2_to(out, 0, x10);
			store2_to(out, 2, x11);
			store2_to(out, 4, x12);
			store2_to(out, 6, x13);

			first1 += 8;
			first2 += 8;
			first3 += 8;
			out += 8;
		}

		while(partitions.second) {
			--partitions.second;
			*out = op(*first1, *first2, *first3);
			++first1;
			++first2;
			++first3;
			++out;
		}
	}

	struct multiply_add_op {
		inline
		double operator()(double x1, double x2, double x3) {
			return x3 + x1 * x2;
		}

		inline
		vector2_t operator()(vector2_t x1, vector2_t x2, vector2_t x3) {
			return add(x3, multiply(x1, x2));
		}
	};

	template <typename N>
	inline
	void multiply_add(const double* first1, const double* first2, N n, double* out) {
		ternary_transform(first1, first2, out, n, out, multiply_add_op());
	}

	struct linear_combination_add_op {
		double a1;
		double a2;
		vector2_t a1_v;
		vector2_t a2_v;

		linear_combination_add_op(double a1, double a2)
		: a1(a1), a2(a2), a1_v(_mm_set_pd(a1, a1)), a2_v(_mm_set_pd(a2, a2)) {}

		inline
		double operator()(double x1, double x2, double x3) {
			return x3 + a1 * x1 + a2 * x2;
		}

		inline
		vector2_t operator()(vector2_t x1, vector2_t x2, vector2_t x3) {
			vector2_t m1 = multiply(x1, a1_v);
			vector2_t m2 = multiply(x2, a2_v);
			return add(x3, add(m1, m2));
		}
	};

	template <typename N>
	inline
	void linear_combination_add(const double* first1, const double* first2, N n, double* out, double a1, double a2) {
		ternary_transform(first1, first2, out, n, out, multiply_add_op());
	}

	template <typename N>
	inline
	void mix_add(const double* first1, const double* first2, N n, double* out, double frac) {
		linear_combination_add(first1, first2, n, out, 1.0 - frac, frac);
	}
}

#endif
