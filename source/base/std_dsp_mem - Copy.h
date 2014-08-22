
#ifndef STD_DSP_MEM_GUARD
#define STD_DSP_MEM_GUARD

#include <cstdint>
#include <cstdlib>
#include <cassert>

#ifdef WIN32
#include <Windows.h>
#endif

#include "std_dsp_alignment.h"

#include "base.h"

namespace std_dsp {
	template <typename N>
	inline
	double* alloc_buf(N n) {
		assert(n >= N(0));

#ifdef WIN32
		double* x = static_cast<double*>(_aligned_malloc(static_cast<std::size_t>(n) * sizeof(double), 16));
#else
		double* x = static_cast<double*>(malloc(static_cast<std::size_t>(n) * sizeof(double)));
#endif
	
		//If the underlying implementation returns zero rather than throwing an exception
		//we throw the exception ourselves on failure
		if(x == 0)
			throw std::bad_alloc();
	
		return x;
	}

	inline
	void free_buf(double* buf) {
#ifdef WIN32
		_aligned_free(buf);
#else
		free(buf);
#endif
	}

	namespace detail {
		template <typename N>
		inline
		N make_even(N n) {
			if ((n & N(1)) == N(1))
				return n + N(1);
			return n;
		}
	}

	template <integer_t CHANNELS, integer_t SIZE>
	class static_storage {
	private:
		SSE_ALIGN double data[CHANNELS * SIZE + ((SIZE & 1) == 1) * (CHANNELS - 1)];

		inline
		integer_t channel_offset(integer_t channel) const {
			static const integer_t STRIDE = detail::make_even(SIZE);
			return STRIDE * channel;
		}
		inline
		integer_t end_offset(integer_t channel) const {
			return channel_offset(channel) + SIZE;
		}
	public:
		using value_type = double;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using difference_type = integer_t;
		using iterator = pointer;
		using const_iterator = const_pointer;

		inline
		double* begin() { return data; }
		inline
		const double* begin() const { return data; }
		inline
		const double* cbegin() const { return data; }
		inline
		double* end() { return data + SIZE * CHANNELS; }
		inline
		const double* end() const { return data + SIZE * CHANNELS; }
		inline
		const double* cend() const { return data + SIZE * CHANNELS; }
		inline
		double* begin(integer_t channel) { return data + channel_offset(channel); }
		inline
		const double* begin(integer_t channel) const { return data + channel_offset(channel); }
		inline
		const double* cbegin(integer_t channel) const { return data + channel_offset(channel); }
		inline
		double* end(integer_t channel) { return data + end_offset(channel); }
		inline
		const double* end(integer_t channel) const { return data + end_offset(channel); }
		inline
		const double* cend(integer_t channel) const { return data + end_offset(channel); }

		inline
		integer_t channels() const { return CHANNELS; }
		inline
		integer_t size() const { return SIZE; }
	};

	template <integer_t CHANNELS>
	class dynamic_storage {
	private:
		double* data;
		integer_t buf_size;

		inline
		integer_t channel_offset(integer_t channel) const {
			integer_t stride = detail::make_even(buf_size);
			return stride * channel;
		}
		inline
		integer_t end_offset(integer_t channel) const {
			return channel_offset(channel) + buf_size;
		}
	public:
		using value_type = double;
		using pointer = value_type*;
		using const_pointer = const pointer;
		using reference = value_type&;
		using const_reference = const reference;
		using difference_type = integer_t;
		using iterator = pointer;
		using const_iterator = const_pointer;

		dynamic_storage() : data(nullptr), buf_size(0LL) {}
		explicit dynamic_storage(integer_t n) : data(nullptr), buf_size(0LL) { resize(n); }
		dynamic_storage(const dynamic_storage& x) {
			data = nullptr;
			resize(x.size());
			copy_n(x.begin(), x.physical_size(), data);
		}
		dynamic_storage(dynamic_storage&& x) {
			data = x.data;
			buf_size = x.buf_size;
			x.data = nullptr;
			x.buf_size = 0LL;
		}
		~dynamic_storage() {
			free_buf(data);
		}

		dynamic_storage& operator=(const dynamic_storage& x) {
			using std::swap;
			dynamic_storage tmp(x);
			swap(*this, tmp);
		}
		dynamic_storage& operator=(dynamic_storage&& x) {
			if(this == &x)
				return *this;
			data = x.data;
			buf_size = x.buf_size;
			x.data = nullptr;
			x.buf_size = 0LL;
		}

		inline
		friend
		void swap(dynamic_storage& x, dynamic_storage& y) {
			using std::swap;
			swap(x.data, y.data);
			swap(x.buf_size, y.buf_size);
		}

