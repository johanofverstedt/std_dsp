
#ifndef STD_DSP_CIRCULAR_ITERATOR_GUARD
#define STD_DSP_CIRCULAR_ITERATOR_GUARD

#include <cstdint>
#include <cassert>
#include <algorithm>
#include <utility>

#include "../base/std_dsp_mem.h"
#include "../base/std_dsp_computational_basis.h"

namespace std_dsp {
	namespace detail {	
		inline
		integer_t wrap_forward(integer_t i, integer_t buf_size) {
			while (i >= buf_size)
				i -= buf_size;
			return i;
		}
		inline
		integer_t wrap_reverse(integer_t i, integer_t buf_size) {
			while (i < 0)
				i += buf_size;
			return i;
		}
		inline
		integer_t wrap_forward_1(integer_t i, integer_t buf_size) {
			if (i == buf_size)
				return 0;
			return i;
		}
		inline
		integer_t wrap_reverse_1(integer_t i, integer_t buf_size) {
			if (i < 0)
				i += buf_size;
			return i;
		}
		inline
		integer_t wrap_bidirectional(integer_t i, integer_t buf_size) {
			if (i >= buf_size)
				return i - buf_size;
			if (i < 0)
				return i + buf_size;
			return i;
		}
	}

	template <typename I>
	class circular_iterator {
	private:
		I offset;
		integer_t pos;
		integer_t size;
	public:
		circular_iterator() {}
		circular_iterator(I offset, integer_t pos, integer_t size) : offset(offset), pos(pos), size(size) {}

		inline
		circular_iterator operator++() { pos = detail::wrap_forward_1(pos + 1, size); return *this; }
		inline
		circular_iterator operator++(int) {
			auto tmp = *this;
			pos = detail::wrap_forward_1(pos + 1, size);
			return tmp;
		}
		inline
		circular_iterator operator--() { pos = detail::wrap_reverse_1(pos - 1, size); return *this; }
		inline
		circular_iterator operator--(int) {
			auto tmp = *this;
			pos = detail::wrap_reverse_1(pos - 1, size);
			return tmp;
		}

		inline
		circular_iterator& operator+=(integer_t n) {
			//assert(n >= 0)
			//assert(n <= size);
			pos = detail::wrap_forward(pos + n, size);
			return *this;
		}
		inline
		circular_iterator& operator-=(integer_t n) {
			//assert(n >= 0)
			//assert(abs(n) <= size);
			pos = detail::wrap_reverse(pos - n, size);
			return *this;
		}

		inline
		circular_iterator operator+(integer_t n) {
			//assert(n >= 0)
			//assert(n <= size);
			circular_iterator tmp(offset, pos, size);
			tmp += n;
			return tmp;
		}
		inline
		circular_iterator operator-(integer_t n) {
			//assert(n >= 0)
			//assert(n <= size);
			circular_iterator tmp(offset, pos, size);
			tmp -= n;
			return tmp;
		}

		inline
		double& operator*() {
			return offset[pos];
		}
		inline
		const double& operator*() const {
			return offset[pos];
		}

		inline
		double& operator[](integer_t n) {
			return offset[detail::wrap_bidirectional(pos + n, size)];
		}
		inline
		const double& operator[](integer_t n) const {
			return offset[detail::wrap_bidirectional(pos + n, size)];
		}

		//
		//  Computational basis functions
		//
		inline
		I get_iterator() {
			return offset + pos;
		}

		inline
		friend
		bool is_odd_aligned(circular_iterator it) {
			return is_odd_aligned(it.offset + it.pos);
		}

		inline
		friend
		vector2_t load2_from(circular_iterator it, integer_t n) {
			return load2_from(it.offset, detail::wrap_forward(it.pos + n, it.size));
		}

		inline
		friend
		void store2_to(circular_iterator it, integer_t n, vector2_t value) {
			return rotate(store2_to(it.offset, detail::wrap_forward(it.pos + n, it.size), value));
		}

		inline
		friend
		bool supports_fast_processing(circular_iterator it) {
			return supports_fast_processing(it.offset);
		}
		inline
		friend
		integer_t fast_count(circular_iterator it, integer_t n) {
			return (std::min)(it.size - it.pos, n);
		}
		inline
		friend
		integer_t fast_reverse_count(circular_iterator it, integer_t n) {
			return (std::min)(it.pos, n);
		}
		inline
		friend
		std::size_t get_alignment(circular_iterator it) {
			return get_alignment(it.offset + it.pos);
		}
		
		inline
		friend
		circular_iterator make_delay_iterator(circular_iterator it, integer_t delay) {
			//assert(delay >= 0);
			return circular_iterator(it, detail::wrap_reverse(it.pos - delay), it.size);
		}
	};

	template <typename I>
	circular_iterator<I> make_circular_iterator(I it, integer_t pos, integer_t size) {
		return circular_iterator<I>(it, pos, size);
	}

	template <typename I>
	auto get_fast_iterator(circular_iterator<I> it) -> decltype(get_fast_iterator(it.get_iterator())) {
		return get_fast_iterator(it.get_iterator());
	}
}

#endif
