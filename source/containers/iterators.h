
#ifndef STD_DSP_ITERATORS_GUARD
#define STD_DSP_ITERATORS_GUARD

#include <cstdint>
#include <cassert>
#include <algorithm>
#include <utility>

#include "../base/std_dsp_mem.h"
#include "../base/std_dsp_computational_basis.h"

namespace std_dsp {
	namespace detail {	
		inline
		storage_size_t wrap_forward(storage_size_t i, storage_size_t buf_size) {
			while (i >= buf_size)
				i -= buf_size;
			return i;
		}
		inline
		storage_size_t wrap_reverse(storage_size_t i, storage_size_t buf_size) {
			while (i < 0)
				i += buf_size;
			return i;
		}
		inline
		storage_size_t wrap_forward_1(storage_size_t i, storage_size_t buf_size) {
			if (i == buf_size)
				return 0;
			return i;
		}
		inline
		storage_size_t wrap_reverse_1(storage_size_t i, storage_size_t buf_size) {
			if (i < 0)
				i += buf_size;
			return i;
		}
		inline
		storage_size_t wrap_bidirectional(storage_size_t i, storage_size_t buf_size) {
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
		storage_size_t pos;
		storage_size_t size;
	public:
		using iterator = I;

		circular_iterator() {}
		circular_iterator(I offset, storage_size_t pos, storage_size_t size) : offset(offset), pos(pos), size(size) {}

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
		circular_iterator& operator+=(storage_size_t n) {
			//assert(n >= 0)
			//assert(n <= size);
			pos = detail::wrap_forward(pos + n, size);
			return *this;
		}
		inline
		circular_iterator& operator-=(storage_size_t n) {
			//assert(n >= 0)
			//assert(abs(n) <= size);
			pos = detail::wrap_reverse(pos - n, size);
			return *this;
		}

		inline
		circular_iterator operator+(storage_size_t n) {
			//assert(n >= 0)
			//assert(n <= size);
			circular_iterator tmp(offset, pos, size);
			tmp += n;
			return tmp;
		}
		inline
		circular_iterator operator-(storage_size_t n) {
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
		double& operator[](storage_size_t n) {
			return *offset[detail::wrap_bidirectional(pos + n, size)];
		}
		inline
		const double& operator[](storage_size_t n) const {
			return *offset[detail::wrap_bidirectional(pos + n, size)];
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
		vector2_t load2_from(circular_iterator it, storage_size_t n) {
			return load2_from(it.offset, detail::wrap_forward(it.pos + n, it.size));
		}

		inline
		friend
		void store2_to(circular_iterator it, storage_size_t n, vector2_t value) {
			return rotate(store2_to(it.offset, detail::wrap_forward(it.pos + n, it.size), value));
		}

		inline
		friend
		bool supports_fast_processing(circular_iterator it) {
			return supports_fast_processing(it.offset);
		}
		inline
		friend
		storage_size_t fast_count(circular_iterator it, storage_size_t n) {
			return (std::min)(it.size - it.pos, n);
		}
		inline
		friend
		storage_size_t fast_reverse_count(circular_iterator it, storage_size_t n) {
			return (std::min)(it.pos, n);
		}
		inline
		friend
		std::size_t get_alignment(circular_iterator it) {
			return get_alignment(it.offset + it.pos);
		}
	};

	template <typename I>
	circular_iterator<I> make_circular_iterator(I it, storage_size_t pos, storage_size_t size) {
		return circular_iterator<I>(it, pos, size);
	}

	template <typename I>
	auto get_fast_iterator(circular_iterator<I> it) -> decltype(get_fast_iterator(it.get_iterator())) {
		return get_fast_iterator(it.get_iterator());
	}

	template <typename I>
	class reverse_iterator {
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
		reverse_iterator& operator+=(storage_size_t n) {
			it -= n;
			return *this;
		}
		inline
		reverse_iterator& operator-=(storage_size_t n) {
			it += n;
			return *this;
		}

		inline
		reverse_iterator operator+(storage_size_t n) {
			return reverse_iterator(it - n);
		}
		inline
		reverse_iterator operator-(storage_size_t n) {
			return reverse_iterator(it + n);
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
		double& operator[](storage_size_t n) {
			return it[-(n+1)];
		}
		inline
		const double& operator[](storage_size_t n) const {
			return it[-(n+1)];
		}

		//
		//  Computational basis functions
		//

		inline
		I get_iterator() {
			return it;
		}

/*		inline
		friend
		auto get_fast_iterator(reverse_iterator it) -> reverse_iterator<decltype(get_fast_iterator(it.it))> {
			return reverse_iterator(get_fast_iterator(it.it));
		}
*/
		inline
		friend
		bool is_odd_aligned(reverse_iterator it) {
			return is_odd_aligned(it.it);
		}

		inline
		friend
		vector2_t load2_from(reverse_iterator it, storage_size_t n) {
			return rotate(load2_from(it.it, -(2 + n)));
		}

		inline
		friend
		void store2_to(reverse_iterator it, storage_size_t n, vector2_t value) {
			return rotate(store2_to(it.it, -(2 + n), value));
		}

		inline
		friend
		bool supports_fast_processing(reverse_iterator it) {
			return supports_fast_processing(it.it);
		}
		inline
		friend
		storage_size_t fast_count(reverse_iterator it, storage_size_t n) {
			return fast_reverse_count(it.it, n);
		}
		inline
		friend
		storage_size_t fast_reverse_count(reverse_iterator it, storage_size_t n) {
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
