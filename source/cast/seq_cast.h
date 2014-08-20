
#ifndef STD_DSP_SEQUENCE_CAST_GUARD
#define STD_DSP_SEQUENCE_CAST_GUARD

#include <utility>
#include <iterator>

namespace std_dsp {
	template <typename T>
	struct static_cast_op {
		using value_type = T;
		using pointer = T*;
		using reference = T&;

		template <typename U>
		inline
			T operator()(const U& x) {
			return static_cast<T>(x);
		}
	};

	namespace detail {
		template <typename I, typename O, typename Op>
		inline
		O sequence_element_cast(I first, I last, O out, Op op) {
			while(first != last) {
				*out = op(*first);

				++first;
				++out;
			}

			return out;
		}

		template <typename I, typename N, typename O, typename Op>
		inline
		std::pair<I, O> sequence_element_cast_n(I first, N n, O out, Op op) {
			static_assert(std::is_integral<N>::value, "Count not integral.");

			while(n) {
				--n;
				*out = op(*first);

				++first;
				++out;
			}

			return std::make_pair(first, out);
		}
	}

	//Algorithm

	template <typename I, typename O>
	inline
	O seq_cast(I first, I last, O out) {
		return detail::sequence_element_cast(first, last, out, static_cast_op<typename std::iterator_traits<O>::value_type>());
	}
	
	template <typename I, typename N, typename O>
	inline
	std::pair<I, O> seq_cast_n(I first, N n, O out) {
		return detail::sequence_element_cast_n(first, n, out, static_cast_op<typename std::iterator_traits<O>::value_type>());
	}

	//Sequence cast iterator adaptor

	template <typename I, typename Op>
	class transform_iterator
		: public std::iterator<typename std::iterator_traits<I>::iterator_category,
		                       typename Op::value_type,
		                       typename std::iterator_traits<I>::difference_type,
		                       typename Op::pointer,
		                       typename Op::reference> {
	private:
		I it;
		Op op;
	public:
		transform_iterator() : {}
		transform_iterator(I it, Op op) : it(it), op(op) {}

		inline
		value_type operator*() {
			return op(*it);
		}

		inline
		transform_iterator& operator++() {
			++it;
			return *this;
		}
		inline
		transform_iterator operator++(int) {
			transform_iterator tmp(it);
			++it;
			return tmp;
		}
		inline
		transform_iterator& operator--() {
			--it;
			return *this;
		}
		inline
		transform_iterator operator--(int) {
			transform_iterator tmp(it);
			--it;
			return tmp;
		}

		inline
		transform_iterator& operator+=(difference_type n) {
			it += n;
			return *this;
		}
		inline
		transform_iterator& operator-=(difference_type n) {
			it -= n;
			return *this;
		}

		inline
		transform_iterator operator+(difference_type n) {
			transform_iterator tmp(it);
			tmp += n;
			return tmp;
		}

		inline
		value_type operator[](difference_type n) {
			return op(*(*this + n));
		}

		inline
		friend
		difference_type operator-(const transform_iterator& x, const transform_iterator& y) {
			return x.it - y.it;
		}

		inline
		friend
		bool operator==(const transform_iterator& x, const transform_iterator& y) {
			return x.it == y.it;
		}
		inline
		friend
		bool operator!=(const transform_iterator& x, const transform_iterator& y) {
			return !(x == y);
		}
	};

	template <typename T, typename I>
	using casting_iterator = transform_iterator<I, static_cast_op<T>>; 

	template <typename T, typename I>
	inline
	casting_iterator<T, I> make_casting_iterator(I it) {
		return casting_iterator<T, I>(it, static_cast_op<T>());
	}
}

#endif
