
#ifndef STD_DSP_BINARY_TRANSFORMS_GUARD
#define STD_DSP_BINARY_TRANSFORMS_GUARD

#include "../../base/std_dsp_computational_basis.h"

#include "functors.h"

namespace std_dsp {
	template <typename I1, typename I2, typename N, typename O, typename Op>
	inline
	void binary_transform(I1 first1, I2 first2, N n, O out, Op op) {
		std::pair<std::size_t, std::size_t> partitions = unroll_partition_8(n);
		while(partitions.first) {
			double2_t x10 = load2(first1, 0);
			double2_t x11 = load2(first1, 2);
			double2_t x12 = load2(first1, 4);
			double2_t x13 = load2(first1, 6);
			double2_t x20 = load2(first2, 0);
			double2_t x21 = load2(first2, 2);
			double2_t x22 = load2(first2, 4);
			double2_t x23 = load2(first2, 6);

			partitions.first -= 8;

			x10 = op(x10, x20);
			x11 = op(x11, x21);
			x12 = op(x12, x22);
			x13 = op(x13, x23);

			store2(out, 0, x10);
			store2(out, 2, x11);
			store2(out, 4, x12);
			store2(out, 6, x13);

			first1 += 8;
			first2 += 8;
			out += 8;
		}

		while(partitions.second) {
			--partitions.second;
			*out = op(*first1, *first2);
			++first1;
			++first2;
			++out;
		}
	}

	struct linear_combination_op {
		scalar_t a1;
		scalar_t a2;
		double2_t a1_v;
		double2_t a2_v;

		linear_combination_op(scalar_t a1, scalar_t a2)
		: a1(a1), a2(a2), a1_v(_mm_set_pd(a1, a1)), a2_v(_mm_set_pd(a2, a2)) {}

		inline
		scalar_t operator()(scalar_t x1, scalar_t x2) {
			return a1 * x1 + a2 * x2;
		}

		inline
		double2_t operator()(double2_t x1, double2_t x2) {
			double2_t m1 = multiply(x1, a1_v);
			double2_t m2 = multiply(x2, a2_v);
			return add(m1, m2);
		}
	};

	template <typename N>
	inline
	void linear_combination(const scalar_t* first1, const scalar_t* first2, N n, scalar_t* out, scalar_t a1, scalar_t a2) {
		linear_combination_op op(a1, a2);
		binary_transform(first1, first2, n, out, op);
	}

	template <typename N>
	inline
	void mix(const scalar_t* first1, const scalar_t* first2, N n, scalar_t* out, scalar_t frac) {
		linear_combination(first1, first2, n, out, 1.0 - frac, frac);
	}

	struct add_op {
		inline
		scalar_t operator()(scalar_t x1, scalar_t x2) {
			return x1 + x2;
		}

		inline
		double2_t operator()(double2_t x1, double2_t x2) {
			return add(x1, x2);
		}
	};
	struct multiply_op {
		inline
		scalar_t operator()(scalar_t x1, scalar_t x2) {
			return x1 * x2;
		}

		inline
		double2_t operator()(double2_t x1, double2_t x2) {
			return multiply(x1, x2);
		}
	};
	struct multiply_with_scalar_and_add_op {
		scalar_t scalar;
		double2_t scalar_v;

		multiply_with_scalar_and_add_op(scalar_t scalar) : scalar(scalar), scalar_v(_mm_set_pd(scalar, scalar)) {}

		inline
		scalar_t operator()(scalar_t x1, scalar_t x2) {
			return x2 + x1 * scalar;
		}

		inline
		double2_t operator()(double2_t x1, double2_t x2) {
			return add(x2, multiply(x1, scalar_v));
		}
	};

	template <typename N>
	inline
	void add(const scalar_t* first1, const scalar_t* first2, N n, scalar_t* out) {
		binary_transform(first1, first2, n, out, add_op());
	}

	template <typename N>
	inline
	void multiply(const scalar_t* first1, const scalar_t* first2, N n, scalar_t* out) {
		binary_transform(first1, first2, n, out, multiply_op());
	}

	template <typename N>
	inline
	void multiply_add(const scalar_t* first, N n, scalar_t* out, scalar_t scalar) {
		binary_transform(first, out, n, out, multiply_with_scalar_and_add_op(scalar));
	}
}

#endif
