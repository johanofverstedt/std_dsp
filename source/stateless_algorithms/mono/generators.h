
#ifndef STD_DSP_GENERATORS_GUARD
#define STD_DSP_GENERATORS_GUARD

#include "../../base/std_dsp_computational_basis.h"

#include "functors.h"

namespace std_dsp {
	template <typename N, typename Op>
	void generate(N n, double* out, Op op) {
		if(op.is_const()) {
			const double x1 = op.get1();
			const double2_t x2 = op.get2();

			if(is_odd_aligned(out)) {
				*out = x1;
				++out;
				--n;
			}

			std::pair<std::size_t, std::size_t> partitions = unroll_partition_8(n);
			while(partitions.first) {
				partitions.first -= 8;

				store2(out, 0, x2);
				store2(out, 2, x2);
				store2(out, 4, x2);
				store2(out, 6, x2);

				out += 8;
			}

			while(partitions.second) {
				--partitions.second;
				*out = x1;
				++out;
			}
		} else {
			if(is_odd_aligned(out)) {
				*out = op.get1();
				++out;
				--n;
			}

			std::pair<std::size_t, std::size_t> partitions = unroll_partition_8(n);
			while(partitions.first) {
				partitions.first -= 8;

				double2_t x0 = op.get2();
				double2_t x1 = op.get2();
				double2_t x2 = op.get2();
				double2_t x3 = op.get2();

				store2(out, x0);
				store2(out + 2, x1);
				store2(out + 4, x2);
				store2(out + 6, x3);

				out += 8;
			}

			while(partitions.second) {
				--partitions.second;
				*out = op.get1();
				++out;
			}
		}
	}

	template <typename N>
	inline
	void zero(N n, double* out) {
		generate(n, out, generator_functors::zero_generator_op());
	}

	template <typename N>
	inline
	void assign(N n, double* out, double value) {
		generate(n, out, generator_functors::constant_generator_op(value));
	}

	template <typename N>
	inline
	void randomize(N n, double* out, double a, double b) {
		generate(n, out, generator_functors::random_generator_op(a, b));
	}
}

#endif
