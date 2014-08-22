
#ifndef STD_DSP_UNARY_INPLACE_TRANSFORM_AND_REDUCE_GUARD
#define STD_DSP_UNARY_INPLACE_TRANSFORM_AND_REDUCE_GUARD

#include "../../base/std_dsp_computational_basis.h"

#include "functors.h"

namespace std_dsp {
    template <typename N, typename Op>
	double unary_inplace_transform_and_reduce(double* first, N n, Op op) {
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

		return op.get();
	}
}

#endif
