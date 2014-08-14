
#ifndef STD_DSP_CHANNEL_ITERATOR_GUARD
#define STD_DSP_CHANNEL_ITERATOR_GUARD

#include "../base/base.h"

namespace std_dsp {
		template <typename I = double*>
		class channel_iterator {
	private:
		I it;
		integer_t stride;
	public:
		channel_iterator() {}
		channel_iterator(I it, integer_t stride) : it(it), stride(stride) {}

		inline
		double* begin() {
			return it;
		}
		inline
		double* end() {
			return it + stride;
		}
		inline
		const double* begin() const {
			return it;
		}
		inline
		const double* end() const {
			return it + stride;
		}
		inline
		const double* cbegin() {
			return it;
		}
		inline
		const double* cend() {
			return it + stride;
		}
		inline
		const double* cbegin() const {
			return it;
		}
		inline
		const double* cend() const {
			return it + stride;
		}

		inline
		channel_iterator& operator++() {
			it += stride;
			return *this;
		}
		inline
		channel_iterator operator++(int) {
			channel_iterator tmp = *this;
			it += stride;
			return *this;
		}
		inline
		channel_iterator& operator--() {
			it -= stride;
			return *this;
		}
		inline
		channel_iterator operator--(int) {
			channel_iterator tmp = *this;
			it -= stride;
			return *this;
		}
		inline
		channel_iterator& operator+=(integer_t n) {
			it += (stride * n);
			return *this;
		}
		inline
		channel_iterator& operator-=(integer_t n) {
			it -= (stride * n);
			return *this;
		}

		inline
		I operator*() { return it; }
		inline
		const I operator*() const { return it; }

		inline
		I operator[](integer_t n) {
			return it + (n * stride);
		}
		inline
		const I operator[](integer_t n) const {
			return it + (n * stride);
		}

		inline
		friend
		bool operator==(const channel_iterator& x, const channel_iterator& y) {
			return x.it == y.it;
		}
		inline
		friend
		bool operator!=(const channel_iterator& x, const channel_iterator& y) {
			return !(x == y);
		}
	};

	template <typename I>
	channel_iterator<I> make_channel_iterator(I first, integer_t stride) {
		return channel_iterator<I>(first, stride);
	}
}

#endif
