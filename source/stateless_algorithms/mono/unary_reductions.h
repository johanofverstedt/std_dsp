
#ifndef STD_DSP_UNARY_REDUCTION_GUARD
#define STD_DSP_UNARY_REDUCTION_GUARD

#include "../../base/std_dsp_computational_basis.h"

#include "functors.h"

namespace std_dsp {
	template <typename N, typename Op>
	inline
	double unary_reduction(const double* first, N n, Op op) {
		if(is_odd_aligned(first)) {
			op(*first);
			++first;
			--n;
		}

		std::pair<std::size_t, std::size_t> partitions = unroll_partition_8(n);
		while(partitions.first) {
			double2_t x0 = load2(first, 0);
			double2_t x1 = load2(first, 2);
			double2_t x2 = load2(first, 4);
			double2_t x3 = load2(first, 6);

			first += 8;
			partitions.first -= 8;

			op(x0);
			op(x1);
			op(x2);
			op(x3);
		}

		while(partitions.second) {
			--partitions.second;
			op(*first);
			++first;
		}

		return op.get();
	}

	template <typename N>
	inline
	double min_value(const double* first, N n) {
		assert(n > 0);
        reduction_functors::min_value_op op;
		op.init(*first);

		return unary_reduction(first, n, op);
	}

	template <typename N>
	inline
	double max_value(const double* first, N n) {
		assert(n > 0);
		reduction_functors::max_value_op op;
		op.init(*first);

		return unary_reduction(first, n, op);
	}

	template <typename N>
	inline
	double min_abs_value(const double* first, N n) {
		assert(n > 0);
        reduction_functors::min_value_op inner_op;
		reduction_functors::abs_op<reduction_functors::min_value_op> op(inner_op);
		op.init(*first);

		return unary_reduction(first, n, op);
	}

	template <typename N>
	inline
	double max_abs_value(const double* first, N n) {
		assert(n > 0);
        reduction_functors::max_value_op inner_op;
		reduction_functors::abs_op<reduction_functors::max_value_op> op(inner_op);
		op.init(0.0);

		return unary_reduction(first, n, op);
	}

	template <typename N>
	inline
	double sum(const double* first, N n) {
		reduction_functors::sum_op op;
		if(n == 0)
			return 0.0;
		op.init(0.0);

		return unary_reduction(first, n, op);
	}

	template <typename N>
	inline
	double sum_of_squares(const double* first, N n) {
        reduction_functors::sum_op inner_op;
		reduction_functors::square_op<reduction_functors::sum_op> op(inner_op);
		if(n == 0)
			return 0.0;
		op.init(0.0);

		return unary_reduction(first, n, op);
	}

	template <typename N>
	inline
	double product(const double* first, N n) {
		reduction_functors::product_op op;
		if(n == 0)
			return 1.0;
		op.init(1.0);

		return unary_reduction(first, n, op);
	}

}

#endif
