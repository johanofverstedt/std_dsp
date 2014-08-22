
#ifndef STD_DSP_BIQUAD_FILTER_GUARD
#define STD_DSP_BIQUAD_FILTER_GUARD

#include "base/base.h"
#include "base/std_dsp_mem.h"
#include "base/computational_basis.h"

#include <cstdint>
#include <type_traits>

namespace std_dsp {
	struct biquad_coeffs {
		double a1, a2;
		double b0, b1, b2;
	};

	template <integer_t CHANNELS = 1>
	struct biquad_state {
		double w1[CHANNELS];
		double w2[CHANNELS];

		void reset() {
			for(integer_t i = 0; i < CHANNELS; ++i) {
				w1[i] = 0.0;
				w2[i] = 0.0;
			}
		}
		void undenormalize() {
			for(integer_t i = 0; i < CHANNELS; ++i) {
				if(fabs(w1[i]) < 1.e-15)
                    w1[i] = 0.0;
				if(fabs(w2[i]) < 1.e-15)
                    w2[i] = 0.0;
			}
		}
	};

	template <integer_t CHANNELS = 1>
	class biquad_op;

	template <>
	class biquad_op<1> {
	private:
		biquad_coeffs c;
		double w1;
		double w2;
	public:
		biquad_op() {}
		biquad_op(biquad_coeffs c, biquad_state<1> s) : c(c), w1(s.w1), w2(s.w2) {}

		inline
		double operator()(double in) {
			const double w0 = *first - c.a1 * w1 - c.a2 * w2;
			const double result = c.b0 * w0 + c.b1 * w1 + c.b2 * w2;

			w2 = w1;
			w1 = w0;

			return result;
		}

		inline
		biquad_state<1> get_state() {
			biquad_state<1> s;
			s.w1 = w1;
			s.w2 = w2;
			s.undenormalize();
			return s;
		}
	};

	template <>
	class biquad_op<2> {
	private:
		double2_t a1 = load2(c.a1);
		double2_t a2 = load2(c.a2);
		double2_t b0 = load2(c.b0);
		double2_t b1 = load2(c.b1);
		double2_t b2 = load2(c.b2);

		double2_t w1;
		double2_t w2;
	public:
		biquad_op() {}
		biquad_op(biquad_coeffs c, biquad_state<2> s) {
			w1 = load2(s.w)
			a1 = load2(c.a1);
			a2 = load2(c.a2);
			b0 = load2(c.b0);
			b1 = load2(c.b1);
			b2 = load2(c.b2);

			w1 = load2u(s.w1);
			w2 = load2u(s.w2);
		}
		biquad_op(biquad_coeffs c1, biquad_coeffs c2, biquad_state<2> s) {
			w1 = load2(s.w)
			a1 = load2(c1.a1, c2.a1);
			a2 = load2(c1.a2, c2.a2);
			b0 = load2(c1.b0, c2.b0);
			b1 = load2(c1.b1, c2.b1);
			b2 = load2(c1.b2, c2.b2);

			w1 = load2u(s.w1);
			w2 = load2u(s.w2);
		}

		inline
		double2_t operator()(double2_t in) {
			double2_t w0 = subtract(input, subtract(multiply(a1, w1), multiply(a2, w2)));
			double2_t result = add(multiply(b0, w0), add(multiply(b1, w1), multiply(b2, w2)));

			w2 = w1;
			w1 = w0;

			return result;
		}

		inline
		biquad_state<2> get_state() {
			biquad_state<2> s;
			store2u(s.w1, w1);
			store2u(s.w2, w1);
			s.undenormalize();
			return s;
		}
	};

	template <integer_t CHANNELS>
	struct biquad_replace_op;

	template <>
	struct biquad_replace_op<1> {
		template <typename I, typename N, typename O, typename Op>
		biquad_state<1> operator()(I first, N n, O out, Op op) {
			while(n) {
				--n;

				*out = op(*first);

				++first;
				++out;
			}

			return op.get_state();;
		}
	};

