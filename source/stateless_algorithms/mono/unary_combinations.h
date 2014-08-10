
#ifndef STD_DSP_UNARY_COMBINATIONS_GUARD
#define STD_DSP_UNARY_COMBINATIONS_GUARD

#include "unary_reductions.h"
#include "inplace_transforms.h"
#include "unary_inplace_transform_and_reduce.h"

#include "functors.h"

namespace std_dsp {
	template <typename N>
	inline
	bool clip_with_indicator(double* first, N n, double abs_clip) {
		using reduction_functors::max_value_op;
		using transform_functors::abs_op;
		using transform_functors::clip_op;
		using functors::reduction_and_transform_op;

		assert(n > 0);
        max_value_op r_op {};
		abs_op rt_op;
		clip_op t_op(-abs_clip, abs_clip);

		reduction_and_transform_op<max_value_op, abs_op, clip_op> op(r_op, rt_op, t_op);

		op.init(0.0);

		return unary_inplace_transform_and_reduce(first, n, op) >= abs_clip;
	}
}

#endif
