
#ifndef STD_DSP_CHANNEL_ITERATOR_GUARD
#define STD_DSP_CHANNEL_ITERATOR_GUARD

#include "../base/base.h"

namespace std_dsp {
		class channel_iterator {
	private:
		double* ptr;
		integer_t stride;
	public:
		channel_iterator() {}
		channel_iterator(double* ptr, integer_t stride) : ptr(ptr), stride(stride) {}

		inline
		double* begin() {
			return ptr;
		}
		inline
		double* end() {
			return ptr + stride;
		}
		inline
		const double* begin() const {
			return ptr;
		}
		inline
		const double* end() const {
			return ptr + stride;
		}
		inline
		const double* cbegin() {
			return ptr;
		}
		inline
		const double* cend() {
			return ptr + stride;
		}
		inline
		const double* cbegin() const {
			return ptr;
		}
		inline
		const double* cend() const {
			return ptr + stride;
		}

		inline
		channel_iterator& operator++() {
			ptr += stride;
			return *this;
		}
		inline
		channel_iterator operator++(int) {
			channel_iterator tmp = *this;
			ptr += stride;
			return *this;
		}
		inline
		channel_iterator& operator--() {
			ptr -= stride;
			return *this;
		}
		inline
		channel_iterator operator--(int) {
			channel_iterator tmp = *this;
			ptr -= stride;
			return *this;
		}
		inline
		channel_iterator& operator+=(integer_t n) {
			ptr += (stride * n);
			return *this;
		}
		inline
		channel_iterator& operator-=(integer_t n) {
			ptr -= (stride * n);
			return *this;
		}

		inline
		double* operator*() { return ptr; }
		inline
		const double* operator*() const { return ptr; }

		inline
		double* operator[](integer_t n) {
			return ptr + (n * stride);
		}
		inline
		const double* operator[](integer_t n) const {
			return ptr + (n * stride);
		}

		inline
		friend
		bool operator==(const channel_iterator& x, const channel_iterator& y) {
			return x.ptr == y.ptr;
		}
		inline
		friend
		bool operator!=(const channel_iterator& x, const channel_iterator& y) {
			return !(x == y);
		}
	};
}

#endif
