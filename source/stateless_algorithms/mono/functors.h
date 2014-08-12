
#ifndef STD_DSP_MONO_FUNCTORS_GUARD
#define STD_DSP_MONO_FUNCTORS_GUARD

#include <cstdlib>
#include <cmath>

#include "../../base/std_dsp_computational_basis.h"

namespace std_dsp {
	namespace functors {
		template <typename Op1, typename Op2>
		struct binary_op {
			Op1 op1;
			Op2 op2;

			binary_op(Op1 op1, Op2 op2) : op1(op1), op2(op2) {}

			inline
			scalar_t operator()(scalar_t x) {
				return op2(op1(x));
			}
			inline
			vector2_t operator()(vector2_t x) {
				return op2(op1(x));
			}			
		};

		template <typename R, typename RT, typename T>
		struct reduction_and_transform_op {
			R r_op;
			RT rt_op;
			T t_op;

			reduction_and_transform_op(R r_op, RT rt_op, T t_op) : r_op(r_op), rt_op(rt_op), t_op(t_op) {}

			inline
			void init(scalar_t x) {
				r_op.init(x);
			}

			inline
			scalar_t operator()(scalar_t x) {
				r_op(rt_op(x));
				return t_op(x);
			}
			inline
			vector2_t operator()(vector2_t x) {
				r_op(rt_op(x));
				return t_op(x);
			}

			inline
			scalar_t get() {
				return r_op.get();
			}
		};		
	}
	namespace generator_functors {
		struct zero_generator_op {
			inline
			bool is_const() { return true; }

			inline
			scalar_t get1() { return 0.0; }

			inline
			vector2_t get2() { return zero(); }
		};

		struct constant_generator_op {
			scalar_t s;
			vector2_t v;

			constant_generator_op(scalar_t s) : s(s), v(load2_from(s)) {}

			inline
			bool is_const() { return true; }

			inline
			scalar_t get1() { return s; }

			inline
			vector2_t get2() { return v; }
		};

		namespace detail {
			inline
			scalar_t uniform1() {
				return rand() / static_cast<scalar_t>(RAND_MAX);
			}
			inline
			vector2_t uniform2() {
				return load2_from(uniform1(), uniform1());
			}
		}

		struct random_generator_op {
			scalar_t offset;
			scalar_t scale;
			vector2_t offset_v;
			vector2_t scale_v;

			random_generator_op(scalar_t a, scalar_t b) : offset(a), scale(b-a), offset_v(load2_from(a)), scale_v(load2_from(scale)) {}

			inline
			bool is_const() { return false; }

			inline
			scalar_t get1() { return offset + scale * detail::uniform1(); }

			inline
			vector2_t get2() { return add(offset_v, multiply(scale_v, detail::uniform2())); }			
		};
	}

	namespace transform_functors {
		struct identity_op {
			inline
			scalar_t operator()(scalar_t x) { return x; }
			inline
			vector2_t operator()(vector2_t x) { return x; }
		};

		struct add_op {
			scalar_t s;
			vector2_t v;

			add_op(scalar_t t) {
				s = t;
				v = load2_from(t, t);
			}

			inline
			scalar_t operator()(scalar_t x) {
				return x * s;
			}
			inline
			vector2_t operator()(vector2_t x) {
				return add(x, v);
			}
		};

		struct multiply_op {
			scalar_t s;
			vector2_t v;

			multiply_op(scalar_t g) {
				s = g;
				v = load2_from(g, g);
			}

			inline
			scalar_t operator()(scalar_t x) {
				return x * s;
			}
			inline
			vector2_t operator()(vector2_t x) {
				return multiply(x, v);
			}
		};

		struct abs_op {
			vector2_t sign_mask;

			abs_op() {
				sign_mask = load2_from(-0.0);
			}

			inline
			scalar_t operator()(scalar_t x) {
				return fabs(x);
			}
			inline
			vector2_t operator()(vector2_t x) {
				return std_dsp::abs(x, sign_mask);
			}
		};		

		struct square_op {
			inline
			scalar_t operator()(scalar_t x) {
				return x * x;
			}
			inline
			vector2_t operator()(vector2_t x) {
				return multiply(x, x);
			}
		};

		struct clip_op {
			scalar_t min_level;
			scalar_t max_level;
			vector2_t min_level_v;
			vector2_t max_level_v;

			clip_op(scalar_t min_level, scalar_t max_level) : min_level(min_level), max_level(max_level) {
				min_level_v = load2_from(min_level, min_level);
				max_level_v = load2_from(max_level, max_level);
			}

			inline
			scalar_t operator()(scalar_t x) {
				if(x < min_level)
					return min_level;
				if(x > max_level)
					return max_level;
				return x;
			}
			inline
			vector2_t operator()(vector2_t x) {
				return minimum(maximum(x, min_level_v), max_level_v);
			}
		};

