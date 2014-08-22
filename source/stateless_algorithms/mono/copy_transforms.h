
#ifndef STD_DSP_COPY_TRANSFORMS_GUARD
#define STD_DSP_COPY_TRANSFORMS_GUARD

#include <iostream>

#include "../../base/std_dsp_computational_basis.h"

#include "functors.h"

namespace std_dsp {
	template <typename I, typename N, typename O, typename Op>
	inline
	void copy_transform_scalar(I first, N n, O out, Op op) {
		while(n) {
			auto fast_n = fast_count(first, out, n);
			auto fast_first = get_fast_iterator(first);
			auto fast_out = get_fast_iterator(out);

			n -= fast_n;
			first += fast_n;
			out += fast_n;

			while(fast_n) {
				--fast_n;
				*fast_out = op(*fast_first);
				++fast_first;
				++fast_out;
			}
		}
	}

	template <typename I, typename N, typename O, typename Op>
	inline
	void copy_transform_vector(I first, N n, O out, Op op) {
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

			store2(out, 0, x0);
			store2(out, 2, x1);
			store2(out, 4, x2);
			store2(out, 6, x3);

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

	template <typename I, typename N, typename O, typename Op>
	inline
	void copy_transform(I first, N n, O out, Op op) {
		while(n) {
			if(!check_alignment(first, out)) {
				copy_transform_scalar(first, n, out, op);
				return;
			}

			if(is_odd_aligned(first)) {
				*out = op(*first);
				++first;
				++out;
				--n;
			}

			auto fast_n = fast_count(first, out, n);
			auto fast_first = get_fast_iterator(first);
			auto fast_out = get_fast_iterator(out);

			copy_transform_vector(fast_first, fast_n, fast_out, op);

			n -= fast_n;
			first += fast_n;
			out += fast_n;
		}
	}

	template <typename I, typename N, typename O>
	inline
	void add(I first, N n, O out, double term) {
        transform_functors::add_op op(term);
		copy_transform(first, n, out, op);
	}
	template <typename I, typename N, typename O>
	inline
	void multiply(I first, N n, O out, double factor) {
		transform_functors::multiply_op op(factor);
		copy_transform(first, n, out, op);
	}
	template <typename I, typename N, typename O>
	inline
	void clip(I first, N n, O out, double min_level, double max_level) {
		transform_functors::clip_op op(min_level, max_level);
		copy_transform(first, n, out, op);
	}
	template <typename I, typename N, typename O>
	inline
	void cubic_clip(I first, N n, O out) {
		transform_functors::cubic_clip_op op;
		copy_transform(first, n, out, op);
	}
	template <typename I, typename N, typename O>
	inline
	void copy(I first, N n, O out) {
		transform_functors::identity_op op;
		copy_transform(first, n, out, op);
	}
	template <typename I, typename N, typename O>
	inline
	void phase_reverse(I first, N n, O out) {
		transform_functors::multiply_op op(-1.0);
		copy_transform(first, n, out, op);
	}
	template <typename I, typename N, typename O>
	inline
	void abs(I first, N n, O out) {
		transform_functors::abs_op op;
		copy_transform(first, n, out, op);
	}
	template <typename I, typename N, typename O>
	inline
	void square(I first, N n, O out) {
		transform_functors::square_op op;
		copy_transform(first, n, out, op);
	}
}

#endif
