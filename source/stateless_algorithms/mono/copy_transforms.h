
#ifndef STD_DSP_COPY_TRANSFORMS_GUARD
#define STD_DSP_COPY_TRANSFORMS_GUARD

#include "../../base/std_dsp_computational_basis.h"

#include "functors.h"

namespace std_dsp {
	template <typename N, typename Op>
	inline
	void copy_transform(const double* first, N n, double* out, Op op) {
		if(n == 0)
			return;

		if(!compare_alignment(first, out)) {
			while(n) {
				*out = op(*first);
				--n;
				++first;
				++out;
			}
			return;
		}

		if(is_odd_aligned(first)) {
			*out = op(*first);
			++first;
			++out;
			--n;
		}

		std::pair<std::size_t, std::size_t> partitions = unroll_partition_8(n);
		while(partitions.first) {
			vector2_t x0 = load2_from(first, 0);
			vector2_t x1 = load2_from(first, 2);
			vector2_t x2 = load2_from(first, 4);
			vector2_t x3 = load2_from(first, 6);

			partitions.first -= 8;

			x0 = op(x0);
			x1 = op(x1);
			x2 = op(x2);
			x3 = op(x3);

			store2_to(out, 0, x0);
			store2_to(out, 2, x1);
			store2_to(out, 4, x2);
			store2_to(out, 6, x3);

			first += 8;
			out += 8;
		}

		while(partitions.second) {
			--partitions.second;
			*out = op(*first);
			++first;
			++out;
		}
	}

	template <typename N>
	inline
	void add(const double* first, N n, double* out, double term) {
        transform_functors::add_op op(term);
		copy_transform(first, n, out, op);
	}
	template <typename N>
	inline
	void multiply(const double* first, N n, double* out, double factor) {
		transform_functors::multiply_op op(factor);
		copy_transform(first, n, out, op);
	}
	template <typename N>
	inline
	void clip(const double* first, N n, double* out, double min_level, double max_level) {
		transform_functors::clip_op op(min_level, max_level);
		copy_transform(first, n, out, op);
	}
	template <typename N>
	inline
	void cubic_clip(const double* first, N n, double* out) {
		transform_functors::cubic_clip_op op;
		copy_transform(first, n, out, op);
	}
	template <typename N>
	inline
	void copy(const double* first, N n, double* out) {
		transform_functors::identity_op op;
		copy_transform(first, n, out, op);
	}
	template <typename N>
	inline
	void phase_reverse(const double* first, N n, double* out) {
		transform_functors::multiply_op op(-1.0);
		copy_transform(first, n, out, op);
	}
	template <typename N>
	inline
	void abs(const double* first, N n, double* out) {
		transform_functors::abs_op op;
		copy_transform(first, n, out, op);
	}
	template <typename N>
	inline
	void square(const double* first, N n, double* out) {
		transform_functors::square_op op;
		copy_transform(first, n, out, op);
	}
}

#endif
