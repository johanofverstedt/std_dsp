
#ifndef STD_DSP_STRIDE_ITERATOR_GUARD
#define STD_DSP_STRIDE_ITERATOR_GUARD

#include <cstdint>
#include <cassert>
#include <algorithm>
#include <utility>

#include "../base/std_dsp_mem.h"
#include "../base/std_dsp_computational_basis.h"

namespace std_dsp {
	template <typename I>
	class stride_iterator {
	private:
		I it;
		integer_t stride;
	public:
		stride_iterator() {}
		stride_iterator(I offset, integer_t stride) : it(it), stride(stride) {}

		inline
		stride_iterator& operator++() { it += stride; return *this; }
		inline
		stride_iterator operator++(int) {
			auto tmp = *this;
			it += stride;
			return tmp;
		}
		inline
		stride_iterator& operator--() { it -= stride; return *this; }
		inline
		stride_iterator operator--(int) {
			auto tmp = *this;
			it -= stride;
			return tmp;
		}

		inline
		stride_iterator& operator+=(integer_t n) {
			//assert(n >= 0)
			//assert(n <= size);
			it += (stride * n);
			return *this;
		}
		inline
		stride_iterator& operator-=(integer_t n) {
			//assert(n >= 0)
			//assert(abs(n) <= size);
			it -= (stride * n);
			return *this;
		}

		inline
		stride_iterator operator+(integer_t n) {
			//assert(n >= 0)
			//assert(n <= size);
			stride_iterator tmp = *this;
			tmp += (stride * n);
			return tmp;
		}
		inline
		stride_iterator operator-(integer_t n) {
			//assert(n >= 0)
			//assert(n <= size);
			stride_iterator tmp = *this;
			tmp -= (stride * n);
			return tmp;
		}

		inline
		double& operator*() {
			return *it;
		}
		inline
		const double& operator*() const {
			return *it;
		}

		inline
		double& operator[](integer_t n) {
			return it[stride * n];
		}
		inline
		const double& operator[](integer_t n) const {
			return it[stride * n];
		}

		//
		//  Computational basis functions
		//
		inline
		I get_iterator() {
			return it;
		}

		inline
		friend
		bool is_odd_aligned(stride_iterator it) {
			return is_odd_aligned(it.it);
		}

		inline
		friend
		bool supports_fast_processing(stride_iterator it) {
			return supports_fast_processing(it.offset);
		}
		inline
		friend
		integer_t fast_count(stride_iterator it, integer_t n) {
			return 0;
		}
		inline
		friend
		integer_t fast_reverse_count(stride_iterator it, integer_t n) {
			return 0;
		}
		inline
		friend
		std::size_t get_alignment(stride_iterator it) {
			return get_alignment(it.it);
		}
	};

	template <typename I>
	stride_iterator<I> make_stride_iterator(I it, integer_t stride) {
		return stride_iterator<I>(it, stride);
	}

	//Should not be used
	template <typename I>
	double* get_fast_iterator(stride_iterator<I> it) {
		assert(false);
		return get_fast_iterator(it.get_iterator());
	}
}

#endif