		inline
		double* begin() { return data; }
		inline
		const double* begin() const { return data; }
		inline
		const double* cbegin() const { return data; }
		inline
		double* end() { return data + CHANNELS * buf_size; }
		inline
		const double* end() const { return data + CHANNELS * buf_size; }
		inline
		const double* cend() const { return data + CHANNELS * buf_size; }
		inline
		double* begin(integer_t channel) { return data + channel_offset(channel); }
		inline
		const double* begin(integer_t channel) const { return data + channel_offset(channel); }
		inline
		const double* cbegin(integer_t channel) const { return data + channel_offset(channel); }
		inline
		double* end(integer_t channel) { return data + end_offset(channel); }
		inline
		const double* end(integer_t channel) const { return data + end_offset(channel); }
		inline
		const double* cend(integer_t channel) const { return data + end_offset(channel); }

		inline
		integer_t channels() const { return CHANNELS; }
		inline
		integer_t size() const { return buf_size; }
		inline
		integer_t physical_size() const { return channels * detail::make_even(buf_size); }

		inline
		void resize(integer_t n) {
			integer_t even_n = detail::make_even(n);
			if(n == buf_size)
				return;

			double* p = alloc_buf(CHANNELS * even_n);
			if(data != nullptr)
				free_buf(data);
			data = p;
			buf_size = n;
		}
	};

	template <>
	class dynamic_storage<0LL> {
	private:
		double* data;
		integer_t channel_count;
		integer_t buf_size;

		inline
		integer_t channel_offset(integer_t channel) const {
			integer_t stride = detail::make_even(buf_size);
			return stride * channel;
		}
		inline
			integer_t end_offset(integer_t channel) const {
			return channel_offset(channel) + buf_size;
		}
	public:
		using value_type = double;
		using pointer = value_type*;
		using const_pointer = const pointer;
		using reference = value_type&;
		using const_reference = const reference;
		using difference_type = integer_t;
		using iterator = pointer;
		using const_iterator = const_pointer;

		dynamic_storage() : data(nullptr), channel_count(0LL), buf_size(0LL) {}
		dynamic_storage(integer_t ch, integer_t n) : data(nullptr), channel_count(0LL), buf_size(0LL) { resize(ch, n); }
		dynamic_storage(const dynamic_storage& x) {
			data = nullptr;
			channel_count = 0LL;
			buf_size = 0LL;
			resize(x.channels(), x.size());
			std::copy_n(x.begin(), x.channels() * x.size(), data);
		}
		dynamic_storage(dynamic_storage&& x) {
			data = x.data;
			buf_size = x.buf_size;
			x.data = nullptr;
			x.buf_size = 0LL;
		}
		~dynamic_storage() {
			free_buf(data);
		}

		dynamic_storage& operator=(const dynamic_storage& x) {
			using std::swap;
			dynamic_storage tmp(x);
			swap(*this, tmp);
		}
		dynamic_storage& operator=(dynamic_storage&& x) {
			if (this == &x)
				return *this;
			data = x.data;
			buf_size = x.buf_size;
			x.data = nullptr;
			x.buf_size = 0LL;
		}

		inline
		friend
		void swap(dynamic_storage& x, dynamic_storage& y) {
			using std::swap;
			swap(x.data, y.data);
			swap(x.channels, y.channels);
			swap(x.buf_size, y.buf_size);
		}

		inline
			double* begin() { return data; }
		inline
			const double* begin() const { return data; }
		inline
			const double* cbegin() const { return data; }
		inline
			double* end() { return data + channels() * buf_size; }
		inline
			const double* end() const { return data + channels() * buf_size; }
		inline
			const double* cend() const { return data + channels() * buf_size; }
		inline
			double* begin(integer_t channel) { return data + channel_offset(channel); }
		inline
			const double* begin(integer_t channel) const { return data + channel_offset(channel); }
		inline
			const double* cbegin(integer_t channel) const { return data + channel_offset(channel); }
		inline
			double* end(integer_t channel) { return data + end_offset(channel); }
		inline
			const double* end(integer_t channel) const { return data + end_offset(channel); }
		inline
			const double* cend(integer_t channel) const { return data + end_offset(channel); }

		inline
			integer_t channels() const { return channels(); }
		inline
			integer_t size() const { return buf_size; }
		inline
			integer_t physical_size() const { return channels() * detail::make_even(buf_size); }

		inline
			void resize(integer_t ch, integer_t n) {
			integer_t even_n = detail::make_even(n);
			if (ch == channels() && n == buf_size)
				return;

			double* p = alloc_buf(ch * even_n);
			if (data != nullptr)
				free_buf(data);
			data = p;
			channel_count = ch;
			buf_size = n;
		}

		inline
		void resize(integer_t n) {
			resize(channel_count, n);
		}
	};

	template <typename S1, typename S2>
	inline
	bool compare_split(const S1& x, const S2& y) {
		if (x.channels() != y.channels())
			return false;
		for (integer_t i = 0; i < x.channels(); ++i) {
			if (!compare(x.cbegin(i), y.cbegin(i), x.size()))
				return false;
		}
		return true;
	}
	template <typename S1, typename S2>
	inline
		bool compare_interleaved(const S1& x, const S2& y) {
		if (x.channels() != y.channels())
			return false;
		if (!compare(x.cbegin(), y.cbegin(), x.channels() * x.size()))
			return false;
		return true;
	}
}

#endif
