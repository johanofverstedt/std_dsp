
#pragma once

#include <cstdint>
#include <cassert>

#include "../iterators/reverse_iterator.h"

namespace std_dsp {
	template <typename I>
	class range final {
	public:
		using value_type = typename I::value_type;
		using difference_type = integer_t;
		using iterator = I;
	private:
		I first_;
		difference_type n_;
	public:
		range() = default;
		inline
		range(I first, difference_type n) : first_(first), n_(n) {}
		range(const range& that) = default;
		range(range&& that) = default;

		range& operator=(const range& that) = default;
		range& operator=(range&& that) = default;

		inline
		friend
		bool operator==(const range& r1, const range& r2) {
			return (r1.first_ == r2.first_) && (r1.n_ == r2.n_);
		}

		inline
		friend
		bool operator!=(const range& r1, const range& r2) {
			return !(r1 == r2);
		}

		I begin() { return first_; }
		I end() { return first_ + n_; }
		I begin() const { return first_; }
		I end() const { return first_ + n_; }
		I cbegin() const { return first_; }
		I cend() const { return first_ + n_; }

		difference_type size() const { return n_; }

		range take(difference_type n) const {
			assert(n <= n_);
			return range(first_, n);
		}
		range drop(difference_type n) const {
			assert(n <= n_);
			return range(first_+ n, n_ - n);
		}

		range<reverse_iterator<I>> reverse() {
			return range<reverse_iterator<I>>(make_reverse_iterator(first_, n_), n_);
		}

		//Mutators - interface only available on range views

		range& take_inplace(difference_type n) {
			assert(n <= n_);
			n_ -= n;
			return *this;
		}
		range& drop_inplace(difference_type n) {
			assert(n <= n_);
			first_ += n;
			n_ -= n;
			return *this;
		}
	};

	template <typename I>
	inline
	range<I> make_range(I first, integer_t n) {
		return range<I>(first, n);
	}
}