	template <>
	struct biquad_replace_op<2> {
		template <typename I, typename N, typename O, typename Op>
		biquad_state<2> operator()(I first, N n, O out, Op op) {
			while(n) {
				--n;

				double2_t in = load2(first);
				store2(out, op(in));

				first += 2;
				out += 2;
			}

			return op.get_state();;
		}
	};

	template <integer_t CHANNELS>
	struct biquad_invert_op;

	template <>
	struct biquad_invert_op<1> {
		template <typename I, typename N, typename O, typename Op>
		biquad_state<1> operator()(I first, N n, O out, Op op) {
			while(n) {
				--n;

				*out = -op(*first);

				++first;
				++out;
			}

			return op.get_state();;
		}
	};

	template <>
	struct biquad_invert_op<2> {
		template <typename I, typename N, typename O, typename Op>
		biquad_state<2> operator()(I first, N n, O out, Op op) {
			while(n) {
				--n;

				double2_t in = load2(first);
				store2(out, negate(op(in)));

				first += 2;
				out += 2;
			}

			return op.get_state();;
		}
	};

	template <integer_t CHANNELS>
	struct biquad_add_op;

	template <>
	struct biquad_add_op<1> {
		template <typename I, typename N, typename O, typename Op>
		biquad_state<1> operator()(I first, N n, O out, Op op) {
			while(n) {
				--n;

				*out += op(*first);

				++first;
				++out;
			}

			return op.get_state();;
		}
	};

	template <>
	struct biquad_add_op<2> {
		template <typename I, typename N, typename O, typename Op>
		biquad_state<2> operator()(I first, N n, O out, Op op) {
			while(n) {
				--n;

				double2_t old_output = load2(out);
				double2_t in = load2(first);
				store2(out, add(old_output, op(in)));

				first += 2;
				out += 2;
			}

			return op.get_state();;
		}
	};

	template <integer_t CHANNELS>
	struct biquad_subtract_op;

	template <>
	struct biquad_subtract_op<1> {
		template <typename I, typename N, typename O, typename Op>
		biquad_state<1> operator()(I first, N n, O out, Op op) {
			while(n) {
				--n;

				*out -= op(*first);

				++first;
				++out;
			}

			return op.get_state();;
		}
	};

	template <>
	struct biquad_subtract_op<2> {
		template <typename I, typename N, typename O, typename Op>
		biquad_state<2> operator()(I first, N n, O out, Op op) {
			while(n) {
				--n;

				double2_t old_output = load2(out);
				double2_t in = load2(first);
				store2(out, subtract(old_output, op(in)));

				first += 2;
				out += 2;
			}

			return op.get_state();
		}
	};

	template <typename I, typename N, typename O>
	inline
	biquad_state<1> biquad(I first, N n, O out, biquad_coeffs c, biquad_state<1> s) {
		biquad_op<1> op(c, s);
		biquad_replace_op<1> replace_op{};

		return replace_op(first, n, out, op);
	}
	template <typename I, typename N, typename O>
	inline
	biquad_state<1> biquad_phase_invert(I first, N n, O out, biquad_coeffs c, biquad_state<1> s) {
		biquad_op<1> op(c, s);
		biquad_replace_op<1> replace_op{};

		return replace_op(first, n, out, op);
	}
	template <typename I, typename N, typename O>
	inline
	biquad_state<2> biquad(I first, N n, O out, biquad_coeffs c, biquad_state<2> s) {
		assert(!is_odd_aligned(first));
		biquad_op<2> op;
		return op(first, n, out, c, s);
	}
	template <typename I, typename N, typename O>
	inline
	biquad_state<2> biquad(I first, N n, O out, biquad_coeffs c1, biquad_coeffs c2, biquad_state<2> s) {
		assert(!is_odd_aligned(first));
		biquad_op<2> op;
		return op(first, n, out, c1, c2, s);
	}
}

#endif
