
#ifndef STD_DSP_INPLACE_TRANSFORMS_GUARD
#define STD_DSP_INPLACE_TRANSFORMS_GUARD

#include "../../base/std_dsp_computational_basis.h"

#include "functors.h"

namespace std_dsp {
	template <typename I, typename N, typename Op>
	inline
	void inplace_transform(I first, N n, Op op) {
		if(n == 0)
			return;

		if(is_odd_aligned(first)) {
			*first = op(*first);
			++first;
			--n;
		}

		std::pair<std::size_t, std::size_t> partitions = unroll_partition_8(n);
		while(partitions.first) {
			double2_t x0 = load2(first, 0);
			double2_t x1 = load2(first, 2);
			double2_t x2 = load2(first, 4);
			double2_t x3 = load2(first, 6);

			partitions.first -= 8;

			x0 = op(x0);
			x1 = op(x1);
			x2 = op(x2);
			x3 = op(x3);

			store2(first, 0, x0);
			store2(first, 2, x1);
			store2(first, 4, x2);
			store2(first, 6, x3);

			first += 8;
		}

		while(partitions.second) {
			--partitions.second;
			*first = op(*first);
			++first;
		}
	}

	template <typename N>
	inline
	void add(double* first, N n, double term) {
        transform_functors::add_op op(term);
		inplace_transform(first, n, op);
	}
	template <typename N>
	inline
	void multiply(double* first, N n, double factor) {
		transform_functors::multiply_op op(factor);
		inplace_transform(first, n, op);
	}
	template <typename N>
	inline
	void clip(double* first, N n, double min_level, double max_level) {
		transform_functors::clip_op op(min_level, max_level);
		inplace_transform(first, n, op);
	}
	template <typename N>
	inline
	void cubic_clip(double* first, N n) {
		transform_functors::cubic_clip_op op;
		inplace_transform(first, n, op);
	}
	template <typename N>
	inline
	void phase_reverse(double* first, N n) {
		transform_functors::multiply_op op(-1.0);
		inplace_transform(first, n, op);
	}
	template <typename N>
	inline
	void abs(double* first, N n) {
		transform_functors::abs_op op;
		inplace_transform(first, n, op);
	}
	template <typename N>
	inline
	void square(double* first, N n) {
		transform_functors::square_op op;
		inplace_transform(first, n, op);
	}

	//Special cases

	template <typename N>
	inline
	void undenormalize(double *first, N n) {
		while(n) {
			--n;
			if(fabs(*first) < 1.e-15)
				*first = 0.0;
			++first;
		}
	}	
}

#endif
