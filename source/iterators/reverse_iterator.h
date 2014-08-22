
#ifndef STD_DSP_REVERSE_ITERATOR_GUARD
#define STD_DSP_REVERSE_ITERATOR_GUARD

#include <cstdint>
#include <cassert>
#include <algorithm>
#include <utility>
#include <iterator>

#include "../base/std_dsp_mem.h"
#include "../base/std_dsp_computational_basis.h"

namespace std_dsp {
	template <typename I>
	class reverse_iterator : public std::iterator<std::random_access_iterator_tag, double, integer_t> {
	private:
		I it;
	public:
		reverse_iterator() {}
		reverse_iterator(I it) : it(it) {}
		
		inline
		reverse_iterator operator++() { --it; return *this; }
		inline
		reverse_iterator operator++(int) {
			auto tmp = *this;
			--it;
			return tmp;
		}
		inline
		reverse_iterator operator--() { ++it; return *this; }
		inline
		reverse_iterator operator--(int) {
			auto tmp = *this;
			++it;
			return tmp;
		}

		inline
		reverse_iterator& operator+=(integer_t n) {
			it -= n;
			return *this;
		}
		inline
		reverse_iterator& operator-=(integer_t n) {
			it += n;
			return *this;
		}

		inline
		reverse_iterator operator+(integer_t n) {
			return reverse_iterator(it - n);
		}
		inline
		reverse_iterator operator-(integer_t n) {
			return reverse_iterator(it + n);
		}

		inline
		integer_t operator-(reverse_iterator x) {
			return x.it - it;
		}

		inline
		double& operator*() {
			return *(it - 1);
		}
		inline
		const double& operator*() const {
			return *(it - 1);
		}

		inline
		double& operator[](integer_t n) {
			return it[-(n+1)];
		}
		inline
		const double& operator[](integer_t n) const {
			return it[-(n+1)];
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
		bool is_odd_aligned(reverse_iterator it) {
			return is_odd_aligned(it.it);
		}

		inline
		friend
		double2_t load2(reverse_iterator it, integer_t n) {
			return rotate(load2(it.it, -(2 + n)));
		}

		inline
		friend
		void store2(reverse_iterator it, integer_t n, double2_t value) {
			store2(it.it, -(2 + n), rotate(value));
		}

		inline
		friend
		bool supports_fast_processing(reverse_iterator it) {
			return supports_fast_processing(it.it);
		}
		inline
		friend
		integer_t fast_count(reverse_iterator it, integer_t n) {
			return fast_reverse_count(it.it, n);
		}
		inline
		friend
		integer_t fast_reverse_count(reverse_iterator it, integer_t n) {
			return fast_count(it.it, n);
		}
		inline
		friend
		std::size_t get_alignment(reverse_iterator it) {
			return get_alignment(it.it);
		}		
	};

	template <typename I>
	inline
	auto get_fast_iterator(reverse_iterator<I> it) -> reverse_iterator<decltype(get_fast_iterator(it.get_iterator()))> {
		return reverse_iterator<decltype(get_fast_iterator(it.get_iterator()))>
		(get_fast_iterator(it.get_iterator()));
	}

	template <typename I, typename N>
	inline
	reverse_iterator<I> make_reverse_iterator(I it, N n) {
		return reverse_iterator<I>(it + n);
	}
}

#endif