		struct cubic_clip_op {
			scalar_t one_third;
			scalar_t three_halves;
			vector2_t one_third_v;
			vector2_t three_halves_v;
			vector2_t max_value_v;
			vector2_t min_value_v;
			vector2_t pre_gain_v;

			cubic_clip_op() {
				one_third = 1.0 / 3.0;
				three_halves = 3.0 / 2.0;
				one_third_v = load2_from(one_third, one_third);
				three_halves_v = load2_from(three_halves, three_halves);
				max_value_v = load2_from(1.0, 1.0);
				min_value_v = load2_from(-1.0, -1.0);
				pre_gain_v = load2_from(0.707945784384138, 0.707945784384138);
			}

			inline
			scalar_t operator()(scalar_t x) {
				x *= 0.707945784384138;
				if(x > 1.0)
					return 1.0;
				if(x < -1.0)
					return -1.0;
				return (x - x * x * x * one_third) * three_halves;
			}

			inline
			vector2_t operator()(vector2_t x) {
				x = multiply(x, pre_gain_v);
				x = maximum(x, min_value_v);
				x = minimum(x, max_value_v);
				vector2_t y = multiply(x, x);
				y = multiply(y, x);
				y = multiply(y, one_third_v);
				x = subtract(x, y);
				return multiply(x, three_halves_v);
			}
		};		
	}

	namespace reduction_functors {
		struct min_value_op {
			scalar_t s; //Scalar
			vector2_t v; //Vector

			min_value_op() : s(0.0), v(load2_from(0.0, 0.0)) {}

			inline
			void init(scalar_t first) {
				s = first;
				v = load2_from(first, first);
			}

			inline
			void operator()(scalar_t x) {
				if(x < s)
					s = x;
			}
			inline
			void operator()(vector2_t x) {
				v = minimum(x, v);
			}

			scalar_t get() {
				SSE_ALIGN scalar_t tmp[2];
				store2_to(tmp, v);

				if(s < tmp[0]) {
					if(s < tmp[1])
						return s;
					else
						return tmp[1];
				} else {
					if(tmp[0] < tmp[1])
						return tmp[0];
					else
						return tmp[1];
				}
			}
		};

		struct max_value_op {
			scalar_t s; //Scalar
			vector2_t v; //Vector

			inline
			void init(scalar_t first) {
				s = first;
				v = load2_from(first, first);
			}

			inline
			void operator()(scalar_t x) {
				if(s < x)
					s = x;
			}
			inline
			void operator()(vector2_t x) {
				v = maximum(x, v);
			}

			scalar_t get() {
				SSE_ALIGN scalar_t tmp[2];
				store2_to(tmp, v);

				if(tmp[0] < s) {
					if(tmp[1] < s)
						return s;
					else
						return tmp[1];
				} else {
					if(tmp[1] < tmp[0])
						return tmp[0];
					else
						return tmp[1];
				}
			}
		};

		struct sum_op {
			scalar_t s; //Scalar
			vector2_t v; //Vector

			inline
			void init(scalar_t first) {
				s = first;
				v = load2_from(0.0, 0.0);
			}

			inline
			void operator()(scalar_t x) {
				s += x;
			}
			inline
			void operator()(vector2_t x) {
				v = add(x, v);
			}

			scalar_t get() {
				SSE_ALIGN scalar_t tmp[2];
				store2_to(tmp, v);

				return s + tmp[0] + tmp[1];
			}
		};

		struct product_op {
			scalar_t s; //Scalar
			vector2_t v; //Vector

			inline
			void init(scalar_t first) {
				s = first;
				v = load2_from(1.0, 1.0);
			}

			inline
			void operator()(scalar_t x) {
				s *= x;
			}
			inline
			void operator()(vector2_t x) {
				v = multiply(x, v);
			}

			scalar_t get() {
				SSE_ALIGN scalar_t tmp[2];
				store2_to(tmp, v);

				return s * tmp[0] * tmp[1];
			}
		};

		template <typename Op>
		struct abs_op {
			Op inner_op;
			vector2_t sign_mask;

			abs_op(Op op) : inner_op(op), sign_mask(load2_from(-0.0)) {
			}

			inline
			void init(scalar_t first) {
				inner_op.init(fabs(first));
			}

			inline
			void operator()(scalar_t x) {
				inner_op(fabs(x));
			}
			inline
			void operator()(vector2_t x) {
				inner_op(std_dsp::abs(x, sign_mask));
			}

			inline
			scalar_t get() {
				return inner_op.get();
			}
		};		

		template <typename Op>
		struct square_op {
			Op inner_op;

			square_op(Op op) : inner_op(op) {}

			inline
			void init(scalar_t first) {
				inner_op.init(first * first);
			}

			inline
			void operator()(scalar_t x) {
				inner_op(x * x);
			}
			inline
			void operator()(vector2_t x) {
				inner_op(multiply(x, x));
			}

			inline
			scalar_t get() {
				return inner_op.get();
			}
		};
	}	
}

#endif
